#pragma once

// Ref: https://github.com/leandromoreira/ffmpeg-libav-tutorial

#pragma warning(disable : 4819)
#pragma warning(disable : 4996)

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")

#include <string>
#include <functional>

#include "WindowsCommon.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class FFMpeg;

#include <SDL.h>
#include <SDL_thread.h>

// SDL
constexpr auto SDL_AUDIO_BUFFER_SIZE = 1024;
constexpr auto MAX_AUDIO_FRAME_SIZE = 192000;

constexpr auto SDL_EXCEPTION_AUDIO = 0;

//数据包队列(链表)结构体
typedef struct PacketQueue {
	AVPacketList* first_pkt, * last_pkt;	//队列首尾节点指针
	int nb_packets;							//队列长度
	int size;								//保存编码数据的缓存长度，size=packet->size
	SDL_mutex* qlock;						//队列互斥量，保护队列数据
	SDL_cond* qready;						//队列就绪条件变量
} PacketQueue;

//队列初始化函数
inline void packet_queue_init(PacketQueue* q) {
	memset(q, 0, sizeof(PacketQueue));//全零初始化队列结构体对象
	q->qlock = SDL_CreateMutex();//创建互斥量对象
	q->qready = SDL_CreateCond();//创建条件变量对象
}

//清除队列缓存，释放队列中所有动态分配的内存
inline void packet_queue_flush(PacketQueue* q) {
	AVPacketList* pkt, * pkttmp;//队列当前节点，临时节点

	SDL_LockMutex(q->qlock);//锁定互斥量
	for (pkt = q->first_pkt; pkt != NULL; pkt = pkttmp) {//遍历队列所有节点
		pkttmp = pkt->next;//队列头节点后移
		av_packet_unref(&pkt->pkt);//当前节点引用计数-1
		av_freep(&pkt);//释放当前节点缓存
	}
	q->last_pkt = NULL;//队列尾节点指针置零
	q->first_pkt = NULL;//队列头节点指针置零
	q->nb_packets = 0;//队列长度置零
	q->size = 0;//队列编码数据的缓存长度置零
	SDL_UnlockMutex(q->qlock);//互斥量解锁
}

//向队列中插入数据包
inline int packet_queue_put(PacketQueue* q, AVPacket* pkt) {
	/*-------准备队列(链表)节点对象------*/
	AVPacketList* pktlist;//创建链表节点对象指针
	pktlist = (AVPacketList*)av_malloc(sizeof(AVPacketList));//在堆上创建链表节点对象

	if (!pktlist) {//检查链表节点对象是否创建成功
		return -1;
	}
	pktlist->pkt = *pkt;//将输入数据包赋值给新建链表节点对象中的数据包对象
	pktlist->next = NULL;//链表后继指针为空
	//	if (av_packet_ref(pkt, pkt)<0) {//增加pkt编码数据的引用计数(输入参数中的pkt与新建链表节点中的pkt共享同一缓存空间)
	//		return -1;
	//	}
	/*---------将新建节点插入队列-------*/
	SDL_LockMutex(q->qlock);//队列互斥量加锁，保护队列数据

	if (!q->last_pkt) {//检查队列尾节点是否存在(检查队列是否为空)
		q->first_pkt = pktlist;//若不存在(队列尾空)，则将当前节点作队列为首节点
	}
	else {
		q->last_pkt->next = pktlist;//若已存在尾节点，则将当前节点挂到尾节点的后继指针上，并作为新的尾节点
	}
	q->last_pkt = pktlist;//将当前节点作为新的尾节点
	q->nb_packets++;//队列长度+1
	q->size += pktlist->pkt.size;//更新队列编码数据的缓存长度

	SDL_CondSignal(q->qready);//给等待线程发出消息，通知队列已就绪

	SDL_UnlockMutex(q->qlock);//释放互斥量
	return 0;
}

//从队列中提取数据包，并将提取的数据包出队列
inline int packet_queue_get(PacketQueue* q, AVPacket* pkt, int block) {
	AVPacketList* pktlist;//临时链表节点对象指针
	int ret;//操作结果

	SDL_LockMutex(q->qlock);//队列互斥量加锁，保护队列数据

	for (;;) {
		pktlist = q->first_pkt;//传递将队列首个数据包指针
		if (pktlist) {//检查数据包是否为空(队列是否有数据)
			q->first_pkt = pktlist->next;//队列首节点指针后移
			if (!q->first_pkt) {//检查首节点的后继节点是否存在
				q->last_pkt = NULL;//若不存在，则将尾节点指针置空
			}
			q->nb_packets--;//队列长度-1
			q->size -= pktlist->pkt.size;//更新队列编码数据的缓存长度
			*pkt = pktlist->pkt;//将队列首节点数据返回
			av_free(pktlist);//清空临时节点数据(清空首节点数据，首节点出队列)
			ret = 1;//操作成功
			break;
		}
		else if (!block) {
			ret = 0;
			break;
		}
		else {
			//队列处于未就绪状态，此时通过SDL_CondWait函数等待qready就绪信号，并暂时对互斥量解锁
			/*---------------------
			 * 等待队列就绪信号qready，并对互斥量暂时解锁
			 * 此时线程处于阻塞状态，并置于等待条件就绪的线程列表上
			 * 使得该线程只在临界区资源就绪后才被唤醒，而不至于线程被频繁切换
			 * 该函数返回时，互斥量再次被锁住，并执行后续操作
			 --------------------*/
			SDL_CondWait(q->qready, q->qlock);//暂时解锁互斥量并将自己阻塞，等待临界区资源就绪(等待SDL_CondSignal发出临界区资源就绪的信号)
		}
	}//end for for-loop

	SDL_UnlockMutex(q->qlock);//释放互斥量

	return ret;
}


/*------Audio Callback-------
 * 音频输出回调函数，sdl通过该回调函数将解码后的pcm数据送入声卡播放,
 * sdl通常一次会准备一组缓存pcm数据，通过该回调送入声卡，声卡根据音频pts依次播放pcm数据
 * 待送入缓存的pcm数据完成播放后，再载入一组新的pcm缓存数据(每次音频输出缓存为空时，sdl就调用此函数填充音频输出缓存，并送入声卡播放)
 * When we begin playing audio, SDL will continually call this callback function
 * and ask it to fill the audio buffer with a certain number of bytes
 * The audio function callback takes the following parameters:
 * stream: A pointer to the audio buffer to be filled，输出音频数据到声卡缓存
 * len: The length (in bytes) of the audio buffer,缓存长度wanted_spec.samples=SDL_AUDIO_BUFFER_SIZE(1024)
 --------------------------*/
 inline void audio_callback(void* userdata, Uint8* stream, int len) {
 	//AVCodecContext* aCodecCtx = (AVCodecContext*)userdata;//传递用户数据
 	//int wt_stream_len, audio_size;//每次写入stream的数据长度，解码后的数据长度
 
 	//static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];//保存解码一个packet后的多帧原始音频数据
 	//static unsigned int audio_buf_size = 0;//解码后的多帧音频数据长度
 	//static unsigned int audio_buf_index = 0;//累计写入stream的长度
 
 	//while (len > 0) {//检查音频缓存的剩余长度
 	//	if (audio_buf_index >= audio_buf_size) {//检查是否需要执行解码操作
 	//		// We have already sent all our data; get more，从缓存队列中提取数据包、解码，并返回解码后的数据长度，audio_buf缓存中可能包含多帧解码后的音频数据
 	//		audio_size = audio_decode_frame(aCodecCtx, audio_buf, audio_buf_size);
 	//		if (audio_size < 0) {//检查解码操作是否成功
 	//			// If error, output silence.
 	//			audio_buf_size = 1024; // arbitrary?
 	//			memset(audio_buf, 0, audio_buf_size);//全零重置缓冲区
 	//		}
 	//		else {
 	//			audio_buf_size = audio_size;//返回packet中包含的原始音频数据长度(多帧)
 	//		}
 	//		audio_buf_index = 0;//初始化累计写入缓存长度
 	//	}//end for if
 
 	//	wt_stream_len = audio_buf_size - audio_buf_index;//计算解码缓存剩余长度
 	//	if (wt_stream_len > len) {//检查每次写入缓存的数据长度是否超过指定长度(1024)
 	//		wt_stream_len = len;//指定长度从解码的缓存中取数据
 	//	}
 	//	//每次从解码的缓存数据中以指定长度抽取数据并写入stream传递给声卡
 	//	memcpy(stream, (uint8_t*)audio_buf + audio_buf_index, wt_stream_len);
 	//	len -= wt_stream_len;//更新解码音频缓存的剩余长度
 	//	stream += wt_stream_len;//更新缓存写入位置
 	//	audio_buf_index += wt_stream_len;//更新累计写入缓存数据长度
 	//}//end for while
 }


// Exceptions
constexpr auto FFMpegException_InitFailed = -1;

// pData, width, height
using rawDataCallBack = std::function<void(const unsigned char*, const int, const int)>;

using Uint8 = unsigned char;

class FFMpeg {
private:
	bool bLoop = true;

	AVFormatContext* pFormatContext = nullptr;

	const AVCodec* pVCodec = NULL;
	AVCodecParameters* pVCodecParameters = NULL;

	const AVCodec* pACodec = NULL;
	AVCodecParameters* pACodecParameters = NULL;

	int video_stream_index = -1;
	int audio_stream_index = -1;

	AVCodecContext* pVCodecContext = nullptr;
	AVCodecContext* pACodecContext = nullptr;

	AVFrame* pFrame = nullptr;
	AVPacket* pPacket = nullptr;

	PacketQueue videoQueue;
	PacketQueue audioQueue;
	
	SDL_AudioSpec spec;
	SDL_AudioSpec wanted_spec;	

	SwsContext* swsContext = nullptr;

	AVStream* pViedoStream = nullptr;
	AVStream* pAudioStream = nullptr;

	AVDictionary* pVOptionsDict = nullptr;
	AVDictionary* pAOptionsDict = nullptr;

	AVRational rational = { 0 };
	double decimalRational = 0;

	int64_t totalFrame = 0;
	double timePerFrameInMs = 0;

	double totalTime = 0;
	double totalTimeInMs = 0;

	int decode_vpacket(AVPacket* pPacket, AVCodecContext* pVCodecContext, AVFrame* pFrame, rawDataCallBack callBack) {
		// Supply raw packet data as input to a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
		int response = avcodec_send_packet(pVCodecContext, pPacket);

		if (response < 0) {
			return response;
		}


		while (response >= 0) {
			// Return decoded output data (into a frame) from a decoder
			// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
			response = avcodec_receive_frame(pVCodecContext, pFrame);

			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				break;
			}
			else if (response < 0) {
				av_frame_unref(pFrame);

				return response;
			}

			if (response >= 0) {
				// Check if the frame is a planar YUV 4:2:0, 12bpp
				// That is the format of the provided .mp4 file
				// https://zhuanlan.zhihu.com/p/53305541
				if (pFrame->format != AV_PIX_FMT_BGR24) {
					int linesize [8] = { pFrame->linesize [0] * 3 };
					int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pFrame->width, pFrame->height, 1);

					//auto p_global_bgr_buffer = (uint8_t*)malloc(num_bytes * sizeof(uint8_t));
					auto p_global_bgr_buffer = new uint8_t [num_bytes];
					uint8_t* bgr_buffer [8] = { p_global_bgr_buffer };

					sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, bgr_buffer, linesize);

					//bgr_buffer[0] is the BGR raw data
					callBack(bgr_buffer [0], pFrame->width, pFrame->height);

					delete[] p_global_bgr_buffer;
				}
				else {
					// call callback
					callBack(pFrame->data [0], pFrame->width, pFrame->height);
				}

				av_frame_unref(pFrame);
			}
		}

		return 0;
	}

	int decode_apacket(AVPacket* pPacket, AVCodecContext* pACodecContext, AVFrame* pFrame, rawDataCallBack callBack) {
		int response = avcodec_send_packet(pACodecContext, pPacket);

		if (response < 0) {
			return response;
		}

		while (response >= 0) {
			// Return decoded output data (into a frame) from a decoder
			// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
			response = avcodec_receive_frame(pACodecContext, pFrame);

			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				break;
			}
			else if (response < 0) {
				av_frame_unref(pFrame);

				return response;
			}

			if (response >= 0) {
				//while (audio_pkt_size > 0) {
				//	int got_frame = 0;
				//	len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
				//	if (len1 < 0) {
				//		/* if error, skip frame */
				//		audio_pkt_size = 0;
				//		break;
				//	}
				//	audio_pkt_data += len1;
				//	audio_pkt_size -= len1;
				//	data_size = 0;
				//	if (got_frame) {
				//		data_size = av_samples_get_buffer_size(NULL,
				//			aCodecCtx->channels,
				//			frame.nb_samples,
				//			aCodecCtx->sample_fmt,
				//			1);
				//		assert(data_size <= buf_size);
				//		memcpy(audio_buf, frame.data[0], data_size);
				//	}
				//	if (data_size <= 0) {
				//		/* No data yet, get more frames */
				//		continue;
				//	}
				//	/* We have data, return it and come back for more later */
				//	return data_size;
				//}

				av_frame_unref(pFrame);
			}

		}

		return 0;
	}

public:
	FFMpeg(const std::wstring& filePath) {
		pFormatContext = avformat_alloc_context();
		if (!pFormatContext) {
			throw FFMpegException_InitFailed;
		}

		if (avformat_open_input(&pFormatContext, ConvertWStrToStr(filePath).c_str(), NULL, NULL) != 0) {
			throw FFMpegException_InitFailed;
		}

		if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
			throw FFMpegException_InitFailed;
		}

		for (unsigned int i = 0; i < pFormatContext->nb_streams; i++) {
			AVCodecParameters* pLocalCodecParameters = NULL;
			pLocalCodecParameters = pFormatContext->streams [i]->codecpar;

			const AVCodec* pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

			if (pLocalCodec == NULL) {
				continue;
			}

			if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
				if (video_stream_index == -1) {
					video_stream_index = i;
					pVCodec = pLocalCodec;
					pVCodecParameters = pLocalCodecParameters;
				}
			}

			if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
				if (audio_stream_index == -1) {
					audio_stream_index = i;
					pACodec = pLocalCodec;
					pACodecParameters = pLocalCodecParameters;
				}
			}
		}

		pVCodecContext = avcodec_alloc_context3(pVCodec);
		if (!pVCodecContext) {
			throw FFMpegException_InitFailed;
		}

		if (avcodec_parameters_to_context(pVCodecContext, pVCodecParameters) < 0) {
			throw FFMpegException_InitFailed;
		}

		if (avcodec_open2(pVCodecContext, pVCodec, NULL) < 0) {
			throw FFMpegException_InitFailed;
		}

		swsContext = sws_getContext(pVCodecContext->width, pVCodecContext->height, pVCodecContext->pix_fmt,
			pVCodecContext->width, pVCodecContext->height, AV_PIX_FMT_BGR24
			, NULL, NULL, NULL, NULL);

		pACodecContext = avcodec_alloc_context3(pACodec);
		if (!pACodecContext) {
			throw FFMpegException_InitFailed;
		}

		if (avcodec_parameters_to_context(pACodecContext, pACodecParameters) < 0) {
			throw FFMpegException_InitFailed;
		}

		if (avcodec_open2(pACodecContext, pACodec, NULL) < 0) {
			throw FFMpegException_InitFailed;
		}

		packet_queue_init(&audioQueue);

		pFrame = av_frame_alloc();
		if (!pFrame) {
			throw FFMpegException_InitFailed;
		}

		pPacket = av_packet_alloc();
		if (!pPacket) {
			throw FFMpegException_InitFailed;
		}

		pViedoStream = pFormatContext->streams [video_stream_index];
		pAudioStream = pFormatContext->streams [audio_stream_index];

		rational = pViedoStream->time_base;
		decimalRational = (double)rational.num / rational.den;

		totalFrame = pViedoStream->duration;
		timePerFrameInMs = decimalRational * 1000;

		totalTime = totalFrame * decimalRational;
		totalTimeInMs = totalTime * 1000;

		wanted_spec.freq = pACodecContext->sample_rate;//采样频率 DSP frequency -- samples per second
		wanted_spec.format = AUDIO_S16SYS;//采样格式 Audio data format
		wanted_spec.channels = pACodecContext->channels;//声道数 Number of channels: 1 mono, 2 stereo
		wanted_spec.silence = 0;//无输出时是否静音
		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;//默认每次读音频缓存的大小，推荐值为 512~8192，ffplay使用的是1024 specifies a unit of audio data refers to the size of the audio buffer in sample frames

		wanted_spec.callback = audio_callback;//设置取音频数据的回调接口函数 the function to call when the audio device needs more data
		wanted_spec.userdata = (void*)pACodecContext;//传递用户数据

		if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_AUDIO;
		}

		SDL_PauseAudio(0);
	}

	~FFMpeg() {
		avformat_close_input(&pFormatContext);
		av_packet_free(&pPacket);
		av_frame_free(&pFrame);
		avcodec_close(pVCodecContext);
		avcodec_free_context(&pVCodecContext);

		sws_freeContext(swsContext);
	}

	//inline void update_audiosettings(std::function<void(const AVCodecContext*)> callBack) {
	//	callBack(pACodecContext);
	//}

	inline int64_t get_timePerFrame() {
		return (int64_t)(timePerFrameInMs);
	}

	inline int set_videoPosition(size_t ms = 0) {
		auto protectedTimeInMs = min(totalTimeInMs, max(0, ms));
		auto protectedTime = protectedTimeInMs / 1000;
		auto protectedFrame = protectedTime / decimalRational;

		//auto flags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;
		auto flags = AVSEEK_FLAG_FRAME;

		if (!(av_seek_frame(pFormatContext, video_stream_index
			, (int64_t)(protectedFrame)
			, flags) >= 0)) {
			return -1;
		}
		else {
			if (audio_stream_index != -1) {
				//TODO
				packet_queue_flush(&audioQueue);
				//packet_queue_put(&audioQ);
			}
		}

		return 0;
	}

	inline int read_frame(rawDataCallBack callBack) {
		int response = 0;

		while (av_read_frame(pFormatContext, pPacket) >= 0) {
			// if it's the video stream
			if (pPacket->stream_index == video_stream_index) {
				response = decode_vpacket(pPacket, pVCodecContext, pFrame, callBack);

				if (response < 0) {
					av_packet_unref(pPacket);
					return response;
				}

				break;
			}

			//if it's the audio stream
			//if (pPacket->stream_index == audio_stream_index) {
			//	packet_queue_put(&audioQ, pPacket); //向缓存队列中填充编码数据包
			//}

			//av_packet_unref(pPacket);
		}

		return 0;
	}

	inline int get_videoFrame(size_t ms, rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = set_videoPosition(ms);
		response = read_frame(callBack);

		return response;
	}

	inline int get_nextFrame(rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = read_frame(callBack);

		if (bLoop && (int64_t)(pVCodecContext->frame_number + 1) == totalFrame) {
			response = set_videoPosition();

			if (response < 0) { return response; }

			pVCodecContext->frame_number = 0;
			pACodecContext->frame_number = 0;
		}

		return response;
	}
};
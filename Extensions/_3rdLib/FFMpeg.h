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
#include <libswresample/swresample.h>
}

class FFMpeg;

extern "C" {
#include <SDL.h>
#include <SDL_thread.h>
}

// SDL
constexpr auto SDL_AUDIO_BUFFER_SIZE = 1024;
constexpr auto MAX_AUDIO_FRAME_SIZE = 192000;

constexpr auto MAX_AUDIOQ_SIZE = (5 * 16 * 1024);
constexpr auto MAX_VIDEOQ_SIZE = (5 * 256 * 1024);

constexpr auto SDL_EXCEPTION_AUDIO = 0;

static int quit = 0;

//数据包队列(链表)结构体
typedef struct PacketQueue {
	AVPacketList* first_pkt, * last_pkt;	//队列首尾节点指针
	int nb_packets;							//队列长度
	int size;								//保存编码数据的缓存长度，size=packet->size
	SDL_mutex* qlock;						//队列互斥量，保护队列数据
	SDL_cond* qready;						//队列就绪条件变量
} PacketQueue;

static PacketQueue videoQueue;
static PacketQueue audioQueue;

//队列初始化函数
inline void packet_queue_init(PacketQueue* q) {
	memset(q, 0, sizeof(PacketQueue));//全零初始化队列结构体对象

	q->qlock = SDL_CreateMutex();//创建互斥量对象
	q->qready = SDL_CreateCond();//创建条件变量对象
}

//清除队列缓存，释放队列中所有动态分配的内存
inline void packet_queue_flush(PacketQueue* q) {
	AVPacketList* pkt = nullptr;//队列当前节点
	AVPacketList* pkttmp = nullptr;//临时节点

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
	int ret = 0;//操作结果

	SDL_LockMutex(q->qlock);//队列互斥量加锁，保护队列数据

	for (;;) {
		if (quit) {
			ret = -1;

			break;
		}

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
	}

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
inline void audio_callback(void* userdata, Uint8* stream, int len);

// Exceptions
constexpr auto FFMpegException_InitFailed = -1;

// pData, width, height
using rawDataCallBack = std::function<void(const unsigned char*, const int, const int)>;

using Uint8 = unsigned char;

#define _GET_AUDIO_BYLOOP

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

	AVPacket* pVPacket = nullptr;
	AVPacket* pAPacket = nullptr;
	AVPacket* pFlushPacket = nullptr;

	uint8_t* audio_buf = nullptr;
	//unsigned int audio_buf_size;
	//unsigned int audio_buf_index;

	//uint8_t* audio_pkt_data;
	//int audio_pkt_size;
	//int audio_hw_buf_size;

	SDL_AudioSpec spec = { 0 };
	SDL_AudioSpec wanted_spec = { 0 };

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

	int64_t globalPts = 0;
	double videoClock = 0;

	inline void init_general() {
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

#ifdef _GET_AUDIO_BYLOOP
			if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
				if (audio_stream_index == -1) {
					audio_stream_index = i;
					pACodec = pLocalCodec;
					pACodecParameters = pLocalCodecParameters;
				}
			}
#endif
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

		packet_queue_init(&videoQueue);

		swsContext = sws_getContext(pVCodecContext->width, pVCodecContext->height, pVCodecContext->pix_fmt,
			pVCodecContext->width, pVCodecContext->height, AV_PIX_FMT_BGR24
			, NULL, NULL, NULL, NULL);

#ifndef _GET_AUDIO_BYLOOP
		audio_stream_index = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &pACodec, 0);

		if (audio_stream_index < 0){
			throw FFMpegException_InitFailed;

		}

		pACodecParameters = pFormatContext->streams[audio_stream_index]->codecpar;
#endif

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

		pVPacket = av_packet_alloc();
		if (!pVPacket) {
			throw FFMpegException_InitFailed;
		}

		pAPacket = av_packet_alloc();
		if (!pAPacket) {
			throw FFMpegException_InitFailed;
		}

		pFlushPacket = av_packet_alloc();
		if (!pFlushPacket) {
			throw FFMpegException_InitFailed;
		}

		pFlushPacket->data = (unsigned char*)"FLUSH";

		pViedoStream = pFormatContext->streams [video_stream_index];
		pAudioStream = pFormatContext->streams [audio_stream_index];

		rational = pViedoStream->time_base;
		decimalRational = (double)rational.num / rational.den;

		totalFrame = pViedoStream->duration;
		timePerFrameInMs = decimalRational * 1000;

		totalTime = totalFrame * decimalRational;
		totalTimeInMs = totalTime * 1000;

		audio_buf = new uint8_t [(MAX_AUDIO_FRAME_SIZE * 3) / 2];
		memset(audio_buf, 0, (MAX_AUDIO_FRAME_SIZE * 3) / 2);

		wanted_spec.freq = pACodecContext->sample_rate;//采样频率 DSP frequency -- samples per second
		wanted_spec.format = AUDIO_S16SYS;//采样格式 Audio data format
		wanted_spec.channels = pACodecContext->channels;//声道数 Number of channels: 1 mono, 2 stereo
		wanted_spec.silence = 0;//无输出时是否静音
		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;//默认每次读音频缓存的大小，推荐值为 512~8192，ffplay使用的是1024 specifies a unit of audio data refers to the size of the audio buffer in sample frames
		//wanted_spec.samples = pACodecContext->frame_size;

		wanted_spec.callback = audio_callback;//设置取音频数据的回调接口函数 the function to call when the audio device needs more data
		wanted_spec.userdata = (void*)this;//传递用户数据

		if (SDL_OpenAudio(&wanted_spec, nullptr) < 0) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_AUDIO;
		}

		SDL_PauseAudio(0);
	}

	inline int read_frame() {
		int response = 0;

		while (true) {
			if (audioQueue.size > MAX_AUDIOQ_SIZE
				|| videoQueue.size > MAX_VIDEOQ_SIZE) {
				SDL_Delay(10);

				return 0;
			}

			auto readResult = av_read_frame(pFormatContext, pVPacket);

			if (readResult < 0) {
				SDL_Delay(100);

				return 0;
			}

			bool bValid = false;

			// if it's the video stream
			if (pVPacket->stream_index == video_stream_index) {
				packet_queue_put(&videoQueue, pVPacket);
				bValid = true;
			}

			//if it's the audio stream
			if (pVPacket->stream_index == audio_stream_index) {
				packet_queue_put(&audioQueue, pVPacket);
				bValid = true;
			}

			if (!bValid) {
				av_packet_unref(pVPacket);
			}
		}

		return 0;
	}

	int decode_vpacket(AVPacket* pVPacket, AVCodecContext* pVCodecContext, AVFrame* pFrame, rawDataCallBack callBack) {
		// Supply raw packet data as input to a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
		int response = avcodec_send_packet(pVCodecContext, pVPacket);

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

	//https://github.com/brookicv/FFMPEG-study/blob/master/FFmpeg-playAudio.cpp
	int decode_apacket() {
		AVFrame* pFrame = av_frame_alloc();
		int data_size = 0;
		AVPacket pkt = { 0 };

		SwrContext* swr_ctx = nullptr;

		if (!packet_queue_get(&audioQueue, &pkt, true)) {
			return -1;
		}

		if (quit) {
			return -1;
		}

		int response = avcodec_send_packet(pACodecContext, &pkt);
		if (response < 0 && response != AVERROR(EAGAIN) && response != AVERROR_EOF) {
			return -1;
		}

		response = avcodec_receive_frame(pACodecContext, pFrame);
		if (response < 0 && response != AVERROR_EOF) {
			//av_frame_unref(pFrame);

			return -1;
		}

		int index = av_get_channel_layout_channel_index(av_get_default_channel_layout(4), AV_CH_FRONT_CENTER);

		// 设置通道数或channel_layout
		if (pFrame->channels > 0 && pFrame->channel_layout == 0) {
			pFrame->channel_layout = av_get_default_channel_layout(pFrame->channels);
		}
		else if (pFrame->channels == 0 && pFrame->channel_layout > 0) {
			pFrame->channels = av_get_channel_layout_nb_channels(pFrame->channel_layout);
		}

		AVSampleFormat dst_format = AV_SAMPLE_FMT_S16;//av_get_packed_sample_fmt((AVSampleFormat)frame->format);
		Uint64 dst_layout = av_get_default_channel_layout(pFrame->channels);

		// 设置转换参数
		swr_ctx = swr_alloc_set_opts(nullptr, dst_layout, dst_format, pFrame->sample_rate,
			pFrame->channel_layout, (AVSampleFormat)pFrame->format, pFrame->sample_rate, 0, nullptr);

		if (!swr_ctx || swr_init(swr_ctx) < 0) {
			return -1;
		}

		// 计算转换后的sample个数 a * b / c
		int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swr_ctx, pFrame->sample_rate) + pFrame->nb_samples, pFrame->sample_rate, pFrame->sample_rate, AVRounding(1));

		// 转换，返回值为转换后的sample个数
		int nb = swr_convert(swr_ctx, &audio_buf, dst_nb_samples, (const uint8_t**)pFrame->data, pFrame->nb_samples);
		data_size = pFrame->channels * nb * av_get_bytes_per_sample(dst_format);

		av_frame_free(&pFrame);
		swr_free(&swr_ctx);

		return data_size;
	}

	/*---------------------------
	 * 更新内部视频播放计时器(记录视频已经播时间(video_clock)）
	 * @is：全局状态参数集
	 * @src_frame：当前(输入的)(待更新的)图像帧对象
	 * @pts：当前图像帧的显示时间戳
	 ---------------------------*/
	inline double synchronize_video(AVFrame* src_frame, double pts) {
		/*----------检查显示时间戳----------*/
		if (pts != 0) {//检查显示时间戳是否有效
			// If we have pts, set video clock to it.
			videoClock = pts;//用显示时间戳更新已播放时间
		}
		else {//若获取不到显示时间戳
			// If we aren't given a pts, set it to the clock.
			pts = videoClock;//用已播放时间更新显示时间戳
		}
		/*--------更新视频已经播时间--------*/
			// Update the video clock，若该帧要重复显示(取决于repeat_pict)，则全局视频播放时序video_clock应加上重复显示的数量*帧率
		double frame_delay = av_q2d(pVCodecContext->time_base);//该帧显示完将要花费的时间
		// If we are repeating a frame, adjust clock accordingly,若存在重复帧，则在正常播放的前后两帧图像间安排渲染重复帧
		frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);//计算渲染重复帧的时值(类似于音符时值)
		videoClock += frame_delay;//更新视频播放时序

		return pts;//此时返回的值即为下一帧将要开始显示的时间戳
	}

public:
	//Load from file
	FFMpeg(const std::wstring& filePath) {
		pFormatContext = avformat_alloc_context();
		if (!pFormatContext) {
			throw FFMpegException_InitFailed;
		}

		if (avformat_open_input(&pFormatContext, ConvertWStrToStr(filePath).c_str(), NULL, NULL) != 0) {
			throw FFMpegException_InitFailed;
		}

		init_general();
	}

	//Load from memory
	//https://blog.csdn.net/leixiaohua1020/article/details/12980423
	FFMpeg(unsigned char* pBuffer, size_t bfSz) {
		pFormatContext = avformat_alloc_context();
		if (!pFormatContext) {
			throw FFMpegException_InitFailed;
		}

		auto pAvio = avio_alloc_context(pBuffer, bfSz, 0, NULL
			, [] (void* opaque, uint8_t* buf, int bufsize)->int {
				return bufsize;
			}
		, NULL, NULL);

		if (!pAvio) {
			throw FFMpegException_InitFailed;
		}

		pFormatContext->pb = pAvio;

		if (avformat_open_input(&pFormatContext, NULL, NULL, NULL) != 0) {
			throw FFMpegException_InitFailed;
		}

		init_general();
	}

	~FFMpeg() {
		avformat_close_input(&pFormatContext);
		av_packet_free(&pVPacket);
		av_packet_free(&pAPacket);
		av_packet_free(&pFlushPacket);
		av_frame_free(&pFrame);
		avcodec_close(pVCodecContext);
		avcodec_free_context(&pVCodecContext);
		avcodec_close(pACodecContext);
		avcodec_free_context(&pACodecContext);

		sws_freeContext(swsContext);

		delete[] audio_buf;

		quit = 1;

		SDL_CondSignal(audioQueue.qready);
		SDL_CondSignal(videoQueue.qready);
	}

	inline int decode_videoFrame(rawDataCallBack callBack) {
		int response = 0;
		double pts = 0;

		if (packet_queue_get(&videoQueue, pVPacket, 1) < 0) {
			return -1;
		}

		if (pVPacket->data == pFlushPacket->data) {
			avcodec_flush_buffers(pVCodecContext);
			return 0;
		}

		globalPts = pVPacket->pts;

		response = decode_vpacket(pVPacket, pVCodecContext, pFrame, callBack);

		av_packet_unref(pVPacket);

		return response;
	}

	inline int decode_audioFrame(Uint8* stream, int len) {
		//每次写入stream的数据长度
		int wt_stream_len = 0;
		//每解码后的数据长度
		int audio_size = 0;

		//static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];//保存解码一个packet后的多帧原始音频数据
		static unsigned int audio_buf_size = 0;//解码后的多帧音频数据长度
		static unsigned int audio_buf_index = 0;//累计写入stream的长度

		SDL_memset(stream, 0, len);

		//检查音频缓存的剩余长度
		while (len > 0) {
			//检查是否需要执行解码操作
			if (audio_buf_index >= audio_buf_size) {
				// We have already sent all our data; get more
				// 从缓存队列中提取数据包、解码，并返回解码后的数据长度，audio_buf缓存中可能包含多帧解码后的音频数据
				audio_size = decode_apacket();

				if (quit) {
					return -1;
				}

				//检查解码操作是否成功
				if (audio_size < 0) {
					// If error, output silence.
					audio_buf_size = SDL_AUDIO_BUFFER_SIZE; // arbitrary?
					memset(audio_buf, 0, audio_buf_size);//全零重置缓冲区
				}
				else {
					//返回packet中包含的原始音频数据长度(多帧)
					audio_buf_size = audio_size;
				}

				audio_buf_index = 0;//初始化累计写入缓存长度
			}

			wt_stream_len = audio_buf_size - audio_buf_index;//计算解码缓存剩余长度

			if (wt_stream_len > len) {//检查每次写入缓存的数据长度是否超过指定长度(1024)
				wt_stream_len = len;//指定长度从解码的缓存中取数据
			}
			//每次从解码的缓存数据中以指定长度抽取数据并写入stream传递给声卡
			//memcpy(stream, (uint8_t*)audio_buf + audio_buf_index, wt_stream_len);
			SDL_MixAudio(stream, audio_buf + audio_buf_index, len, SDL_MIX_MAXVOLUME);

			len -= wt_stream_len;//更新解码音频缓存的剩余长度
			stream += wt_stream_len;//更新缓存写入位置
			audio_buf_index += wt_stream_len;//更新累计写入缓存数据长度
		}

		return 0;
	}

	inline int64_t get_timePerFrame() {
		return (int64_t)(timePerFrameInMs);
	}

	//inline int set_videoPosition(size_t ms = 0, bool bInit = false) {
	inline int set_videoPosition(size_t ms = 0, bool bInit = true) {
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
			if (video_stream_index != -1) {
				if (!bInit) {
					packet_queue_flush(&videoQueue);
					packet_queue_put(&videoQueue, pFlushPacket);
				}
			}
			if (audio_stream_index != -1) {
				if (!bInit) {
					packet_queue_flush(&audioQueue);
					packet_queue_put(&audioQueue, pFlushPacket);
				}
			}
		}

		return 0;
	}

	inline int get_videoFrame(size_t ms, rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = set_videoPosition(ms);

		response = read_frame();
		response = decode_videoFrame(callBack);

		return response;
	}

	inline int get_nextFrame(rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = read_frame();
		response = decode_videoFrame(callBack);
		//response = decode_audioFrame();

		globalPts;

		if (bLoop && (int64_t)(pVCodecContext->frame_number + 1) == totalFrame) {
			response = set_videoPosition();

			if (response < 0) { return response; }

			pVCodecContext->frame_number = 0;
			pACodecContext->frame_number = 0;
		}

		return response;
	}
};

inline void audio_callback(void* userdata, Uint8* stream, int len) {
	FFMpeg* pFFMpeg = (FFMpeg*)userdata;
	pFFMpeg->decode_audioFrame(stream, len);
}
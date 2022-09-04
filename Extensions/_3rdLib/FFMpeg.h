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

static volatile int Stat_Quit = 0;
static volatile int Stat_QuitComplete = 1;

// Exceptions
constexpr auto FFMpegException_InitFailed = -1;

// Define
//constexpr auto seekFlags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;
constexpr auto seekFlags = AVSEEK_FLAG_FRAME;

// pData, width, height
using rawDataCallBack = std::function<void(const unsigned char*, const int, const int)>;

using Uint8 = unsigned char;

#define _GET_AUDIO_BYLOOP

class FFMpeg {
private:
	bool bLoop = true;

	AVFormatContext* pFormatContext = nullptr;
	AVFormatContext* pSeekFormatContext = new AVFormatContext;

	const AVCodec* pVCodec = NULL;
	AVCodecParameters* pVCodecParameters = NULL;

	const AVCodec* pACodec = NULL;
	AVCodecParameters* pACodecParameters = NULL;

	int video_stream_index = -1;
	int audio_stream_index = -1;

	AVCodecContext* pVCodecContext = nullptr;
	AVCodecContext* pACodecContext = nullptr;

	AVFrame* pVFrame = nullptr;
	AVFrame* pAFrame = nullptr;

	AVPacket* pVPacket = nullptr;
	AVPacket* pAPacket = nullptr;
	AVPacket* pFlushPacket = nullptr;

	//保存解码一个packet后的多帧原始音频数据
	uint8_t* audio_buf = nullptr;
	//解码后的多帧音频数据长度
	unsigned int audio_buf_size = 0;
	//累计写入stream的长度
	unsigned int audio_buf_index = 0;

	SDL_AudioSpec spec = { 0 };
	SDL_AudioSpec wanted_spec = { 0 };

	SwsContext* swsContext = nullptr;
	SwrContext* swrContext = nullptr;

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

	int volume = SDL_MIX_MAXVOLUME;

	int64_t globalPts = 0;

	double audioClock = 0;
	double videoClock = 0;

	double frameTimer = 0;
	double frameLastPts = 0;
	double frameLastDelay = 0;

	SDL_AudioDeviceID deviceID;

	inline void init_formatContext(AVFormatContext** pFormatContext, const std::wstring& filePath) {
		*pFormatContext = avformat_alloc_context();
		if (!*pFormatContext) {
			throw FFMpegException_InitFailed;
		}

		if (avformat_open_input(pFormatContext, ConvertWStrToStr(filePath).c_str(), NULL, NULL) != 0) {
			throw FFMpegException_InitFailed;
		}
	}

	inline void init_formatContext(AVFormatContext** pFormatContext, unsigned char* pBuffer, size_t bfSz) {
			*pFormatContext = avformat_alloc_context();
			if (!*pFormatContext) {
				throw FFMpegException_InitFailed;
			}

			auto pAvio = avio_alloc_context(pBuffer, bfSz, 0, NULL
				, [](void* opaque, uint8_t* buf, int bufsize)->int {
					return bufsize;
				}
			, NULL, NULL);

			if (!pAvio) {
				throw FFMpegException_InitFailed;
			}

			(*pFormatContext)->pb = pAvio;

			if (avformat_open_input(pFormatContext, NULL, NULL, NULL) != 0) {
				throw FFMpegException_InitFailed;
			}
	}

	inline void init_general() {
		//pSeekFormatContext
		// find stream
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

		// init video codec
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

		// init audio codec
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

		//int index = av_get_channel_layout_channel_index(av_get_default_channel_layout(4), AV_CH_FRONT_CENTER);

		int channels = pACodecParameters->channels;
		uint64_t channel_layout = pACodecParameters->channel_layout;

		if (channels > 0 && channel_layout == 0) {
			channel_layout = av_get_default_channel_layout(channels);
		}
		else if (channels == 0 && channel_layout > 0) {
			channels = av_get_channel_layout_nb_channels(channel_layout);
		}

		swrContext = swr_alloc_set_opts(nullptr
			, av_get_default_channel_layout(channels)
			, AV_SAMPLE_FMT_S16, pACodecParameters->sample_rate
			, channel_layout
			, (AVSampleFormat)pACodecParameters->format, pACodecParameters->sample_rate
			, 0, nullptr);

		if (!swrContext || swr_init(swrContext) < 0) {
			throw FFMpegException_InitFailed;
		}

		// init others
		pVFrame = av_frame_alloc();
		if (!pVFrame) {
			throw FFMpegException_InitFailed;
		}

		pAFrame = av_frame_alloc();
		if (!pAFrame) {
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

		// init SDL audio
		audio_buf = new uint8_t [(MAX_AUDIO_FRAME_SIZE * 3) / 2];
		memset(audio_buf, 0, (MAX_AUDIO_FRAME_SIZE * 3) / 2);

		wanted_spec.freq = pACodecContext->sample_rate;//采样频率 DSP frequency -- samples per second
		wanted_spec.format = AUDIO_S16SYS;//采样格式 Audio data format
		wanted_spec.channels = pACodecContext->channels;//声道数 Number of channels: 1 mono, 2 stereo
		wanted_spec.silence = 0;//无输出时是否静音
		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;//默认每次读音频缓存的大小，推荐值为 512~8192，ffplay使用的是1024 specifies a unit of audio data refers to the size of the audio buffer in sample frames
		//wanted_spec.samples = pACodecContext->frame_size;

		wanted_spec.callback = nullptr;//设置取音频数据的回调接口函数 the function to call when the audio device needs more data
		wanted_spec.userdata = (void*)this;//传递用户数据

		Stat_Quit = 0;
		Stat_QuitComplete = 0;

		deviceID = SDL_OpenAudioDevice(nullptr, 0, &wanted_spec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
		if (deviceID <= 0) {
			auto error = SDL_GetError();

			throw SDL_EXCEPTION_AUDIO;
		}

		SDL_PauseAudioDevice(deviceID, 0);
	}

	inline int64_t get_protectedFrame(size_t ms) {
		auto protectedTimeInMs = min(totalTimeInMs, max(0, ms));
		auto protectedTime = protectedTimeInMs / 1000;
		auto protectedFrame = protectedTime / decimalRational;

		return (int64_t)(protectedFrame);
	}

	inline int decode_frame(bool bVideoOnly, rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		while (av_read_frame(pFormatContext, pVPacket) >= 0) {
			// if it's the video stream
			if (pVPacket->stream_index == video_stream_index) {
				response = decode_videoFrame(callBack);

				if (response < 0) { break; }
				if (--how_many_packets_to_process <= 0) { break; }
			}

			if (pVPacket->stream_index == audio_stream_index) {
				if (!bVideoOnly) {
					response = decode_audioFrame();
				}

				if (response < 0) { break; }
			}

			av_packet_unref(pVPacket);
		}
		
		return 0;
	}

	inline int decode_vpacket(AVPacket* pVPacket, AVCodecContext* pVCodecContext, AVFrame* pFrame, rawDataCallBack callBack) {
		// Supply raw packet data as input to a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
		int response = avcodec_send_packet(pVCodecContext, pVPacket);

		if (response < 0) {
			auto e1 = AVERROR(EAGAIN);
			auto e2 = AVERROR_EOF;
			auto e3 = AVERROR(EINVAL);
			auto e4 = AVERROR(ENOMEM);

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
	inline int decode_apacket() {
		int response = avcodec_send_packet(pACodecContext, pVPacket);
		if (response < 0 && response != AVERROR(EAGAIN) && response != AVERROR_EOF) {
			return -1;
		}

		response = avcodec_receive_frame(pACodecContext, pAFrame);
		if (response < 0 && response != AVERROR_EOF) {
			av_frame_unref(pAFrame);

			return -1;
		}

		// 计算转换后的sample个数 a * b / c
		int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swrContext, pAFrame->sample_rate) + pAFrame->nb_samples, pAFrame->sample_rate, pAFrame->sample_rate, AVRounding(1));

		// 转换，返回值为转换后的sample个数
		int nb = swr_convert(swrContext, &audio_buf, dst_nb_samples, (const uint8_t**)pAFrame->data, pAFrame->nb_samples);

		return pAFrame->channels * nb * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
	}

public:
	//Load from file
	FFMpeg(const std::wstring& filePath) {
		init_formatContext(&pFormatContext, filePath);
		init_formatContext(&pSeekFormatContext, filePath);

		init_general();
	}

	//Load from memory
	//https://blog.csdn.net/leixiaohua1020/article/details/12980423
	FFMpeg(unsigned char* pBuffer, size_t bfSz) {
		init_formatContext(&pFormatContext, pBuffer, bfSz);
		init_formatContext(&pSeekFormatContext, pBuffer, bfSz);

		init_general();
	}

	~FFMpeg() {
		avformat_close_input(&pFormatContext);
		
		av_packet_free(&pVPacket);
		av_packet_free(&pAPacket);
		av_packet_free(&pFlushPacket);

		av_frame_free(&pVFrame);
		av_frame_free(&pAFrame);

		avcodec_close(pVCodecContext);
		avcodec_free_context(&pVCodecContext);
		avcodec_close(pACodecContext);
		avcodec_free_context(&pACodecContext);

		sws_freeContext(swsContext);
		swr_free(&swrContext);

		delete[] audio_buf;

		Stat_Quit = 1;
		Stat_QuitComplete = 0;

		SDL_CloseAudioDevice(deviceID);
	}

	inline int get_volume() {
		return int((volume / 128.0) * 100);
	}

	inline void set_volume(int volume) {
		this->volume = int((max(0, min(100, volume)) / 100.0) * 128);
	}

	inline int decode_videoFrame(rawDataCallBack callBack) {
		int response = 0;

		response = decode_vpacket(pVPacket, pVCodecContext, pVFrame, callBack);

		return response;
	}

	inline int decode_audioFrame() {
		auto audio_size = decode_apacket();

		//检查解码操作是否成功
		if (audio_size < 0) {
			// If error, output silence.
			audio_size = SDL_AUDIO_BUFFER_SIZE;
			memset(audio_buf, 0, SDL_AUDIO_BUFFER_SIZE);//全零重置缓冲区
		}
		
		SDL_QueueAudio(deviceID, audio_buf, audio_size);

		return 0;		
	}

	inline int64_t get_timePerFrame() {
		return (int64_t)(timePerFrameInMs);
	}

	//inline int set_videoPosition(size_t ms = 0, bool bInit = false) {
	inline int set_videoPosition(size_t ms = 0, bool bInit = true) {
		if (!(av_seek_frame(pFormatContext, video_stream_index
			, get_protectedFrame(ms)
			, seekFlags) >= 0)) {
			return -1;
		}

		return 0;
	}

	//TODO 不影响主Context
	inline int get_videoFrame(size_t ms, rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

//#define _SEPARATE_SEEK

#ifndef _SEPARATE_SEEK
		response = set_videoPosition(ms);

		response = decode_frame(true, callBack);
#else
		AVCodecContext* pCodecContext = avcodec_alloc_context3(pVCodec);
		if (!pCodecContext) {
			return -1;
		}

		if (avcodec_parameters_to_context(pCodecContext, pVCodecParameters) < 0) {
			return -1;
		}

		if (avcodec_open2(pCodecContext, pVCodec, NULL) < 0) {
			return -1;
		}

		AVPacket* pPacket = av_packet_alloc();
		if (!pPacket) {
			return -1;
		}

		AVFrame* pFrame = av_frame_alloc();
		if (!pFrame) {
			return -1;
		}

		if (!(av_seek_frame(pSeekFormatContext, video_stream_index
			, get_protectedFrame(ms)
			, seekFlags) >= 0)) {
			return -1;
		}

		while (av_read_frame(pSeekFormatContext, pPacket) >= 0) {
			// if it's the video stream
			if (pPacket->stream_index == video_stream_index) {
				response = decode_vpacket(pPacket, pCodecContext, pFrame, callBack);

				if (response < 0) { break; }

				// stop it, otherwise we'll be saving hundreds of frames
				if (--how_many_packets_to_process <= 0) { break; }
			}

			av_packet_unref(pPacket);
		}

		avcodec_free_context(&pCodecContext);
		av_packet_free(&pPacket);
		av_frame_free(&pFrame);
#endif

		return response;
	}

	inline int get_nextFrame(rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = decode_frame(false, callBack);

		if (bLoop && (int64_t)(pVCodecContext->frame_number + 1) == totalFrame) {
			auto pause = 1;
		}

		//globalPts;

		//if (bLoop && (int64_t)(pVCodecContext->frame_number + 1) == totalFrame) {
		//	response = set_videoPosition();

		//	if (response < 0) { return response; }

		//	pVCodecContext->frame_number = 0;
		//	pACodecContext->frame_number = 0;
		//}

		return response;
	}
};

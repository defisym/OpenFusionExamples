// Ref: http://dranger.com/ffmpeg/ffmpeg.html
// Ref: https://github.com/leandromoreira/ffmpeg-libav-tutorial
// SafeSEH:NO

#pragma once

#pragma warning(disable : 4819)
#pragma warning(disable : 4996)

//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"swscale.lib")

#include <string>
#include <format>
#include <functional>
#include <inttypes.h>

#include "MemBuf.h"
#include "PacketQueue.h"
#include "WindowsCommon.h"
#include "GeneralDefinition.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>

#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}

extern "C" {
#include <SDL.h>
#include <SDL_thread.h>
}

// SDL
constexpr auto SDL_AUDIO_BUFFER_SIZE = 1024;
constexpr auto MAX_AUDIO_FRAME_SIZE = 192000;

constexpr auto AUDIO_BUFFER_SIZE = (MAX_AUDIO_FRAME_SIZE * 3) / 2;

constexpr auto MAX_AUDIOQ_SIZE = (5 * 16 * 1024);
constexpr auto MAX_VIDEOQ_SIZE = (5 * 256 * 1024);

constexpr auto PROBE_SIZE = (32 * 4096);

// if defined, then this class won't call SDL_OpenAudio, and convert data by the following spec
// for the sake of performance (SDL_OpenAudio need about 20ms on my PC)
// 
// Spec:
//		wanted_spec.freq = TARGET_SAMPLE_RATE;
//		wanted_spec.format = AUDIO_S16SYS;
//		wanted_spec.channels = 2;
//		//sclient if no output
//		wanted_spec.silence = 0;
//		//specifies a unit of audio data refers to the size of the audio buffer in sample frames
//		//recommand: 512~8192, ffplay: 1024
//		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;

#define _EXTERNAL_SDL_AUDIO_INIT

// Channel Layout https://www.cnblogs.com/wangguchangqing/p/5851490.html
constexpr auto TARGET_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
constexpr auto TARGET_SAMPLE_FORMAT = AV_SAMPLE_FMT_S16;
constexpr auto TARGET_SAMPLE_RATE = 48000;

constexpr auto DEFAULT_ATEMPO = 1.0f;

constexpr auto AV_SYNC_THRESHOLD = 0.01;
constexpr auto AV_NOSYNC_THRESHOLD = 10.0;

constexpr auto SDL_EXCEPTION_INIT = 0;
constexpr auto SDL_EXCEPTION_AUDIO = 1;

static volatile int Stat_Quit = 0;
static volatile int Stat_QuitComplete = 1;

// Exceptions & error code
constexpr auto FFMpegException_InitFailed = -1;
constexpr auto FFMpegException_HWInitFailed = -2;

constexpr auto FFMpegException_HWDecodeFailed = -3;

constexpr auto FFMpegException_FilterInitFailed = -4;

constexpr auto END_OF_QUEUE = -1;

// Defines
constexpr auto seekFlags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;
//constexpr auto seekFlags = AVSEEK_FLAG_BYTE | AVSEEK_FLAG_FRAME;
//constexpr auto seekFlags = AVSEEK_FLAG_ANY | AVSEEK_FLAG_FRAME;
//constexpr auto seekFlags = AVSEEK_FLAG_FRAME;

#define _VIDEO_ALPHA

#ifdef _VIDEO_ALPHA
// Has alpha
constexpr auto PIXEL_FORMAT = AV_PIX_FMT_BGRA;
constexpr auto PIXEL_BYTE = 4;
#else
// No alpha
constexpr auto PIXEL_FORMAT = AV_PIX_FMT_BGR24;
constexpr auto PIXEL_BYTE = 3;
#endif

#define _HW_DECODE

#ifdef _HW_DECODE
static enum AVPixelFormat hw_pix_fmt_global = AV_PIX_FMT_NONE;
#endif

constexpr auto FFMpegFlag_Default = 0;

constexpr auto FFMpegFlag_HWDeviceMask = 0xFFFF;
constexpr auto FFMpegFlag_Flag2_Fast = (0x0001) << 16;

constexpr AVRational time_base_q = { 1, AV_TIME_BASE };

// pData, stride, height
using rawDataCallBack = std::function<void(const unsigned char*, const int, const int)>;

using Uint8 = unsigned char;

class FFMpeg {
private:
#pragma region value
#pragma region control
	bool bExit = false;

	bool bLoop = false;
	bool bPause = false;	// used to sclient autio

	bool bReadFinish = false;
	bool bVideoFinish = false;
	bool bAudioFinish = false;

	bool bFinish = false;

#ifdef _CONSOLE
	bool bJumped = false;
#endif

	int volume = SDL_MIX_MAXVOLUME;

	double videoPts = 0;
	double audioPts = 0;

	//int64_t globalPts = 0;

	double audioClock = 0;
	double videoClock = 0;

	double frameTimer = -1;
	double frameLastPts = 0;
	double frameLastDelay = 40e-3;

	enum class SyncState {
		SYNC_VIDEOFASTER,
		SYNC_CLOCKFASTER,
		SYNC_SYNC,
	};

	bool bSeeking = false;
	double seekingTargetPts = 0.0;

#pragma endregion

#pragma region FFMpeg
	DWORD flag = FFMpegFlag_Default;

	bool bFromMem = false;
	bool bNoAudio = false;

	AVIOContext* pAvioContext = nullptr;
	AVIOContext* pSeekAvioContext = nullptr;

	MemBuf* pMemBuf = nullptr;
	MemBuf* pSeekMemBuf = nullptr;

	AVFormatContext* pFormatContext = nullptr;
	AVFormatContext* pSeekFormatContext = nullptr;

#ifdef  _HW_DECODE
	// fall back to CPU decode
	bool bHWFallback = false;

	AVBufferRef* hw_device_ctx = nullptr;
	//AVHWDeviceType hw_type = AV_HWDEVICE_TYPE_DXVA2;
	AVHWDeviceType hw_type = AV_HWDEVICE_TYPE_D3D11VA;
	AVPixelFormat hw_pix_fmt= AV_PIX_FMT_NONE;
#endif //  _HW_DECODE

	AVFilterGraph* filter_graph = nullptr;
	const char* filters_descr = "";

	AVFilterContext* buffersrc_ctx = nullptr;
	AVFilterContext* buffersink_ctx = nullptr;

	AVFrame* pAFilterFrame = nullptr;

	float atempo = DEFAULT_ATEMPO;	

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

#ifdef _HW_DECODE
	bool bHWDecode = false;

	AVFrame* pSWFrame = nullptr;
#endif // _HW_DECODE

	//AVFrame vFrame = { 0 };
	//AVFrame aFrame = { 0 };

	//AVFrame* pVFrame = &vFrame;
	//AVFrame* pAFrame = &aFrame;

	AVPacket* pPacket = nullptr;
	//AVPacket* pFlushPacket = nullptr;
	AVPacket flushPacket;

	packetQueue audioQueue;
	packetQueue videoQueue;

	int audioQSize = MAX_AUDIOQ_SIZE;
	int videoQSize = MAX_VIDEOQ_SIZE;

	AVPacket* pVPacket = nullptr;
	AVPacket* pAPacket = nullptr;

	//AVPacket vPacket = { 0 };
	//AVPacket aPacket = { 0 };

	//AVPacket* pVPacket = &vPacket;
	//AVPacket* pAPacket = &aPacket;

	SwsContext* swsContext = nullptr;

	bool bUpdateSwr = true;
	SwrContext* swrContext = nullptr;

	AVStream* pVideoStream = nullptr;
	AVStream* pAudioStream = nullptr;

	AVDictionary* pVOptionsDict = nullptr;
	AVDictionary* pAOptionsDict = nullptr;

	AVRational rational = { 0 };
	double decimalRational = 0;

	int64_t totalDuration = 0;
	//double timePerFrameInMs = 0;

	double totalTime = 0;
	int64_t totalTimeInMs = 0;

	uint8_t* p_global_bgr_buffer = nullptr;
	int num_bytes = 0;

	//double firstKeyFrame = -1;

#pragma endregion

#pragma region SDL
	//保存解码一个packet后的多帧原始音频数据
	uint8_t* audio_buf = nullptr;
	//解码后的多帧音频数据长度
	unsigned int audio_buf_size = 0;
	//累计写入stream的长度
	unsigned int audio_buf_index = 0;

	SDL_AudioSpec spec = { 0 };
	SDL_AudioSpec wanted_spec = { 0 };

	SDL_mutex* mutex;
	SDL_cond* cond;
#pragma endregion

#pragma endregion

#ifdef _HW_DECODE
	inline std::vector<AVHWDeviceType> HW_GetDeviceType() {
		auto type = AV_HWDEVICE_TYPE_NONE;

		std::vector<AVHWDeviceType> types;

		while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
			auto pName = av_hwdevice_get_type_name(type);

			// valid
			types.emplace_back(type);
		}

		if (types.empty()) {
			types.emplace_back(AV_HWDEVICE_TYPE_NONE);
		}

		return types;
	}

	inline AVPixelFormat HW_GetPixelFormat(const AVCodec* pCodec, AVHWDeviceType type) {
		for (int i = 0;; i++) {
			const AVCodecHWConfig* config = avcodec_get_hw_config(pCodec, i);
			if (!config) {
				// Decoder does not support device type
				return AV_PIX_FMT_NONE;
			}

			if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
				config->device_type == type) {
				return config->pix_fmt;
			}
		}
	}

	inline int HW_InitDecoder(AVCodecContext* pCodecContext, AVHWDeviceType type) {
		int err = 0;

		if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
			NULL, NULL, 0)) < 0) {
			//fprintf(stderr, "Failed to create specified HW device.\n");
			return err;
		}

		pCodecContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);

		return err;
	}
#endif

	inline std::wstring GetErrorStr(int errnum) {
		auto buf = new char[AV_ERROR_MAX_STRING_SIZE];

		av_make_error_string(buf, AV_ERROR_MAX_STRING_SIZE, errnum);

		auto result = ConvertStrToWStr(buf);

		delete[] buf;

		return result;
	}

	inline int64_t get_ChannelLayout() {
		//int index = av_get_channel_layout_channel_index(av_get_default_channel_layout(4), AV_CH_FRONT_CENTER);

		int channels = pACodecParameters->channels;
		uint64_t channel_layout = pACodecParameters->channel_layout;

		if (channels > 0 && channel_layout == 0) {
			channel_layout = av_get_default_channel_layout(channels);
		}
		else if (channels == 0 && channel_layout > 0) {
			channels = av_get_channel_layout_nb_channels(channel_layout);
		}

		auto layout = av_get_default_channel_layout(channels);

		return layout;
	}

	inline void init_SwrContext(int64_t in_ch_layout, AVSampleFormat in_sample_fmt, int in_sample_rate) {
		swrContext = swr_alloc_set_opts(swrContext
#ifndef _EXTERNAL_SDL_AUDIO_INIT
			, av_get_default_channel_layout(channels)
			, AV_SAMPLE_FMT_S16, pACodecParameters->sample_rate
#else
			, TARGET_CHANNEL_LAYOUT
			, TARGET_SAMPLE_FORMAT, TARGET_SAMPLE_RATE
#endif
			, in_ch_layout
			, in_sample_fmt, in_sample_rate
			, 0, nullptr);

		if (!swrContext || swr_init(swrContext) < 0) {
			throw FFMpegException_InitFailed;
		}

		bUpdateSwr = false;
	}

	inline int init_audioFilters(AVFormatContext* fmt_ctx, AVCodecContext* dec_ctx
		, AVFilterGraph* filter_graph, const char* filters_descr) {		
		if (this->bNoAudio) {
			return -1;
		}

		SDL_CondWait(cond, mutex);

		SDL_LockMutex(mutex);

		int ret = 0;
				
		// release
		avfilter_graph_free(&filter_graph);

		// re-alloc
		AVFilterInOut* inputs = avfilter_inout_alloc();
		AVFilterInOut* outputs = avfilter_inout_alloc();

		try {
			filter_graph = avfilter_graph_alloc();
			if (!outputs || !inputs || !filter_graph) {
				//ret = AVERROR(ENOMEM);
				throw FFMpegException_FilterInitFailed;
			}

			/* buffer audio source: the decoded frames from the decoder will be inserted here. */
			if (!dec_ctx->channel_layout) {
				dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
			}
			
			// match source
			AVRational time_base = fmt_ctx->streams[audio_stream_index]->time_base;

			auto filterArgs = std::format("time_base={}/{}:sample_rate={}:sample_fmt={}:channel_layout={:#x}"
				, time_base.num, time_base.den, dec_ctx->sample_rate,
				av_get_sample_fmt_name(dec_ctx->sample_fmt), dec_ctx->channel_layout);

			// create
			const AVFilter* abuffersrc = avfilter_get_by_name("abuffer");

			ret = avfilter_graph_create_filter(&buffersrc_ctx, abuffersrc, "in",
				filterArgs.c_str(), NULL, filter_graph);
			if (ret < 0) {
				//av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
				throw FFMpegException_FilterInitFailed;
			}

			/* buffer audio sink: to terminate the filter chain. */
			const AVFilter* abuffersink = avfilter_get_by_name("abuffersink");

			ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out",
				NULL, NULL, filter_graph);
			if (ret < 0) {
				//av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
				throw FFMpegException_FilterInitFailed;
			}

			//ret = av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
			//	AV_OPT_SEARCH_CHILDREN);
			//if (ret < 0) {
			//	//av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
			//	throw FFMpegException_FilterInitFailed;
			//}

			//ret = av_opt_set(buffersink_ctx, "ch_layouts", "mono",
			//	AV_OPT_SEARCH_CHILDREN);
			//if (ret < 0) {
			//	//av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
			//	throw FFMpegException_FilterInitFailed;
			//}

			//ret = av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates, -1,
			//	AV_OPT_SEARCH_CHILDREN);
			//if (ret < 0) {
			//	//av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
			//	throw FFMpegException_FilterInitFailed;
			//}

			/*
			 * Set the endpoints for the filter graph. The filter_graph will
			 * be linked to the graph described by filters_descr.
			 */

			 /*
			  * The buffer source output must be connected to the input pad of
			  * the first filter described by filters_descr; since the first
			  * filter input label is not specified, it is set to "in" by
			  * default.
			  */
			outputs->name = av_strdup("in");
			outputs->filter_ctx = buffersrc_ctx;
			outputs->pad_idx = 0;
			outputs->next = NULL;

			/*
			 * The buffer sink input must be connected to the output pad of
			 * the last filter described by filters_descr; since the last
			 * filter output label is not specified, it is set to "out" by
			 * default.
			 */
			inputs->name = av_strdup("out");
			inputs->filter_ctx = buffersink_ctx;
			inputs->pad_idx = 0;
			inputs->next = NULL;

			if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
				&inputs, &outputs, NULL)) < 0) {
				throw FFMpegException_FilterInitFailed;
			}
			
			if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
				throw FFMpegException_FilterInitFailed;
			}

			memset(audio_buf, 0, AUDIO_BUFFER_SIZE);
			bUpdateSwr = true;

			/* Print summary of the sink buffer
			 * Note: args buffer is reused to store channel layout string */

			//const AVFilterLink* outlink = buffersink_ctx->inputs[0];
			//av_get_channel_layout_string(args, sizeof(args), -1, outlink->channel_layout);
			//av_log(NULL, AV_LOG_INFO, "Output: srate:%dHz fmt:%s chlayout:%s\n",
			//	(int)outlink->sample_rate,
			//	(char*)av_x_if_null(av_get_sample_fmt_name((AVSampleFormat)outlink->format), "?"),
			//	args);
		}
		catch (int) {
			// init failed
			ret = FFMpegException_FilterInitFailed;
		}

		avfilter_inout_free(&inputs);
		avfilter_inout_free(&outputs);

		SDL_UnlockMutex(mutex);

		return ret;
	}

	inline const AVInputFormat* init_Probe(BYTE* pBuffer, size_t bfSz, LPCSTR pFileName) {		
		AVProbeData probeData = { 0 };
		probeData.buf = pBuffer;
		//probeData.buf_size = bfSz;
		probeData.buf_size = min(MEM_BUFFER_SIZE, bfSz);
		probeData.filename = pFileName;

		// Determine the input-format:
		while (true) {
			auto iformat = av_probe_input_format(&probeData, 1);

			if (iformat != nullptr) {
				return iformat;
			}

			if (size_t(probeData.buf_size + 1) > bfSz) {
				throw FFMpegException_InitFailed;
			}

			probeData.buf_size = min(size_t(2 * probeData.buf_size), bfSz);
		}

		return nullptr;
	}

	inline void init_formatContext(AVFormatContext** pFormatContext, const std::wstring& filePath) {
		*pFormatContext = avformat_alloc_context();
		if (!*pFormatContext) {
			throw FFMpegException_InitFailed;
		}
		
		//auto iformat = init_Probe(nullptr, 0, ConvertWStrToStr(filePath, CP_UTF8).c_str());

		// convert to UTF-8 to avoid crash in some versions
		if (avformat_open_input(pFormatContext, ConvertWStrToStr(filePath, CP_UTF8).c_str(), NULL, NULL) != 0) {
			throw FFMpegException_InitFailed;
		}
	}

	inline void init_formatContext(AVFormatContext** pFormatContext, AVIOContext** pAvioContext, MemBuf** pBuf, unsigned char* pBuffer, size_t bfSz) {
		if (pBuffer == nullptr) {
			throw FFMpegException_InitFailed;
		}

		(*pBuf) = new MemBuf(pBuffer, bfSz);

		*pAvioContext = avio_alloc_context((*pBuf)->get(), (*pBuf)->getSize(), 0, (*pBuf)
			, [](void* opaque, uint8_t* buf, int buf_size) {
				auto pMemBuf = (MemBuf*)opaque;
				return pMemBuf->read(buf, buf_size);
			}
			, NULL
			, [](void* opaque, int64_t offset, int whence) {
				auto pMemBuf = (MemBuf*)opaque;
				return pMemBuf->seek(offset, whence);
			});

		if (!*pAvioContext) {
			throw FFMpegException_InitFailed;
		}

		*pFormatContext = avformat_alloc_context();
		if (!*pFormatContext) {
			throw FFMpegException_InitFailed;
		}

		(*pFormatContext)->pb = *pAvioContext;

		//https://www.codeproject.com/Tips/489450/Creating-Custom-FFmpeg-IO-Context
		//might crash in avformat_open_input due to access violation if not set
		AVProbeData probeData = { 0 };
		probeData.buf = pBuffer;
		//probeData.buf_size = bfSz;
		probeData.buf_size = min(MEM_BUFFER_SIZE, bfSz);
		probeData.filename = "";

		// Determine the input-format:
		while(true){
			(*pFormatContext)->iformat = av_probe_input_format(&probeData, 1);

			if ((*pFormatContext)->iformat != nullptr) {
				break;
			}

			if (size_t(probeData.buf_size + 1) > bfSz) {
				throw FFMpegException_InitFailed;
			}

			probeData.buf_size = min(size_t(2 * probeData.buf_size), bfSz);
		}

		(*pFormatContext)->flags |= AVFMT_FLAG_CUSTOM_IO;

		if (avformat_open_input(pFormatContext, NULL, NULL, NULL) < 0) {
			throw FFMpegException_InitFailed;
		}
	}

	inline void init_general() {
#pragma region FFMpegInit
		// find stream
		pFormatContext->probesize = PROBE_SIZE;

		if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
			throw FFMpegException_InitFailed;
		}

//#define _GET_STREAM_BYLOOP

#ifdef _GET_STREAM_BYLOOP
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
#endif

#ifndef _GET_STREAM_BYLOOP
		// init video codec
		video_stream_index = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pVCodec, 0);
#endif
		
		if (video_stream_index < 0) {
			throw FFMpegException_InitFailed;
		}

		pVideoStream = pFormatContext->streams[video_stream_index];
		pVCodecParameters = pFormatContext->streams[video_stream_index]->codecpar;

#ifdef _HW_DECODE
		auto hw_deviceType = (AVHWDeviceType)(flag & FFMpegFlag_HWDeviceMask);

		bHWDecode = hw_deviceType != AV_HWDEVICE_TYPE_NONE;

		if (bHWDecode) {
			hw_type = hw_deviceType;
			hw_pix_fmt = HW_GetPixelFormat(pVCodec, hw_type);

			if (hw_pix_fmt == AV_PIX_FMT_NONE) {
				auto hw_types = HW_GetDeviceType();

				for (auto& type : hw_types) {
					hw_type = type;
					hw_pix_fmt = HW_GetPixelFormat(pVCodec, hw_type);

					if (hw_pix_fmt != AV_PIX_FMT_NONE) {
						break;
					}
				}

				if (hw_pix_fmt == AV_PIX_FMT_NONE) {
					//throw FFMpegException_HWInitFailed;
					bHWDecode = false;
				}
			}			
		}

		if (bHWDecode) {
			hw_pix_fmt_global = hw_pix_fmt;
		}
#endif

		pVCodecContext = avcodec_alloc_context3(pVCodec);
		if (!pVCodecContext) {
			throw FFMpegException_InitFailed;
		}

		if (avcodec_parameters_to_context(pVCodecContext, pVCodecParameters) < 0) {
			throw FFMpegException_InitFailed;
		}

		pVCodecContext->thread_count = std::thread::hardware_concurrency();

		if (flag & FFMpegFlag_Flag2_Fast) {
			pVCodecContext->flags2 |= AV_CODEC_FLAG2_FAST;
		}

#ifdef _HW_DECODE
		if (bHWDecode) {
			//pVCodecContext->extra_hw_frames = 4;
			pVCodecContext->get_format = [](AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts)->AVPixelFormat {
				const enum AVPixelFormat* p;

				for (p = pix_fmts; *p != -1; p++) {
					if (*p == hw_pix_fmt_global) {
						return *p;
					}
				}

				//fprintf(stderr, "Failed to get HW surface format.\n");

				return AV_PIX_FMT_NONE;
			};

			if (HW_InitDecoder(pVCodecContext, hw_type) < 0) {
				//throw FFMpegException_HWInitFailed;
				bHWDecode = false;
			}
		}
#endif

		if (avcodec_open2(pVCodecContext, pVCodec, NULL) < 0) {
			throw FFMpegException_InitFailed;
		}

		if (pVCodecContext->pix_fmt == AV_PIX_FMT_NONE) {
			throw FFMpegException_InitFailed;
		}

		//swsContext = sws_getContext(pVCodecContext->width, pVCodecContext->height
		//	, pVCodecContext->pix_fmt,
		//	pVCodecContext->width, pVCodecContext->height
		//	, PIXEL_FORMAT
		//	, NULL, NULL, NULL, NULL);
		
#ifndef _GET_STREAM_BYLOOP
		// init audio codec
		audio_stream_index = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &pACodec, 0);
#endif

		if (audio_stream_index < 0) {
			//throw FFMpegException_InitFailed;
			bNoAudio = true;
		}

		if (!bNoAudio) {
			pAudioStream = pFormatContext->streams [audio_stream_index];
			pACodecParameters = pFormatContext->streams [audio_stream_index]->codecpar;

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

		pAFilterFrame = av_frame_alloc();
		if (!pAFilterFrame) {
			throw FFMpegException_InitFailed;
		}

#ifdef _HW_DECODE
		if (bHWDecode) {
			pSWFrame = av_frame_alloc();
			if (!pSWFrame) {
				throw FFMpegException_InitFailed;
			}
		}
#endif // _HW_DECODE

		pPacket = av_packet_alloc();
		if (!pPacket) {
			throw FFMpegException_InitFailed;
		}

		//pFlushPacket = av_packet_alloc();
		//if (!pFlushPacket) {
		//	throw FFMpegException_InitFailed;
		//}

		//pFlushPacket->data = (unsigned char*)"FLUSH";

		av_init_packet(&flushPacket);
		flushPacket.data = (unsigned char*)"FLUSH";

		pVPacket = av_packet_alloc();
		if (!pVPacket) {
			throw FFMpegException_InitFailed;
		}

		pAPacket = av_packet_alloc();
		if (!pAPacket) {
			throw FFMpegException_InitFailed;
		}

		//pVideoStream = pFormatContext->streams [video_stream_index];

		// https://www.appsloveworld.com/cplus/100/107/finding-duration-number-of-frames-of-webm-using-ffmpeg-libavformat
		if (pVideoStream->duration != AV_NOPTS_VALUE) {
			totalDuration = pVideoStream->duration;

			rational = pVideoStream->time_base;
		}
		else if (pFormatContext->duration != AV_NOPTS_VALUE) {
			totalDuration = pFormatContext->duration;

			rational = time_base_q;
		}
		else {
			totalDuration = INT64_MAX;

			rational = { 1,1000 };
		}

		decimalRational = (double)rational.num / rational.den;

		//timePerFrameInMs = decimalRational * 1000;
		totalTime = totalDuration * decimalRational;

		//totalTimeInMs = double(pVideoStream->duration)* av_q2d(pVideoStream->time_base) * 1000;
		totalTimeInMs = totalDuration == INT64_MAX
						? totalDuration
						:(int64_t)totalTime * 1000;

		//int num_bytes = av_image_get_buffer_size(PIXEL_FORMAT, this->get_width(), this->get_width(), 1);
		//p_global_bgr_buffer = new uint8_t[num_bytes];
#pragma endregion

#pragma region SDLInit
		mutex = SDL_CreateMutex();
		cond = SDL_CreateCond();

		if (!bNoAudio) {
			// init SDL audio
			audio_buf = new uint8_t [AUDIO_BUFFER_SIZE];
			memset(audio_buf, 0, AUDIO_BUFFER_SIZE);

#ifndef _EXTERNAL_SDL_AUDIO_INIT
			//DSP frequency -- samples per second
			wanted_spec.freq = pACodecContext->sample_rate;
			wanted_spec.format = AUDIO_S16SYS;
			wanted_spec.channels = pACodecContext->channels;
			//sclient if no output
			wanted_spec.silence = 0;
			//specifies a unit of audio data refers to the size of the audio buffer in sample frames
			//recommand: 512~8192，ffplay: 1024
			wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
			//wanted_spec.samples = pACodecContext->frame_size;

			wanted_spec.userdata = (void*)this;
			wanted_spec.callback = [] (void* userdata, Uint8* stream, int len) {
				FFMpeg* pFFMpeg = (FFMpeg*)userdata;

				//#define _TESTDATA	// Enable this macro to disable callback, return mute directly
#ifndef _TESTDATA
				pFFMpeg->audio_fillData(stream, len);
#else
				SDL_memset(stream, 0, len);
#endif
			};

			if (SDL_OpenAudio(&wanted_spec, nullptr) < 0) {
				auto error = SDL_GetError();

				throw SDL_EXCEPTION_AUDIO;
			}
#endif // _EXTERNAL_SDL_AUDIO_INIT	

			SDL_PauseAudio(false);
		}
#pragma endregion
	
		// must be here, as it requires mutex of SDL
		if (!bNoAudio) {
			set_audioTempo(DEFAULT_ATEMPO);
		}
}

	inline int64_t get_protectedTimeInSecond(int64_t ms) {
		auto protectedTimeInMs = min(totalTimeInMs, max(0, ms));
		auto protectedTimeInSecond = protectedTimeInMs / 1000;

		return protectedTimeInSecond;
	}

	inline int64_t get_protectedTimeStamp(int64_t ms) {
		//av_rescale_q(int64_t(ms * 1000.0), time_base_q, pFormatContext->streams[stream_index]->time_base);
		auto protectedTimeStamp = get_protectedTimeInSecond(ms) / av_q2d(pVideoStream->time_base);

		return (int64_t)(protectedTimeStamp);
	}

	inline int seekFrame(AVFormatContext* pFormatContext, int stream_index, int64_t ms = 0, int flags = seekFlags) {
		int response = 0;

		// input second
		auto seek_target = (flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE
			? get_protectedTimeStamp(ms)
			: ms;

		response = flags >= 0
			? av_seek_frame(pFormatContext, stream_index
				, seek_target
				, flags)
			: avformat_seek_file(pFormatContext, stream_index
				, seek_target, seek_target, seek_target
				, -1 * flags);

		//response = avformat_seek_file(pFormatContext, stream_index
		//	, seek_target, seek_target, seek_target
		//	, seekFlags);

		//https://stackoverflow.com/questions/45526098/repeating-ffmpeg-stream-libavcodec-libavformat
		//avio_seek(pFormatContext->pb, 0, SEEK_SET);

		return response;
	}

	inline int forwardFrame(AVFormatContext* pFormatContext, AVCodecContext* pCodecContext
		, double targetPts, rawDataCallBack callBack) {
		int response = 0;

		AVPacket* pPacket = av_packet_alloc();
		if (!pPacket) {
			return -1;
		}

		AVFrame* pFrame = av_frame_alloc();
		if (!pFrame) {
			return -1;
		}

		int loaclStride = 0;
		int loaclHeight = 0;

		this->bSeeking = true;
		this->seekingTargetPts = targetPts;

		//int count = 0;

		while (av_read_frame(pFormatContext, pPacket) >= 0) {
			// if it's the video stream
			if (pPacket->stream_index == video_stream_index) {
				// decode
				response = decode_vpacket(pPacket, pCodecContext, pFrame
					, [&](const unsigned char* pData, const int stride, const int height) {
						loaclStride = stride;
						loaclHeight = height;
					});

				// wait until receive enough packets
				if (response == AVERROR(EAGAIN)) {
					continue;
				}

				if (response < 0) { break; }

				// goto target
				if (videoClock >= targetPts) { break; }

				//count++;
			}

			av_packet_unref(pPacket);
		}

		this->bSeeking = false;

		av_frame_free(&pFrame);
		av_packet_free(&pPacket);

		callBack(p_global_bgr_buffer, loaclStride, loaclHeight);

		return response;
	}

	//Decode
	inline int fill_queue() {
		int response = 0;

		while (true) {
			if (videoQueue.getDataSize() > videoQSize || audioQueue.getDataSize() > audioQSize) {
				break;
			}

			response = av_read_frame(pFormatContext, pPacket);

			bReadFinish = (response == AVERROR_EOF);

			if (bReadFinish) {
				videoQueue.stopBlock();
				audioQueue.stopBlock();
			}

			if (response < 0) {
				break;
			}

			bool bValid = false;

			// if it's the video stream
			if (pPacket->stream_index == video_stream_index) {
				videoQueue.put(pPacket);
				bValid = true;
			}

			// if it's the audio stream
			if (pPacket->stream_index == audio_stream_index) {
				audioQueue.put(pPacket);
				bValid = true;
			}

			if (!bValid) {
				av_packet_unref(pPacket);
			}
		}

		return response;
	}

	inline int decode_frame(rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		SyncState syncState = SyncState::SYNC_SYNC;

		int loaclStride = 0;
		int loaclHeight = 0;

		do {
			// fill buffer
			response = fill_queue();

			// calc delay
			syncState = get_delayState();

			// decode
			switch (syncState) {
				// video is faster, wait
			case FFMpeg::SyncState::SYNC_VIDEOFASTER:
				return 0;

				break;
				// decode new video frame
			case FFMpeg::SyncState::SYNC_CLOCKFASTER:
			case FFMpeg::SyncState::SYNC_SYNC:
				//response = decode_videoFrame(callBack);
				response = decode_videoFrame([&](const unsigned char* pData, const int stride, const int height) {
					loaclStride = stride;
					loaclHeight = height;
					});

				if (response == AVERROR(EAGAIN)) {
					syncState = SyncState::SYNC_CLOCKFASTER;

					continue;
				}

				if (response < 0) {
					//auto e1 = AVERROR_EOF;
					//auto e2 = AVERROR(EINVAL);
					//auto e3 = AVERROR(ENOMEM);

					return response;
				}

				break;
			}

		} while (syncState != SyncState::SYNC_SYNC);

		//callBack(p_global_bgr_buffer, this->get_width(), this->get_height());
		callBack(p_global_bgr_buffer, loaclStride, loaclHeight);

		return 0;
	}

	// Drain
	// https://ffmpeg.org/doxygen/5.0/group__lavc__encdec.html
	// End of stream situations. These require "flushing" (aka draining) the codec, as the codec might buffer multiple frames or packets internally for performance or out of necessity (consider B-frames). This is handled as follows:
	// - Instead of valid input, send NULL to the avcodec_send_packet() (decoding) or avcodec_send_frame() (encoding) functions. This will enter draining mode.
	// - Call avcodec_receive_frame() (decoding) or avcodec_receive_packet() (encoding) in a loop until AVERROR_EOF is returned. The functions will not return AVERROR(EAGAIN), unless you forgot to enter draining mode.
	// - Before decoding can be resumed again, the codec has to be reset with avcodec_flush_buffers().
	inline int decode_videoFrame(rawDataCallBack callBack) {
		auto bNoPacket = !videoQueue.get(pVPacket, false);

		//if (pVPacket->data == flushPacket.data) {
		//	avcodec_flush_buffers(pVCodecContext);

		//	return 0;
		//}

		//auto response = decode_vpacket(pVPacket, pVCodecContext, pVFrame, callBack);
		auto response = decode_vpacket(!bNoPacket ? pVPacket : nullptr, pVCodecContext, pVFrame, callBack);		

		bVideoFinish = (response == AVERROR_EOF);

		if (pVPacket && pVPacket->data) {
			av_packet_unref(pVPacket);
		}

		if (bVideoFinish) {
			return END_OF_QUEUE;
		}

		return response;
	}

	inline int decode_audioFrame() {
		//bAudioFinish = !audioQueue.get(pAPacket);
		//bAudioFinish = !audioQueue.get(pAPacket, false);
		//bAudioFinish = !audioQueue.get(pAPacket, !bReadFinish);
		auto bNoPacket = !audioQueue.get(pAPacket, !bReadFinish);
			
		//if (pAPacket->data == flushPacket.data) {
		//	avcodec_flush_buffers(pACodecContext);

		//	return 0;
		//}

		// return data size here
		auto response = decode_apacket(!bNoPacket ? pAPacket : nullptr, pACodecContext, pAFrame);

		bAudioFinish = (response == AVERROR_EOF);

		if (pAPacket && pAPacket->data) {
			av_packet_unref(pAPacket);
		}

		if (bAudioFinish) {
			return END_OF_QUEUE;
		}

		return response;
	}

	// Convert data to Fusion data
	// https://zhuanlan.zhihu.com/p/53305541
	inline void convertData(AVFrame* pFrame, rawDataCallBack callBack) {
		// allocate buffer
		if (p_global_bgr_buffer == nullptr) {
			num_bytes = av_image_get_buffer_size(PIXEL_FORMAT, pFrame->linesize[0], pFrame->height, 1);
			p_global_bgr_buffer = new uint8_t[num_bytes];
		}

		// allocate sws
		if (swsContext == nullptr) {
			//auto pFormat = pFrame->format;

			swsContext = sws_getContext(pFrame->width, pFrame->height
				, (AVPixelFormat)pFrame->format,
				pFrame->width, pFrame->height
				, PIXEL_FORMAT
				, NULL, NULL, NULL, NULL);
		}

		// make sure the sws_scale output is point to start.
		int linesize [8] = { abs(pFrame->linesize [0] * PIXEL_BYTE) };

		uint8_t* bgr_buffer[8] = { p_global_bgr_buffer };

		sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, bgr_buffer, linesize);

		//bgr_buffer[0] is the BGR raw data
		callBack(bgr_buffer[0], linesize[0], pFrame->height);
	}

	inline int decode_vpacket(AVPacket* pVPacket, AVCodecContext* pVCodecContext, AVFrame* pFrame, rawDataCallBack callBack) {
		// Supply raw packet data as input to a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
		int response = avcodec_send_packet(pVCodecContext, pVPacket);

		if (response < 0) {
			return response;
		}

#define _NOLOOP

#ifndef _NOLOOP
		while (response >= 0) {
#endif // !_NOLOOP
			// Return decoded output data (into a frame) from a decoder
			// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
			response = avcodec_receive_frame(pVCodecContext, pFrame);

			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
#ifdef _NOLOOP
				return response;
#endif
#ifndef _NOLOOP
				break;
#endif // !_NOLOOP
			}
			else if (response < 0) {
				av_frame_unref(pFrame);
#ifdef _HW_DECODE
				if (bHWDecode) {
					av_frame_unref(pSWFrame);
				}
#endif // _HW_DECODE

				return response;
			}

			if (response >= 0) {
				videoPts = 0;

				if (pVPacket != nullptr) {
					if (pVPacket->dts == AV_NOPTS_VALUE
						&& pFrame->opaque
						&& *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE) {
						videoPts = double(*(uint64_t*)pFrame->opaque);
					}
					else {
						videoPts = double(pVPacket->dts != AV_NOPTS_VALUE
							? pFrame->best_effort_timestamp
							: 0);
					}
				}

				videoPts *= av_q2d(pVideoStream->time_base);

				//if (pFrame->key_frame == 1) {
				//	firstKeyFrame = min(videoPts, firstKeyFrame);
				//}

#ifdef _CONSOLE
				if (bJumped) {
					printf("Cur video pts: %f\n", videoPts);
				}
#endif
				videoPts = synchronize_video(pFrame, videoPts);

#ifdef _CONSOLE
				if (bJumped) {
					printf("Cur synced video pts: %f\n", videoPts);
				}
#endif

				if (!bSeeking
					|| (bSeeking && (videoClock >= seekingTargetPts))) {
					auto pFrameToConvert = pFrame;

#ifdef _HW_DECODE
					if (bHWDecode) {
						if (pFrame->format == hw_pix_fmt) {
							/* retrieve data from GPU to CPU */
							if ((response = av_hwframe_transfer_data(pSWFrame, pFrame, 0)) < 0) {
								//fprintf(stderr, "Error transferring the data to system memory\n");
								throw FFMpegException_HWDecodeFailed;
							}

							pFrameToConvert = pSWFrame;
						}
					}
#endif

					convertData(pFrameToConvert, callBack);
				}				

				av_frame_unref(pFrame);
			}
#ifndef _NOLOOP
		}
#endif // !_NOLOOP


		return response;
	}

	//https://github.com/brookicv/FFMPEG-study/blob/master/FFmpeg-playAudio.cpp
	inline int decode_apacket(AVPacket* pAPacket, AVCodecContext* pACodecContext, AVFrame* pArame) {
		int response = avcodec_send_packet(pACodecContext, pAPacket);
		if (response < 0 && response != AVERROR(EAGAIN) && response != AVERROR_EOF) {
			return -1;
		}

		response = avcodec_receive_frame(pACodecContext, pAFrame);
		if (response < 0 && response != AVERROR_EOF) {
			av_frame_unref(pAFrame);

			return -1;
		}

		response = av_buffersrc_add_frame_flags(buffersrc_ctx, pAFrame, AV_BUFFERSRC_FLAG_KEEP_REF);
		//response = av_buffersrc_add_frame_flags(buffersrc_ctx, pAFrame, 0);
		if (response < 0) {
			return -1;
		}

		while (1) {
			//response = av_buffersink_get_frame(buffersink_ctx, pAFrame);
			response = av_buffersink_get_frame(buffersink_ctx, pAFilterFrame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				break;
			}

			if (response < 0) {
				av_frame_unref(pAFrame);
				av_frame_unref(pAFilterFrame);
			}

			if (pAPacket != nullptr) {
				if (pAPacket->pts != AV_NOPTS_VALUE) {
					audioClock = av_q2d(pAudioStream->time_base) * pAPacket->pts;
#ifdef _CONSOLE
					if (bJumped) {
						printf("Cur audio clock: %f\n", audioClock);
					}
#endif
				}
			}

			//auto pBaseFrame = pAFrame;
			auto pBaseFrame = pAFilterFrame;

			// update context to avoid crash
			if (bUpdateSwr) {
				init_SwrContext(pBaseFrame->channel_layout
					, (AVSampleFormat)pBaseFrame->format
					, pBaseFrame->sample_rate);
			}

			// 计算转换后的sample个数 a * b / c
			// https://blog.csdn.net/u013346305/article/details/48682935
			int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swrContext, pBaseFrame->sample_rate)
								+ pBaseFrame->nb_samples
#ifndef _EXTERNAL_SDL_AUDIO_INIT
				, pBaseFrame->sample_rate, pBaseFrame->sample_rate, AVRounding(1));
#else
				, TARGET_SAMPLE_RATE, pBaseFrame->sample_rate, AVRounding(1));
#endif

			// 转换，返回值为转换后的sample个数
			int nb = swr_convert(swrContext, &audio_buf, dst_nb_samples
				, (const uint8_t**)pBaseFrame->data, pBaseFrame->nb_samples);

			auto audioSize = pBaseFrame->channels * nb * av_get_bytes_per_sample(TARGET_SAMPLE_FORMAT);

			audioPts = audioClock;

			auto pcm_bytes = 2 * pBaseFrame->channels;
			audioClock += (double)audioSize / (double)(pcm_bytes * pACodecContext->sample_rate);

			return audioSize;
		}

		av_frame_unref(pAFrame);

		return -1;
	}

	//synchronize
	inline double synchronize_video(AVFrame* pVFrame, double videoPts) {
		double frameDelay = 0;

		if (videoPts != 0) {
			videoClock = videoPts;
		}
		else {
			videoPts = videoClock;
		}

		frameDelay = av_q2d(pVideoStream->time_base);
		frameDelay += pVFrame->repeat_pict * (frameDelay * 0.5);

		videoClock += frameDelay;

		return videoPts;
	}

	inline double get_audioClock() {
		double pts = audioClock; /* maintained in the audio thread */

		int hw_buf_size = audio_buf_size - audio_buf_index;
		int  bytes_per_sec = 0;

		if (pAudioStream) {
			bytes_per_sec = pACodecContext->sample_rate * pACodecContext->channels * 2;
		}

		if (bytes_per_sec) {
			pts -= (double)hw_buf_size / bytes_per_sec;
		}

		return pts;
	}

	inline SyncState get_delayState() {
		//if (bFinish) {
		//	return SyncState::SYNC_SYNC;
		//}

		auto delay = videoPts - frameLastPts;

		if (delay <= 0 || delay >= 1) {
			delay = frameLastDelay;
		}

		frameLastPts = videoPts;
		frameLastDelay = delay;

		auto curTime = av_gettime() / 1000000.0;

		// reset frameTimer with a extimate time
		if (frameTimer == -1) {
			frameTimer = curTime - videoPts;
		}

		auto syncClock = !bNoAudio ? get_audioClock() : curTime - frameTimer;
		auto diff = videoPts - syncClock;

		auto syncThreshold = delay > AV_SYNC_THRESHOLD
			? delay
			: AV_SYNC_THRESHOLD;

		if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
			// audio is faster
			if (diff <= -syncThreshold) {
				return SyncState::SYNC_CLOCKFASTER;
			}
			// video is faster
			else if (diff >= syncThreshold) {
				return SyncState::SYNC_VIDEOFASTER;
			}
			else {
				return SyncState::SYNC_SYNC;
			}
		}

		return SyncState::SYNC_SYNC;
	}

	inline void reset_sync() {
		videoPts = 0;
		audioPts = 0;

		//globalPts = 0;

		videoClock = 0;
		audioClock = 0;

		frameTimer = -1;
		frameLastPts = 0;
		frameLastDelay = 40e-3;
	}

	inline void reset_finishState() {
		bReadFinish = false;
		bAudioFinish = false;
		bVideoFinish = false;
	}

	inline double get_ptsMS(AVPacket* pPacket, AVStream* pStream) {
		return pPacket->pts * av_q2d(pStream->time_base) * 1000;
	}

	inline double get_dtsMS(AVPacket* pPacket, AVStream* pStream) {
		return pPacket->dts * av_q2d(pStream->time_base) * 1000;
	}

	inline int handleLoop() {
		int response = 0;

		if (bLoop && bFinish) {
			response = set_videoPosition();
		}

		return response;
	}
public:
	//Load from file
	FFMpeg(const std::wstring& filePath, DWORD flag = FFMpegFlag_Default) {
		bFromMem = false;
		this->flag = flag;

		init_formatContext(&pFormatContext, filePath);
		init_formatContext(&pSeekFormatContext, filePath);

		init_general();
	}

	//Load from memory
	//https://blog.csdn.net/leixiaohua1020/article/details/12980423
	FFMpeg(unsigned char* pBuffer, size_t bfSz, DWORD flag = FFMpegFlag_Default) {
		bFromMem = true;
		this->flag = flag;

		init_formatContext(&pFormatContext, &pAvioContext, &pMemBuf, pBuffer, bfSz);
		init_formatContext(&pSeekFormatContext, &pSeekAvioContext, &pSeekMemBuf, pBuffer, bfSz);

		init_general();
	}

	~FFMpeg() {
		bExit = true;

		audioQueue.exit();
		videoQueue.exit();

		audioQueue.flush();
		videoQueue.flush();

		SDL_PauseAudio(true);

#ifndef _EXTERNAL_SDL_AUDIO_INIT
		SDL_CloseAudio();
#endif // _EXTERNAL_SDL_AUDIO_INIT		

		//Wait for callback finish
		SDL_CondWait(cond, mutex);

		SDL_LockMutex(mutex);

		//audioQueue.flush();
		//videoQueue.flush();

		delete[] audio_buf;

		av_packet_free(&pPacket);
		//av_packet_free(&pFlushPacket);

		av_packet_free(&pVPacket);
		av_packet_free(&pAPacket);

		av_frame_free(&pVFrame);
		av_frame_free(&pAFrame);
		av_frame_free(&pAFilterFrame);

#ifdef _HW_DECODE
		if (bHWDecode) {
			av_frame_free(&pSWFrame);
		}
#endif // _HW_DECODE		

		avfilter_graph_free(&filter_graph);

		sws_freeContext(swsContext);
		swr_free(&swrContext);

		avcodec_close(pVCodecContext);
		avcodec_free_context(&pVCodecContext);
		avcodec_close(pACodecContext);
		avcodec_free_context(&pACodecContext);

		avformat_close_input(&pFormatContext);
		avformat_close_input(&pSeekFormatContext);

#ifdef _HW_DECODE
		if (bHWDecode) {
			av_buffer_unref(&hw_device_ctx);
		}
#endif // _HW_DECODE		

		if (bFromMem) {
			delete pMemBuf;
			pMemBuf = nullptr;
			delete pSeekMemBuf;
			pSeekMemBuf = nullptr;

			av_freep(&pAvioContext);
			av_freep(&pSeekAvioContext);
		}

		delete[] p_global_bgr_buffer;

		SDL_UnlockMutex(mutex);

		SDL_DestroyMutex(mutex);
		SDL_DestroyCond(cond);
	}

	//Get
	//inline int64_t get_timePerFrame() {
	//	return (int64_t)(timePerFrameInMs);
	//}

	inline bool get_finishState() {
		return bFinish;
	}

	inline bool get_pause() {
		return this->bPause;
	}

	inline int get_volume() {
		return int((volume / 128.0) * 100);
	}

	inline int64_t get_videoPosition() {
		return int64_t(videoPts * 1000);
	}

	inline int64_t get_videoDuration() {
		return int64_t(totalTimeInMs);
	}

	inline bool get_loopState() {
		return bLoop;
	}

	inline int get_width() {
		return pVideoStream->codecpar->width;
	}

	inline int get_height() {
		return pVideoStream->codecpar->height;
	}

	inline bool get_hwDecodeState() {
		return bHWDecode;
	}

	inline const wchar_t* get_hwDeviceName() {
		return get_hwDeviceNameByType(bHWDecode ? hw_type : AV_HWDEVICE_TYPE_NONE);
	}

	inline static const wchar_t* get_hwDeviceNameByType(AVHWDeviceType type) {
		switch (type)
		{
		case AV_HWDEVICE_TYPE_NONE:
			return L"NONE";
		case AV_HWDEVICE_TYPE_VDPAU:
			return L"VDPAU";
		case AV_HWDEVICE_TYPE_CUDA:
			return L"CUDA";
		case AV_HWDEVICE_TYPE_VAAPI:
			return L"VAAPI";
		case AV_HWDEVICE_TYPE_DXVA2:
			return L"DXVA2";
		case AV_HWDEVICE_TYPE_QSV:
			return L"QSV";
		case AV_HWDEVICE_TYPE_VIDEOTOOLBOX:
			return L"VIDEOTOOLBOX";
		case AV_HWDEVICE_TYPE_D3D11VA:
			return L"D3D11VA";
		case AV_HWDEVICE_TYPE_DRM:
			return L"DRM";
		case AV_HWDEVICE_TYPE_OPENCL:
			return L"OPENCL";
		case AV_HWDEVICE_TYPE_MEDIACODEC:
			return L"MEDIACODEC";
		case AV_HWDEVICE_TYPE_VULKAN:
			return L"VULKAN";
		default:
			return L"NONE";
		}
	}

	inline static const AVHWDeviceType get_hwDeviceTypeByName(std::wstring& deviceName) {
		if (StrIEqu(deviceName.c_str(), L"NONE")) {
			return AV_HWDEVICE_TYPE_NONE;
		}
		if (StrIEqu(deviceName.c_str(), L"VDPAU")) {
			return AV_HWDEVICE_TYPE_VDPAU;
		}
		if (StrIEqu(deviceName.c_str(), L"CUDA")) {
			return AV_HWDEVICE_TYPE_CUDA;
		}
		if (StrIEqu(deviceName.c_str(), L"VAAPI")) {
			return AV_HWDEVICE_TYPE_VAAPI;
		}
		if (StrIEqu(deviceName.c_str(), L"DXVA2")) {
			return AV_HWDEVICE_TYPE_DXVA2;
		}
		if (StrIEqu(deviceName.c_str(), L"QSV")) {
			return AV_HWDEVICE_TYPE_QSV;
		}
		if (StrIEqu(deviceName.c_str(), L"VIDEOTOOLBOX")) {
			return AV_HWDEVICE_TYPE_VIDEOTOOLBOX;
		}
		if (StrIEqu(deviceName.c_str(), L"D3D11VA")) {
			return AV_HWDEVICE_TYPE_D3D11VA;
		}
		if (StrIEqu(deviceName.c_str(), L"DRM")) {
			return AV_HWDEVICE_TYPE_DRM;
		}
		if (StrIEqu(deviceName.c_str(), L"OPENCL")) {
			return AV_HWDEVICE_TYPE_OPENCL;
		}
		if (StrIEqu(deviceName.c_str(), L"MEDIACODEC")) {
			return AV_HWDEVICE_TYPE_MEDIACODEC;
		}
		if (StrIEqu(deviceName.c_str(), L"VULKAN")) {
			return AV_HWDEVICE_TYPE_VULKAN;
		}

		return AV_HWDEVICE_TYPE_NONE;
	}

	inline float get_audioTempo() {
		return this->atempo;
	}

	//Set
	inline void set_queueSize(int audioQSize = MAX_AUDIOQ_SIZE, int videoQSize = MAX_VIDEOQ_SIZE) {
		this->audioQSize = audioQSize;
		this->videoQSize = videoQSize;
	}

	inline void set_pause(bool bPause) {
		this->bPause = bPause;
	}

	inline void set_volume(int volume) {
		this->volume = int((max(0, min(100, volume)) / 100.0) * 128);
	}

	inline int set_videoPosition(int64_t ms = 0, int flags = seekFlags) {
		int response = 0;

		int steam_index = -1;

		if (video_stream_index >= 0) { steam_index = video_stream_index; }
		else if (audio_stream_index >= 0) { steam_index = audio_stream_index; }

		// protection
		ms = (flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE
			? min(max(ms, 0), get_videoDuration())
			: ms;
		response = seekFrame(pFormatContext, steam_index, ms, flags);

		if (response < 0) {
			return response;
		}

		if (video_stream_index >= 0) {
			videoQueue.flush();
			//videoQueue.put(&flushPacket);
			avcodec_flush_buffers(pVCodecContext);

		}

		if (audio_stream_index >= 0) {
			audioQueue.flush();
			//audioQueue.put(&flushPacket);
			avcodec_flush_buffers(pACodecContext);
		}

#ifdef _CONSOLE
		bJumped = true;
		auto oldPos = get_videoPosition();

		printf("Cur Video Pts: %f, Cur Clock: %f, Cur Pos: %lld, Jump to MS: %zu\n", videoPts, videoClock, oldPos, ms);
#endif

		//if (ms == 0) {
			//reset_sync();
			//reset_finishState();
		//}

		reset_sync();
		reset_finishState();

		return response;
	}

	inline void set_loop(bool bLoop) {
		this->bLoop = bLoop;
	}

	inline void set_audioTempo(float atempo) {
		this->atempo = atempo > 0
			? atempo
			: DEFAULT_ATEMPO;

		// make sure member value is updated
		if (this->bNoAudio) {
			return;
		}

		std::string fliters;
		double tempo = this->atempo;

		if (this->atempo > 2.0) {
			do {
				fliters += "atempo=2.0,";
			} while ((tempo /= 2) > 2.0);
		}
		else if (this->atempo < 0.5) {
			do {
				fliters += "atempo=0.5,";
			} while ((tempo *= 2) < 0.5);
		}

		fliters += std::format("atempo={:1.1f}", tempo);

		if (strlen(filters_descr) != 0) {
			fliters += std::format(",{}", filters_descr);
		}

		init_audioFilters(pFormatContext, pACodecContext, filter_graph, fliters.c_str());
	}

	//Play core
	inline int goto_videoPosition(size_t ms, rawDataCallBack callBack) {
		return forwardFrame(pFormatContext, pVCodecContext, ms / 1000.0, callBack);
	}

	inline int get_videoFrame(size_t ms, bool bAccurateSeek, rawDataCallBack callBack) {
		int response = 0;

		AVCodecContext* pCodecContext = avcodec_alloc_context3(pVCodec);
		if (!pCodecContext) {
			return -1;
		}

		if (avcodec_parameters_to_context(pCodecContext, pVCodecParameters) < 0) {
			return -1;
		}

		pCodecContext->thread_count = std::thread::hardware_concurrency();
		pCodecContext->flags2 |= AV_CODEC_FLAG2_FAST;

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

#ifdef _HW_DECODE
		AVFrame* pLocalSWFrame = nullptr;
		AVFrame* pOldSWFrame = nullptr;

		if (bHWDecode) {
			AVFrame* pLocalSWFrame = av_frame_alloc();
			if (!pLocalSWFrame) {
				return -1;
			}

			auto pOldSWFrame = pSWFrame;
			pSWFrame = pLocalSWFrame;
		}
#endif // _HW_DECODE

		response = seekFrame(pSeekFormatContext, video_stream_index, ms);

		if (response < 0) {
			return -1;
		}

		// keep current pts & clock
		auto oldClock = videoClock;
		auto oldPts = videoPts;

		videoClock = 0;
		videoPts = 0;

		if (bAccurateSeek) {
			response = forwardFrame(pSeekFormatContext, pCodecContext, ms / 1000.0, callBack);

			if (response < 0) {
				return -1;
			}
		}
		else {
			int how_many_packets_to_process = 0;

			while (av_read_frame(pSeekFormatContext, pPacket) >= 0) {
				// if it's the video stream
				if (pPacket->stream_index == video_stream_index) {
					response = decode_vpacket(pPacket, pCodecContext, pFrame, callBack);

					//wait until receive enough frames
					if (response == AVERROR(EAGAIN)) {
						continue;
					}

					if (response < 0) { break; }

					// stop it, otherwise we'll be saving hundreds of frames
					if (--how_many_packets_to_process <= 0) { break; }
				}

				av_packet_unref(pPacket);
			}
		}

		videoClock = oldClock;
		videoPts = oldPts;

#ifdef _HW_DECODE
		if (bHWDecode) {
			pSWFrame = pOldSWFrame;
			av_frame_free(&pLocalSWFrame);
		}
#endif // _HW_DECODE

		av_frame_free(&pFrame);
		av_packet_free(&pPacket);
		avcodec_free_context(&pCodecContext);

		return response;
	}

	inline int get_nextFrame(rawDataCallBack callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = handleLoop();

		response = decode_frame(callBack);

		//bFinish = bReadFinish && bVideoFinish && bAudioFinish;

		//do not wait audio finish to get fluent loop
		bFinish = bReadFinish && bVideoFinish;

#ifdef _CONSOLE
		bJumped = false;
#endif

		return response;
	}

	inline int audio_fillData(Uint8* stream, int len) {
		SDL_LockMutex(mutex);

		if (!bExit && !bPause) {
			//每次写入stream的数据长度
			int wt_stream_len = 0;
			//每解码后的数据长度
			int audio_size = 0;

			SDL_memset(stream, 0, len);

			//检查音频缓存的剩余长度
			while (len > 0) {
				//检查是否需要执行解码操作
				if (audio_buf_index >= audio_buf_size) {
					// We have already sent all our data; get more
					// 从缓存队列中提取数据包、解码，并返回解码后的数据长度，audio_buf缓存中可能包含多帧解码后的音频数据
					audio_size = decode_audioFrame();

					// If error, output silence.
					if (audio_size < 0) {
						audio_buf_size = SDL_AUDIO_BUFFER_SIZE;
						memset(audio_buf, 0, audio_buf_size);
					}
					else {
						//返回packet中包含的原始音频数据长度(多帧)						
						audio_buf_size = audio_size;
					}

					//初始化累计写入缓存长度
					audio_buf_index = 0;
				}

				//计算解码缓存剩余长度
				wt_stream_len = audio_buf_size - audio_buf_index;

				//检查每次写入缓存的数据长度是否超过指定长度(1024)
				if (wt_stream_len > len) {
					//指定长度从解码的缓存中取数据
					wt_stream_len = len;
				}

				//每次从解码的缓存数据中以指定长度抽取数据并写入stream传递给声卡
				//memcpy(stream, (uint8_t*)audio_buf + audio_buf_index, wt_stream_len);
				SDL_MixAudio(stream, audio_buf + audio_buf_index, len, volume);

				//更新解码音频缓存的剩余长度
				len -= wt_stream_len;
				//更新缓存写入位置
				stream += wt_stream_len;
				//更新累计写入缓存数据长度
				audio_buf_index += wt_stream_len;
			}
		}
		else {
			SDL_memset(stream, 0, len);
		}

		SDL_CondSignal(cond);

		SDL_UnlockMutex(mutex);

		return 0;
	}
};

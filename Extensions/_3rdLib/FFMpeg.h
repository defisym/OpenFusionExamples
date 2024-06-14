// API Changes
// https://github.com/FFmpeg/FFmpeg/blob/master/doc/APIchanges

// Ref: http://dranger.com/ffmpeg/ffmpeg.html
// Ref: https://github.com/leandromoreira/ffmpeg-libav-tutorial
// SafeSEH:NO

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppClangTidyClangDiagnosticShadow
// ReSharper disable CppClangTidyClangDiagnosticReservedMacroIdentifier

#pragma once

#pragma warning(disable : 4819)
#pragma warning(disable : 4996)

// uncomment if you don't want to set in properties
//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"avdevice.lib")
//#pragma comment(lib,"avfilter.lib")
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"swresample.lib")
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
constexpr auto SDL_AUDIO_BUFFER_SIZE = SDLGeneral_BufferSize;
constexpr auto MAX_AUDIO_FRAME_SIZE = 192000;

constexpr auto AUDIO_BUFFER_SIZE = (MAX_AUDIO_FRAME_SIZE * 3) / 2;

constexpr auto MAX_AUDIOQ_SIZE = (5 * 16 * 1024);
constexpr auto MAX_VIDEOQ_SIZE = (5 * 256 * 1024);

constexpr auto PROBE_SIZE = (32 * 4096);

// This object will convert data to the following format for playing
// Channel Layout https://www.cnblogs.com/wangguchangqing/p/5851490.html
constexpr auto TARGET_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
constexpr auto TARGET_CHANNEL_NUMBER = 2;
constexpr auto TARGET_SAMPLE_FORMAT = AV_SAMPLE_FMT_S16;
constexpr auto TARGET_SAMPLE_RATE = 44100;

#define AUDIO_TEMPO

#ifdef AUDIO_TEMPO
constexpr auto DEFAULT_ATEMPO = 1.0f;
#endif

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

constexpr auto END_OF_QUEUE = -5;

#ifdef _DEBUG
constexpr auto FFMpegError_EOF = AVERROR_EOF;
constexpr auto FFMpegError_EAGAIN = AVERROR(EAGAIN);
constexpr auto FFMpegError_EINVAL = AVERROR(EINVAL);
constexpr auto FFMpegError_ENOMEM = AVERROR(ENOMEM);
#endif

// Defines				   
constexpr auto seekFlags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;
//constexpr auto seekFlags = AVSEEK_FLAG_BYTE | AVSEEK_FLAG_FRAME;
//constexpr auto seekFlags = AVSEEK_FLAG_ANY | AVSEEK_FLAG_FRAME;
//constexpr auto seekFlags = AVSEEK_FLAG_FRAME;

#define VIDEO_ALPHA

#ifdef VIDEO_ALPHA
// Has alpha
constexpr auto PIXEL_FORMAT = AV_PIX_FMT_BGRA;
constexpr auto PIXEL_BYTE = 4;
#else
// No alpha
constexpr auto PIXEL_FORMAT = AV_PIX_FMT_BGR24;
constexpr auto PIXEL_BYTE = 3;
#endif

#define HW_DECODE

#ifdef HW_DECODE
// global variable for AVCodecContext->get_format callback
inline static enum AVPixelFormat hw_pix_fmt_global = AV_PIX_FMT_NONE;
#endif

constexpr auto FFMpegFlag_Default = 0;

constexpr auto FFMpegFlag_HWDeviceMask = 0xFFFF;

constexpr auto FFMpegFlag_MakeFlag(const auto flag) {
	return flag << 16;
}

constexpr auto FFMpegFlag_Fast = FFMpegFlag_MakeFlag(0b0000000000000001);
constexpr auto FFMpegFlag_ForceNoAudio = FFMpegFlag_MakeFlag(0b0000000000000010);

class FFMpeg;

class FFMpegOptions {
private:
	friend class FFMpeg;

	const AVCodec* pVideoCodec = nullptr;
	const AVCodec* pAudioCodec = nullptr;
	
	static inline bool CodecValid(const char* pCodecName, AVMediaType type, const AVCodec*& pCodec) {
		const auto pOverriderCodec = avcodec_find_decoder_by_name(pCodecName);

		if (pOverriderCodec == nullptr) {
			return false;
		}

		const auto bValid = av_codec_is_decoder(pOverriderCodec) != 0
			&& pOverriderCodec->type == type;

		if(bValid) {
			pCodec = pOverriderCodec;
		}

		return bValid;
	}

	inline bool VideoCodecValid() {
		return CodecValid(videoCodecName.c_str(), AVMEDIA_TYPE_VIDEO, pVideoCodec);
	}

	inline bool AudioCodecValid() {	
		return CodecValid(audioCodecName.c_str(), AVMEDIA_TYPE_AUDIO, pAudioCodec);
	}
public:
	DWORD flag = FFMpegFlag_Default;
	std::string videoCodecName;
	std::string audioCodecName;

	FFMpegOptions() = default;

	inline void ResetOverride() {
		videoCodecName.clear();
		audioCodecName.clear();
	}

	inline bool NoOverride() const {
		return videoCodecName.empty() && audioCodecName.empty();
	}
};

constexpr AVRational time_base_q = { 1, AV_TIME_BASE };

// pData, stride, height
using rawDataCallBack = std::function<void(const unsigned char*, const int, const int)>;
using Setter = std::function<void*(void* dst, int val, size_t size)>;
using Mixer = std::function<void(void* dst, const void* src, size_t len, int volume)>;

using Uint8 = unsigned char;

class FFMpeg {
private:
#pragma region value
#pragma region control
	bool bExit = false;

	bool bLoop = false;
	bool bPause = false;	// used in audio

	double pausePts = 0;
	double unPausePts = 0;

	bool bReadFinish = false;
	bool bVideoFinish = false;
	bool bAudioFinish = false;

	bool bVideoSendEAgain = false;
	bool bAudioSendEAgain = false;

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
	FFMpegOptions options;
	//DWORD flag = FFMpegFlag_Default;

	bool bFromMem = false;
	bool bNoAudio = false;
	bool bForceNoAudio = false;

	AVIOContext* pAvioContext = nullptr;
	AVIOContext* pSeekAvioContext = nullptr;

	MemBuf* pMemBuf = nullptr;
	MemBuf* pSeekMemBuf = nullptr;

	AVFormatContext* pFormatContext = nullptr;
	AVFormatContext* pSeekFormatContext = nullptr;

#ifdef  HW_DECODE
	// fall back to CPU decode
	bool bHWFallback = false;

	AVBufferRef* hw_device_ctx = nullptr;
	//AVHWDeviceType hw_type = AV_HWDEVICE_TYPE_DXVA2;
	AVHWDeviceType hw_type = AV_HWDEVICE_TYPE_D3D11VA;
	AVPixelFormat hw_pix_fmt= AV_PIX_FMT_NONE;
#endif //  HW_DECODE

#ifdef AUDIO_TEMPO
	AVFilterGraph* filter_graph = nullptr;
	const char* filters_descr = "";

	AVFilterContext* buffersrc_ctx = nullptr;
	AVFilterContext* buffersink_ctx = nullptr;

	AVFrame* pAFilterFrame = nullptr;
#endif //  AUDIO_TEMPO

	//float atempo = DEFAULT_ATEMPO;
	float atempo = -1;
	double tempoTimer = 0.0;

	const AVCodec* pVCodec = nullptr;
	AVCodecParameters* pVCodecParameters = nullptr;

	const AVCodec* pACodec = nullptr;
	AVCodecParameters* pACodecParameters = nullptr;

	int video_stream_index = -1;
	int audio_stream_index = -1;

	AVCodecContext* pVCodecContext = nullptr;
	AVCodecContext* pACodecContext = nullptr;

	AVFrame* pVFrame = nullptr;
	AVFrame* pAFrame = nullptr;

#ifdef HW_DECODE
	bool bHWDecode = false;

	AVFrame* pSWFrame = nullptr;
#endif // HW_DECODE

	//AVFrame vFrame = { 0 };
	//AVFrame aFrame = { 0 };

	//AVFrame* pVFrame = &vFrame;
	//AVFrame* pAFrame = &aFrame;

	AVPacket* pPacket = nullptr;
	//AVPacket* pFlushPacket = nullptr;
	AVPacket flushPacket;

	PacketQueue audioQueue;
	PacketQueue videoQueue;

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

#pragma region Audio
	//保存解码一个packet后的多帧原始音频数据
	uint8_t* audio_buf = nullptr;
	//解码后的多帧音频数据长度
	unsigned int audio_buf_size = 0;
	//累计写入stream的长度
	unsigned int audio_buf_index = 0;

	double audioCallbackStartPts = 0;
	double audioCallbackStartTimer = 0;

	// output
	void* audio_stream = nullptr;
	size_t audio_stream_len = 0;
	
	SDL_SpinLock audioLock = 0;
	bool bAudioCallbackPause = false;
#pragma endregion

#pragma endregion

#ifdef HW_DECODE
	static inline std::vector<AVHWDeviceType> HW_GetDeviceType() {
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

	static inline AVPixelFormat HW_GetPixelFormat(const AVCodec* pCodec, const AVHWDeviceType type) {
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

	inline int HW_InitDecoder(AVCodecContext* pCodecContext, const AVHWDeviceType type) {
		int err = 0;

		if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
			nullptr, nullptr, 0)) < 0) {
			//fprintf(stderr, "Failed to create specified HW device.\n");
			return err;
		}

		pCodecContext->hw_device_ctx = av_buffer_ref(hw_device_ctx);

		return err;
	}
#endif

	static inline std::wstring GetErrorStr(const int errnum) {
		const auto buf = new char[AV_ERROR_MAX_STRING_SIZE];

		av_make_error_string(buf, AV_ERROR_MAX_STRING_SIZE, errnum);

		auto result = ConvertStrToWStr(buf);

		delete[] buf;

		return result;
	}

	inline int64_t get_ChannelLayout() const {
		//int index = av_get_channel_layout_channel_index(av_get_default_channel_layout(4), AV_CH_FRONT_CENTER);

		int channels = pACodecParameters->channels;
		uint64_t channel_layout = pACodecParameters->channel_layout;

		if (channels > 0 && channel_layout == 0) {
			channel_layout = av_get_default_channel_layout(channels);
		}
		else if (channels == 0 && channel_layout > 0) {
			channels = av_get_channel_layout_nb_channels(channel_layout);
		}

		const auto layout = av_get_default_channel_layout(channels);

		return layout;
	}

	inline void init_SwrContext(const int64_t in_ch_layout, const AVSampleFormat in_sample_fmt, const int in_sample_rate) {
		swrContext = swr_alloc_set_opts(swrContext
			, TARGET_CHANNEL_LAYOUT
			, TARGET_SAMPLE_FORMAT, TARGET_SAMPLE_RATE
			, in_ch_layout
			, in_sample_fmt, in_sample_rate
			, 0, nullptr);

		if (!swrContext || swr_init(swrContext) < 0) {
			throw FFMpegException_InitFailed;
		}

		bUpdateSwr = false;
	}

#ifdef AUDIO_TEMPO
	inline int init_audioFilters(const AVFormatContext* fmt_ctx, AVCodecContext* dec_ctx
		, AVFilterGraph* filter_graph, const char* filters_descr) {		
		if (this->bNoAudio) {
			return -1;
		}

		this->bAudioCallbackPause = true;
		this->audioQueue.pause();

		SDL_AtomicLock(&audioLock);
		
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

			const auto filterArgs = std::format("time_base={}/{}:sample_rate={}:sample_fmt={}:channel_layout={:#x}"
				, time_base.num, time_base.den, dec_ctx->sample_rate,
				av_get_sample_fmt_name(dec_ctx->sample_fmt), dec_ctx->channel_layout);

			// create
			const AVFilter* abuffersrc = avfilter_get_by_name("abuffer");

			ret = avfilter_graph_create_filter(&buffersrc_ctx, abuffersrc, "in",
				filterArgs.c_str(), nullptr, filter_graph);
			if (ret < 0) {
				//av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
				throw FFMpegException_FilterInitFailed;
			}

			/* buffer audio sink: to terminate the filter chain. */
			const AVFilter* abuffersink = avfilter_get_by_name("abuffersink");

			ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out",
				nullptr, nullptr, filter_graph);
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
			outputs->next = nullptr;

			/*
			 * The buffer sink input must be connected to the output pad of
			 * the last filter described by filters_descr; since the last
			 * filter output label is not specified, it is set to "out" by
			 * default.
			 */
			inputs->name = av_strdup("out");
			inputs->filter_ctx = buffersink_ctx;
			inputs->pad_idx = 0;
			inputs->next = nullptr;

			if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
				&inputs, &outputs, nullptr)) < 0) {
				throw FFMpegException_FilterInitFailed;
			}
			
			if ((ret = avfilter_graph_config(filter_graph, nullptr)) < 0) {
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

		SDL_AtomicUnlock(&audioLock);
		
		this->audioQueue.restore();
		this->bAudioCallbackPause = false;

		return ret;
	}
#endif //  AUDIO_TEMPO

	// TODO merge into init_formatContext
	static inline const AVInputFormat* init_Probe(BYTE* pBuffer, const size_t bfSz, const LPCSTR pFileName) {		
		AVProbeData probeData = { nullptr };
		probeData.buf = pBuffer;
		//probeData.buf_size = bfSz;
		probeData.buf_size = static_cast<int>((std::min)(static_cast<size_t>(MEM_BUFFER_SIZE), bfSz));
		probeData.filename = pFileName;

		// Determine the input-format:
		while (true) {
			const auto iformat = av_probe_input_format(&probeData, 1);

			if (iformat != nullptr) {
				return iformat;
			}

			if (static_cast<size_t>(probeData.buf_size + 1) > bfSz) {
				throw FFMpegException_InitFailed;
			}

			probeData.buf_size = static_cast<int>((std::min)(static_cast<size_t>(2 * probeData.buf_size), bfSz));
		}

		return nullptr;
	}

	static inline void init_formatContext(AVFormatContext** pFormatContext, const std::wstring& filePath) {
		*pFormatContext = avformat_alloc_context();
		if (!*pFormatContext) {
			throw FFMpegException_InitFailed;
		}
		
		//auto iformat = init_Probe(nullptr, 0, ConvertWStrToStr(filePath, CP_UTF8).c_str());

		// convert to UTF-8 to avoid crash in some versions
		if (avformat_open_input(pFormatContext, ConvertWStrToStr(filePath, CP_UTF8).c_str(), nullptr, nullptr) != 0) {
			throw FFMpegException_InitFailed;
		}
	}

	static inline void init_formatContext(AVFormatContext** pFormatContext, AVIOContext** pAvioContext, MemBuf** pBuf, unsigned char* pBuffer, const size_t bfSz) {
		if (pBuffer == nullptr) {
			throw FFMpegException_InitFailed;
		}

		(*pBuf) = new MemBuf(pBuffer, bfSz);

		*pAvioContext = avio_alloc_context((*pBuf)->get(), (*pBuf)->getSize(), 0, (*pBuf)
			, [](void* opaque, uint8_t* buf, const int buf_size) {
				const auto pMemBuf = static_cast<MemBuf*>(opaque);
				return pMemBuf->read(buf, buf_size);
			}
			, nullptr
			, [](void* opaque, const int64_t offset, const int whence) {
				const auto pMemBuf = static_cast<MemBuf*>(opaque);
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
		AVProbeData probeData = { nullptr };
		probeData.buf = pBuffer;
		//probeData.buf_size = bfSz;
		probeData.buf_size = static_cast<int>((std::min)(static_cast<size_t>(MEM_BUFFER_SIZE), bfSz));
		probeData.filename = "";

		// Determine the input-format:
		while(true){
			(*pFormatContext)->iformat = av_probe_input_format(&probeData, 1);

			if ((*pFormatContext)->iformat != nullptr) {
				break;
			}

			if (static_cast<size_t>(probeData.buf_size + 1) > bfSz) {
				throw FFMpegException_InitFailed;
			}
						
			probeData.buf_size = static_cast<int>((std::min)(static_cast<size_t>(2 * probeData.buf_size), bfSz));
		}

		(*pFormatContext)->flags |= AVFMT_FLAG_CUSTOM_IO;

		if (avformat_open_input(pFormatContext, nullptr, nullptr, nullptr) < 0) {
			throw FFMpegException_InitFailed;
		}
	}

	inline void init_general() {
#pragma region FFMpegInit
		// find stream
		pFormatContext->probesize = PROBE_SIZE;

		if (avformat_find_stream_info(pFormatContext, nullptr) < 0) {
			throw FFMpegException_InitFailed;
		}

		//---------------------
		// Video
		//---------------------

		// init video codec
		video_stream_index = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &pVCodec, 0);
		
		if (video_stream_index < 0) {
			throw FFMpegException_InitFailed;
		}

		pVideoStream = pFormatContext->streams[video_stream_index];
		pVCodecParameters = pVideoStream->codecpar;
		
		if(options.VideoCodecValid()) {
			pVCodec = options.pVideoCodec;
		}

#ifdef HW_DECODE
		const auto hw_deviceType = static_cast<AVHWDeviceType>(options.flag & FFMpegFlag_HWDeviceMask);

		bHWDecode = hw_deviceType != AV_HWDEVICE_TYPE_NONE;

		if (bHWDecode) {
			hw_type = hw_deviceType;
			hw_pix_fmt = HW_GetPixelFormat(pVCodec, hw_type);

			if (hw_pix_fmt == AV_PIX_FMT_NONE) {
				// enum types to get a valid device
				const auto hw_types = HW_GetDeviceType();

				for (const auto& type : hw_types) {
					const auto fmt = HW_GetPixelFormat(pVCodec, type);

					if (fmt != AV_PIX_FMT_NONE) {
						hw_type = type;
						hw_pix_fmt = fmt;

						break;
					}
				}

				if (hw_pix_fmt == AV_PIX_FMT_NONE) {
					//throw FFMpegException_HWInitFailed;
					bHWDecode = false;
					hw_type = AV_HWDEVICE_TYPE_NONE;
				}
			}

			hw_pix_fmt_global = bHWDecode ? hw_pix_fmt : AV_PIX_FMT_NONE;
		}
#endif

		pVCodecContext = avcodec_alloc_context3(pVCodec);
		if (!pVCodecContext) {
			throw FFMpegException_InitFailed;
		}

		if (avcodec_parameters_to_context(pVCodecContext, pVCodecParameters) < 0) {
			throw FFMpegException_InitFailed;
		}

		pVCodecContext->thread_count = static_cast<int>(std::thread::hardware_concurrency());
		
		if (options.flag & FFMpegFlag_Fast) {
			pVCodecContext->flags2 |= AV_CODEC_FLAG2_FAST;
		}

#ifdef HW_DECODE
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

		if (avcodec_open2(pVCodecContext, pVCodec, nullptr) < 0) {
			throw FFMpegException_InitFailed;
		}

		if (pVCodecContext->pix_fmt == AV_PIX_FMT_NONE) {
			throw FFMpegException_InitFailed;
		}

		//---------------------
		// Audio
		//---------------------
		
		if (options.flag & FFMpegFlag_ForceNoAudio) {
			bForceNoAudio = true;
		}

		// init audio codec
		if (!bForceNoAudio) {
			audio_stream_index = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &pACodec, 0);
		}

		if (audio_stream_index < 0) {
			//throw FFMpegException_InitFailed;
			bNoAudio = true;
		}

		if (!bNoAudio) {
			pAudioStream = pFormatContext->streams[audio_stream_index];
			pACodecParameters = pAudioStream->codecpar;

			if (options.AudioCodecValid()) {
				pACodec = options.pAudioCodec;
			}

			pACodecContext = avcodec_alloc_context3(pACodec);
			if (!pACodecContext) {
				throw FFMpegException_InitFailed;
			}

			if (avcodec_parameters_to_context(pACodecContext, pACodecParameters) < 0) {
				throw FFMpegException_InitFailed;
			}

			if (avcodec_open2(pACodecContext, pACodec, nullptr) < 0) {
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

#ifdef AUDIO_TEMPO
		pAFilterFrame = av_frame_alloc();
		if (!pAFilterFrame) {
			throw FFMpegException_InitFailed;
		}
#endif //  AUDIO_TEMPO

#ifdef HW_DECODE
		if (bHWDecode) {
			pSWFrame = av_frame_alloc();
			if (!pSWFrame) {
				throw FFMpegException_InitFailed;
			}
		}
#endif // HW_DECODE

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

		decimalRational = static_cast<double>(rational.num) / rational.den;

		//timePerFrameInMs = decimalRational * 1000;
		totalTime = totalDuration * decimalRational;

		//totalTimeInMs = double(pVideoStream->duration)* av_q2d(pVideoStream->time_base) * 1000;
		totalTimeInMs = totalDuration == INT64_MAX
						? totalDuration
						:static_cast<int64_t>(round(totalTime * 1000));

		//int num_bytes = av_image_get_buffer_size(PIXEL_FORMAT, this->get_width(), this->get_width(), 1);
		//p_global_bgr_buffer = new uint8_t[num_bytes];
#pragma endregion

#pragma region AudioInit	
		if (!bNoAudio) {
			// init SDL audio
			audio_buf = new uint8_t [AUDIO_BUFFER_SIZE];
			memset(audio_buf, 0, AUDIO_BUFFER_SIZE);

			set_audioTempo(DEFAULT_ATEMPO);
		}
#pragma endregion
}

	inline int64_t get_protectedTimeInSecond(const int64_t ms) const {		
		const auto protectedTimeInMs = Range(ms, static_cast<int64_t>(0), totalTimeInMs);
		const auto protectedTimeInSecond = protectedTimeInMs / 1000;

		return protectedTimeInSecond;
	}

	inline int64_t get_protectedTimeStamp(const int64_t ms, const int flags = seekFlags) const {
		// seek by byte, do nothing
		// If flags contain AVSEEK_FLAG_BYTE, then all timestamps are in bytes and are the file position
		// (this may not be supported by all demuxers).
		if ((flags & AVSEEK_FLAG_BYTE) == AVSEEK_FLAG_BYTE) {
			return ms;
		}

		// seek by frame, do conversion
		// If flags contain AVSEEK_FLAG_FRAME, then all timestamps are in frames in the stream with stream_index
		// (this may not be supported by all demuxers).
		if ((flags & AVSEEK_FLAG_FRAME) == AVSEEK_FLAG_FRAME){
			const auto protectedTimeStamp = get_protectedTimeInSecond(ms) / av_q2d(pVideoStream->time_base);

			return static_cast<int64_t>(protectedTimeStamp);
		}

		// default
		// all timestamps are in units of the stream selected by stream_index or if stream_index is -1, in AV_TIME_BASE units
		// av_rescale_q(int64_t(ms * 1000.0), time_base_q, pFormatContext->streams[stream_index]->time_base);
		const auto protectedTimeStamp = get_protectedTimeInSecond(ms) / av_q2d(pVideoStream->time_base);

		return static_cast<int64_t>(protectedTimeStamp);
	}

	inline int seekFrame(AVFormatContext* pFormatContext, const int stream_index, const int64_t ms = 0, const int flags = seekFlags) const {
		int response = 0;

		// input second
		const auto seek_target = get_protectedTimeStamp(ms, flags);

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
		, double targetPts, const rawDataCallBack& callBack) {
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

		//how many packet processed
		//int count = 0;

		while (av_read_frame(pFormatContext, pPacket) >= 0) {
			// if it's the video stream
			if (pPacket->stream_index == video_stream_index) {
				// decode
				response = avcodec_send_packet(pCodecContext, pPacket);

				if (response < 0 && response != AVERROR(EAGAIN) && response != AVERROR_EOF) {
#ifdef _DEBUG
					auto err = GetErrorStr(response);
#endif // _DEBUG

					break;
				}

				response = decode_vpacket(pCodecContext, pPacket, pFrame
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

//#define _REVERT_TO_TARGET

#ifdef _REVERT_TO_TARGET
		// revert to target
		set_videoPosition((int64_t)(targetPts * 1000));
		videoClock = targetPts * 1000;
#else
		// revert to start
		if (targetPts == 0.0) {			
			set_videoPosition(0);
		}
#endif // _REVERT_TO_TARGET

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

			const bool bStop = videoQueue.readFinish() && audioQueue.readFinish();

			if (bReadFinish && !bStop) {
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

	inline int decode_frame(const rawDataCallBack& callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		SyncState syncState = SyncState::SYNC_SYNC;

		int loaclStride = 0;
		int loaclHeight = 0;

		do {
			// fill buffer
			response = fill_queue();

			// calc delay
			syncState = get_syncState();

			//syncState = SyncState::SYNC_SYNC;

			// decode
			switch (syncState) {
				// video is faster, wait
			case FFMpeg::SyncState::SYNC_VIDEOFASTER:
				return 0;
				// decode new video frame
			case FFMpeg::SyncState::SYNC_CLOCKFASTER:
			case FFMpeg::SyncState::SYNC_SYNC:
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
				
		callBack(p_global_bgr_buffer, loaclStride, loaclHeight);

		return 0;
	}

	// Drain
	// https://ffmpeg.org/doxygen/5.0/group__lavc__encdec.html
	// End of stream situations. These require "flushing" (aka draining) the codec, as the codec might buffer multiple frames or packets internally for performance or out of necessity (consider B-frames). This is handled as follows:
	// - Instead of valid input, send NULL to the avcodec_send_packet() (decoding) or avcodec_send_frame() (encoding) functions. This will enter draining mode.
	// - Call avcodec_receive_frame() (decoding) or avcodec_receive_packet() (encoding) in a loop until AVERROR_EOF is returned. The functions will not return AVERROR(EAGAIN), unless you forgot to enter draining mode.
	// - Before decoding can be resumed again, the codec has to be reset with avcodec_flush_buffers().

	using Decoder = int(FFMpeg::*)(AVCodecContext*, const AVPacket*, AVFrame*, const rawDataCallBack&);

	inline int decode_frameCore(bool& bFinishState, bool& bSendEAgain
		, const bool& bBlockState
		, PacketQueue& queue
		, AVCodecContext* pCodecContext, AVPacket* pPacket, AVFrame* pFrame
		, Decoder decoder
		, const rawDataCallBack& callBack) {
		BOOL bRespond = true;
		
		if (!bSendEAgain) {
			bRespond = queue.get(pPacket, bBlockState);

			if (bRespond == QUEUE_WAITING) {
				return QUEUE_WAITING;
			}
		}

		//if (pPacket->data == flushPacket.data) {
		//	avcodec_flush_buffers(pCodecContext);

		//	return 0;
		//}

		const auto bNoPacket = bRespond == false;
		const auto pInputPacket = !bNoPacket ? pPacket : nullptr;

		// Supply raw packet data as input to a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
		int response = avcodec_send_packet(pCodecContext, pPacket);
		bSendEAgain = (response == AVERROR(EAGAIN));

		if (response < 0 && response != AVERROR(EAGAIN) && response != AVERROR_EOF) {
			return response;
		}

		response = (this->*decoder)(pCodecContext, pInputPacket, pFrame, callBack);

		bFinishState = (response == AVERROR_EOF);

		// won't release if current packed didn't used
		if (!bSendEAgain) {
			if (pPacket && pPacket->data) {
				av_packet_unref(pPacket);
			}
		}

		if (bFinishState) {
			return END_OF_QUEUE;
		}

		return response;
	}

	inline int decode_videoFrame(const rawDataCallBack& callBack) {
		return decode_frameCore(bVideoFinish, bVideoSendEAgain
			, false
			, videoQueue
			, pVCodecContext, pVPacket, pVFrame
			, &FFMpeg::decode_vpacket
			, callBack);
	}

	inline int decode_audioFrame() {
		return decode_frameCore(bAudioFinish, bAudioSendEAgain
			, !bReadFinish && !bAudioCallbackPause
			, audioQueue
			, pACodecContext, pAPacket, pAFrame
			, &FFMpeg::decode_apacket
			, nullptr);
	}

	// Convert data to Fusion data
	// https://zhuanlan.zhihu.com/p/53305541
	inline void convertData(AVFrame* pFrame, const rawDataCallBack& callBack) {
		// allocate buffer
		if (p_global_bgr_buffer == nullptr) {
			num_bytes = av_image_get_buffer_size(PIXEL_FORMAT, pFrame->linesize[0], pFrame->height, 1);
			p_global_bgr_buffer = new uint8_t[num_bytes];
		}

		// allocate sws
		if (swsContext == nullptr) {
			//auto pFormat = pFrame->format;

			swsContext = sws_getContext(pFrame->width, pFrame->height
				, static_cast<AVPixelFormat>(pFrame->format),
				pFrame->width, pFrame->height
				, PIXEL_FORMAT
				, NULL, nullptr, nullptr, nullptr);
		}

		// make sure the sws_scale output is point to start.
		const int linesize [8] = { abs(pFrame->linesize [0] * PIXEL_BYTE) };

		uint8_t* bgr_buffer[8] = { p_global_bgr_buffer };

		sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, bgr_buffer, linesize);

		//bgr_buffer[0] is the BGR raw data
		callBack(bgr_buffer[0], linesize[0], pFrame->height);
	}

	inline int decode_vpacket(AVCodecContext* pVCodecContext, const AVPacket* pVPacket, AVFrame* pFrame, const rawDataCallBack& callBack) {
		// Return decoded output data (into a frame) from a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
		int response = avcodec_receive_frame(pVCodecContext, pFrame);

#ifdef _DEBUG
		auto err = GetErrorStr(response);
#endif

		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			return response;
		}
		else if (response < 0) {
			av_frame_unref(pFrame);

#ifdef HW_DECODE
			if (bHWDecode) {
				av_frame_unref(pSWFrame);
			}
#endif // HW_DECODE

			return response;
		}

		if (response >= 0) {
			//videoPts = 0;
			videoPts = static_cast<double>(pFrame->best_effort_timestamp);

			if (pVPacket != nullptr) {
				if (pVPacket->dts == AV_NOPTS_VALUE
					&& pFrame->opaque
					&& *static_cast<uint64_t*>(pFrame->opaque) != AV_NOPTS_VALUE) {
					videoPts = static_cast<double>(*(uint64_t*)pFrame->opaque);
				}
				else {
					videoPts = static_cast<double>(pVPacket->dts != AV_NOPTS_VALUE
						? pFrame->best_effort_timestamp
						: 0);
				}
			}

			videoPts *= av_q2d(pVideoStream->time_base);

			//if (pFrame->key_frame == 1) {
			//	firstKeyFrame = (std::min)(videoPts, firstKeyFrame);
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

#ifdef HW_DECODE
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

		return response;
	}

	//https://github.com/brookicv/FFMPEG-study/blob/master/FFmpeg-playAudio.cpp
	inline int decode_apacket(AVCodecContext* pACodecContext, const AVPacket* pAPacket, AVFrame* pFrame, const rawDataCallBack& callBack) {
		int response = avcodec_receive_frame(pACodecContext, pFrame);

		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			return response;
		}
		else if (response < 0) {
			av_frame_unref(pFrame);

			return response;
		}

#ifdef AUDIO_TEMPO
		response = av_buffersrc_add_frame_flags(buffersrc_ctx, pFrame, AV_BUFFERSRC_FLAG_KEEP_REF);
		//response = av_buffersrc_add_frame_flags(buffersrc_ctx, pFrame, 0);
		if (response < 0) {
			return -1;
		}

		while (1) {
			//response = av_buffersink_get_frame(buffersink_ctx, pFrame);
			response = av_buffersink_get_frame(buffersink_ctx, pAFilterFrame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				break;
			}

			if (response < 0) {
				av_frame_unref(pFrame);
				av_frame_unref(pAFilterFrame);
			}
#endif //  AUDIO_TEMPO

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

#ifdef AUDIO_TEMPO
			auto pBaseFrame = pAFilterFrame;
#else
			auto pBaseFrame = pFrame;
#endif //  AUDIO_TEMPO

			// update context to avoid crash
			if (bUpdateSwr) {
				init_SwrContext(pBaseFrame->channel_layout
					, static_cast<AVSampleFormat>(pBaseFrame->format)
					, pBaseFrame->sample_rate);
			}

			// 计算转换后的sample个数 a * b / c
			// https://blog.csdn.net/u013346305/article/details/48682935
			const int dst_nb_samples = static_cast<int>(av_rescale_rnd(swr_get_delay(swrContext, pBaseFrame->sample_rate)
				+ pBaseFrame->nb_samples
				, TARGET_SAMPLE_RATE, pBaseFrame->sample_rate, static_cast<AVRounding>(1)));

			// 转换，返回值为转换后的sample个数
			const int nb = swr_convert(swrContext, &audio_buf, dst_nb_samples
				, const_cast<const uint8_t**>(pBaseFrame->data), pBaseFrame->nb_samples);

			const auto audioSize = pBaseFrame->channels * nb * av_get_bytes_per_sample(TARGET_SAMPLE_FORMAT);

			audioPts = audioClock;

			const auto pcm_bytes = 2 * pBaseFrame->channels;
			audioClock += static_cast<double>(audioSize) / (static_cast<double>(pcm_bytes) * static_cast<double>(pACodecContext->sample_rate));

			return audioSize;

#ifdef AUDIO_TEMPO
		}

		av_frame_unref(pAFrame);

		return -1;
#endif //  AUDIO_TEMPO
	}

	//synchronize
	inline double synchronize_video(const AVFrame* pVFrame, double videoPts) {
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

	inline double get_externalClock() {
		const auto curTime = get_curTime();
		const auto pausedTime = get_pausedTime();
		
		// reset frameTimer with a extimate time
		if (frameTimer == -1) {
			frameTimer = curTime - videoPts;
			tempoTimer = curTime;
		}

		const auto timeTempo = curTime - pausedTime - tempoTimer;

		return  (tempoTimer - frameTimer) + timeTempo * static_cast<double>(this->atempo);
	}

	inline double get_audioClock() {
		double pts = audioClock; /* maintained in the audio thread */

		auto hw_buf_size = audio_buf_size - audio_buf_index;
		auto bytes_per_sec = 0;

		if (pAudioStream) {
			bytes_per_sec = (TARGET_SAMPLE_RATE * TARGET_CHANNEL_NUMBER) * 2;
		}

		if (bytes_per_sec) {
#define TIMER_OFFSET

#ifdef TIMER_OFFSET
			const auto maxPtsOffset = static_cast<double>(this->audio_stream_len) / bytes_per_sec;

			const auto curTime = get_curTime();
			const auto pausedTime = get_pausedTime();

			const auto timePlayed = curTime - pausedTime - audioCallbackStartTimer;		
			const auto protectedTimePlayed = Range(timePlayed, 0.0, maxPtsOffset);

			pts = audioCallbackStartPts + protectedTimePlayed;
#else
			pts -= static_cast<double>(hw_buf_size) / bytes_per_sec;
#endif
		}

		return pts;
	}

	inline SyncState get_syncState() {
		//if (bFinish) {
		//	return SyncState::SYNC_SYNC;
		//}

		auto delay = videoPts - frameLastPts;

		if (delay <= 0 || delay >= 1) {
			delay = frameLastDelay;
		}

		frameLastPts = videoPts;
		frameLastDelay = delay;

#ifdef _EXTERNAL_CLOCK_SYNC
		auto syncClock = get_externalClock();
#else
		auto syncClock = !bNoAudio
			? get_audioClock()
			: get_externalClock();
#endif

		const auto diff = videoPts - syncClock;

		const auto syncThreshold = delay > AV_SYNC_THRESHOLD
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

		reset_pausePts();
		audioCallbackStartPts = 0;
	}

	inline void reset_finishState() {
		bReadFinish = false;
		bAudioFinish = false;
		bVideoFinish = false;
	}

	static inline double get_curTime() {
		return static_cast<double>(av_gettime()) / 1000000.0;
	}

	static inline double get_ptsMS(const AVPacket* pPacket, const AVStream* pStream) {
		return pPacket->pts * av_q2d(pStream->time_base) * 1000;
	}

	static inline double get_dtsMS(const AVPacket* pPacket, const AVStream* pStream) {
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
	//FFMpeg(const std::wstring& filePath, const DWORD flag = FFMpegFlag_Default) {
	FFMpeg(const std::wstring& filePath, const FFMpegOptions& opt = FFMpegOptions()) {
		bFromMem = false;
		this->options = opt;

		init_formatContext(&pFormatContext, filePath);
		init_formatContext(&pSeekFormatContext, filePath);

		init_general();
	}

	//Load from memory
	//https://blog.csdn.net/leixiaohua1020/article/details/12980423
	//FFMpeg(unsigned char* pBuffer, const size_t bfSz, const DWORD flag = FFMpegFlag_Default) {
	FFMpeg(unsigned char* pBuffer, const size_t bfSz, const FFMpegOptions& opt = FFMpegOptions()) {
		bFromMem = true;
		this->options = opt;

		init_formatContext(&pFormatContext, &pAvioContext, &pMemBuf, pBuffer, bfSz);
		init_formatContext(&pSeekFormatContext, &pSeekAvioContext, &pSeekMemBuf, pBuffer, bfSz);

		init_general();
	}

	~FFMpeg() {
		bExit = true;

		// must be flushed before free the context
		audioQueue.flush();
		videoQueue.flush();

		// flush will restart queue and may cause dead lock
		// so pause again here
		// as call destructor directly is UB
		audioQueue.pause();
		videoQueue.pause();

		//Wait for callback finish
		this->bAudioCallbackPause = true;

		SDL_AtomicLock(&audioLock);

		delete[] audio_buf;

		av_packet_free(&pPacket);
		//av_packet_free(&pFlushPacket);

		av_packet_free(&pVPacket);
		av_packet_free(&pAPacket);

		av_frame_free(&pVFrame);
		av_frame_free(&pAFrame);

#ifdef AUDIO_TEMPO
		av_frame_free(&pAFilterFrame);
#endif //  AUDIO_TEMPO

#ifdef HW_DECODE
		if (bHWDecode) {
			av_frame_free(&pSWFrame);
		}
#endif // HW_DECODE		

#ifdef AUDIO_TEMPO
		avfilter_graph_free(&filter_graph);
#endif //  AUDIO_TEMPO

		sws_freeContext(swsContext);
		swr_free(&swrContext);

		avcodec_close(pVCodecContext);
		avcodec_free_context(&pVCodecContext);
		avcodec_close(pACodecContext);
		avcodec_free_context(&pACodecContext);

		avformat_close_input(&pFormatContext);
		avformat_close_input(&pSeekFormatContext);

#ifdef HW_DECODE
		if (bHWDecode) {
			av_buffer_unref(&hw_device_ctx);
		}
#endif // HW_DECODE		

		if (bFromMem) {
			delete pMemBuf;
			pMemBuf = nullptr;
			delete pSeekMemBuf;
			pSeekMemBuf = nullptr;

			av_freep(&pAvioContext);
			av_freep(&pSeekAvioContext);
		}

		delete[] p_global_bgr_buffer;

		SDL_AtomicUnlock(&audioLock);
	}

	//Get
	//inline int64_t get_timePerFrame() {
	//	return (int64_t)(timePerFrameInMs);
	//}

	inline const uint8_t* get_memBufSrc() const {
		if (bFromMem == false) {
			return nullptr;
		}

		if (pMemBuf == nullptr) {
			return nullptr;
		}

		const auto pSrc = pMemBuf->getSrc();

		return pSrc;
	}

	inline bool get_finishState() const {
		return bFinish;
	}

	inline bool get_pause() const {
		return this->bPause;
	}

	inline int get_sdl_volume() const {
		return volume;
	}

	inline int get_volume() const {
		return static_cast<int>((volume / 128.0) * 100);
	}

	inline int64_t get_videoPosition() const {
		return static_cast<int64_t>(videoPts * 1000);
	}

	inline int64_t get_videoDuration() const {
		return static_cast<int64_t>(totalTimeInMs);
	}

	inline bool get_loopState() const {
		return bLoop;
	}

	inline int get_width() const {
		return pVideoStream->codecpar->width;
	}

	inline int get_height() const {
		return pVideoStream->codecpar->height;
	}

	inline bool get_hwDecodeState() const {
		return bHWDecode;
	}

	inline const wchar_t* get_hwDeviceName() const {
		return get_hwDeviceNameByType(bHWDecode ? hw_type : AV_HWDEVICE_TYPE_NONE);
	}

	inline static const wchar_t* get_hwDeviceNameByType(const AVHWDeviceType type) {
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

	inline static AVHWDeviceType get_hwDeviceTypeByName(const std::wstring& deviceName) {
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

	inline float get_audioTempo() const {
		return this->atempo;
	}

	//Set
	inline void set_queueSize(const int audioQSize = MAX_AUDIOQ_SIZE, const int videoQSize = MAX_VIDEOQ_SIZE) {
		this->audioQSize = audioQSize;
		this->videoQSize = videoQSize;
	}

	inline void set_pause(const bool bPause) {
		this->bPause = bPause;

		if (bPause) {
			pausePts = get_curTime();
		}
		else {
			unPausePts = get_curTime();
		}
	}

	inline void reset_pausePts() {
		pausePts = 0;
		unPausePts = 0;
	}

	inline double get_pausedTime(bool bReset = true) {
		const auto pausedTime = unPausePts - pausePts;
		reset_pausePts();

		return pausedTime;
	}

	inline void set_sdl_volume(const int volume) {
		this->volume = Range(volume, 0, 128);
	}

	inline void set_volume(const int volume) {		
		this->volume = static_cast<int>((Range(volume, 0, 100) / 100.0) * 128);
	}

	inline int set_videoPosition(int64_t ms = 0, const int flags = seekFlags) {
		int response = 0;

		int steam_index = -1;

		if (video_stream_index >= 0) { steam_index = video_stream_index; }
		else if (audio_stream_index >= 0) { steam_index = audio_stream_index; }

		// protection	
		ms = (flags & AVSEEK_FLAG_BYTE) != AVSEEK_FLAG_BYTE
			? Range(ms, static_cast<int64_t>(0), get_videoDuration())
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
		//TODO
		//if (!bForceNoAudio) {
		//	audio_buf_index = 0;
		//	audio_buf_size = 0;
		//	memset(audio_buf, 0, AUDIO_BUFFER_SIZE);
		//	memset(audio_stream, 0, audio_stream_len);
		//}

		//if (ms == 0) {
			//reset_sync();
			//reset_finishState();
		//}

		reset_sync();
		reset_finishState();

		return response;
	}

	inline void set_loop(const bool bLoop) {
		this->bLoop = bLoop;
	}

	inline void set_audioTempo(float atempo) {
#ifndef AUDIO_TEMPO
		this->atempo = DEFAULT_ATEMPO;
#else
		const auto newTempo = atempo > 0
			                      ? atempo
			                      : DEFAULT_ATEMPO;

		// won't update if param is the same
		if (this->atempo == newTempo) {
			return;
		}

		this->atempo = newTempo;

		// make sure member value is updated
		if (this->bNoAudio) {
			// reset timer, also trigger tempoTimer update
			frameTimer = -1;

			return;
		}

		std::string fliters;
		auto tempo = this->atempo;

		if (this->atempo > 2.0f) {
			do {
				fliters += "atempo=2.0,";
			} while ((tempo /= 2) > 2.0f);
		}
		else if (this->atempo < 0.5f) {
			do {
				fliters += "atempo=0.5,";
			} while ((tempo *= 2) < 0.5f);
		}

		fliters += std::format("atempo={:1.1f}", tempo);

		if (strlen(filters_descr) != 0) {
			fliters += std::format(",{}", filters_descr);
		}

		init_audioFilters(pFormatContext, pACodecContext, filter_graph, fliters.c_str());
#endif //  AUDIO_TEMPO
	}

	//Play core
	inline int goto_videoPosition(const size_t ms, const rawDataCallBack& callBack) {
		return forwardFrame(pFormatContext, pVCodecContext, ms / 1000.0, callBack);
	}

	inline int get_videoFrame(const size_t ms, const bool bAccurateSeek, const rawDataCallBack& callBack) {
		int response = 0;

		AVCodecContext* pCodecContext = avcodec_alloc_context3(pVCodec);
		if (!pCodecContext) {
			return -1;
		}

		if (avcodec_parameters_to_context(pCodecContext, pVCodecParameters) < 0) {
			return -1;
		}

		pCodecContext->thread_count = static_cast<int>(std::thread::hardware_concurrency());
		pCodecContext->flags2 |= AV_CODEC_FLAG2_FAST;

		if (avcodec_open2(pCodecContext, pVCodec, nullptr) < 0) {
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

#ifdef HW_DECODE
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
#endif // HW_DECODE

		response = seekFrame(pSeekFormatContext, video_stream_index, ms);

		if (response < 0) {
			return -1;
		}

		// keep current pts & clock
		const auto oldClock = videoClock;
		const auto oldPts = videoPts;

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
					response = avcodec_send_packet(pCodecContext, pPacket);

					if (response < 0 && response != AVERROR(EAGAIN) && response != AVERROR_EOF) {
						break;
					}

					response = decode_vpacket(pCodecContext, pPacket, pFrame, callBack);

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

#ifdef HW_DECODE
		if (bHWDecode) {
			pSWFrame = pOldSWFrame;
			av_frame_free(&pLocalSWFrame);
		}
#endif // HW_DECODE

		av_frame_free(&pFrame);
		av_packet_free(&pPacket);
		avcodec_free_context(&pCodecContext);

		return response;
	}

	inline int get_nextFrame(const rawDataCallBack& callBack) {
		int response = 0;
		int how_many_packets_to_process = 0;

		response = handleLoop();

		if (!bFinish) {
			response = decode_frame(callBack);
		}

		//do not wait audio finish to get fluent loop

		//bFinish = bReadFinish && bVideoFinish && bAudioFinish;
		bFinish = bReadFinish && bVideoFinish;

#ifdef _CONSOLE
		bJumped = false;
#endif

		return response;
	}

	inline int audio_fillData(Uint8* stream, int len, const Setter& setter, const Mixer& mixer) {
		SDL_AtomicLock(&audioLock);

		this->audio_stream = stream;
		this->audio_stream_len = len;
		//this->audioCallbackStartPts = audioClock;
		//this->audioCallbackStartTimer = get_curTime();

		setter(stream, 0, len);

		if (!bNoAudio && !bExit && !bPause) {
			//每次写入stream的数据长度
			int wt_stream_len = 0;
			//每解码后的数据长度
			int audio_size = 0;

			this->audioCallbackStartPts = audioClock;
			this->audioCallbackStartTimer = get_curTime();

			//检查音频缓存的剩余长度
			while (len > 0) {
				if (this->bAudioCallbackPause) {
					//SDL_CondWait(cond_audioCallbackPause, mutex_audio);
					break;
				}

				//检查是否需要执行解码操作
				if (audio_buf_index >= audio_buf_size) {
					// We have already sent all our data; get more
					// 从缓存队列中提取数据包、解码，并返回解码后的数据长度，audio_buf缓存中可能包含多帧解码后的音频数据
					audio_size = decode_audioFrame();

					// If error, output silence.
					if (audio_size < 0) {
						audio_buf_size = SDL_AUDIO_BUFFER_SIZE;
						memset(audio_buf, 0, AUDIO_BUFFER_SIZE);
					}
					else {
						//返回packet中包含的原始音频数据长度(多帧)						
						audio_buf_size = audio_size;
					}

					//初始化累计写入缓存长度
					audio_buf_index = 0;
				}

				//计算解码缓存剩余长度
				wt_stream_len = static_cast<int>(audio_buf_size - audio_buf_index);

				//检查每次写入缓存的数据长度是否超过指定长度(1024)
				if (wt_stream_len > len) {
					//指定长度从解码的缓存中取数据
					wt_stream_len = len;
				}

				//每次从解码的缓存数据中以指定长度抽取数据并写入stream传递给声卡
				mixer(stream, audio_buf + audio_buf_index, len, volume);

				//更新解码音频缓存的剩余长度
				len -= wt_stream_len;
				//更新缓存写入位置
				stream += wt_stream_len;
				//更新累计写入缓存数据长度
				audio_buf_index += wt_stream_len;
			}
		}

		SDL_AtomicUnlock(&audioLock);

		return 0;
	}
};

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

// pData, width, height
using rawDataCallBack = std::function<void(const unsigned char*, const int, const int)>;

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/imgutils.h>
	#include <libswscale/swscale.h>
}

static int decode_packet(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame, rawDataCallBack callBack) {
	// Supply raw packet data as input to a decoder
	// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
	int response = avcodec_send_packet(pCodecContext, pPacket);

	if (response < 0) {
		return response;
	}

	while (response >= 0)	{
		// Return decoded output data (into a frame) from a decoder
		// https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
		response = avcodec_receive_frame(pCodecContext, pFrame);

		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			break;
		}
		else if (response < 0) {
			return response;
		}

		if (response >= 0) {
			uint8_t* pData = pFrame->data[0];
			bool release = false;

			// Check if the frame is a planar YUV 4:2:0, 12bpp
			// That is the format of the provided .mp4 file
			if (pFrame->format != AV_PIX_FMT_BGR24) {	
				release = true;

				SwsContext* swsContext = sws_getContext(pFrame->width, pFrame->height, AVPixelFormat(pFrame->format),
					pFrame->width, pFrame->height, AV_PIX_FMT_BGR24
					,NULL, NULL, NULL, NULL);

				int linesize[8] = { pFrame->linesize[0] * 3 };
				int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pFrame->width, pFrame->height, 1);

				//auto p_global_bgr_buffer = (uint8_t*)malloc(num_bytes * sizeof(uint8_t));
				auto p_global_bgr_buffer = new uint8_t[num_bytes];
				uint8_t* bgr_buffer[8] = { p_global_bgr_buffer };

				sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, bgr_buffer, linesize);
				
				//bgr_buffer[0] is the BGR raw data
				pData = bgr_buffer[0];		

				sws_freeContext(swsContext);
			}

			// call callback
			callBack(pData, pFrame->width, pFrame->height);

			if (release) {
				delete[] pData;
			}		
		}
	}

	return 0;
}

inline int get_videoFrame(std::wstring filePath, size_t ms, rawDataCallBack callBack) {
	AVFormatContext* pFormatContext = avformat_alloc_context();
	if (!pFormatContext) {
		return -1;
	}
	
	if (avformat_open_input(&pFormatContext, ConvertWStrToStr(filePath).c_str(), NULL, NULL) != 0) {
		return -1;
	}

	if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
		return -1;
	}

	const AVCodec* pCodec = NULL;
	AVCodecParameters* pCodecParameters = NULL;
	int video_stream_index = -1;

	for (unsigned int i = 0; i < pFormatContext->nb_streams; i++) {
		AVCodecParameters* pLocalCodecParameters = NULL;
		pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

		const AVCodec* pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

		if (pLocalCodec == NULL) {
			continue;
		}

		if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
			if (video_stream_index == -1) {
				video_stream_index = i;
				pCodec = pLocalCodec;
				pCodecParameters = pLocalCodecParameters;
			}
		}
	}

	if (video_stream_index == -1) {
		return -1;
	}

	AVCodecContext* pCodecContext = avcodec_alloc_context3(pCodec);
	if (!pCodecContext)	{
		return -1;
	}

	if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)	{
		return -1;
	}

	if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)	{
		return -1;
	}

	AVFrame* pFrame = av_frame_alloc();
	if (!pFrame) {
		return -1;
	}

	AVPacket* pPacket = av_packet_alloc();
	if (!pPacket) {
		return -1;
	}

	int response = 0;
	int how_many_packets_to_process = 0;

	//while (av_read_frame(pFormatContext, pPacket) >= 0) {
	//	// if it's the video stream
	//	if (pPacket->stream_index == video_stream_index) {
	//		response = decode_packet(pPacket, pCodecContext, pFrame, callBack);

	//		if (response < 0) { break; }

	//		// stop it, otherwise we'll be saving hundreds of frames
	//		if (--how_many_packets_to_process <= 0) { break; }
	//	}

	//	av_packet_unref(pPacket);
	//}
	
	auto pViedoStream=pFormatContext->streams[video_stream_index];
	
	auto rational = pViedoStream->time_base;
	auto decimalRational = (double)rational.num / rational.den;

	auto totalFrame = pViedoStream->duration;
	auto totalTime = totalFrame * decimalRational;
	auto totalTimeInMs = totalTime * 1000;

	auto protectedTimeInMs = min(totalTimeInMs, max(0, ms));
	auto protectedTime = protectedTimeInMs / 1000;
	auto protectedFrame = protectedTime / decimalRational;

	//auto flags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;
	auto flags =  AVSEEK_FLAG_FRAME;

	if (!(av_seek_frame(pFormatContext, video_stream_index
		, (int64_t)(protectedFrame)
		, flags) >= 0)) {
		return -1;
	}

	while (av_read_frame(pFormatContext, pPacket) >= 0)	{
		// if it's the video stream
		if (pPacket->stream_index == video_stream_index) {
			response = decode_packet(pPacket, pCodecContext, pFrame, callBack);

			if (response < 0) { break; }

			// stop it, otherwise we'll be saving hundreds of frames
			if (--how_many_packets_to_process <= 0) { break; }
		}

		av_packet_unref(pPacket);
	}

	avformat_close_input(&pFormatContext);
	av_packet_free(&pPacket);
	av_frame_free(&pFrame);
	avcodec_free_context(&pCodecContext);

	return 0;
}

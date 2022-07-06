// Ref: https://github.com/leandromoreira/ffmpeg-libav-tutorial

#pragma warning(disable : 4996)

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")

extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
}

static void save_gray_frame(unsigned char* buf, int wrap, int xsize, int ysize, char* filename) {
	FILE* f;
	int i;
	f = fopen(filename, "w");
	// writing the minimal required header for a pgm file format
	// portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
	fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

	// writing line by line
	for (i = 0; i < ysize; i++)
		fwrite(buf + i * wrap, 1, xsize, f);
	fclose(f);
}

static int decode_packet(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame) {
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
			char frame_filename[1024];
			snprintf(frame_filename, sizeof(frame_filename), "%s-%d.pgm", "frame", pCodecContext->frame_number);
			// Check if the frame is a planar YUV 4:2:0, 12bpp
			// That is the format of the provided .mp4 file
			// RGB formats will definitely not give a gray image
			// Other YUV image may do so, but untested, so give a warning
			if (pFrame->format != AV_PIX_FMT_YUV420P) {
			}
			// save a grayscale frame into a .pgm file
			save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, frame_filename);
		}
	}
	return 0;
}

inline int GetFirstFrame(std::wstring filePath) {
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

	while (av_read_frame(pFormatContext, pPacket) >= 0)	{
		// if it's the video stream
		if (pPacket->stream_index == video_stream_index) {
			response = decode_packet(pPacket, pCodecContext, pFrame);
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

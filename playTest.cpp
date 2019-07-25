#define __STDC_CONSTANT_MACROS

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <mutex>
#include <condition_variable>

extern "C"
{
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_syswm.h>
#include <SDL_render.h>
#include <SDL_audio.h>
}

#define SDL_AUDIO_BUFFER_SIZE 1024;

typedef struct _PacketQueue
{
	AVPacketList *first, *last;
	int nb_packets, size;
	std::mutex* packetMutex;
	std::condition_variable* packetCondition;

} PacketQueue;

int quit = 0;

PacketQueue audioq;

SwrContext *swrCtx;

void InitPacketQueue(PacketQueue *pq)
{
	memset(pq, 0, sizeof(PacketQueue));
	pq->packetCondition = new std::condition_variable;
	pq->packetMutex = new std::mutex;
}

int PacketQueuePut(PacketQueue *pq, const AVPacket *srcPkt)
{
	AVPacketList *elt;
	AVPacket pkt;
	int rv;
	if (!pq)
		return -1;
	rv = av_packet_ref(&pkt, srcPkt);
	if (rv)
		return rv;
	elt = (AVPacketList *)av_malloc(sizeof(AVPacketList));
	if (!elt)
		return -1;
	elt->pkt = pkt;
	elt->next = NULL;

	pq->packetMutex->lock();

	if (!pq->last)
		pq->first = elt;
	else
		pq->last->next = elt;
	pq->last = elt;
	pq->nb_packets++;
	pq->size += elt->pkt.size;
	pq->packetMutex->unlock();
	pq->packetCondition->notify_one();
	return 0;
}

int PacketQueueGet(PacketQueue *pq, AVPacket *pkt, int block)
{
	AVPacketList *elt;
	int rv;

	if (!pq || !pkt)
		return -1;

	std::unique_lock<std::mutex> ulock(*(pq->packetMutex));
	for (;;)
	{
		if (quit)
		{
			rv = -1;
			break;
		}

		elt = pq->first;
		if (elt)
		{
			pq->first = elt->next;
			if (!pq->first)
				pq->last = NULL;
			pq->nb_packets--;
			pq->size -= elt->pkt.size;
			*pkt = elt->pkt;
			av_free(elt);
			rv = 1;
			break;
		}
		else if (!block)
		{
			rv = 0;
			break;
		}
		else
		{
			///??SleepConditionVariableCS(&pq->cv, &pq->cs, INFINITE);
			pq->packetCondition->wait(ulock);
		}
	}
	ulock.unlock();
	return rv;
}

int DecodeAudioFrame(AVCodecContext *ctx, uint8_t *audioBuf, int bufSize)
{
	static AVPacket packet = {0};
	static uint8_t *packetData = NULL;
	static int packetSize = 0;
	static AVFrame *frame = NULL;
	static uint8_t converted_data[(192000 * 3) / 2];
	static uint8_t *converted;

	int len1, len2, dataSize = 0;

	if (!frame)
	{
		frame = av_frame_alloc();
		converted = &converted_data[0];
	}

	for (;;)
	{
		while (packetSize > 0)
		{
			int gotFrame = 0;
			len1 = avcodec_decode_audio4(ctx, frame, &gotFrame, &packet);
			if (len1 < 0)
			{
				/* if error, skip frame */
				packetSize = 0;
				break;
			}

			packetData += len1;
			packetSize -= len1;
			if (gotFrame)
			{
				dataSize = av_samples_get_buffer_size(NULL, ctx->channels, frame->nb_samples, ctx->sample_fmt, 1);
				int outSize = av_samples_get_buffer_size(NULL, ctx->channels, frame->nb_samples, AV_SAMPLE_FMT_FLT, 1);
				len2 = swr_convert(swrCtx, &converted, frame->nb_samples, (const uint8_t **)&frame->data[0], frame->nb_samples);
				memcpy(audioBuf, converted_data, outSize);
				dataSize = outSize;
			}
			if (dataSize <= 0)
			{
				/* No data yet, get more frames */
				continue;
			}
			/* We have data, return it and come back for more later */
			return dataSize;
		}

		if (packet.data)
		{
			//av_free_packet(&packet);
		}

		if (quit)
			return -1;

		if (PacketQueueGet(&audioq, &packet, 1) < 0)
			return -1;

		packetData = packet.data;
		packetSize = packet.size;
	}

	return -1;
}

void AudioCallback(void *userdata, uint8_t *stream, int len)
{
	AVCodecContext *ctx = (AVCodecContext *)userdata;
	int len1, audioSize;

	static uint8_t audioBuf[(192000 * 3) / 2];
	static unsigned int audioBufSize = 0;
	static unsigned int audioBufIndex = 0;

	while (len > 0)
	{
		if (audioBufIndex >= audioBufSize)
		{
			// already sent all data; get more
			audioSize = DecodeAudioFrame(ctx, audioBuf, sizeof(audioBuf));
			if (audioSize < 0)
			{
				// error
				audioBufSize = SDL_AUDIO_BUFFER_SIZE;
				memset(audioBuf, 0, sizeof(audioBuf));
			}
			else
			{
				audioBufSize = audioSize;
			}
			audioBufIndex = 0;
		}
		len1 = audioBufSize - audioBufIndex;
		if (len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t *)audioBuf + audioBufIndex, len1);
		len -= len1;
		stream += len1;
		audioBufIndex += len1;
	}
}

int main(int argc, char *argv[])
{
	int rv = 0;
	AVFormatContext *ictx = NULL, *octx = NULL;
	AVOutputFormat *ofmt = NULL;
	AVInputFormat *ifmt = NULL;
	SwsContext *swsCtx = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	SDL_AudioSpec wantedSpec = {0}, audioSpec = {0};
	char *filename = argv[1];
	char err[1024];
	int audioStream = -1, videoStream = -1;
	AVCodec *audioCodec = NULL, *videoCodec = NULL;
	AVCodecContext *audioCtx = NULL, *videoCtx = NULL;
	AVFrame *pFrame = NULL, *pFrameRGB = NULL;
	uint8_t *frameBuffer = NULL;
	int numBytes, width, height;
	AVPacket packet;
	SDL_Event evt;
	int iVideo = 0, iAudio = 0;



	av_register_all();
	avcodec_register_all();
	avdevice_register_all();
	avformat_network_init();

	rv = avformat_open_input(&ictx, filename, NULL, NULL);
	if (rv)
		goto cleanup;

	rv = avformat_find_stream_info(ictx, NULL);
	if (rv)
		goto cleanup;

	for (unsigned int s = 0; s < ictx->nb_streams; ++s)
	{
		av_dump_format(ictx, s, filename, false);
		if (ictx->streams[s]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audioStream < 0)
		{
			audioStream = s;
		}
		else if (ictx->streams[s]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0)
		{
			videoStream = s;
		}
	}
	if (audioStream < 0 && videoStream < 0)
		goto cleanup;

	if (audioStream >= 0)
	{
		audioCodec = avcodec_find_decoder(ictx->streams[audioStream]->codecpar->codec_id);
		if (!audioCodec)
			goto cleanup;
		audioCtx = avcodec_alloc_context3(audioCodec);
		if (!audioCtx)
			goto cleanup;
		rv = avcodec_parameters_to_context(audioCtx, ictx->streams[audioStream]->codecpar);
		if (rv)
			goto cleanup;
		rv = avcodec_open2(audioCtx, audioCodec, NULL);
		if (rv)
			goto cleanup;
		swrCtx = swr_alloc();
		if (!swrCtx)
			goto cleanup;
		av_opt_set_channel_layout(swrCtx, "in_channel_layout", audioCtx->channel_layout, 0);
		av_opt_set_channel_layout(swrCtx, "out_channel_layout", audioCtx->channel_layout, 0);
		av_opt_set_int(swrCtx, "in_sample_rate", audioCtx->sample_rate, 0);
		av_opt_set_int(swrCtx, "out_sample_rate", audioCtx->sample_rate, 0);
		av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", audioCtx->sample_fmt, 0);
		av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
		rv = swr_init(swrCtx);
		if (rv)
			goto cleanup;
	}
	if (videoStream >= 0)
	{
		videoCodec = avcodec_find_decoder(ictx->streams[videoStream]->codecpar->codec_id);
		if (!videoCodec)
			goto cleanup;
		videoCtx = avcodec_alloc_context3(videoCodec);
		if (!videoCtx)
			goto cleanup;
		rv = avcodec_parameters_to_context(videoCtx, ictx->streams[videoStream]->codecpar);
		if (rv)
			goto cleanup;
		rv = avcodec_open2(videoCtx, videoCodec, NULL);
		if (rv)
			goto cleanup;
		swsCtx = sws_getContext(videoCtx->width,
								videoCtx->height,
								videoCtx->pix_fmt,
								videoCtx->width,
								videoCtx->height,
								AV_PIX_FMT_RGB24,
								SWS_BILINEAR,
								NULL,
								NULL,
								NULL);
		if (!swsCtx)
			goto cleanup;
	}

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER))
	{
		fprintf(stderr, "Unable to init SDL: %s \n", SDL_GetError());
		goto cleanup;
	}

	if (audioCodec)
	{
		InitPacketQueue(&audioq);

		wantedSpec.channels = audioCtx->channels;
		wantedSpec.freq = audioCtx->sample_rate;
		wantedSpec.format = AUDIO_F32;
		wantedSpec.silence = 0;
		wantedSpec.samples = SDL_AUDIO_BUFFER_SIZE;
		wantedSpec.userdata = audioCtx;
		wantedSpec.callback = AudioCallback;

		if (SDL_OpenAudio(&wantedSpec, &audioSpec) < 0)
		{
			fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
			goto cleanup;
		}

		SDL_PauseAudio(0);
	}

	if (videoCodec)
	{
		width = ictx->streams[videoStream]->codecpar->width;
		height = ictx->streams[videoStream]->codecpar->height;
		pFrame = av_frame_alloc();
		if (!pFrame)
			goto cleanup;
		pFrameRGB = av_frame_alloc();
		if (!pFrameRGB)
			goto cleanup;
		numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 8);
		if (numBytes < 1)
		{
			rv = numBytes;
			goto cleanup;
		}
		frameBuffer = (uint8_t *)av_malloc(numBytes);
		if (!frameBuffer)
			goto cleanup;

		rv = av_image_fill_arrays(&pFrameRGB->data[0], &pFrameRGB->linesize[0], frameBuffer, AV_PIX_FMT_RGB24, width, height, 1);
		if (rv < 0)
			goto cleanup;

		window = SDL_CreateWindow(filename,
								  SDL_WINDOWPOS_UNDEFINED,
								  SDL_WINDOWPOS_UNDEFINED,
								  videoCtx->width,
								  videoCtx->height,
								  0);

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (!renderer)
			goto cleanup;

		texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGB24,
			SDL_TEXTUREACCESS_STREAMING,
			videoCtx->width,
			videoCtx->height);
		if (!texture)
			goto cleanup;
	}

	while (av_read_frame(ictx, &packet) >= 0)
	{
		if (packet.stream_index == videoStream)
		{
			rv = avcodec_send_packet(videoCtx, &packet);
			if (rv)
				goto cleanup;
			while (!avcodec_receive_frame(videoCtx, pFrame))
			{
				sws_scale(swsCtx,
						  pFrame->data,
						  pFrame->linesize,
						  0,
						  videoCtx->height,
						  pFrameRGB->data,
						  pFrameRGB->linesize);

				SDL_UpdateTexture(texture, NULL, pFrameRGB->data[0], pFrameRGB->linesize[0]);

				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);

				++iVideo;
			}
		}
		else if (packet.stream_index == audioStream)
		{
			PacketQueuePut(&audioq, &packet);
		}
		av_packet_unref(&packet);
		SDL_PollEvent(&evt);
		switch (evt.type)
		{
		case SDL_QUIT:
			quit = 1;
			goto cleanup;
			break;
		default:
			break;
		}
	}

cleanup:
	audioq.packetCondition->notify_all();
	quit = 1;
	if (rv)
	{
		av_strerror(rv, err, sizeof(err));
		fprintf(stderr, "ERROR: %s\n", err);
	}
	if (frameBuffer)
		av_free(frameBuffer);
	if (pFrame)
		av_frame_free(&pFrame);
	if (pFrameRGB)
		av_frame_free(&pFrameRGB);
	if (videoCtx)
		avcodec_close(videoCtx);
	if (audioCtx)
		avcodec_close(audioCtx);
	if (swsCtx)
		sws_freeContext(swsCtx);
	if (swrCtx)
		swr_free(&swrCtx);
	if (texture)
		SDL_DestroyTexture(texture);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	if (ictx)
		avformat_close_input(&ictx);
	if (octx)
		avformat_free_context(octx);
	avformat_network_deinit();
	SDL_Quit();
	return rv;
}
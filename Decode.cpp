#include "Decode.h"
#include <unistd.h>

CDecode::CDecode() : pFormatCtx(NULL)
{
    av_register_all();
    m_pCodecCtx = NULL;
    m_audioCodecCtx = NULL;
    m_videoStream = -1;
    m_audioStream = -1;
    m_endFrame = false;
    m_endAudio = false;
    m_screen = NULL;
    m_audio_buf_size = 0;
    m_audio_buf_index = 0;
    m_audio_pkt_size = 0;
    m_audio_pkt_data = NULL;
    sws_ctx = NULL;

    /*
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }
    */
}

CDecode::~CDecode()
{
    /*
    SDL_CloseAudio();
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        SDL_Quit();
        exit(0);
        break;
    default:
        break;
    }
    */
}

int CDecode::Init(const char *filePath)
{
    // Open video file
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filePath, NULL, NULL) != 0)
    {
        std::cerr << "file open error " << filePath << std::endl;
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        std::cerr << "find stream info error " << std::endl;
        return -1; // Couldn't find stream information
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, NULL, 0);
    {
        std::cerr << "av dump format" << std::endl;
    }
    std::cout << "av duration : " << pFormatCtx->duration << std::endl;
    std::cout << "av nb_streams : " << pFormatCtx->nb_streams << std::endl;

    int i;
    int ret = 0;

    // Find the first video stream
    for (i = 0; i < (int)pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoStream = i;
        }
        else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_audioStream = i;
        }
    }
    if (m_videoStream == -1 || m_audioStream == -1)
    {
        std::cerr << "Didn't find audio(" << m_audioStream << ") or video(" << m_videoStream << ")" << std::endl;
        return -1; // Didn't find a video stream
    }
    std::cout << "find audio(" << m_audioStream << ")  video(" << m_videoStream << ")" << std::endl;

    // Get a pointer to the codec context for the video stream
    AVCodec *pCodec = NULL;
    pCodec = avcodec_find_decoder(pFormatCtx->streams[m_videoStream]->codecpar->codec_id);
    m_pCodecCtx = avcodec_alloc_context3(pCodec);
    ret = avcodec_parameters_to_context(m_pCodecCtx, pFormatCtx->streams[m_videoStream]->codecpar);
    if (!pCodec)
    {
        return -1;
    }
    /* Open codec */
    ret = avcodec_open2(m_pCodecCtx, pCodec, NULL);
    if (ret < 0)
    {
        return -1;
    }

    ///audioCodecCtx
    AVCodec *audioCodec = NULL;
    audioCodec = avcodec_find_decoder(pFormatCtx->streams[m_audioStream]->codecpar->codec_id);
    m_audioCodecCtx = avcodec_alloc_context3(audioCodec);
    if (avcodec_copy_context(m_audioCodecCtx, pFormatCtx->streams[m_audioStream]->codec) != 0)
    {
        fprintf(stderr, "Couldn't copy codec context");
        return -1; // Error copying codec context
    }
    //ret = avcodec_parameters_to_context(m_audioCodecCtx, pFormatCtx->streams[m_audioStream]->codecpar);
    //if (!audioCodec)
    //{
    //    return -1;
    //}
    /* Open codec */
    ret = avcodec_open2(m_audioCodecCtx, audioCodec, NULL);
    if (ret < 0)
    {
        return -1;
    }

    sws_ctx = sws_getContext(m_pCodecCtx->width,
                             m_pCodecCtx->height,
                             m_pCodecCtx->pix_fmt,
                             m_pCodecCtx->width,
                             m_pCodecCtx->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL);

    //ReadFrame();
    //PlayAudio();
    //DecodeFrame();

    /*
    std::thread t1(&CDecode::ReadFrame, this);
    std::thread t2(&CDecode::DecodeFrame, this);
    t1.join();
    t2.join();
 */

    return 0;
}

int CDecode::ReadFrame()
{
    AVPacket *packet;
    packet = av_packet_alloc();
    while (av_read_frame(pFormatCtx, packet) >= 0)
    {
        // Is this a packet from the video stream?
        if (packet->stream_index == m_videoStream)
        {
            AddPacket(packet);
        }
        else if (packet->stream_index == m_audioStream)
        {

            AddAudioPacket(packet);
        }
        else
        {
            av_packet_unref(packet);
        }
        packet = av_packet_alloc();
    }
    av_packet_unref(packet);

    m_endFrame = true;
    return 0;
}

int CDecode::DecodeFrame(AVFrame* pFrameRGB)
{
    /*
    SDL_Overlay *bmp = NULL;
    bmp = SDL_CreateYUVOverlay(m_pCodecCtx->width, m_pCodecCtx->height,
                               SDL_YV12_OVERLAY, m_screen);
                               */

    AVPacket *packet;
    AVFrame *pFrame = av_frame_alloc();
    // Allocate an AVFrame structure
    /*
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL)
        return -1;
    uint8_t *buffer = NULL;
    int numBytes;
    // Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, m_pCodecCtx->width,
                                  m_pCodecCtx->height);
    buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
                   m_pCodecCtx->width, m_pCodecCtx->height);
                   */
    int ret;
    //SDL_Rect rect;
    while (1)
    {
        std::cerr << "Packet Read" << std::endl;
        while (1)
        {
            packet = GetPacket();
            if (packet == NULL)
            {
                std::cerr << "Packet Empty" << std::endl;
                break;
            }
            if (packet->stream_index == m_videoStream)
            {
                // Decode video frame
                ret = avcodec_send_packet(m_pCodecCtx, packet);
                av_packet_unref(packet);
                if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
                {
                    std::cerr << "send packet Error" << std::endl;
                    break;
                }

                if (avcodec_receive_frame(m_pCodecCtx, pFrame) == 0)
                {
                    /*
                    SDL_LockYUVOverlay(bmp);

                    AVPicture pict;
                    pict.data[0] = bmp->pixels[0];
                    pict.data[1] = bmp->pixels[2];
                    pict.data[2] = bmp->pixels[1];

                    pict.linesize[0] = bmp->pitches[0];
                    pict.linesize[1] = bmp->pitches[2];
                    pict.linesize[2] = bmp->pitches[1];

                    // Convert the image into YUV format that SDL uses
                    sws_scale(sws_ctx, (uint8_t const *const *)pFrame->data,
                              pFrame->linesize, 0, m_pCodecCtx->height,
                              pict.data, pict.linesize);

                    SDL_UnlockYUVOverlay(bmp);
                    rect.x = 0;
                    rect.y = 0;
                    rect.w = m_pCodecCtx->width;
                    rect.h = m_pCodecCtx->height;
                    SDL_DisplayYUVOverlay(bmp, &rect);
                    */

                   sws_scale(sws_ctx, (uint8_t const *const *)pFrame->data,
                           pFrame->linesize, 0, m_pCodecCtx->height,
                           pFrameRGB->data, pFrameRGB->linesize);
                   //SaveFrame(pFrameRGB, m_pCodecCtx->width, m_pCodecCtx->height, i++);
                   std::cout << "Find Frame" << std::endl;
                   return 0;
                }
            }
            else if (packet->stream_index == m_audioStream)
            {
                return -1;
            }
        }
        if (m_endFrame)
            break;
        //usleep(100);
    }
    return -1;
}

int CDecode::AddPacket(AVPacket *packet)
{
    m_mutex.lock();
    m_packets.push(packet);
    m_mutex.unlock();
    return 0;
}

AVPacket *CDecode::GetPacket()
{
    AVPacket *packet = NULL;
    m_mutex.lock();
    if (!m_packets.empty())
    {
        packet = m_packets.front();
        m_packets.pop();
    }
    m_mutex.unlock();
    return packet;
}

int CDecode::AddAudioPacket(AVPacket *packet)
{
    m_mutex.lock();
    m_audioPackets.push(packet);
    m_mutex.unlock();
    return 0;
}

AVPacket *CDecode::GetAudioPacket()
{
    std::cout << "GetAudioPacket: " << m_audioPackets.size() << std::endl;
    AVPacket *packet = NULL;
    m_mutex.lock();
    if (!m_audioPackets.empty())
    {
        packet = m_audioPackets.front();
        m_audioPackets.pop();
    }
    m_mutex.unlock();
    return packet;
}

void CDecode::SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int y;

    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile = fopen(szFilename, "wb");
    if (pFile == NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for (y = 0; y < height; y++)
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

    // Close file
    fclose(pFile);
}

void CDecode::onCallback(Uint8 *stream, int len)
{
    std::cout << "onCallback :" << len << std::endl;
    int len1, audio_size;

    //static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    //static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) ];

    while (len > 0)
    {
        if (m_audio_buf_index >= m_audio_buf_size)
        {
            /* We have already sent all our data; get more */
            audio_size = audio_decode_frame(m_audio_buf, MAX_AUDIO_FRAME_SIZE);
            if (audio_size < 0)
            {
                /* If error, output silence */
                m_audio_buf_size = 1024; // arbitrary?
                memset(m_audio_buf, 0, m_audio_buf_size);
            }
            else
            {
                m_audio_buf_size = audio_size;
            }
            m_audio_buf_index = 0;
        }
        len1 = m_audio_buf_size - m_audio_buf_index;
        if (len1 > len)
            len1 = len;
        std::cout << "memcpy(stream)" << std::endl;
        memcpy(stream, (uint8_t *)m_audio_buf + m_audio_buf_index, len1);
        //SDL_MixAudio(stream, (uint8_t *)m_audio_buf + audio_buf_index, len1, SDL_MIX_MAXVOLUME);
        len -= len1;
        stream += len1;
        m_audio_buf_index += len1;
    }
}

void CDecode::audio_callback(void *userdata, Uint8 *stream, int len)
{
    static_cast<CDecode *>(userdata)->onCallback(stream, len);
}

int CDecode::audio_decode_frame(uint8_t *audio_buf, int buf_size)
{
    std::cout << "audio_decode_frame start" << std::endl;
    static AVPacket* pkt;
    //static AVFrame frame;

    int len1, data_size = 0;

    AVFrame *frame = av_frame_alloc();
    for (;;)
    {
        while (m_audio_pkt_size > 0)
        {
            int got_frame = 0;
            std::cout << "avcodec_decode_audio4" << std::endl;
            len1 = avcodec_decode_audio4(m_audioCodecCtx, frame, &got_frame, pkt);
            if (len1 < 0)
            {
                std::cerr << "avcodec_decode_audio4 error" << std::endl;
                /* if error, skip frame */
                m_audio_pkt_size = 0;
                av_frame_unref(frame);
                break;
            }
            data_size = 0;
            if (got_frame)
            {
                std::cout << "get_frmae" << std::endl;
                data_size = audio_resampling(m_audioCodecCtx, frame,
                                             AV_SAMPLE_FMT_S16,
                                             frame->channels, frame->sample_rate, audio_buf);
                /*
                data_size = av_samples_get_buffer_size(NULL,
                                                       m_audioCodecCtx->channels,
                                                       frame->nb_samples,
                                                       m_audioCodecCtx->sample_fmt,
                                                       1);
                std::cout << "av_samples_get_buffer_size" << std::endl;
                if(data_size > buf_size)
                {
                    std::cerr << "data_size > buf_size" << std::endl;
                }
                std::cout << "memcpy:" << data_size << std::endl;
                std::cout << "memcpy:" << &audio_buf << std::endl;
                std::cout << "memcpy:" << &(frame->data[0]) << std::endl;
                memcpy(audio_buf, frame->data[0], data_size);
                //memcpy(audio_buf, frame->data[0], frame->linesize[0]);
                std::cout << "memcpy ok" << std::endl;
                */
            }
            m_audio_pkt_data += len1;
            m_audio_pkt_size -= len1;
            if (data_size <= 0)
            {
                std::cerr << "date_size <= 0" << std::endl;
                /* No data yet, get more frames */
                continue;
            }
            /* We have data, return it and come back for more later */
            std::cout << "audio_decode_frame end:" << data_size << std::endl;
            av_frame_unref(frame);
            //usleep(1000 * 100);
            return data_size;
        }
        //av_free_packet(pkt);
        if (pkt && pkt->data)
        {
            av_packet_unref(pkt);
        }

        /*
        if (quit)
        {
            return -1;
        }
*/

        pkt = GetAudioPacket();
        if(pkt == NULL)
        {
            m_endAudio = true;
            std::cerr << "GetAudioPacket NULL" << std::endl;
            return -1;
        }
        m_audio_pkt_data = pkt->data;
        m_audio_pkt_size = pkt->size;
    }

    av_frame_unref(frame);
    std::cout << "audio_decode_frame end" << std::endl;
    return 0;
}

int CDecode::PlayAudio()
{
    SDL_AudioSpec wanted_spec, spec;
    wanted_spec.freq = m_audioCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    //wanted_spec.format = AUDIO_F32SYS;
    wanted_spec.channels = m_audioCodecCtx->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = this;

    if (SDL_OpenAudio(&wanted_spec, &spec) < 0)
    {
        fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
        return -1;
    }
    SDL_PauseAudio(0);
    while (!m_endAudio)
    {
        SDL_Delay(100);
    }
    std::cerr << "PlayAudio End" << std::endl;
    return 0;
}

int CDecode::audio_resampling(AVCodecContext *audio_decode_ctx,
                            AVFrame *audio_decode_frame,
                            enum AVSampleFormat out_sample_fmt,
                            int out_channels,
                            int out_sample_rate,
                            uint8_t *out_buf)
{
    SwrContext *swr_ctx = NULL;
    int ret = 0;
    int64_t in_channel_layout = audio_decode_ctx->channel_layout;
    int64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
    int out_nb_channels = 0;
    int out_linesize = 0;
    int in_nb_samples = 0;
    int out_nb_samples = 0;
    int max_out_nb_samples = 0;
    uint8_t **resampled_data = NULL;
    int resampled_data_size = 0;

    swr_ctx = swr_alloc();
    if (!swr_ctx)
    {
        printf("swr_alloc error\n");
        return -1;
    }

    in_channel_layout = (audio_decode_ctx->channels ==
                         av_get_channel_layout_nb_channels(audio_decode_ctx->channel_layout))
                            ? audio_decode_ctx->channel_layout
                            : av_get_default_channel_layout(audio_decode_ctx->channels);
    if (in_channel_layout <= 0)
    {
        printf("in_channel_layout error\n");
        return -1;
    }

    if (out_channels == 1)
    {
        out_channel_layout = AV_CH_LAYOUT_MONO;
    }
    else if (out_channels == 2)
    {
        out_channel_layout = AV_CH_LAYOUT_STEREO;
    }
    else
    {
        out_channel_layout = AV_CH_LAYOUT_SURROUND;
    }

    in_nb_samples = audio_decode_frame->nb_samples;
    if (in_nb_samples <= 0)
    {
        printf("in_nb_samples error\n");
        return -1;
    }

    av_opt_set_int(swr_ctx, "in_channel_layout", in_channel_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", audio_decode_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_decode_ctx->sample_fmt, 0);

    av_opt_set_int(swr_ctx, "out_channel_layout", out_channel_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", out_sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", out_sample_fmt, 0);

    if ((ret = swr_init(swr_ctx)) < 0)
    {
        printf("Failed to initialize the resampling context\n");
        return -1;
    }

    max_out_nb_samples = out_nb_samples = av_rescale_rnd(in_nb_samples,
                                                         out_sample_rate,
                                                         audio_decode_ctx->sample_rate,
                                                         AV_ROUND_UP);

    if (max_out_nb_samples <= 0)
    {
        printf("av_rescale_rnd error\n");
        return -1;
    }

    out_nb_channels = av_get_channel_layout_nb_channels(out_channel_layout);

    ret = av_samples_alloc_array_and_samples(&resampled_data, &out_linesize, out_nb_channels, out_nb_samples, out_sample_fmt, 0);
    if (ret < 0)
    {
        printf("av_samples_alloc_array_and_samples error\n");
        return -1;
    }

    out_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, audio_decode_ctx->sample_rate) + in_nb_samples,
                                    out_sample_rate, audio_decode_ctx->sample_rate, AV_ROUND_UP);
    if (out_nb_samples <= 0)
    {
        printf("av_rescale_rnd error\n");
        return -1;
    }

    if (out_nb_samples > max_out_nb_samples)
    {
        av_free(resampled_data[0]);
        ret = av_samples_alloc(resampled_data, &out_linesize, out_nb_channels, out_nb_samples, out_sample_fmt, 1);
        max_out_nb_samples = out_nb_samples;
    }

    if (swr_ctx)
    {
        ret = swr_convert(swr_ctx, resampled_data, out_nb_samples,
                          (const uint8_t **)audio_decode_frame->data, audio_decode_frame->nb_samples);
        if (ret < 0)
        {
            printf("swr_convert_error\n");
            return -1;
        }

        resampled_data_size = av_samples_get_buffer_size(&out_linesize, out_nb_channels, ret, out_sample_fmt, 1);
        if (resampled_data_size < 0)
        {
            printf("av_samples_get_buffer_size error\n");
            return -1;
        }
    }
    else
    {
        printf("swr_ctx null error\n");
        return -1;
    }

    memcpy(out_buf, resampled_data[0], resampled_data_size);

    if (resampled_data)
    {
        av_freep(&resampled_data[0]);
    }
    av_freep(&resampled_data);
    resampled_data = NULL;

    if (swr_ctx)
    {
        swr_free(&swr_ctx);
    }
    return resampled_data_size;
}
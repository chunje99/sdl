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
    swr_ctx = NULL;
    converted = &converted_data[0];
    m_thread[0] = NULL;
    m_thread[1] = NULL;
    m_basePts = 0;
    Running = true;
    pFrame = av_frame_alloc();
    m_volume = SDL_MIX_MAXVOLUME;
}

CDecode::~CDecode()
{
    if (m_thread[0])
        m_thread[0]->join();
    if (m_thread[1])
        m_thread[1]->join();
}

int CDecode::Init(CApp *capp, const char *filePath)
{
    m_capp = capp;
    // Open video file
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filePath, NULL, NULL) != 0)
    {
        LOG(ERROR) << "file open error " << filePath;
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        LOG(ERROR) << "find stream info error ";
        return -1; // Couldn't find stream information
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, NULL, 0);
    {
        LOG(ERROR) << "av dump format";
    }
    DLOG(INFO) << "av duration : " << pFormatCtx->duration;
    DLOG(INFO) << "av nb_streams : " << pFormatCtx->nb_streams;
    DLOG(INFO) << "av start : " << pFormatCtx->start_time;
    m_basePts = pFormatCtx->start_time;

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
        LOG(ERROR) << "Didn't find audio(" << m_audioStream << ") or video(" << m_videoStream << ")";
        return -1; // Didn't find a video stream
    }
    DLOG(INFO) << "find audio(" << m_audioStream << ")  video(" << m_videoStream << ")";

    // Get a pointer to the codec context for the video stream
    AVCodec *pCodec = NULL;
    pCodec = avcodec_find_decoder(pFormatCtx->streams[m_videoStream]->codecpar->codec_id);
    m_pCodecCtx = avcodec_alloc_context3(pCodec);
    m_pCodecCtx->thread_count = 6;
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

    sws_ctx = sws_getContext(m_pCodecCtx->width,
                             m_pCodecCtx->height,
                             m_pCodecCtx->pix_fmt,
                             WINDOW_W,
                             WINDOW_H,
                             //m_pCodecCtx->width / 2,
                             //m_pCodecCtx->height / 2,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL);

    ///audioCodecCtx
    AVCodec *audioCodec = NULL;
    audioCodec = avcodec_find_decoder(pFormatCtx->streams[m_audioStream]->codecpar->codec_id);
    m_audioCodecCtx = avcodec_alloc_context3(audioCodec);
    if (avcodec_copy_context(m_audioCodecCtx, pFormatCtx->streams[m_audioStream]->codec) != 0)
    {
        LOG(ERROR) << "Couldn't copy codec context";
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

    if (m_audioStream >= 0)
    {
        swr_ctx = swr_alloc();
        if (!swr_ctx)
        {
            LOG(ERROR) << "swr_alloc error";
            return -1;
        }
        av_opt_set_channel_layout(swr_ctx, "in_channel_layout", m_audioCodecCtx->channel_layout, 0);
        av_opt_set_channel_layout(swr_ctx, "out_channel_layout", m_audioCodecCtx->channel_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", m_audioCodecCtx->sample_rate, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", m_audioCodecCtx->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", m_audioCodecCtx->sample_fmt, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
        swr_init(swr_ctx);
    }

    //ReadFrame();
    //PlayAudio();
    //DecodeFrame();

    m_thread[0] = new std::thread(&CDecode::ReadFrame, this);
    m_thread[1] = new std::thread(&CDecode::DecodeFrame, this);
    //std::thread t1(&CDecode::ReadFrame, this);
    //std::thread t2(&CDecode::DecodeFrame, this);
    //t1.join();
    //t2.join();

    return 0;
}

int CDecode::ReadFrame()
{
    AVPacket *packet;
    packet = av_packet_alloc();
    int ret = 0;
    while (m_capp->Running)
    {   
        m_mutex.lock();
        ret = av_read_frame(pFormatCtx, packet);
        m_mutex.unlock();
        if(ret < 0)
            break;

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
            //av_packet_unref(packet);
            av_packet_free(&packet);
        }
        packet = av_packet_alloc();
    }
    //av_packet_unref(packet);
    av_packet_free(&packet);

    m_endFrame = true;
    return 0;
}

AVFrame* CDecode::GetFrame()
{
    AVFrame* frame = NULL;
    if (!m_decodedFrames.empty())
    {
        m_mutex.lock();
        frame = m_decodedFrames.front();
        m_decodedFrames.pop();
        m_mutex.unlock();
    }
    return frame;
}

int CDecode::DecodeFrame()
{
    AVPacket *packet;
    // Allocate an AVFrame structure
    int ret;
    uint8_t *buffer = NULL;
    int numBytes;
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                        m_pCodecCtx->width,
                                        m_pCodecCtx->height,
                                        8);
    buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    while (m_capp->Running)
    {
        while (m_capp->Running)
        {
            packet = GetPacket();
            if (packet == NULL)
            {
                LOG(ERROR) << "Packet Empty";
                usleep(1000);
                continue;
            }
            if (packet->stream_index == m_videoStream)
            {
                // Decode video frame
                ret = avcodec_send_packet(m_pCodecCtx, packet);
                if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
                {
                    LOG(ERROR) << "send packet Error";
                    //av_packet_unref(packet);
                    av_packet_free(&packet);
                    break;
                }

                if (avcodec_receive_frame(m_pCodecCtx, pFrame) == 0)
                {
                    double pts = 0;
                    if (packet->dts != AV_NOPTS_VALUE)
                    {
                        pts = av_frame_get_best_effort_timestamp(pFrame);
                    }
                    pts *= av_q2d(pFormatCtx->streams[m_videoStream]->time_base);
                    pts = synchronize_video(pFrame, pts);
                    double aPts = get_audio_clock();
                    m_curPts = pts;
                    double diff = pts - aPts;
                    if (diff > 0)
                    {
                        if (diff > 10.0)
                        {
                            LOG(INFO) << "usleep" << diff;
                        }
                        else
                        {
                            usleep(diff * 1000000);
                        }
                    }
                    AVFrame *pFrameRGB = av_frame_alloc();
                    if (pFrameRGB == NULL)
                        return -1;
                    av_image_fill_arrays(&pFrameRGB->data[0], &pFrameRGB->linesize[0], buffer, AV_PIX_FMT_RGB24,
                                         m_pCodecCtx->width,
                                         m_pCodecCtx->height,
                                         1);
                    sws_scale(sws_ctx, (uint8_t const *const *)pFrame->data,
                              pFrame->linesize, 0, m_pCodecCtx->height,
                              pFrameRGB->data, pFrameRGB->linesize);
                    //SaveFrame(pFrameRGB, m_pCodecCtx->width, m_pCodecCtx->height, idx);
                    pFrameRGB->best_effort_timestamp = pFrame->best_effort_timestamp;
                    pFrameRGB->pts = pFrame->pts;
                    pFrameRGB->pkt_pts = pFrame->pkt_pts;
                    //av_packet_unref(packet);
                    m_mutex.lock();
                    m_decodedFrames.push(pFrameRGB);
                    m_mutex.unlock();
                    av_packet_free(&packet);
                    break;
                }
                //av_packet_unref(packet);
                av_packet_free(&packet);
            }
            else if (packet->stream_index == m_audioStream)
            {
                return -1;
            }
        }
        if (m_endFrame)
            break;
    }
    return -1;
}

int CDecode::AddPacket(AVPacket *packet)
{
    m_mutex.lock();
    m_packets.push(packet);
    m_mutex.unlock();
    if (m_packets.size() > 50)
    {
        //LOG(INFO) << "video packet size:" << m_packets.size();
        usleep(1000* 20);
    }
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
    if (m_audioPackets.size() > 50)
    {
        //LOG(INFO) << "audio packets :" << m_audioPackets.size();
        usleep(1000* 20);
    }
    return 0;
}

AVPacket *CDecode::GetAudioPacket()
{
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
        //memcpy(stream, (uint8_t *)m_audio_buf + m_audio_buf_index, len1);
        SDL_memset(stream, 0, len1);
        SDL_MixAudio(stream, (uint8_t *)m_audio_buf + m_audio_buf_index, len1, m_volume);// mix from one buffer into another
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
    static AVPacket *pkt;
    //static AVFrame frame;

    int len1, data_size = 0, len2;

    AVFrame *aFrame = av_frame_alloc();
    for (; m_capp->Running;)
    {
        while (m_audio_pkt_size > 0 && m_capp->Running)
        {
            int got_frame = 0;
            if (pkt->pts != AV_NOPTS_VALUE)
            {
                m_audio_clock = av_q2d(pFormatCtx->streams[m_audioStream]->time_base) * pkt->pts;
            }
            m_mutex.lock();
            len1 = avcodec_decode_audio4(m_audioCodecCtx, aFrame, &got_frame, pkt);
            m_mutex.unlock();
            if (len1 < 0)
            {
                LOG(ERROR) << "avcodec_decode_audio4 error";
                /* if error, skip frame */
                m_audio_pkt_size = 0;
                //av_frame_unref(aFrame);
                av_frame_free(&aFrame);
                break;
            }
            //data_size = 0;
            m_audio_pkt_data += len1;
            m_audio_pkt_size -= len1;
            if (got_frame)
            {
                data_size = av_samples_get_buffer_size(NULL, m_audioCodecCtx->channels, aFrame->nb_samples,
                                                       m_audioCodecCtx->sample_fmt, 1);
                int outSize = av_samples_get_buffer_size(NULL, m_audioCodecCtx->channels, aFrame->nb_samples,
                                                         AV_SAMPLE_FMT_FLT, 1);
                len2 = swr_convert(swr_ctx, &converted, aFrame->nb_samples,
                                   (const uint8_t **)&aFrame->data[0], aFrame->nb_samples);
                memcpy(audio_buf, converted_data, outSize);
                data_size = outSize;
            }
            if (data_size <= 0)
            {
                LOG(ERROR) << "date_size <= 0";
                /* No data yet, get more frames */
                continue;
            }
            /* We have data, return it and come back for more later */
            //av_frame_unref(aFrame);
            av_frame_free(&aFrame);
            return data_size;
        }
        //av_free_packet(pkt);
        if (pkt && pkt->data)
        {
            //av_packet_unref(pkt);
            av_packet_free(&pkt);
        }

        /*
        if (quit)
        {
            return -1;
        }
*/

        pkt = GetAudioPacket();
        if (pkt == NULL)
        {
            //m_endAudio = true;
            LOG(ERROR) << "GetAudioPacket NULL";
            return -1;
        }
        m_audio_pkt_data = pkt->data;
        m_audio_pkt_size = pkt->size;
    }

    //av_frame_unref(aFrame);
    av_frame_free(&aFrame);
    return 0;
}

double CDecode::synchronize_video(AVFrame *src_frame, double pts)
{

    double frame_delay;

    if (pts != 0)
    {
        /* if we have pts, set video clock to it */
        m_video_clock = pts;
    }
    else
    {
        /* if we aren't given a pts, set it to the clock */
        pts = m_video_clock;
    }
    /* update the video clock */
    frame_delay = av_q2d(pFormatCtx->streams[m_videoStream]->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    m_video_clock += frame_delay;
    //DLOG(INFO) << "video_clock:" << m_video_clock << " pts: " << pts << " audio_clock:" << get_audio_clock();
    //DLOG(INFO) << "videoPacket:" << m_packets.size() << " audioPacket: " << m_audioPackets.size();
    return pts;
}

void CDecode::Seek(int64_t seek_target, int seek_flags)
{
    //m_audio_clock += seek_target;
    //m_curPts += seek_target;
    seek_target += m_curPts;
    seek_target *= AV_TIME_BASE;
    LOG(INFO) << "seek_target:" << seek_target;
    seek_target = av_rescale_q(seek_target, AV_TIME_BASE_Q,
                               pFormatCtx->streams[m_videoStream]->time_base);
    LOG(INFO) << "seek_target:" << seek_target;
    m_mutex.lock();
    int ret = av_seek_frame(pFormatCtx, m_videoStream,
                            seek_target, seek_flags);
    if (ret < 0)
    {
        LOG(ERROR) << "error while seeking";
    }
    else
    {
        /* handle packet queues... more later... */
        ///flush buffer
        AVPacket *packet = NULL;
        ///flush video
        while (!m_packets.empty())
        {
            packet = m_packets.front();
            m_packets.pop();
            av_packet_free(&packet);
        }
        ///fulsh audio
        while (!m_audioPackets.empty())
        {
            packet = m_audioPackets.front();
            m_audioPackets.pop();
            av_packet_free(&packet);
        }
    }
    m_mutex.unlock();
}

double CDecode::get_audio_clock()
{
  double pts;
  int hw_buf_size, bytes_per_sec, n;
  /* maintained in the audio thread */
  pts = m_audio_clock;
  hw_buf_size = m_audio_buf_size - m_audio_buf_index;
  bytes_per_sec = 0;
  n = m_audioCodecCtx->channels * 2;
  if(m_audioStream) {
    bytes_per_sec = m_audioCodecCtx->sample_rate * n;
  }
  if(bytes_per_sec) {
    pts -= (double)hw_buf_size / bytes_per_sec;
  }
  //pts = pts - m_basePts/1000000;
  //if( pts < 0 )
  //  pts = 0;
  return pts;
}

void CDecode::SetVolume(int volume)
{
    if( volume < 0 || volume > SDL_MIX_MAXVOLUME)
        return ;
    m_volume = volume;
}
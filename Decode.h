#ifndef SERVER_DECODE_H
#define SERVER_DECODE_H

#include <iostream>
extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avstring.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include <queue>
#include <mutex>
#include <thread>
#include <SDL.h>
#include <SDL_thread.h>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

class CDecode
{
public:
    CDecode();
    ~CDecode();
    int Init(const char *filePath);
    int ReadFrame();
    int DecodeFrame(AVFrame *pFrameRGB);
    int PlayAudio();

    AVCodecContext* GetVideoCtx() {return m_pCodecCtx;};
    AVCodecContext* GetAudioCtx() {return m_audioCodecCtx;};

private:
    int AddPacket(AVPacket *packet);
    AVPacket *GetPacket();
    int AddAudioPacket(AVPacket *packet);
    AVPacket *GetAudioPacket();

    void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);
    void onCallback(Uint8 *stream, int len);
    static void audio_callback(void *userdata, Uint8 *stream, int len);
    int audio_decode_frame(uint8_t *audio_buf, int buf_size);
    int audio_resampling(AVCodecContext *audio_decode_ctx,
                         AVFrame *audio_decode_frame,
                         enum AVSampleFormat out_sample_fmt,
                         int out_channels,
                         int out_sample_rate,
                         uint8_t *out_buf);

    AVFormatContext *pFormatCtx;
    AVCodecContext *m_pCodecCtx;
    AVCodecContext *m_audioCodecCtx;
    struct SwsContext *sws_ctx;
    int m_videoStream;
    int m_audioStream;

    std::queue<AVPacket *> m_packets;
    std::queue<AVPacket *> m_audioPackets;
    std::mutex m_mutex;
    bool m_endFrame;
    bool m_endAudio;
    std::thread m_thread[2];
    SDL_Surface *m_screen;
    uint8_t m_audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int m_audio_buf_size;
    unsigned int m_audio_buf_index;
    int m_audio_pkt_size;
    uint8_t *m_audio_pkt_data;
};

#endif
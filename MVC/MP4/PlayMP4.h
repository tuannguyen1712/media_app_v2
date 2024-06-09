#ifndef __PLAY_MP4__
#define __PLAY_MP4__

#include <iostream>
#include <assert.h>
#include <thread>
#include <atomic>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libpostproc/postprocess.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}
#include "SDL2/SDL.h"
#include "SDL2/SDL_thread.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_mixer.h"



#define SDL_AUDIO_BUFFER_SIZE 2048
#define MAX_AUDIO_FRAME_SIZE 192000

#define MAX_VOLUME 128
#define DEFAULT_VOLUME 96
#define MIN_VOLUME 0
#define VALUE_CHANGE_VOLUME 16

struct AudioData {
    uint8_t* pos;
    int len;
};

class Player_MP4 {
private:
    AVFormatContext *pFormatCtx = NULL;
    AVCodecParameters* pCodecParAudio = NULL;
    AVCodec* pCodecAudio = NULL;
    AVCodecContext* pCodecCtxAudio = NULL;
    AVFrame* pFrame = NULL;
    AVPacket packet = {0};

    AudioData audioData = {nullptr, 0};
    int audioStream = -1;
    SDL_AudioSpec wanted_spec = {0}, audio_spec = {0};
    SwrContext* swr_ctx = NULL;
    uint8_t* audio_buf = NULL;
    

    std::thread myThread;
    SDL_Event event;

    bool configPlayer();
    void loopPlay();
    void controlPlay();
    void resetPlayback();
    static void audioCallback(void *userdata, Uint8 *stream, int len);
public:
    static int volume;
    int is_pause = 0;
    bool play_status = false;
    bool initPlayer(const char* filename);
    void playMusic();
    void stopMusic();
};


#endif
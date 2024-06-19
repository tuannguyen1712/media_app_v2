#include "PlayMP4.h"

// static int volume = DEFAULT_VOLUME;
// static std::atomic<float> volume(0.5f); // Initial volume: 50%

int Player_MP4::volume = 64;

void Player_MP4::audioCallback(void *userdata, Uint8 *stream, int len)
{
    AudioData* audio = (AudioData*)userdata;
    SDL_memset(stream, 0, len);

    len = (len > audio->len ? audio->len : len);
    // SDL_MixAudioFormat(stream, audio->pos, AUDIO_F32SYS, len, volume);
    int word = len / sizeof(float);
    for (int i = 0; i < word; ++i) {
        ((float*)stream)[i] = ((float*)audio->pos)[i] * (float) volume / 128;
    }
    audio->pos += len;
    audio->len -= len;
}

bool Player_MP4::initPlayer(const char* filename)
{
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
        std::cerr << "Couldn't open file: " << filename << std::endl;
        return false;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        std::cerr << "Couldn't find stream information" << std::endl;
        return false;
    }
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audioStream == -1) {
            audioStream = i;
        }
    }
    if (audioStream == -1) {
        std::cerr << "Didn't find an audio stream" << std::endl;
        return false;
    }
    pCodecParAudio = pFormatCtx->streams[audioStream]->codecpar;
    pCodecAudio = avcodec_find_decoder(pCodecParAudio->codec_id);
    if (pCodecAudio == NULL) {
        std::cerr << "Unsupported audio codec!" << std::endl;
        return false;
    }
    pCodecCtxAudio = avcodec_alloc_context3(pCodecAudio);
    if (avcodec_parameters_to_context(pCodecCtxAudio, pCodecParAudio) < 0) {
        std::cerr << "Couldn't copy audio codec context" << std::endl;
        return false;
    }
    if (avcodec_open2(pCodecCtxAudio, pCodecAudio, NULL) < 0) {
        std::cerr << "Couldn't open audio codec" << std::endl;
        return false;
    }
    pFrame = av_frame_alloc();
    if (!pFrame) {
        std::cerr << "Could not allocate audio frame" << std::endl;
        return false;
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        std::cerr << "Could not initialize SDL - " << SDL_GetError() << std::endl;
        return false;
    }
    return configPlayer();
}

bool Player_MP4::configPlayer()
{
    wanted_spec.freq = pCodecCtxAudio->sample_rate;
    wanted_spec.format = AUDIO_F32SYS;
    wanted_spec.channels = pCodecCtxAudio->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.callback = audioCallback;
    wanted_spec.userdata = &audioData;
    if (SDL_OpenAudio(&wanted_spec, &audio_spec) < 0) {
        std::cerr << "SDL_OpenAudio: " << SDL_GetError() << std::endl;
        return false;
    }
    swr_ctx = swr_alloc();
    av_opt_set_int(swr_ctx, "in_channel_layout", pCodecCtxAudio->channel_layout, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout", audio_spec.channels == 1 ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", pCodecCtxAudio->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", audio_spec.freq, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", pCodecCtxAudio->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    swr_init(swr_ctx);
    audio_buf = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE);
    if (!audio_buf) {
        std::cerr << "Could not allocate audio buffer" << std::endl;
        return false;
    }
    return true;
}

void Player_MP4::loopPlay()
{
    SDL_PauseAudio(0);
    while (play_status)
    {
        if ((av_read_frame(pFormatCtx, &packet) < 0))
        {
            resetPlayback();
        }

        if (packet.stream_index == audioStream)
        {
            avcodec_send_packet(pCodecCtxAudio, &packet);
            avcodec_receive_frame(pCodecCtxAudio, pFrame);
            int out_linesize;

            int out_nb_samples = av_rescale_rnd(
                swr_get_delay(swr_ctx, pCodecCtxAudio->sample_rate) + pFrame->nb_samples,
                audio_spec.freq, pCodecCtxAudio->sample_rate, AV_ROUND_INF);

            int len2 = swr_convert(
                swr_ctx, (uint8_t **)&audio_buf, out_nb_samples,
                (const uint8_t **)pFrame->data, pFrame->nb_samples);

            int out_buffer_size = av_samples_get_buffer_size(
                &out_linesize, audio_spec.channels, len2, AV_SAMPLE_FMT_FLT, 1);

            while (audioData.len > 0)
                ;

            audioData.pos = audio_buf;
            audioData.len = out_buffer_size;
        }
        av_packet_unref(&packet);
    }
}

void Player_MP4::controlPlay()
{
    std::string opt;
    while (true) {
        getline(std::cin, opt);
        if (opt == "q" || opt == "Q") {
            break;
        }
        else if(opt == "+") {
            if (volume < 128) {
                volume += 16;
            }
            // float new_volume = volume.load() + 0.125f;
            // volume.store(new_volume > 1.0f ? 1.0f : new_volume);
        }
        else if(opt == "-") {
            if (volume > 0)
            {
                volume -= 16;
            }
            // float new_volume = volume.load() - 0.125f;
            // volume.store(new_volume < 0.0f ? 0.0f : new_volume);
       
        }
        else if(opt == "p" || opt == "P") {
            is_pause ^= 1;
            SDL_PauseAudio(is_pause);
        }
    }    
    stopMusic();
}

void Player_MP4::playMusic()
{   
    play_status = true;
    myThread = std::thread(&Player_MP4::loopPlay, this);
    // Player_MP4::controlPlay();
}

void Player_MP4::stopMusic()
{
    play_status = false;
    is_pause = 0;
    SDL_PauseAudio(0);
    // std::cout << "Stop" << std::endl;
    myThread.join();
    SDL_CloseAudio();
    SDL_Quit();
    av_free(audio_buf);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecCtxAudio);
    avformat_close_input(&pFormatCtx);
    swr_free(&swr_ctx);
}

void Player_MP4::resetPlayback()
{
    av_seek_frame(pFormatCtx, audioStream, 0, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(pCodecCtxAudio);
}
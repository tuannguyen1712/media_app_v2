#include "control.h"
#include <string>
#include <cstring>

//update

// const char *ext1 = ".c";
// const char *ext2 = ".cpp";

const char *ext1 = ".mp3";
const char *ext2 = ".mp4";

std::string opt;

std::vector<std::string> list_files;
std::vector<std::string> playing_list;
int cnt = 0;
int page = 0;
int pli = 0;
int total_page = 0;
int pre_total_page = 0;

int media_file_index = 0; // for choose media file when play media file
int file_index = 0;       // for choose media file when not play media file

Mix_Music *music = NULL;
volatile int is_play = 0;
volatile int is_pause = 0;
volatile int is_fst = 0;
volatile int is_replay = 0;
volatile time_t last = 0;
volatile time_t current = 0;
volatile time_t diff = 0;
int size = 0;
volatile int is_back = 0;

volatile int err = 0; // check error when using usb
volatile int err_cnt = 0;
volatile int err_idx = -2;
volatile int err_1 = 0;

std::atomic<int> volume(MIX_MAX_VOLUME / 2);

std::vector<std::string> playlists;

Application::Application()
{
    Screen_stack.push(new Screen_start());
    playlists = findPlaylist();
    for (int i = 0; i < (int)playlists.size(); i++)
    {
        List_playlist.push_back(Playlist(playlists[i]));
    }
}

Application::~Application()
{
    while (!Screen_stack.empty())
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    // if (is_play) {
    //     Mix_FreeMusic(music);
    //     Mix_CloseAudio();
    //     SDL_Quit();
    // }
}

void Application::run_app()
{
    while (!Screen_stack.empty())
    {
        int cur_id = Screen_stack.top()->getID();
        switch (cur_id)
        {
        case 1:
            Screen_start_act();
            break;
        case 2:
            Screen_find_act();
            break;
        case 3:
            Screen_find_result_act();
            break;
        case 4:
            Screen_playlist_act();
            break;
        case 5:
            Screen_playlist_element_act();
            break;
        case 6:
            Screen_playlist_element_add_act();
            break;
        case 7:
            Screen_media_detail_act();
            break;
        case 8:
            Screen_playlist_element_add_list_act();
            break;
        case 9:
            Screen_playlist_add_act();
            break;
        case 10:
            Screen_playlist_delete_act();
            break;
        case 11:
            Screen_playlist_element_delete_act();
            break;
        case 12:
            Screen_playlist_rename_act();
            break;
        case 13:
            Screen_media_rename_act();
            break;
        case 14:
            Screen_play_media_act();
            break;
        case 15:
            Screen_usb_act();
            break;
        default:
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Application::Screen_start_act()
{
    Screen_stack.top()->display(is_play);
    
    opt = Screen_stack.top()->getChoice();
    // std::cout << opt << std::endl;
    // sleep(5);
    if (opt == "1")
    {
        Screen_stack.push(new Screen_find());
    }
    else if (opt == "2")
    {
        Screen_stack.push(new Screen_playlist());
    }
    else if (opt == "3")
    {
        // USB
        std::vector<std::string> result = checkUSB();
        if (!result.empty())
        {
            Screen_stack.top()->usb_path = result;
            Screen_stack.push(new Screen_usb());
        }
    }
    else if (opt == "4")
    {
        if (is_play) {
            is_play = 0;
            thread_play.join();
        }
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else if (opt == "`" || opt == "~") 
    {
        is_back = 0;    
        list_files.clear();
        media_file_index = pre_media_file_index;
        total_page = pre_total_page;
        switch (pre_menu)
        {
        case 0:
            Screen_stack.top()->media.clear();
            Screen_stack.top()->setMedia(playing_list);
            list_files.assign(playing_list.begin(), playing_list.end());
            Screen_stack.push(new Screen_find());
            Screen_stack.push(new Screen_find_result());
            Screen_stack.push(new Screen_media_detail());
            Screen_stack.push(new Screen_play_media());
            break;
        case 1:
            pli = pre_pli;
            Screen_stack.top()->media.clear();
            Screen_stack.top()->setMedia(playing_list);
            Screen_stack.push(new Screen_playlist());
            Screen_stack.push(new Screen_playlist_element());
            Screen_stack.push(new Screen_media_detail());
            Screen_stack.push(new Screen_play_media());
            break;
        case 2:
            Screen_stack.top()->media.clear();
            Screen_stack.top()->setMedia(playing_list);
            list_files.assign(playing_list.begin(), playing_list.end());
            Screen_stack.push(new Screen_usb());
            Screen_stack.push(new Screen_find_result());
            Screen_stack.push(new Screen_media_detail());
            Screen_stack.push(new Screen_play_media());
            break;
        default:
            break;
        }
        menu = pre_menu;
    }
    try {
        menu = std::stoi(opt) - 1;
    }
    catch (const std::invalid_argument &e) {}
}

void Application::Screen_find_act()
{
    Screen_stack.top()->display();
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else if (std::filesystem::exists(opt))
    {
        Screen_stack.push(new Screen_find_result);
        find_files(const_cast<char *>(opt.c_str()), list_files, cnt);
        Screen_stack.top()->setMedia(list_files);
    }
}

void Application::Screen_find_result_act()
{
    Screen_stack.top()->display(menu);
    size = list_files.size();
    total_page = (size % FILES_PER_PAGE == 0) ? (size / FILES_PER_PAGE) : (size / FILES_PER_PAGE) + 1;
    // print_files_in_pages(list_files, page, size, total_page);
    Screen_stack.top()->printMedia(page, total_page);
    opt = Screen_stack.top()->getChoice();
    try
    {
        file_index = std::stoi(opt) - 1;
        if (file_index + 1 <= size && pathExists(const_cast<char *>(list_files[file_index].c_str())))
        {
            Screen_stack.push(new Screen_media_detail);
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt == "B" || opt == "b")
        {
            delete Screen_stack.top();
            list_files.clear();
            page = 0;
            cnt = 0;
            Screen_stack.pop();
        }
        else if (opt == ">" || opt == ".")
        {
            if (page < total_page - 1)
            {
                page++;
            }
            else if (page == total_page - 1)
            {
                page = 0;
            }
        }
        else if (opt == "<" || opt == ",")
        {
            if (page > 0)
            {
                page--;
            }
            else if (page == 0)
            {
                page = total_page - 1;
            }
        }
    }
}

void Application::Screen_usb_act()
{
    Screen_stack.top()->display();
    opt = Screen_stack.top()->getChoice();

    try
    {
        int i = std::stoi(opt) - 1;
        if (i < (int)Screen_stack.top()->usb_path.size())
        {
            Screen_stack.push(new Screen_find_result);
            find_files(const_cast<char *>(Screen_stack.top()->usb_path[i].c_str()), list_files, cnt);
            Screen_stack.top()->setMedia(list_files);
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt == "B" || opt == "b")
        {
            delete Screen_stack.top();
            Screen_stack.pop();
            list_files.clear();
        }
    }
}

void Application::Screen_playlist_act()
{
    Screen_stack.top()->display();

    for (int i = 0; i < PLAYLIST_NUM; i++)
    {
        if (i < (int)List_playlist.size())
        {
            Screen_stack.top()->set_playlist_name(getPlaylistName(List_playlist[i].getName()));
            Screen_stack.top()->print_playlist_name(i);
        }
    }
    opt = Screen_stack.top()->getChoice();
    try
    {
        pli = std::stoi(opt) - 1;
        if (pli + 1 <= (int)List_playlist.size())
        {
            Screen_stack.push(new Screen_playlist_element);
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt == "B" || opt == "b")
        {
            delete Screen_stack.top();
            Screen_stack.pop();
        }
        else if (opt == "A" || opt == "a")
        {
            Screen_stack.push(new Screen_playlist_add);
        }
        else if (opt == "D" || opt == "d")
        {
            Screen_stack.push(new Screen_playlist_delete);
        }
    }
}

void Application::Screen_playlist_add_act()
{
    Screen_stack.top()->display();
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else
    {
        createPlaylist(opt);
        delete Screen_stack.top();
        Screen_stack.pop();
    }
}

void Application::Screen_playlist_delete_act()
{
    Screen_stack.top()->display();
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    try
    {
        int i = std::stoi(opt);
        if (i > 0 && i <= (int)List_playlist.size())
        {
            deletePlaylist(List_playlist[i - 1].getName());
            List_playlist.erase(List_playlist.begin() + i - 1);
            delete Screen_stack.top();
            Screen_stack.pop();
        }
    }
    catch (const std::invalid_argument &e)
    {
    }
}

void Application::Screen_playlist_element_act()
{
    Screen_stack.top()->setMedia(List_playlist[pli].__list);
    Screen_stack.top()->display();
    Screen_stack.top()->set_playlist_name(getPlaylistName(List_playlist[pli].getName()));
    Screen_stack.top()->print_playlist_name(pli);
    size = List_playlist[pli].__list.size();
    total_page = (size % FILES_PER_PAGE == 0) ? (size / FILES_PER_PAGE) : (size / FILES_PER_PAGE) + 1;
    Screen_stack.top()->printMedia(page, total_page); 
    // if (!playing_list.empty())
    // {
    //     std::cout << media_file_index << ":" << file_index << ": " << playing_list[0] << std::endl;
    // }
    opt = Screen_stack.top()->getChoice();
    try
    {
        file_index = std::stoi(opt) - 1;
        if (file_index + 1 <= size && pathExists(const_cast<char *>(List_playlist[pli].__list[file_index].c_str())))
        {
            Screen_stack.push(new Screen_media_detail);
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt == "B" || opt == "b")
        {
            page = 0;
            delete Screen_stack.top();
            Screen_stack.pop();
        }
        else if (opt == "A" || opt == "a")
        {
            Screen_stack.push(new Screen_playlist_element_add);
        }
        else if (opt == "D" || opt == "d")
        {
            page = 0;
            Screen_stack.push(new Screen_playlist_element_delete);
        }
        else if (opt == "R" || opt == "r")
        {
            Screen_stack.push(new Screen_playlist_rename);
        }
        else if (opt == ">" || opt == ".")
        {
            if (page < total_page - 1)
            {
                page++;
            }
            else if (page == total_page - 1)
            {
                page = 0;
            }
        }
        else if (opt == "<" || opt == ",")
        {
            if (page > 0)
            {
                page--;
            }
            else if (page == 0)
            {
                page = total_page - 1;
            }
        }
    }
}

void Application::Screen_playlist_element_add_act()
{
    Screen_stack.top()->display();
    Screen_stack.top()->set_playlist_name(getPlaylistName(List_playlist[pli].getName()));
    Screen_stack.top()->print_playlist_name(pli);
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else if (std::filesystem::exists(opt))
    {
        find_files(const_cast<char *>(opt.c_str()), list_files, cnt);
        Screen_stack.push(new Screen_playlist_element_add_list);
        Screen_stack.top()->setMedia(list_files);
    }
}

void Application::Screen_playlist_element_add_list_act()
{
    Screen_stack.top()->display();
    Screen_stack.top()->set_playlist_name(getPlaylistName(List_playlist[pli].getName()));
    Screen_stack.top()->print_playlist_name(pli);
    size = list_files.size();
    total_page = (size % FILES_PER_PAGE == 0) ? (size / FILES_PER_PAGE) : (size / FILES_PER_PAGE) + 1;
    Screen_stack.top()->printMedia(page, total_page);
    opt = Screen_stack.top()->getChoice();
    try
    {
        int i = std::stoi(opt) - 1;
        if (i + 1 <= size && !List_playlist[pli].is_exist(list_files[i])) // edit this
        {
            List_playlist[pli].__list.push_back(list_files[i]);
            List_playlist[pli].addFile(List_playlist[pli].__list);
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt == "B" || opt == "b")
        {
            delete Screen_stack.top();
            list_files.clear();
            page = 0;
            cnt = 0;
            Screen_stack.pop();
        }
        else if (opt == ">" || opt == ".")
        {
            if (page < total_page - 1)
            {
                page++;
            }
            else if (page == total_page - 1)
            {
                page = 0;
            }
        }
        else if (opt == "<" || opt == ",")
        {
            if (page > 0)
            {
                page--;
            }
            else if (page == 0)
            {
                page = total_page - 1;
            }
        }
        else if (opt == "D" || opt == "d")
        {
            list_files.clear();
            page = 0;
            cnt = 0;
            list_files.clear();
            delete Screen_stack.top();
            Screen_stack.pop();
            delete Screen_stack.top();
            Screen_stack.pop();
        }
    }
}

void Application::Screen_playlist_element_delete_act()
{
    Screen_stack.top()->display();
    Screen_stack.top()->set_playlist_name(getPlaylistName(List_playlist[pli].getName()));
    Screen_stack.top()->print_playlist_name(pli);
    size = List_playlist[pli].__list.size();
    total_page = (size % FILES_PER_PAGE == 0) ? (size / FILES_PER_PAGE) : (size / FILES_PER_PAGE) + 1;
    Screen_stack.top()->printMedia(page, total_page);
    opt = Screen_stack.top()->getChoice();
    try
    {
        int i = std::stoi(opt) - 1;
        if (i + 1 <= size)
        {
            List_playlist[pli].removeFile(i);
            Screen_stack.top()->media.erase(Screen_stack.top()->media.begin() + i);
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt == "B" || opt == "b")
        {
            page = 0;
            delete Screen_stack.top();
            Screen_stack.pop();
        }
        else if (opt == ">" || opt == ".")
        {
            if (page < total_page - 1)
            {
                page++;
            }
            else if (page == total_page - 1)
            {
                page = 0;
            }
        }
        else if (opt == "<" || opt == ",")
        {
            if (page > 0)
            {
                page--;
            }
            else if (page == 0)
            {
                page = total_page - 1;
            }
        }
    }
}

void Application::Screen_playlist_rename_act()
{
    Screen_stack.top()->display();
    Screen_stack.top()->set_playlist_name(getPlaylistName(List_playlist[pli].getName()));
    Screen_stack.top()->print_playlist_name(pli);
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else
    {

        List_playlist[pli].rename(opt);
        delete Screen_stack.top();
        Screen_stack.pop();
    }
}

void Application::Screen_media_detail_act()
{
    Screen_stack.top()->display(file_index);
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else if (opt == "P" || opt == "p")
    {
        //multithread
        is_back = 0;
        is_pause = 0;
        media_file_index = file_index;
        pre_total_page = total_page;
        if (!is_play) {
            pre_menu = menu;
            pre_pli = pli;
            pre_media_file_index = media_file_index;
            Screen_stack.top()->volume = volume;
            is_play = 1;
            is_fst = 1;
            if (list_files.empty())
            {
                // find_files(const_cast<char *>(opt.c_str()), list_files, cnt);
                // std::cout << "HERE1" << std::endl;
                // sleep(4);
                playing_list.clear();
                playing_list.assign(List_playlist[pli].__list.begin(), List_playlist[pli].__list.end());
                // music = Mix_LoadMUS(const_cast<char *>(List_playlist[pli].__list[media_file_index].c_str()));
            }
            else
            {
                playing_list.clear();
                playing_list.assign(list_files.begin(), list_files.end());
                // music = Mix_LoadMUS(const_cast<char *>(list_files[media_file_index].c_str()));
            }
            if (getFileExtension(playing_list[media_file_index]) == "mp3") {
                if (SDL_Init(SDL_INIT_AUDIO) < 0)
                {
                std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                is_play = 0;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
                    SDL_Quit();
                    is_play = 0;
                }
                music = Mix_LoadMUS(const_cast<char *>(playing_list[media_file_index].c_str()));
                if (!music)
                {
                    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
                Mix_VolumeMusic(volume);
                if (Mix_PlayMusic(music, -1) == -1)
                {
                    std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
            }
            else {
                if (player_mp4.play_status)
                {
                    player_mp4.stopMusic();
                }
                if (player_mp4.initPlayer(const_cast<char *>(playing_list[media_file_index].c_str())))
                {
                    player_mp4.playMusic();
                }
            }
            last = time(NULL);

            if (Screen_stack.top()->serial_port != -1)
            {
                Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
            }
            else
            {
                Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                if (Screen_stack.top()->serial_port != -1)
                {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                }
            }

            thread_play = std::thread(&Application::thread_play_media, this);
        }
        else if (is_play && (pre_menu != menu || pre_pli != pli || pre_media_file_index != media_file_index))
        {
            is_play = 0;
            thread_play.join();
            pre_menu = menu;
            pre_pli = pli;
            pre_media_file_index = media_file_index;
            Screen_stack.top()->volume = volume;
            is_play = 1;
            is_fst = 1;
            if (list_files.empty())
            {
                playing_list.clear();
                playing_list.assign(List_playlist[pli].__list.begin(), List_playlist[pli].__list.end());
            }
            else
            {
                playing_list.clear();
                playing_list.assign(list_files.begin(), list_files.end());
            }
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                if (SDL_Init(SDL_INIT_AUDIO) < 0)
                {
                    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                    is_play = 0;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
                    SDL_Quit();
                    is_play = 0;
                }
                music = Mix_LoadMUS(const_cast<char *>(playing_list[media_file_index].c_str()));
                if (!music)
                {
                    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
                Mix_VolumeMusic(volume);
                if (Mix_PlayMusic(music, -1) == -1)
                {
                    std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
            }
            else
            {
                if (player_mp4.play_status)
                {
                    player_mp4.stopMusic();
                }
                if (player_mp4.initPlayer(const_cast<char *>(playing_list[media_file_index].c_str())))
                {
                    player_mp4.playMusic();
                }
            }
            last = time(NULL);
            if (Screen_stack.top()->serial_port != -1) {
                Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
            }
            else {
                Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                if (Screen_stack.top()->serial_port != -1) {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                }
            }
            thread_play = std::thread(&Application::thread_play_media, this);
        }
        Screen_stack.push(new Screen_play_media);
    }
    else if (opt == "R" || opt == "r")
    {
        Screen_stack.push(new Screen_media_rename);
    }
}

void Application::Screen_media_rename_act()
{

    Screen_stack.top()->display(file_index);
    opt = Screen_stack.top()->getChoice();
    if (opt == "B" || opt == "b")
    {
        delete Screen_stack.top();
        Screen_stack.pop();
    }
    else
    {
        if (list_files.empty())
        {
            Edit_Title(List_playlist[pli].__list[file_index], opt);
        }
        else
        {
            Edit_Title(list_files[file_index], opt);
        }
        Screen_stack.top()->media[file_index].name = opt;
        delete Screen_stack.top();
        Screen_stack.pop();
    }
}

void Application::Screen_play_media_act()
{
    opt = Screen_stack.top()->getChoice();
    // std::cout << opt << std::endl;  get string from last line of screen(from thread). Fix?
    try
    {
        int ind = std::stoi(opt) - 1;
        if (ind >= 0 && ind < size && std::filesystem::exists(playing_list[ind]))
        {
            is_fst = 1;
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                Mix_FreeMusic(music);
                Mix_CloseAudio();
                SDL_Quit();
                music = NULL;
            }
            else
            {
                player_mp4.stopMusic();
            }
            media_file_index = ind;
            
            is_pause = 0;
            pre_media_file_index = ind;
            last = time(NULL);
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                if (SDL_Init(SDL_INIT_AUDIO) < 0)
                {
                    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                    is_play = 0;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
                    SDL_Quit();
                    is_play = 0;
                }
                if (music != NULL)
                {
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    music = NULL;
                }
                music = Mix_LoadMUS(const_cast<char *>(playing_list[media_file_index].c_str()));
                if (!music)
                {
                    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
                if (Mix_PlayMusic(music, -1) == -1)
                {
                    std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
            }
            else
            {
                if (player_mp4.play_status)
                {
                    player_mp4.stopMusic();
                }
                if (player_mp4.initPlayer(const_cast<char *>(playing_list[media_file_index].c_str())))
                {
                    player_mp4.playMusic();
                }
            }
            if (Screen_stack.top()->serial_port != -1)
            {
                Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
            }
            else
            {
                Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                if (Screen_stack.top()->serial_port != -1)
                {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                }
            }
        }
    }
    catch (const std::invalid_argument &e)
    {
        if (opt.length() > 1 && opt[0] == 'v') {
            try {
                volume = std::stoi(opt.substr(1)) * 16;
                if (getFileExtension(playing_list[media_file_index]) == "mp3")
                {
                    Mix_VolumeMusic(volume);
                }
                else
                {
                    player_mp4.volume = volume;
                }
                Screen_stack.top()->volume = volume;
                // std::cout << volume << std::endl;
            }
            catch (const std::exception &e) {
                // do nothing
            }
        }
        if (opt == "+")
        {
            volume = std::min(volume.load() + 16, MIX_MAX_VOLUME);
            if (getFileExtension(playing_list[media_file_index]) == "mp3") 
            {
                Mix_VolumeMusic(volume);
            }
            else 
            {
                player_mp4.volume = volume;
            }
            Screen_stack.top()->volume = volume;
        }
        else if (opt == "-")
        {
            volume = std::max(volume.load() - 16, 0);
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                Mix_VolumeMusic(volume);
            }
            else
            {
                player_mp4.volume = volume;
            }
            Screen_stack.top()->volume = volume;
        }
        else if (opt == "P" || opt == "p")
        {
            if (is_pause)
            {
                is_pause = false;
                if (getFileExtension(playing_list[media_file_index]) == "mp3") {
                    Mix_ResumeMusic();
                }
                else {
                    SDL_PauseAudio(is_pause);
                }

                if (Screen_stack.top()->serial_port != -1)
                {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                }
                else
                {
                    Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                    if (Screen_stack.top()->serial_port != -1)
                    {
                        Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                    }
                }
            }
            else
            {
                is_pause = true;
                if (getFileExtension(playing_list[media_file_index]) == "mp3") {
                    Mix_PauseMusic();
                }
                else
                {
                    SDL_PauseAudio(is_pause);
                }
                is_fst = 0;
                if (Screen_stack.top()->serial_port != -1)
                {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_STOPPING);
                }
                else
                {
                    Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                    if (Screen_stack.top()->serial_port != -1)
                    {
                        Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_STOPPING);
                    }
                }
            }
        }
        else if (opt == "S" || opt == "s")
        {
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                Mix_FreeMusic(music);
                Mix_CloseAudio();
                SDL_Quit();
                music = NULL;
            }
            else
            {
                player_mp4.stopMusic();
            }
        out1:
            is_fst = 1;
            is_pause = 0;
            media_file_index++;                     //check
            // std::cout << "HERE1" << std::endl;
            // sleep(10);
            if (media_file_index >= size)
            {
                media_file_index = 0;
            }
            if (err_idx == media_file_index && err && err_cnt == size)
            {
                is_play = 0;
                err = 0;
                err_cnt = 0;
                err_idx = -2;
                thread_play.join();
                delete Screen_stack.top();
                Screen_stack.pop();
                delete Screen_stack.top();
                Screen_stack.pop();
                delete Screen_stack.top();
                Screen_stack.pop();
                delete Screen_stack.top();
                Screen_stack.pop();
                clean_stdin();
                list_files.clear();
                goto exit_s;
            }
            else if (err_idx == media_file_index && err && err_cnt != size)
            {
                err = 0;
                err_cnt = 0;
                err_idx = -2;
            }
            last = time(NULL);
            if (!pathExists(const_cast<char *>(playing_list[media_file_index].c_str())))
            {
                if (!err)
                {
                    err = 1;
                    err_idx = media_file_index;
                }
                err_cnt++;
                goto out1;
            }
            pre_media_file_index = media_file_index;
            if (getFileExtension(playing_list[media_file_index]) == "mp3") {
                // if (music != NULL)
                // {
                //     Mix_FreeMusic(music);
                //     music = NULL;
                // }
                if (SDL_Init(SDL_INIT_AUDIO) < 0)
                {
                    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                    is_play = 0;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
                    SDL_Quit();
                    is_play = 0;
                }
                music = Mix_LoadMUS(const_cast<char *>(playing_list[media_file_index].c_str()));
                // std::cout << "HERE\n";
                // std::this_thread::sleep_for(std::chrono::milliseconds(1));
                if (!music)
                {
                    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
                if (Mix_PlayMusic(music, -1) == -1)
                {
                    std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
            }
            else
            {
                if (player_mp4.play_status)
                {
                    player_mp4.stopMusic();
                }
                if (player_mp4.initPlayer(const_cast<char *>(playing_list[media_file_index].c_str())))
                {
                    player_mp4.playMusic();
                }
            }
            if (Screen_stack.top()->serial_port != -1)
            {
                Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
            }
            else
            {
                Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                if (Screen_stack.top()->serial_port != -1)
                {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                }
            }
        exit_s:
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else if (opt == "R" || opt == "r")
        {
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                Mix_FreeMusic(music);
                Mix_CloseAudio();
                SDL_Quit();
                music = NULL;
            }
            else
            {
                player_mp4.stopMusic();
            }
        out2:
            is_fst = 1;
            is_pause = 0;
            media_file_index--;
            if (media_file_index < 0)
            {
                media_file_index = size - 1;
            }
            if (err_idx == media_file_index && err && err_cnt == size)
            {
                is_play = 0;
                err = 0;
                err_cnt = 0;
                err_idx = -2;
                thread_play.join();
                delete Screen_stack.top();
                Screen_stack.pop();
                delete Screen_stack.top();
                Screen_stack.pop();
                delete Screen_stack.top();
                Screen_stack.pop();
                delete Screen_stack.top();
                Screen_stack.pop();
                clean_stdin();
                list_files.clear();
                goto exit_r;
            }
            else if (err_idx == media_file_index && err && err_cnt != size)
            {
                err = 0;
                err_cnt = 0;
                err_idx = -2;
            }
            last = time(NULL);
            if (!pathExists(const_cast<char *>(playing_list[media_file_index].c_str())))
            {
                if (!err)
                {
                    err = 1;
                    err_idx = media_file_index;
                }
                err_cnt++;
                goto out2;
            }
            pre_media_file_index = media_file_index;
            if (getFileExtension(playing_list[media_file_index]) == "mp3") {
                if (SDL_Init(SDL_INIT_AUDIO) < 0)
                {
                    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                    is_play = 0;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
                    SDL_Quit();
                    is_play = 0;
                }
                music = Mix_LoadMUS(const_cast<char *>(playing_list[media_file_index].c_str()));
                if (!music)
                {
                    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
                if (Mix_PlayMusic(music, -1) == -1)
                {
                    std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
            }
            else
            {
                if (player_mp4.play_status) {
                    player_mp4.stopMusic();
                }
                if (player_mp4.initPlayer(const_cast<char *>(playing_list[media_file_index].c_str())))
                {
                    player_mp4.playMusic();
                }
            }
            if (Screen_stack.top()->serial_port != -1)
            {
                Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
            }
            else
            {
                Screen_stack.top()->serial_port = Screen_stack.top()->Init_Serialport();
                if (Screen_stack.top()->serial_port != -1)
                {
                    Screen_stack.top()->send_data_to_port(SEND_TO_PORT, IS_PLAYING);
                }
            }
        exit_r:
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else if (opt == ">" || opt == ".")
        {
            if (page < total_page - 1)
            {
                page++;
            }
            else if (page == total_page - 1)
            {
                page = 0;
            }
        }
        else if (opt == "<" || opt == ",")
        {
            if (page > 0)
            {
                page--;
            }
            else if (page == 0)
            {
                page = total_page - 1;
            }
        }
        else if (opt == ";")
        {
            is_replay ^= 1;
            Screen_stack.top()->replay = is_replay;
        }
        else if (opt == "B" || opt == "b")
        {
            is_back = 1;
            // is_play = 0;
            // thread_play.join();
            // Screen_stack.top()->thread_serial.join();
            delete Screen_stack.top();
            Screen_stack.pop();
            delete Screen_stack.top();
            Screen_stack.pop();
            // clean_stdin();
        }
    }
}

void find_files(const char *directory, std::vector<std::string> &files, int &file_count)
{
    struct dirent *entry;
    DIR *dp = opendir(directory);
    if (dp == NULL)
    {
        perror("opendir");
        return;
    }

    char path[PATH_MAX];
    struct stat statbuf;

    while ((entry = readdir(dp)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        if (stat(path, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            // Recursively search in the subdirectory
            find_files(path, files, file_count);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            // Check the file extension
            const char *ext = strrchr(entry->d_name, '.');
            if (ext != NULL && (strcmp(ext, ext1) == 0 || strcmp(ext, ext2) == 0))
            {
                // strncpy(files[*file_count], path, PATH_MAX);
                files.push_back(path);
                // files[*file_count] = path;
                // (*file_count)++;
            }
        }
    }
    file_count = files.size();
    closedir(dp);
}

void writeToFile(const std::vector<std::string> &vec, const std::string &filename)
{
    std::ofstream outFile(filename);
    if (!outFile)
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (const auto &str : vec)
    {
        outFile << str << '\n';
    }

    outFile.close();
}

void readFromFile(std::vector<std::string> &vec, const std::string &filename)
{
    vec.clear();
    std::ifstream inFile(filename);
    if (!inFile)
    {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        vec.push_back(line);
    }

    inFile.close();
}

std::string getPlaylistName(const std::string &filePath)
{
    // find position of /
    size_t lastSlashPos = filePath.find_last_of("/\\");
    // set start position = 0 if not found /
    size_t start = (lastSlashPos == std::string::npos) ? 0 : lastSlashPos + 1;
    // find position of .
    size_t lastDotPos = filePath.find_last_of('.');
    size_t end = (lastDotPos == std::string::npos || lastDotPos < start) ? filePath.length() : lastDotPos;
    return filePath.substr(start, end - start);
}

std::vector<std::string> findPlaylist()
{
    std::string folderPath = SRC;
    std::vector<std::string> findPlaylist;

    for (const auto &entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            std::string fileName = entry.path().filename().string();
            if (fileName.find('.') == std::string::npos)
            {
                findPlaylist.push_back(fileName);
            }
        }
    }

    return findPlaylist;
}

bool Application::createPlaylist(const std::string &fileName)
{
    std::string folderPath = SRC;

    std::string filePath = folderPath + "/" + fileName;
    if (!std::filesystem::exists(filePath)) 
    {
        std::ofstream outFile(filePath);
        if (outFile)
        {
            outFile.close();
            List_playlist.push_back(Playlist(fileName));
            return true;
        }
        else
        {
            return false;
        }
    }
    else {
        return false;
    }
}

bool Application::deletePlaylist(const std::string &fileName)
{
    if (std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName))
    {
        try
        {
            std::filesystem::remove(fileName);
            return true;
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void Application::thread_play_media()
{
    while (is_play)
    {
        if (!is_pause && is_fst)
        {
            current = time(NULL);
        }
        else if (!is_pause && !is_fst)
        {
            current = time(NULL);
            last = current - diff;
            diff = 0;
            is_fst = 1;
        }
        else
        {
            diff = current - last;
        }
        if (!is_back) {
            Screen_stack.top()->display((int)difftime(current, last), media_file_index);
            Screen_stack.top()->printMedia(media_file_index / 10, total_page);
            Screen_stack.top()->print_orther();
            if (Screen_stack.top()->serial_port == -1) {
                // std::cout << "HERE\n";
            }
        }
        if (difftime(current, last) >= GetDuration(playing_list[media_file_index]) && !is_replay)
        {
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                Mix_FreeMusic(music);
                Mix_CloseAudio();
                SDL_Quit();
                music = NULL;
            }
            else
            {
                player_mp4.stopMusic();
            }
        out:
            media_file_index++;                                                         //check
            // std::cout << "HERE2 " << current << "  " << last << " " << Screen_stack.top()->media[media_file_index].duration << std::endl;
            // sleep(10);
            if (media_file_index >= (int)playing_list.size())
            {
                media_file_index = 0;
            }
            if (err_idx == media_file_index && err && err_cnt == size)
            {
                is_play = 0;
                err_1 = 1;
                err = 0;
                err_cnt = 0;
                err_idx = -2;
                goto exitl;
            }
            else if (err_idx == media_file_index && err && err_cnt != size)                     
            {
                err = 0;
                err_cnt = 0;
                err_idx = -2;
            }
            if (!pathExists(const_cast<char *>(playing_list[media_file_index].c_str())))
            {
                if (!err)
                {
                    err = 1;
                    err_idx = media_file_index;
                }
                err_cnt++;
                goto out;
            }
            //
            pre_media_file_index = media_file_index;
            last = time(NULL);
            if (getFileExtension(playing_list[media_file_index]) == "mp3")
            {
                if (SDL_Init(SDL_INIT_AUDIO) < 0)
                {
                    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                    is_play = 0;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
                    SDL_Quit();
                    is_play = 0;
                }
                music = Mix_LoadMUS(const_cast<char *>(playing_list[media_file_index].c_str()));
                if (!music)
                {
                    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
                if (Mix_PlayMusic(music, -1) == -1)
                {
                    std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
                    Mix_FreeMusic(music);
                    Mix_CloseAudio();
                    SDL_Quit();
                    is_play = 0;
                }
            }
            else
            {
                if (player_mp4.play_status)
                {
                    player_mp4.stopMusic();
                }
                if (player_mp4.initPlayer(const_cast<char *>(playing_list[media_file_index].c_str())))
                {
                    player_mp4.playMusic();
                }
            }
        }
        else if (difftime(current, last) >= GetDuration(playing_list[media_file_index]) && is_replay)
        {
            last = time(NULL);
        }
    exitl:
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    if (err_1)
    {
        err_1 = 0;
        list_files.clear();
        delete Screen_stack.top();
        Screen_stack.pop();
        delete Screen_stack.top();
        Screen_stack.pop();
        delete Screen_stack.top();
        Screen_stack.pop();
        delete Screen_stack.top();
        Screen_stack.pop();
        clean_stdin();
    }
    if (getFileExtension(playing_list[pre_media_file_index]) == "mp3") {
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
    }
    else if (player_mp4.play_status){
        player_mp4.stopMusic();
    }
}
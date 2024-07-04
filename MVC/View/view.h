#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <model.h>
#include <mutex>
#include <iomanip>

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <thread>
#include <sys/select.h>
#include <time.h>
#include <cstring>
#include <dirent.h>

// Linux headers
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()
#include <utility.h>

#include "taglib.h"
// #include "control.h"

#define TITLE_WIDTH 30
#define ARTIRST_WIDTH 25
#define ALBUM_WIDTH 20
#define YEAR_WIDTH 10
#define EXTENSION_WIDTH 10

#define FILES_PER_PAGE 10

class Screen1
{
    int id = 0;

protected:
    int rcv_done;
    int serial_data = 0;
    
    int read_buf_cnt = 0;
    uint64_t last_rcv = 0;
    // Allocate memory for read buffer, set size according to your needs
    char read_buf[256];
    std::mutex mtx;


public:
    int serial_port = -1;
    static std::vector<Media> media;
    static std::string playlist;
    static std::string name;
    static std::vector<std::string> usb_path;
    static int volume;
    static int replay;
    std::thread thread_serial;
    Screen1(){};
    void setMedia(std::vector<std::string> &input);
    void printMedia(const int &page, const int &total_page);
    void set_playlist_name(const std::string &pl_name);
    void print_playlist_name(const int &index);
    void print_orther();

    void thread_read_serial_port(void);
    int read_from_keyboard(char *buff, uint32_t len, uint32_t sec);
    int Init_Serialport();
    long long getMillisecondsSinceEpoch();
    void get_Choice();
    void send_data_to_port(uint8_t type, uint8_t data);

    virtual ~Screen1(){};
    virtual void display(int input = 0, int input1 = 0) = 0;
    virtual std::string getChoice() = 0;
    virtual int getID();
};

class Screen_start : public Screen1
{
    int id = 1;

public:
    Screen_start() {}
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_find : public Screen1
{
    int id = 2;

public:
    Screen_find() {}
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_find_result : public Screen1
{
    int id = 3;

public:
    Screen_find_result(){};
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist : public Screen1
{
    int id = 4;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_element : public Screen1
{
    int id = 5;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_element_add : public Screen1
{
    int id = 6;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_media_detail : public Screen1
{
    int id = 7;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_element_add_list : public Screen1
{
    int id = 8;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_add : public Screen1
{
    int id = 9;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_delete : public Screen1
{
    int id = 10;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_element_delete : public Screen1
{
    int id = 11;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_playlist_rename : public Screen1
{
    int id = 12;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_media_rename : public Screen1
{
    int id = 13;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_play_media : public Screen1
{
    int id = 14;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

class Screen_usb : public Screen1
{
    int id = 15;

public:
    void display(int input = 0, int input1 = 0);
    std::string getChoice();
    int getID();
};

void clean_stdin();
size_t utf8_strlen(const std::string &str);
std::string truncate_utf8(const std::string &str, size_t max_length);
std::string left_align(const std::string &str, size_t width);
#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <iostream>

#define SRC "/.../Playlist/" // edit this

class Media
{
public:
    std::string name;
    std::string artist;
    std::string album;
    unsigned int year;
    int duration;
    std::string extension;
    Media() {}
    Media(const std::string n, const std::string art, const std::string alb, const unsigned int y, const int dur, const std::string ext)
        : name(n), artist(art), album(alb), year(y), duration(dur), extension(ext)
    {
    }
    Media(const Media &input);
};

class Playlist
{
private:
    std::string __name; // path to playlist file

public:
    std::vector<std::string> __list; // vector contain path of media file in playlist file

    Playlist() {}
    Playlist(const std::string &name);
    std::string getName();
    void addFile(const std::vector<std::string> &file);
    void removeFile(int index);
    void rename(const std::string &name);
};
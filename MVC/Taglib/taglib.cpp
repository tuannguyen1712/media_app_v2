#include "taglib.h"


Media GetMedia(const std::string &path)
{
    Media media;
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::AudioProperties *properties = file.audioProperties();
        TagLib::Tag *tag = file.tag();
        media.name = tag->title().to8Bit(true);
        media.artist = tag->artist().to8Bit(true);
        media.album = tag->album().to8Bit(true);
        media.year = tag->year();
        media.duration = (long)properties->length();
        media.extension = getFileExtension(path);
    }
    return media;
}

long GetDuration(const std::string &path)
{
    long ret;
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::AudioProperties *properties = file.audioProperties();
        ret = (long)properties->length();
    }
    return ret;
}

std::string GetName(const std::string &path)
{
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::Tag *tag = file.tag();
        return tag->title().to8Bit(true);
    }
    else
    {
        TagLib::String ret = "";
        return ret.to8Bit(true);
    }
}
bool Edit_Title(const std::string &path, std::string new_title)
{
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::Tag *tag = file.tag();
        tag->setTitle(new_title);
        file.save();
        return true;
    }
    else
    {
        return false;
    }
}

bool Edit_Artist(const std::string &path, std::string new_artist)
{
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::Tag *tag = file.tag();
        tag->setArtist(new_artist);
        file.save();
        return true;
    }
    else
    {
        return false;
    }
}

bool Edit_Album(const std::string &path, std::string new_album)
{
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::Tag *tag = file.tag();
        tag->setAlbum(new_album);
        file.save();
        return true;
    }
    else
    {
        return false;
    }
}

bool Edit_Year(const std::string &path, int new_year)
{
    TagLib::FileRef file(TagLib::FileName(path.c_str()));
    if (!file.isNull() && file.tag())
    {
        TagLib::Tag *tag = file.tag();
        tag->setYear(new_year);
        file.save();
        return true;
    }
    else
    {
        return false;
    }
}

std::string getFileExtension(const std::string &filename)
{
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        return filename.substr(pos + 1);
    }
    return ""; // Empty string if no extension found
}
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>

#define SEND_TO_PORT        0xFFu
#define RESPONSE            0X00U

#define IS_PLAYING          0xFFu
#define IS_STOPPING         0x00u

#define RESENT_REQUEST      0x00u
#define ERROR_MSG           0xFFu

bool pathExists(const char *path);
std::string getUsername();
std::vector<std::string> findUSBs(const std::string &userPath);

std::vector<std::string> checkUSB();
void packing_frame_data(uint8_t *output, uint8_t len, uint8_t type, uint8_t value);
std::string detect_serial_port();
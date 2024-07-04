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

bool pathExists(const char *path);
std::string getUsername();
std::vector<std::string> findUSBs(const std::string &userPath);

std::vector<std::string> checkUSB();
void packing_frame_data(uint8_t *output, uint8_t len, uint8_t type, uint8_t value);
std::string detect_serial_port();
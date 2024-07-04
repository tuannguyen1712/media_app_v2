#include <iostream>
#include <string>
#include "utility.h"
#include "view.h"

void clean_stdin(void)
{
    int c;
    do
    {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void clear_terminal()
{
    printf("\033[2J\033[H");
}

std::vector<Media> Screen1::media;
std::string Screen1::playlist;
std::string Screen1::name;
std::vector<std::string> Screen1::usb_path;
int Screen1::volume = 0;
int Screen1::replay = 0;

void Screen1::setMedia(std::vector<std::string> &input)
{
    media.clear();
    for (int i = 0; i < (int)input.size(); i++)
    {
        if (std::filesystem::exists(input[i]))
        {
            media.push_back(GetMedia(input[i]));
        }
        else
        {
            media.push_back(Media("File not exist", "N/A", "N/A", 0, 0, "N/A"));
        }
    }
}

void Screen1::printMedia(const int &page, const int &total_page)
{
    std::cout << std::endl
              << std::setw(4) << std::left << "Num" << "| " << std::setw(TITLE_WIDTH) << std::left << "Name"
              << "| " << std::setw(ARTIRST_WIDTH) << std::left << "Artist"
              << "| " << std::setw(ALBUM_WIDTH) << std::left << "Album"
              << "| " << std::setw(YEAR_WIDTH) << std::left << "Year"
              << "| " << std::setw(EXTENSION_WIDTH) << std::left << "Extension";
    std::cout << "| Duration" << std::endl;
    std::cout << "-------------------------------------------------------------------------------------------------------------------------" << std::endl;
    for (int i = page * FILES_PER_PAGE + 1; i <= (page + 1) * FILES_PER_PAGE && i <= (int)media.size(); i++)
    {
        std::cout << std::setw(4) << std::left << i;
        // std::cout << "| " << std::setw(TITLE_WIDTH) << std::left << media[i - 1].name
        //           << "| " << std::setw(ARTIRST_WIDTH) << std::left << media[i - 1].artist
        //           << "| " << std::setw(ALBUM_WIDTH) << std::left << media[i - 1].album
        //           << "| " << std::setw(YEAR_WIDTH) << std::left << media[i - 1].year
        //           << "| " << std::setw(EXTENSION_WIDTH) << std::left << media[i - 1].extension;
        std::cout << "| " << std::left << left_align(truncate_utf8(media[i - 1].name, TITLE_WIDTH - 2), TITLE_WIDTH)
                  << "| " << std::left << left_align(truncate_utf8(media[i - 1].artist, ARTIRST_WIDTH - 2), ARTIRST_WIDTH)
                  << "| " << std::left << left_align(truncate_utf8(media[i - 1].album, ALBUM_WIDTH - 2), ALBUM_WIDTH)
                  << "| " << std::setw(YEAR_WIDTH) << std::left << media[i - 1].year
                  << "| " << std::setw(EXTENSION_WIDTH) << std::left << media[i - 1].extension;
        printf("| %02d:%02d\n", media[i - 1].duration / 60, media[i - 1].duration % 60);
    }
    std::cout << "-------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Page" << page + 1 << "/" << total_page << std::endl
              << std::endl;
}

void Screen1::set_playlist_name(const std::string &pl_name)
{
    playlist = pl_name;
}
void Screen1::print_playlist_name(const int &index)
{
    std::cout << "[" << index + 1 << "]. " << playlist << std::endl;
}

void Screen1::print_orther()
{
    std::cout << "\n\t\t[<]Previous\t\t[>]Next\t\t[;]Repeat\n";
    std::cout << "----------------------------------------------------------------------------------------------------";
    std::cout << "\n<[B]ack\t\t|<|[R]eturn\t\t[P]lay/[P]ause\t\t|>|[S]kip\t\t[-]Volume[+]" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "Enter your choice: \n";
}

int Screen1::getID()
{
    return id;
}

void Screen_start::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n"); // terminal
    printf("=========================================================================================================================\n");
    printf("\nWELCOME\n");

    if (input)
    {
        std::cout << std::endl;
        std::cout << "Press [`] to back play screen\n";
    }
    printf("\n");
    printf("1. Find media files.\n");
    printf("2. Playlist.\n");
    printf("3. Find from USB.\n");
    printf("4. Exit.\n");
}

std::string Screen_start::getChoice()
{
    std::string opt;
    printf("\n=========================================================================================================================\n");
    std::cout << "Enter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    // std::getline(std::cin, opt);
    // clean_stdin();
    return opt;
}

int Screen_start::getID()
{
    return id;
}

void Screen_find::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n"); // terminal1.1
    printf("=========================================================================================================================\n");
    printf("\n1. Find media files.\n");
    printf("\n");
    printf("Enter directoty path: ");
    printf("\n\n<[B]ack\n");
}

std::string Screen_find::getChoice()
{
    std::string opt;
    printf("\n=========================================================================================================================\n");
    std::cout << "Enter your choice: " << std::endl;
    sleep(3);
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_find::getID()
{
    return id;
}

void Screen_find_result::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n%d. Find media files.\n", input + 1);
    printf("Result\n");
}

std::string Screen_find_result::getChoice()
{
    std::string opt;
    std::cout << "<[B]ack\t\t[<]Previous\t\t[>]Next" << std::endl;
    printf("\n=========================================================================================================================\n");
    std::cout << "Enter your choice:\n ";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_find_result::getID()
{
    return id;
}

void Screen_playlist::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\n\n");
}

std::string Screen_playlist::getChoice()
{
    std::string opt;
    std::cout << "<[B]ack\t\t[A]dd\t\t[D]elete" << std::endl;
    printf("\n=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist::getID()
{
    return id;
}

void Screen_playlist_element::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\n\n");
}

std::string Screen_playlist_element::getChoice()
{
    std::string opt;
    std::cout << "<[B]ack\t\t[A]dd\t\t[D]elete\t\t[R]ename\t\t[<]Previous\t\t[>]Next" << std::endl;
    printf("\n=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_element::getID()
{
    return id;
}

void Screen_playlist_element_add::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\n\n");
}

std::string Screen_playlist_element_add::getChoice()
{
    std::string opt;
    std::cout << "Enter soucre folder: " << std::endl;
    std::cout << "<[B]ack" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_element_add::getID()
{
    return id;
}

void Screen_playlist_element_add_list::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\n\n");
}

std::string Screen_playlist_element_add_list::getChoice()
{
    std::string opt;
    std::cout << "Enter file number you want to add: " << std::endl;
    std::cout << "<[B]ack\t\t[<]Previous\t\t[>]Next\t\t[D]one" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_element_add_list::getID()
{
    return id;
}

void Screen_playlist_add::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\nAdd new playlist\n");
}

std::string Screen_playlist_add::getChoice()
{
    std::string opt;
    std::cout << "Enter playlist name: " << std::endl;
    std::cout << "<[B]ack" << std::endl;
    printf("\n=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_add::getID()
{
    return id;
}

void Screen_playlist_delete::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\nDelete playlist\n");
}

std::string Screen_playlist_delete::getChoice()
{
    std::string opt;
    std::cout << "Enter index of playlist: " << std::endl;
    std::cout << "<[B]ack" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_delete::getID()
{
    return id;
}

void Screen_playlist_element_delete::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\nDelete element in playlist\n");
}

std::string Screen_playlist_element_delete::getChoice()
{
    std::string opt;
    std::cout << "Enter index of element: " << std::endl;
    std::cout << "<[B]ack\t\t[<]Previous\t\t[>]Next" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_element_delete::getID()
{
    return id;
}

void Screen_playlist_rename::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\n2. Playlist.\n");
}

std::string Screen_playlist_rename::getChoice()
{
    std::string opt;
    std::cout << "\nRename playlist\nEnter new name: " << std::endl;
    std::cout << "<[B]ack" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "Enter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_playlist_rename::getID()
{
    return id;
}

void Screen_media_detail::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\nCurrent file: ");
    std::cout << "[" << input + 1 << "]. " << media[input].name << std::endl;
}

std::string Screen_media_detail::getChoice()
{
    std::string opt;
    std::cout << "<[B]ack\t\t|>[P]lay\t\t[R]ename" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_media_detail::getID()
{
    return id;
}

void Screen_media_rename::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\nCurrent file: ");
    // std::cout << name << std::endl;
    std::cout << "[" << input + 1 << "]. " << media[input].name << std::endl;
}

std::string Screen_media_rename::getChoice()
{
    std::string opt;
    std::cout << "Enter new name: \n";
    std::cout << "<[B]ack\t\t" << std::endl;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_media_rename::getID()
{
    return id;
}

void Screen_play_media::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n");
    printf("=========================================================================================================================\n");
    printf("\nCurrent file: ");
    // std::cout << name << std::endl;
    std::cout << "[" << input1 + 1 << "]. " << media[input1].name << std::endl;
    printf("\n\t%02d:%02d/%02d:%02d\n", input / 60, input % 60, media[input1].duration / 60, media[input1].duration % 60);
    // std::cout << input / 60 << ":" << input % 60 << "/" << media.duration / 60 << "/" << media.duration % 60 << std::endl;
    double ratio = (double)input / media[input1].duration;
    int num = (int)(ratio * 50);
    double v_ratio = (double)volume / 128;
    int v_level = (int)(v_ratio * 8);

    std::cout << "\t[";
    for (int i = 1; i < 50; i++)
    {
        if (i <= num || num == 49)
        {
            std::cout << "#";
        }
        else
        {
            std::cout << "-";
        }
    }
    std::cout << "]\n\n";
    std::cout << "\tVolume: " << v_level << std::endl;
    std::cout << "\t[";
    for (int i = 1; i <= 8; i++)
    {
        if (i <= v_level)
        {
            std::cout << "=";
        }
        else
        {
            std::cout << " ";
        }
    }
    std::cout << "]";

    if (replay)
    {
        std::cout << "\t\tRepeat" << std::endl;
    }
    else
    {
        std::cout << std::endl;
    }
}

std::string Screen_play_media::getChoice()
{
    std::string opt;
    // std::getline(std::cin, opt);
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_play_media::getID()
{
    return id;
}

void Screen_usb::display(int input, int input1)
{
    clear_terminal();
    printf("-------------PLAY MEDIA FILE-------------\n"); // terminal1.1
    printf("=========================================================================================================================\n");
    printf("\n3. Find from USB.\n");
    printf("\n");
    std::cout << "Current path: \n";
    for (int i = 0; i < (int)usb_path.size(); i++)
    {
        std::cout << "[" << i + 1 << "]. " << usb_path[i] << std::endl;
    }
    std::cout << std::endl;
    printf("Enter directoty path: ");
    printf("\n\n<[B]ack\n");
}

std::string Screen_usb::getChoice()
{
    std::string opt;
    printf("=========================================================================================================================\n");
    std::cout << "\nEnter your choice: \n";
    get_Choice();
    if (serial_port != -1)
    {
        thread_serial.join();
        //close(serial_port);
    }
    opt = std::string(read_buf);
    // clean_stdin();
    return opt;
}

int Screen_usb::getID()
{
    return id;
}

size_t utf8_strlen(const std::string &str)
{
    // Chuyển đổi từ UTF-8 sang wstring (UTF-32)
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32_str = converter.from_bytes(str);

    // Trả về kích thước của chuỗi UTF-32
    return u32_str.length();
}
// Hàm cắt chuỗi UTF-8 nếu dài hơn độ dài tối đa
std::string truncate_utf8(const std::string &str, size_t max_length)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32_str = converter.from_bytes(str);

    if (u32_str.length() > max_length)
    {
        u32_str = u32_str.substr(0, max_length);
    }

    return converter.to_bytes(u32_str);
}
// Hàm để căn chỉnh trái một chuỗi UTF-8 với độ rộng cố định
std::string left_align(const std::string &str, size_t width)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32_str = converter.from_bytes(str);

    size_t length = u32_str.length();
    if (length >= width)
    {
        return converter.to_bytes(u32_str.substr(0, width));
    }

    std::string result = converter.to_bytes(u32_str);
    result.append(width - length, ' '); // Thêm khoảng trắng cho đủ độ rộng
    return result;
}

int Screen1::Init_Serialport()
{
    std::string port = detect_serial_port();

    if (port.empty())
    {
        if (serial_port != -1)
        {
            close(serial_port);
            return -1;
        }
    }

    std::string port_dir = "/dev/" + port;

    int s_port = open(const_cast<char *>(port_dir.c_str()), O_RDWR);
    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if (tcgetattr(s_port, &tty) != 0)
    // if (serial_port != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE;         // Clear all bits that set the data size
    tty.c_cflag |= CS8;            // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;                                                        // Disable echo
    tty.c_lflag &= ~ECHOE;                                                       // Disable erasure
    tty.c_lflag &= ~ECHONL;                                                      // Disable new-line echo
    tty.c_lflag &= ~ISIG;                                                        // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 0; // Wait for up to (2 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B57600);
    cfsetospeed(&tty, B57600);

    // Save tty settings, also checking for error
    if (tcsetattr(s_port, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }
    return s_port;
}

void Screen1::thread_read_serial_port(void)
{
    uint8_t rx_buf[8];
    memset(rx_buf, 0, sizeof(rx_buf));
    read_buf_cnt = 0;
    while (!rcv_done)
    {
        // std::cout << rcv_done << std::endl;
        uint8_t chr;
        int num_bytes = read(serial_port, &chr, 1);
        if (num_bytes)
        {
            rx_buf[read_buf_cnt] = chr;
            read_buf_cnt++;
            last_rcv = getMillisecondsSinceEpoch();
        }
        if (getMillisecondsSinceEpoch() - last_rcv > 20 && read_buf_cnt >= 3)
        {
            rcv_done = 1;
            serial_data = 1;
            rx_buf[sizeof(rx_buf)] = 0;
        }
    }
    if (serial_data == 1) {
        if (rx_buf[0] == 0)
        {
            mtx.lock();
            sprintf(read_buf, "v%d", rx_buf[1]);
            mtx.unlock();
        }
        else if (rx_buf[0] == 0xF0)
        {
            switch (rx_buf[1])
            {
            case 0x01:
                mtx.lock();
                sprintf(read_buf, "P");
                mtx.unlock();
                break;
            case 0x02:
                mtx.lock();
                sprintf(read_buf, "R");
                mtx.unlock();
                break;
            case 0x03:
                mtx.lock();
                sprintf(read_buf, "S");
                mtx.unlock();
                break;
            case 0x04:
                mtx.lock();
                sprintf(read_buf, ";");
                mtx.unlock();
                break;
            case 0x05:
                mtx.lock();
                sprintf(read_buf, "B");
                mtx.unlock();
                break;
            default:
                break;
            }
        }
        else if (rx_buf[0] == 0xFF)
        {
            mtx.lock();
            sprintf(read_buf, "%d", rx_buf[1]);
            mtx.unlock();
        }
    }
}

int Screen1::read_from_keyboard(char *buff, uint32_t len, uint32_t sec)
{
    struct timeval tv;
    fd_set readfds;
    int retval;

    // Chỉnh timeout là 5 giây
    tv.tv_sec = sec;
    tv.tv_usec = 0;

    // Xóa tất cả các bit của readfds
    FD_ZERO(&readfds);
    // Thêm stdin (fd 0) vào tập hợp readfds
    FD_SET(STDIN_FILENO, &readfds);
    retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    if (retval == -1)
    {
        perror("select()");
        return 0;
    }
    else if (retval)
    {
        // std::cout << "Dữ liệu có sẵn trong stdin trong vòng 5 giây.\n";
        char buffer[256];
        fgets(buffer, len, stdin);
        buffer[strlen(buffer) - 1] = 0;
        if (!strlen(read_buf))
        {
            mtx.lock();
            strcpy(read_buf, buffer);
            mtx.unlock();
        }
        return 1;
    }
    else
    {
        // std::cout << "Không có dữ liệu nhập từ bàn phím trong vòng 1 giây.\n";
        return 0;
    }
}

long long Screen1::getMillisecondsSinceEpoch()
{
    // Lấy thời gian hiện tại
    auto now = std::chrono::system_clock::now();

    // Chuyển đổi thời gian hiện tại sang thời gian tính bằng mili giây kể từ epoch
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    // Trả về giá trị mili giây
    return duration.count();
}

void Screen1::get_Choice()
{
    serial_port = Init_Serialport();

    rcv_done = 0;
    serial_data = 0;

    memset(read_buf, 0, strlen(read_buf));

    if (serial_port != -1)
    {
        //  thread_serial = std::thread(thread_read_serial_port);
        thread_serial = std::thread(&Screen1::thread_read_serial_port, this);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        tcflush(serial_port, TCIOFLUSH);
    }

    while (!rcv_done)
    {
        // std::cout << rcv_done << std::endl;
        if (read_from_keyboard(read_buf, 8, 1)) {
            rcv_done = 1;
            serial_data = 0;
        }
        // std::cout << rcv_done << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    // if (serial_port != -1)
    // {
    //     thread_serial.join();
    //     //close(serial_port);
    // }
}

void Screen1::send_data_to_port(uint8_t type, uint8_t data)
{
    if(serial_port != -1) {
        uint8_t tx[4];
        packing_frame_data(tx, 4, type, data);
        write(serial_port, tx, sizeof(tx) - 1);
    }
} 

// FF FF on 
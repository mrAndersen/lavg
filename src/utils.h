#ifndef UTILS_H
#define UTILS_H

#include "string"
#include "vector"
#include <fstream>
#include "regex"
#include <cstdarg>
#include <thread>
#include <fmt/format.h>
#include <sys/sysinfo.h>


#define DEBUG false
#define DOCKER_MODE true

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define START_TIMER std::chrono::high_resolution_clock::now()
#define END_TIMER_MS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()

struct HttpResponse {
    float requestTime;
    std::string rawResponse;
    int sizeBytes = 0;
    int code = 200;
};

struct Socks5Credentials {
    std::string host;
    std::string login;
    std::string password;
    int port = -1;
};

struct BotCredentials {
    std::string apiKey;
    int64_t chatId = 0;
};

struct SystemLoadAverage {
    std::string raw = "";

    bool executed = false;
    int cores = 0;

    float min1Percentage = 0;
    float min5Percentage = 0;
    float min15Percentage = 0;

    float min1 = 0;
    float min5 = 0;
    float min15 = 0;
};

#define CSIZE(array) sizeof(array) / sizeof(array[0])

void message_ok(const char *fmt, ...);

void message_error(const char *fmt, ...);

SystemLoadAverage read_load_avg();

std::string read_hostname();

std::string implode(const std::vector<std::string> &source, const std::string &delimiter);

std::vector<std::string> explode(const std::string &source, const char &delimiter);

size_t curl_callback(void *contents, size_t size, size_t nmemb, std::string *s);

#endif //UTILS_H

#ifndef UTILS_H
#define UTILS_H

#include "string"
#include "vector"
#include <fstream>
#include "regex"
#include <cstdarg>
#include <thread>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define START_TIMER std::chrono::high_resolution_clock::now()
#define END_TIMER_MS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()

#define CSIZE(array) sizeof(array) / sizeof(array[0])

std::map<std::string, std::string> parse_argv(int argc, char *argv[]);

void message_ok(const char *fmt, ...);

void message_error(const char *fmt, ...);

std::string implode(const std::vector<std::string> &source, const std::string &delimiter);

std::vector<std::string> explode(const std::string &source, const char &delimiter);

size_t curl_callback(void *contents, size_t size, size_t nmemb, std::string *s);

#endif //UTILS_H

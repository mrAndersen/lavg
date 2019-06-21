#include "utils.h"


/**
 *
 * @param source
 * @param delimiter
 * @return
 */
std::vector<std::string> explode(const std::string &source, const char &delimiter) {
    auto result = std::vector<std::string>();
    auto buffer = std::string();

    for (int i = 0; i < source.size(); ++i) {
        auto c = source[i];

        if (c == delimiter) {
            result.emplace_back(buffer);
            buffer = "";
        } else {
            buffer += c;
        }
    }

    if (!buffer.empty()) {
        result.emplace_back(buffer);
    }

    return result;
}

/**
 *
 * @param source
 * @param delimiter
 * @return
 */
std::string implode(const std::vector<std::string> &source, const std::string &delimiter) {
    auto result = std::string();

    for (int i = 0; i < source.size(); ++i) {
        result += source[i];

        if (i != source.size() - 1) {
            result += delimiter;
        }
    }

    return result;
}

/**
 *
 * @param format
 * @param ...
 */
void message_error(const char *fmt, ...) {
    auto now = std::time(nullptr);
    auto cpp_format = std::string(fmt);

    char date[100];
    std::strftime(date, sizeof(date), "%Y-%m-%d %H:%I:%S", std::localtime(&now));

    cpp_format = fmt::format("[{:s}] {:s}[-]{:s} ", date, ANSI_COLOR_RED, ANSI_COLOR_RESET) + cpp_format + "\n";

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, cpp_format.c_str(), args);
    va_end(args);
}

/**
 *
 * @param format
 * @param ...
 */
void message_ok(const char *fmt, ...) {
    auto now = std::time(nullptr);
    auto cpp_format = std::string(fmt);

    char date[100];
    std::strftime(date, sizeof(date), "%Y-%m-%d %H:%I:%S", std::localtime(&now));

    cpp_format = fmt::format("[{:s}] {:s}[+]{:s} ", date, ANSI_COLOR_GREEN, ANSI_COLOR_RESET) + cpp_format + "\n";

    va_list args;
    va_start(args, fmt);

    //Dirty hack for docker
    if (DOCKER_MODE) {
        vfprintf(stderr, cpp_format.c_str(), args);
    } else {
        vfprintf(stdout, cpp_format.c_str(), args);
    }

    va_end(args);
}

size_t curl_callback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t newLength = size * nmemb;

    try {
        s->append((char *) contents, newLength);
    } catch (std::bad_alloc &e) {
        message_error("%s", e.what());
        return 1;
    }

    return newLength;
}

SystemLoadAverage read_load_avg() {
    std::string proc;

    auto now = std::chrono::system_clock::now();
    int cores = get_nprocs();

    if (DOCKER_MODE) {
        proc = "/proc_host/loadavg";
    } else {
        proc = "/proc/loadavg";
    }

    std::ifstream stream;
    std::string data;

    SystemLoadAverage result = {};
    stream.open(proc);

    if (stream.good()) {
        data.assign((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
        stream.close();
    } else {
        message_error("Error while reading %s, are you on linux? Are container volumes ok?", proc.c_str());
        exit(1);
    }

    auto array = explode(data, ' ');

    result.raw = data;
    result.cores = cores;

    result.min1 = std::strtod(array[0].c_str(), nullptr);
    result.min5 = std::strtod(array[1].c_str(), nullptr);
    result.min15 = std::strtod(array[2].c_str(), nullptr);

    result.min1Percentage = result.min1 / cores * 100;
    result.min5Percentage = result.min5 / cores * 100;
    result.min15Percentage = result.min15 / cores * 100;

    result.executed = true;

    if (DEBUG == 1) {
        message_ok(
                "Load avg read %.2f, %.2f, %.2f, %.2f%, %.2f%, %.2f%",
                result.min1,
                result.min5,
                result.min15,
                result.min1Percentage,
                result.min5Percentage,
                result.min15Percentage
        );
    }

    return result;
}

std::string read_hostname() {
    std::string proc;

    if (DOCKER_MODE) {
        proc = "/proc_host/sys/kernel/hostname";
    } else {
        proc = "/proc/sys/kernel/hostname";
    }

    std::ifstream stream;
    std::string data;

    stream.open(proc);

    if (stream.good()) {
        data.assign((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
        stream.close();
    } else {
        message_error("Error while reading %s, are you on linux? Are container volumes ok?", proc.c_str());
        exit(1);
    }

    data = std::regex_replace(data, std::regex("\n"), "");
    return data;
}

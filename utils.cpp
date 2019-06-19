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

    vfprintf(stdout, cpp_format.c_str(), args);
    va_end(args);
}

size_t curl_callback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t
    newLength = size * nmemb;

    try {
        s->append((char *) contents, newLength);
    }
    catch (std::bad_alloc &e) {
        cm2_message_error("%s", e.what());
        return 1;
    }

    return newLength;
}

/**
 *
 * @param argv
 * @return
 */
std::map<std::string, std::string> parse_argv(int argc, char **argv) {
    std::vector<LiveMode> availableModes = {
            LiveMode::MODE_DAEMON,
            LiveMode::MODE_SportradarGlobalIceHockeyV1,
            LiveMode::MODE_SportradarSoccerV3,
            LiveMode::MODE_SportradarTennisV2
    };

    auto result = std::map<std::string, std::string>();
    std::vector<std::string> arguments(argv, argv + argc);

    for (auto &argument:arguments) {
        auto e = explode(argument, '=');

        if (e.size() != 2) {
            continue;
        }

        auto name = e[0];
        auto value = e[1];

        name = regex_replace(name, std::regex(R"(-|\s)"), "");

        if (name == "mode" && !vector_contains(availableModes, static_cast<LiveMode >(std::stoi(value)))) {
            cm2_message_error("Mode %s does not exists", value.c_str());
            exit(1);
        }

        result.insert(std::pair(name, value));
    }

    if (result["mode"].empty()) {
        result["mode"] = std::to_string(static_cast<int>(LiveMode::MODE_SportradarSoccerV3));
    }

    return result;
}



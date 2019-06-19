#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>

#define VERBOSITY 1

struct SystemLoadAverage {
    std::string raw = "";
    bool executed = false;

    float min1 = 0;
    float min5 = 0;
    float min15 = 0;
};

struct HttResponse {
    std::map<std::string, std::string> headers;
    std::string body;
    int code = 0;
};

HttResponse request(const std::string &host, const int &port, const std::string &uri) {
    HttResponse result = {};

    int sock = 0;
    int sockConnection = 0;
    int sockSent = 0;
    int sockRead = 0;

    struct sockaddr_in serverInfo = {};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n Code %d", sock);
        return result;
    }

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serverInfo.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported \n");
        return result;
    }

    if ((sockConnection = connect(sock, (struct sockaddr *) &serverInfo, sizeof(serverInfo))) < 0) {
        printf("Connection Failed\n Code %d", sockConnection);
        return result;
    }

    std::string httpMessage = "GET / HTTP/1.1\nHost: localhost\nConnection: close\n\n";

    if ((sockSent = send(sock, httpMessage.c_str(), httpMessage.size(), 0)) < 0) {
        printf("Sending http request failed\n Code %d", sockSent);
        return result;
    } else {
        if (VERBOSITY == 1) {
            printf("Sent %d characters to %s:%d\n", sockSent, host.c_str(), port);
        }
    }

    std::string rawResult;
    char buf[1024];

    while (read(sock, buf, 1024) != 0) {
        rawResult += buf;
    }

    return result;
}

const std::string currentDateTime() {
    time_t now = time(nullptr);
    struct tm tstruct = {};

    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::vector<std::string> split(const std::string &source, const char &delimiter) {
    std::vector<std::string> vbuf = {};
    std::string buf;

    for (char c : source) {
        if (c == delimiter || c == '\000' || c == '\n') {
            vbuf.emplace_back(buf);
            buf = "";
        } else {
            buf += c;
        }
    }

    return vbuf;
}

void sendTgMessage(const int64_t &chatId, const std::string &message) {

}

SystemLoadAverage readLoadAvg() {
    auto now = std::chrono::system_clock::now();
    std::string proc = "/proc/loadavg";
    std::ifstream stream;
    std::string data;
    SystemLoadAverage result = {};

    stream.open("/proc/loadavg");

    if (stream.good()) {
        data.assign((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
    } else {
        printf("Error while reading %s, are you on linux os?", proc.c_str());
        return result;
    }

    auto array = split(data, ' ');

    result.raw = data;
    result.min1 = std::strtod(array[0].c_str(), nullptr);
    result.min5 = std::strtod(array[1].c_str(), nullptr);
    result.min15 = std::strtod(array[2].c_str(), nullptr);
    result.executed = true;

    if (VERBOSITY == 1) {
        printf(
                "[%s] Load avg read %.2f, %.2f, %.2f\n",
                currentDateTime().c_str(),
                result.min1,
                result.min5,
                result.min15
        );
    }

    return result;
}


int main() {
//    auto avg = readLoadAvg();
    sendTgMessage(0, "");
}
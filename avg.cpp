#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/utils.h"
#include "src/Http.h"

int lastMessageSentUnixTime = 0;
int maxNotificationIntervalSeconds = 120;

void sendTgMessage(
        const std::string &tgMessage,
        const std::string &socks5Credentials,
        const BotCredentials &botCredentials = {}
) {
    if (botCredentials.chatId == 0) {
        return;
    }

    auto http = new Http();
    http->setSocks5Credentials(socks5Credentials);

    std::string encodedTgMessage(curl_easy_escape(nullptr, tgMessage.c_str(), tgMessage.length()));
    std::string url = fmt::format(
            "https://api.telegram.org/bot{}/sendMessage?chat_id={}&text={}",
            botCredentials.apiKey,
            botCredentials.chatId,
            encodedTgMessage
    );

    auto response = http->request(url);
}

void sendTgAlert(
        const float maxLoadAllowed,
        const SystemLoadAverage &systemLoadAverage,
        const std::string &socks5Credentials,
        const BotCredentials &botCredentials = {}
) {
    int64_t unixTime = (unsigned long) time(NULL);

    if (lastMessageSentUnixTime + maxNotificationIntervalSeconds > unixTime) {
        return;
    }

    std::string hostname = read_hostname();
    std::string tgMessage = fmt::format(
            "[Warning] load average {} on server {} is {:.2f}% ({:.2f}% max allowed)",
            "5 min",
            hostname,
            systemLoadAverage.min5Percentage,
            maxLoadAllowed
    );

    sendTgMessage(tgMessage, socks5Credentials, botCredentials);
    lastMessageSentUnixTime = unixTime;
}

int main() {
    float max5minLoadPercentage = 60.0;
    int monitoringPeriodSeconds = 10;

    if (getenv("MAX_LOAD") != nullptr) {
        max5minLoadPercentage = atof(getenv("MAX_LOAD"));
    }

    if (getenv("TG_KEY") == nullptr || getenv("TG_CHAT") == nullptr) {
        message_error(
                "Please specify TG_KEY and TG_CHAT env variables for telegram bot api key, and chat id respectively");
        exit(1);
    }

    BotCredentials botCredentials;
    botCredentials.apiKey = std::string(getenv("TG_KEY"));
    botCredentials.chatId = atoi(getenv("TG_CHAT"));

    std::string socks5Credentials;
    auto hostname = read_hostname();

    if (getenv("TG_SOCKS5_PROXY") != nullptr) {
        socks5Credentials = getenv("TG_SOCKS5_PROXY");
    }

    std::string welcomeMessage = fmt::format(
            "lavg started monitoring, interval = {:d} seconds, max load average value = {:d}%, hostname = {}",
            monitoringPeriodSeconds,
            (int) max5minLoadPercentage,
            hostname
    );

    message_ok(welcomeMessage.c_str());
    sendTgMessage(welcomeMessage, socks5Credentials, botCredentials);

    while (true) {
        auto avg = read_load_avg();

        if (avg.min5Percentage > max5minLoadPercentage) {
            sendTgAlert(max5minLoadPercentage, avg, socks5Credentials, botCredentials);
            message_ok("5 min average exceeded (max %.2f%% allowed)", max5minLoadPercentage);
        }

        sleep(monitoringPeriodSeconds);
    }
}
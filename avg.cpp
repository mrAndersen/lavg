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
int maxNotificationIntervalSeconds = 60;

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
    auto http = new Http();

    http->setSocks5Credentials(socks5Credentials);
    std::string tgMessage = fmt::format(
            "[Warning] load average {} on server {} is {:.2f}% ({:.2f}% max allowed)",
            "5 min",
            hostname,
            systemLoadAverage.min5Percentage,
            maxLoadAllowed
    );

    std::string encodedTgMessage(curl_easy_escape(nullptr, tgMessage.c_str(), tgMessage.length()));
    std::string url = fmt::format(
            "https://api.telegram.org/bot{}/sendMessage?chat_id={}&text={}",
            botCredentials.apiKey,
            botCredentials.chatId,
            encodedTgMessage
    );

    auto response = http->request(url);
    lastMessageSentUnixTime = unixTime;
}

int main() {
    float max5minLoadPercentage = 5.0;
    int monitoringPeriodSeconds = 3;

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

    if (getenv("TG_SOCKS5_PROXY") != nullptr) {
        socks5Credentials = getenv("TG_SOCKS5_PROXY");
    }

    message_ok(
            "lavg started monitoring interval = %d seconds, max load average value = %d%",
            monitoringPeriodSeconds,
            (int) max5minLoadPercentage
    );

    while (true) {
        auto avg = read_load_avg();

        if (avg.min5Percentage > max5minLoadPercentage) {
            sendTgAlert(max5minLoadPercentage, avg, socks5Credentials, botCredentials);
            message_ok("5 min average exceeded (max %.2f%% allowed)", max5minLoadPercentage);
        }

        sleep(monitoringPeriodSeconds);
    }
}
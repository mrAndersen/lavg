#include "Http.h"

HttpResponse Http::request(const std::string &url) {
    auto result = std::string();
    auto start = START_TIMER;

    if (curl == nullptr) {
        curl = curl_easy_init();
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

    if (!socks5Credentials.empty()) {
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        curl_easy_setopt(curl, CURLOPT_PROXY, socks5Credentials.c_str());
    }

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        message_error("Error making request: %s", curl_easy_strerror(res));
    }

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    HttpResponse response;

    response.rawResponse = result;
    response.requestTime = END_TIMER_MS;
    response.sizeBytes = (int) (result.size() * sizeof(char));
    response.code = (int) response_code;

    if (DEBUG) {
        message_ok(
                "%s [c=%d, t=%.2fms, size=%.2fkb]",
                url.c_str(),
                response.code,
                response.requestTime,
                ((float) response.sizeBytes / 1024)
        );

        if (response.code != 200) {
            message_ok(
                    "%d\n%s",
                    response.code,
                    response.rawResponse.c_str()
            );
        }
    }

    if (response.code >= 400) {
        message_error(
                "%d\n%s",
                response.code,
                response.rawResponse.c_str()
        );
    }

    return response;
}

/**
 *
 */
Http::~Http() {
    curl_easy_cleanup(curl);
    res = CURLE_OK;
}

const std::string &Http::getSocks5Credentials() const {
    return socks5Credentials;
}

void Http::setSocks5Credentials(const std::string &socks5Credentials) {
    Http::socks5Credentials = socks5Credentials;
}

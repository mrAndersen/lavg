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

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cm2_message_error("Error making request: %s", curl_easy_strerror(res));
    }

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    HttpResponse response;

    response.rawResponse = result;
    response.requestTime = END_TIMER_MS;
    response.sizeBytes = (int) (result.size() * sizeof(char));
    response.code = (int) response_code;

    if (verbose) {
        cm2_message_ok(
                "%s [c=%d, t=%.2fms, size=%.2fkb]",
                url.c_str(),
                response.code,
                response.requestTime,
                ((float) response.sizeBytes / 1024)
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

/**
 *
 * @return
 */
bool Http::isVerbose() const {
    return verbose;
}

/**
 *
 * @param verbose
 */
void Http::setVerbose(bool verbose) {
    Http::verbose = verbose;
}

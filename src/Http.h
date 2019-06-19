#ifndef CM2_REALTIME_HTTP_H
#define CM2_REALTIME_HTTP_H

#include "curl/curl.h"
#include "string"
#include "vector"
#include "map"
#include "time.h"
#include "../utils.h"
#include "mutex"

struct HttpResponse {
    float requestTime;
    std::string rawResponse;
    int sizeBytes = 0;
    int code = 200;
};

class Http {
protected:
    CURL *curl = nullptr;
    CURLcode res;

    bool verbose = true;

public:
    HttpResponse request(const std::string &url);

    virtual ~Http();

    bool isVerbose() const;

    void setVerbose(bool verbose);
};


#endif //CM2_REALTIME_HTTP_H

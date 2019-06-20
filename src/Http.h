#ifndef HTTP_H
#define HTTP_H

#include "curl/curl.h"
#include "string"
#include "vector"
#include "map"
#include "time.h"
#include "utils.h"

class Http {
protected:
    CURL *curl = nullptr;
    CURLcode res;

    std::string socks5Credentials;
public:
    HttpResponse request(const std::string &url);

    virtual ~Http();

    const std::string &getSocks5Credentials() const;

    void setSocks5Credentials(const std::string &socks5Credentials);
};


#endif //HTTP_H

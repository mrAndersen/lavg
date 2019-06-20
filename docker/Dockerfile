FROM debian:buster-slim

RUN echo "deb http://mirror.yandex.ru/debian buster main \
          deb http://mirror.yandex.ru/debian buster-updates main \
          deb http://security.debian.org/debian-security buster/updates main" > /etc/apt/sources.list

RUN apt update \
    && apt install -y libcurl4-openssl-dev cmake gcc g++ make

RUN mkdir /lavg

COPY ./src /lavg/src
COPY ./vendor /lavg/vendor
COPY ./avg.cpp /lavg/avg.cpp
COPY ./CMakeLists.txt /lavg/CMakeLists.txt

WORKDIR /lavg

RUN rm -rf /lavg/build
RUN cmake -B /lavg/build . && cd /lavg/build && make -j 8
RUN ls -lah /lavg/build | grep loadAvg

CMD /lavg/build/loadAvg
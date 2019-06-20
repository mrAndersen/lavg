###To run container
docker run -d \
    --name=load_avg \
    -e TG_KEY=<YOUR BOT API KEY> \
    -e TG_CHAT=<CHAT ID> \
    -e TG_SOCKS5_PROXY=<Socks5 proxy if tg is blocked in your country> \
    -v /proc:/lavg/proc \
    mrandersen7/lavg
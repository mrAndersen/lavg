Usage
```
docker run -d \
    --privileged \
    --net=host \
    --name=load_avg \
    -e TG_KEY=<YOUR BOT API KEY> \
    -e TG_CHAT=<CHAT ID> \
    -e TG_SOCKS5_PROXY=<Socks5 proxy if tg is blocked in your country> \
    -e MAX_LOAD=<Max percentage of load allowed> \
    -v /proc:/proc_host \
    mrandersen7/lavg
```
    
Running loadAvg daemon requires privileged access and host networking mode for docker. 
Also you can set MAX_LOAD env variable to control max load average notification in percents.

For correct work make sure that your bot has access to your chat (or group chat).
Notifications will be sent at most every 120 seconds if load will continue on for longer periods than 120 seconds.

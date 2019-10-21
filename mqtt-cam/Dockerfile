#Minimal Ubuntu!
FROM ubuntu:18.04 

RUN apt-get update && \
    apt-get install mosquitto-clients ffmpeg -y

COPY autorecorder.sh /autorecorder.sh

RUN chmod +x /autorecorder.sh

CMD /autorecorder.sh

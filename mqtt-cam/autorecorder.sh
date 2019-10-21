#! /bin/bash

if [ -z "${STREAMURL}" ]; then
    echo "STREAMURL is unset or set to an empty string"
    exit
fi
if [ -z "${MQTTHOST}" ]; then
    echo "MQTTHOST is unset or set to an empty string"
    exit
fi
if [ -z "${MQTTPORT}" ]; then
    echo "MQTTPORT is unset or set to an empty string"
    exit
fi
if [ -z "${MQTTUSER}" ]; then
    echo "MQTTUSER is unset or set to an empty string"
    exit
fi
if [ -z "${MQTTPASS}" ]; then
    echo "MQTTPASS is unset or set to an empty string"
    exit
fi
if [ -z "${MQTTTOPIC}" ]; then
    echo "MQTTTOPIC is unset or set to an empty string"
    exit
fi

while read MSG
do
   DT=`date +"%Y%m%d-%H%M%S"`
   echo $MSG
   ffmpeg -i $STREAMURL -acodec copy -vcodec copy -t 10 /data/recording-$DT.mp4 
done < <(mosquitto_sub -h $MQTTHOST -p $MQTTPORT -u $MQTTUSER -P $MQTTPASS -t $MQTTTOPIC -q 1)

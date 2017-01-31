#!/usr/bin/env bash

PORT_HOST=80

NGINX_1_11_9="nginx-1.11.9"
NGINX_1_11_9_PORT=8080

NGINX_1_10_2="nginx-1.10.2"
NGINX_1_10_2_PORT=8081

APACHE_2_2_32="apache-2.2.32"
APACHE_2_2_32_PORT=8082

APACHE_2_4_25="apache-2.4.25"
APACHE_2_4_25_PORT=8083

SERVERS=`echo ${NGINX_1_11_9} ${NGINX_1_10_2} ${APACHE_2_2_32} ${APACHE_2_4_25}`
PORTS=`echo ${NGINX_1_11_9_PORT} ${NGINX_1_10_2_PORT} ${APACHE_2_2_32_PORT} ${APACHE_2_4_25_PORT}`

declare -a arr=(${SERVERS})
declare -a arr2=(${PORTS})

LOG=`echo $(dirname "$0")/docker.log`

function start_servers {
    docker run -d -p ${NGINX_1_11_9_PORT}:${PORT_HOST} --name ${NGINX_1_11_9} nginx:1.11.9
    docker run -d -p ${NGINX_1_10_2_PORT}:${PORT_HOST} --name ${NGINX_1_10_2} nginx:1.10.2

    docker run -d -p ${APACHE_2_2_32_PORT}:${PORT_HOST} --name ${APACHE_2_2_32} httpd:2.2.32
    docker run -d -p ${APACHE_2_4_25_PORT}:${PORT_HOST} --name ${APACHE_2_4_25} httpd:2.4.25
}

function stop_servers {
    docker stop ${SERVERS}
}

function remove_servers {
    docker rm   ${SERVERS}
}

function print_mapping {
    printf "%s\t%s\t\t\t%s\n" "index" "container" "port"
    echo "------------------------------------"

    for i in "${!arr[@]}"
    do
       printf "%s\t\t%s\t\t%d->%d\n" "$i" "${arr[$i]}" "$PORT_HOST" "${arr2[$i]}"
       # or do whatever with individual element of the array
    done
}

if [ $# -ne 1 ];
    then >&2 printf "$0 \nparameters:\n- run"
    exit 1
fi

if [ $1="run" ]; then
    printf "%s\n" "starting docker containers..."

    stop_servers    > $LOG
    remove_servers  > $LOG
    start_servers   > $LOG
    print_mapping
fi
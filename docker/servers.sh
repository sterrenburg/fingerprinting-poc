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

LIGHTTPD_1_4_39="lighttpd-1.4.39"
LIGHTTPD_1_4_39_PORT=8084

LOG=`echo $(dirname "$0")/docker.log`

SERVERS=`echo ${NGINX_1_11_9} ${NGINX_1_10_2} ${APACHE_2_2_32} ${APACHE_2_4_25} ${LIGHTTPD_1_4_39}`
PORTS=`echo ${NGINX_1_11_9_PORT} ${NGINX_1_10_2_PORT} ${APACHE_2_2_32_PORT} ${APACHE_2_4_25_PORT} ${LIGHTTPD_1_4_39_PORT}`

declare -a servers_split=(${SERVERS})
declare -a ports_split=(${PORTS})

function start_servers {
    docker run -d -p ${NGINX_1_11_9_PORT}:${PORT_HOST} --name ${NGINX_1_11_9} nginx:1.11.9
    docker run -d -p ${NGINX_1_10_2_PORT}:${PORT_HOST} --name ${NGINX_1_10_2} nginx:1.10.2

    docker run -d -p ${APACHE_2_2_32_PORT}:${PORT_HOST} --name ${APACHE_2_2_32} httpd:2.2.32
    docker run -d -p ${APACHE_2_4_25_PORT}:${PORT_HOST} --name ${APACHE_2_4_25} httpd:2.4.25

    docker run -d -p ${LIGHTTPD_1_4_39_PORT}:${PORT_HOST} --name ${LIGHTTPD_1_4_39} sebp/lighttpd
}

function stop_servers {
    docker stop ${SERVERS}
}

function remove_servers {
    docker rm   ${SERVERS}
}

function print_mapping {
    printf "%s\t\t%s\t\t%s\n" "index" "container" "port"
    echo "------------------------------------"

    for i in "${!servers_split[@]}"
    do
       printf "%s\t\t%s\t\t%d->%d\n" "$i" "${servers_split[$i]}" "$PORT_HOST" "${ports_split[$i]}"
       # or do whatever with individual element of the array
    done
}

function print_help {
    >&2 printf "parameters:\n- start\n- stop\n- remove\n- list\n- info\n"
}

function print_info {
    SERVERS_OR=''

    for i in "${!servers_split[@]}"
    do
       SERVERS_OR="$SERVERS_OR${servers_split[$i]}|"
    done

    SERVERS_REGEX="CONTAINER|${SERVERS_OR::-1}"

    OUTPUT="$(docker ps -a | grep -E $SERVERS_REGEX)"
    echo "${OUTPUT}"
}
if [ $# -ne 1 ]; then
    print_help
    exit 1
fi

if [ $1 == "start" ]; then
    printf "%s\n" "starting docker containers..."

    stop_servers    > $LOG 2>&1
    remove_servers  > $LOG 2>&1
    start_servers   > $LOG 2>&1
    print_info
elif [ $1 == "stop" ]; then
    printf "%s\n" "stopping docker containers..."
    stop_servers    > $LOG 2>&1
elif [ $1 == "remove" ]; then
    printf "%s\n" "removing docker containers..."
    stop_servers    > $LOG 2>&1
    remove_servers  > $LOG 2>&1
elif [ $1 == "list" ]; then
    print_mapping
elif [ $1 == "info" ]; then
    print_info
else
    print_help
    exit 1
fi
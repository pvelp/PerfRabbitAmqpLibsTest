#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/lib/libSimpleAmqpClient.so.7

help=$1
size=${3:-5120}
capacity=${4:-10000}
name=${2:-read_queue}


if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    echo $1
    echo
    ./build/PerformanceTest --help
    exit 0
fi

java -jar perf-test-latest.jar -h amqp://rabbitmq:rabbitmq@localhost -y0 -p -u $name -s $size -C $capacity --id "test-14" -f persistent

./build/PerformanceTest

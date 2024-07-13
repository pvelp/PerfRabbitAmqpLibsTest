#!/bin/bash

size=$2
capaity=$3
name=$1

java -jar perf-test-latest.jar -h amqp://rabbitmq:rabbitmq@localhost -y0 -p -u $name -s $size -C $capaity --id "test-14" -f persistent


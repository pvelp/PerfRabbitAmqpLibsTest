#!/bin/bash

host="localhost"
login="rabbitmq"
pass="rabbitmq"
read_queue_name="read_queue"
write_queue_name="write_queue"
prefetch_message_count=1
capacity=100000
size=5120
type=false

if [ "$1" == "--help" ]; then    
    ./build/PerformanceTest --help
    exit 0
fi

while getopts ":h:l:p:o:i:e:c:s:f" opt; do
  case $opt in
    h) host=$OPTARG ;;
    l) login=$OPTARG ;;
    p) pass=$OPTARG ;;
    o) read_queue_name=$OPTARG ;;
    i) write_queue_name=$OPTARG ;;
    e) prefetch_message_count=$OPTARG ;;
    c) capacity=$OPTARG ;;
    s) size=$OPTARG ;;
    f) type=true ;;
    \?) echo "Invalid option: -$OPTARG" >&2
    ;;
  esac
done

export LD_LIBRARY_PATH=/usr/local/lib

java -jar perf-test-latest.jar -h amqp://$login:$pass@$host -y0 -p -u $read_queue_name --auto-delete false -s $size -C $capacity --id "perf_test" -f persistent

perf_test_cmd="./build/PerformanceTest -h $host -l $login -p $pass -o $read_queue_name -i $write_queue_name -e $prefetch_message_count -c $capacity -s $size"

if [ "$type" == true ]; then
  perf_test_cmd="$perf_test_cmd -f"
fi

echo "RUN: $perf_test_cmd"
$perf_test_cmd

FROM ubuntu:22.04

ARG IMAGE_VERSION="latest"

COPY . /performance_test

RUN apt update && apt upgrade -y \
    && apt install -y software-properties-common && add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && apt install -y ca-certificates g++-13 make default-jre build-essential libssl-dev lld cmake gdb dh-autoreconf \
        libboost-chrono-dev libboost-system-dev libboost-program-options-dev \
        doctest-dev libjsoncpp-dev librabbitmq-dev zlib1g-dev \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 --slave /usr/bin/g++ g++ /usr/bin/g++-13 \
    && rm -rf /var/lib/apt/lists/*

RUN cd /performance_test/common/rabbitmq-c && rm -rf build \
    && cmake -B build . && cmake --build build --target install \
    && cd /performance_test 

RUN cd /performance_test/common/SimpleAmqpClient && rm -rf build \
    && cmake -B build . && cmake --build build --target install \
    && cd /performance_test


RUN cd /performance_test && rm -rf build && cmake -B build . \
    && cmake --build build && cd /performance_test

WORKDIR /performance_test
ENTRYPOINT ["bash", "-c", "export LD_LIBRARY_PATH=/usr/local/lib; ./run.sh \"$@\"", "-" ]



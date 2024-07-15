FROM ubuntu:22.04

ARG IMAGE_VERSION="latest"

WORKDIR /app
COPY . /app

RUN apt update && apt upgrade -y \
    && apt install -y software-properties-common && add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && apt install -y ca-certificates g++-13 make cmake gdb dh-autoreconf \
        libboost-chrono-dev libboost-system-dev libboost-program-options-dev \
        doctest-dev libjsoncpp-dev librabbitmq-dev zlib1g-dev \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 --slave /usr/bin/g++ g++ /usr/bin/g++-13 \
    && rm -rf /var/lib/apt/lists/*


RUN cd /app/common/SimpleAmqpClient && rm -rf build && mkdir build \
    && cmake -B build . && cmake --build build --target install \ 
    && cd /app 

RUN cp /usr/local/lib/libSimpleAmqpClient.so* bin/libs

RUN cd /app && rm -rf build && mkdir build && cmake -B build . \
    && cmake --build build && cd /app/build

CMD ["./PerformanceTest"]
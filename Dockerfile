FROM alpine:3.7 as builder
MAINTAINER Rashad Kamsheh gusalkara@student.gu.se

RUN apk update && \
    apk --no-cache add \
        ca-certificates \
        cmake \
        g++ \
        make && \
    apk add libcluon --no-cache --repository https://chrberger.github.io/libcluon/alpine/v3.7 --allow-untrusted
ADD . examples/
WORKDIR examples/
RUN cd examples/ && \
    rm -r build && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release .. && \
    make && make test

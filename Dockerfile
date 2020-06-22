FROM ubuntu:20.04
LABEL maintainer="Dale Alleshouse dalealleshouse@gmail.com"

RUN apt-get -y update && apt-get install -y \
    clang clang-format clang-tidy llvm lcov lldb libcunit1 libcunit1-doc \
    libcunit1-dev build-essential bear python3-pip && \
    apt-get upgrade -y && \
    apt-get clean

RUN mkdir build/ && \
    mkdir build/src

RUN pip3 install cpplint

VOLUME ["/build"]

WORKDIR /build

CMD ["make", "-B"]

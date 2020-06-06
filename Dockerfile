FROM ubuntu:20.04
LABEL maintainer="Dale Alleshouse dalealleshouse@gmail.com"

RUN apt-get -y update && apt-get install -y \
    clang clang-format clang-tidy llvm lcov libcunit1 libcunit1-doc \
    libcunit1-dev build-essential bear python3-pip && \
    apt-get upgrade -y && \
    apt-get clean

RUN mkdir build/ && \
    mkdir build/src

RUN pip3 install cpplint

VOLUME ["/build/src"]

WORKDIR /build

COPY Makefile /build/Makefile
COPY validate.sh /build/validate.sh
COPY coverage.sh /build/coverage.sh
COPY .clang-format /build/.clang-format
COPY .clang-tidy /build/.clang-tidy
COPY CPPLINT.cfg /build/CPPLINT.cfg
COPY llvm-gcov.sh /build/llv-gcov.sh

CMD ["make", "-B"]

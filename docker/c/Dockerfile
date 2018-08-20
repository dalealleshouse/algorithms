# Downgraded from ubuntu:18.10 b/c of clang bug
FROM ubuntu:18.04
LABEL maintainer="Dale Alleshouse dalealleshouse@gmail.com"

RUN apt-get -y update && apt-get install -y \
    clang-6.0 llvm libcunit1 libcunit1-doc libcunit1-dev build-essential && \
    apt-get upgrade -y && \
    rm -rf /var/lib/apt/lists/* && \
    mkdir /src

VOLUME ["/src"]

WORKDIR /

COPY Makefile /Makefile
COPY validate.sh /validate.sh
COPY coverage.sh /coverage.sh

CMD ["make", "-B"]

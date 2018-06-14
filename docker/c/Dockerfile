FROM ubuntu:18.10
LABEL maintainer="Dale Alleshouse dalealleshouse@gmail.com"

RUN apt-get -y update && apt-get install -y \
    clang-6.0 libcunit1 libcunit1-doc libcunit1-dev build-essential && \
    apt-get upgrade -y && \
    rm -rf /var/lib/apt/lists/* && \
    mkdir /src

VOLUME ["/src"]

WORKDIR /

COPY Makefile /Makefile

CMD ["make"]

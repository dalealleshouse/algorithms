FROM ubuntu:18.10
LABEL maintainer="Dale Alleshouse dalealleshouse@gmail.com"

RUN apt-get -y update && apt-get install -y \
    python3 python3-pip && \
    apt-get upgrade -y && \
    rm -rf /var/lib/apt/lists/*

VOLUME ["/src"]

WORKDIR /src

COPY requirements.txt /src/requirements.txt

RUN pip3 install -r requirements.txt

CMD ["pytest", "-v"]

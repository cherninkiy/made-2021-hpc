FROM gcc:9.3.0-buster

RUN apt-get update
RUN apt-get install -y libblas-dev

ADD . /app

WORKDIR /app/src

ENTRYPOINT ["/bin/bash", "./tests.sh", "docker"]
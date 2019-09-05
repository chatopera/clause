FROM ubuntu:16.04
MAINTAINER Hai Liang Wang <hain@chatopera.com>

ARG VCS_REF

LABEL org.label-schema.vcs-ref=$VCS_REF \
          org.label-schema.vcs-url="https://github.com/chatopera/clause"

# BUILD WITH CN MACHINE
COPY ./config/aliyun.source.list /etc/apt/sources.list

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/app/lib:/app/lib/x86_64-linux-gnu

# Set timezone
RUN apt-get update && \
   apt-get install --no-install-recommends -y tzdata && \ 
   ln -sf /usr/share/zoneinfo/Asia/Shanghai  /etc/localtime && \
   dpkg-reconfigure -f noninteractive tzdata

# Set the locale
ENV LANG C.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL C.UTF-8

RUN apt-get install --no-install-recommends -y libpthread-stubs0-dev \ 
   uuid-dev libcurl4-openssl-dev libssl-dev libhiredis-dev vim \ 
   iputils-ping wget liblbfgs-dev && rm -rf /var/lib/apt/lists/*

RUN mkdir /config
COPY ./tmp/app /app
COPY ./config/server/cmd.flags.docker /config/cmd.flags

WORKDIR /app
expose 8056
CMD ["/app/clause_server", "--flagfile=/config/cmd.flags"]


FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y pkg-config build-essential ninja-build automake autoconf libtool wget curl git gcc libssl-dev bc slib squashfs-tools android-sdk-libsparse-utils jq python3-distutils scons parallel tree python3-dev python3-pip device-tree-compiler ssh cpio fakeroot libncurses5 flex bison libncurses5-dev genext2fs rsync unzip dosfstools mtools tcl openssh-client cmake expect python-is-python3 xxd vim-common libgtest-dev && rm -rf /var/lib/apt/lists/*

RUN pip install jinja2

RUN mkdir -p /opt/eigen && \
    curl -L https://gitlab.com/libeigen/eigen/-/archive/3.3.9/eigen-3.3.9.tar.gz | \
    tar xz --strip-components=1 -C /opt/eigen

ENV Eigen3_DIR=/opt/eigen

RUN mkdir -p /usr/src/gtest && \
    cd /usr/src/gtest && \
    cmake /usr/src/googletest && \
    make && \
    cp lib/*.a /usr/lib

ARG USERNAME=thuyen
ARG UID=1000
ARG GID=1000

RUN groupadd -g ${GID} ${USERNAME} && \
    useradd -m -u ${UID} -g ${GID} -s /bin/bash ${USERNAME} && \
    usermod -aG sudo ${USERNAME}

USER ${USERNAME}
WORKDIR /home/${USERNAME}

CMD [ "bash" ]

FROM ubuntu:24.04

ARG TARGET=x86_64-w64-mingw32
ARG PREFIX=/usr/local
ARG BINUTILS_VERSION=2.45
ARG MINGW_VERSION=13.0.0
ARG CMAKE_VERSION=4.2.1

ENV PATH="${PREFIX}/bin:${PATH}"

WORKDIR /mnt

RUN ln -sf /bin/bash /bin/sh

RUN set -ex \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
        ca-certificates \
        git \
        wget \
        curl \
        build-essential \
        make \
        ninja-build \
        python3 \
        xz-utils \
        bzip2 \
        patch \
        flex \
        bison \
        texinfo \
        gperf \
        file \
        zlib1g-dev \
        libgmp-dev \
        libmpfr-dev \
        libmpc-dev \
        libisl-dev \
    && rm -rf /var/lib/apt/lists/*

# binutils
RUN wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && mkdir build-binutils \
    && cd build-binutils \
    && ../binutils-${BINUTILS_VERSION}/configure \
        --prefix=${PREFIX} \
        --target=${TARGET} \
        --disable-shared \
        --enable-static \
        --disable-multilib \
        --disable-nls \
        --disable-werror \
        --with-system-zlib \
    && make -j"$(nproc)" \
    && make install \
    && cd .. \
    && rm -rf build-binutils binutils-${BINUTILS_VERSION}

# mingw-w64 headers
RUN wget -q https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2 -O - | tar -xj \
    && mkdir build-mingw-headers \
    && cd build-mingw-headers \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-headers/configure \
        --prefix=${PREFIX}/${TARGET} \
        --host=${TARGET} \
        --enable-sdk=all \
    && make install \
    && cd .. \
    && rm -rf build-mingw-headers

# gcc trunk, stage 1
RUN git clone --depth=1 https://gcc.gnu.org/git/gcc.git gcc-trunk \
    && cd gcc-trunk \
    && ./contrib/download_prerequisites \
    && cd .. \
    && mkdir build-gcc \
    && cd build-gcc \
    && ../gcc-trunk/configure \
        --prefix=${PREFIX} \
        --target=${TARGET} \
        --enable-languages=c,c++ \
        --enable-threads=posix \
        --disable-multilib \
        --disable-nls \
        --disable-werror \
        --disable-bootstrap \
        --disable-libstdcxx-pch \
        --with-system-zlib \
    && make -j"$(nproc)" all-gcc \
    && make install-gcc \
    && cd ..

# mingw-w64 CRT
RUN mkdir build-mingw-crt \
    && cd build-mingw-crt \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-crt/configure \
        --prefix=${PREFIX}/${TARGET} \
        --host=${TARGET} \
        --enable-wildcard \
        --disable-lib32 \
        --enable-lib64 \
    && make -j"$(nproc)" \
    && make install \
    && cd .. \
    && rm -rf build-mingw-crt

# winpthreads
RUN mkdir build-winpthreads \
    && cd build-winpthreads \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-libraries/winpthreads/configure \
        --prefix=${PREFIX}/${TARGET} \
        --host=${TARGET} \
        --enable-static \
        --disable-shared \
    && make -j"$(nproc)" \
    && make install \
    && cd .. \
    && rm -rf build-winpthreads mingw-w64-v${MINGW_VERSION}

# finish gcc
RUN cd build-gcc \
    && make -j"$(nproc)" \
    && make install \
    && cd .. \
    && rm -rf build-gcc gcc-trunk

# CMake
RUN wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz -O - | tar -xz -C /opt \
    && ln -sf /opt/cmake-${CMAKE_VERSION}-linux-x86_64/bin/cmake /usr/local/bin/cmake \
    && ln -sf /opt/cmake-${CMAKE_VERSION}-linux-x86_64/bin/ctest /usr/local/bin/ctest \
    && ln -sf /opt/cmake-${CMAKE_VERSION}-linux-x86_64/bin/cpack /usr/local/bin/cpack

WORKDIR /workspace
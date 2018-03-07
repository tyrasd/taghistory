FROM ubuntu:16.04

# docker build -t taghistory . 
# docker run -it --name taghistory --mount type=bind,source="$(pwd)",target=/mount taghistory

# install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    curl \
    doxygen \
    g++ \
    git \
    graphviz \
    libboost-dev \
    libbz2-dev \
    libexpat1-dev \
    libgdal-dev \
    libgeos++-dev \
    libproj-dev \
    libsparsehash-dev \
    make \
    ruby \
    ruby-json \
    spatialite-bin \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# download and build protozero
WORKDIR /root
RUN curl -LO https://github.com/mapbox/protozero/archive/v1.6.1.zip && \
    unzip v1.6.1.zip && \
    rm -rf v1.6.1.zip && \
    cd /root/protozero-1.6.1 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# download libosmium, doesn't require building
WORKDIR /root
RUN curl -LO https://github.com/osmcode/libosmium/archive/v2.13.1.zip && \
    unzip v2.13.1.zip && \
    rm -rf v2.13.1.zip && \
    mv /root/libosmium-2.13.1 /root/libosmium

# build taghistory
WORKDIR /root
COPY CMakeLists.txt taghistory/
COPY cmake taghistory/cmake/
COPY src taghistory/src/

RUN mkdir taghistory/build && \
    cd /root/taghistory/build && \
    cmake .. && \
    make && \
    make install && \
    rm -rf build

# /mount is a the same directory the Dockerfile is contained within
# so you can do something like:
    # docker run -it --mount type=bind,source="$(pwd)",target=/mount taghistory california.pbf
# and it should work seamlessly

WORKDIR /mount
ENTRYPOINT ["taghistory"]

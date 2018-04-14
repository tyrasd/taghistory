FROM alpine:3.7

# docker build -t taghistory . 
# docker run -it --name taghistory --mount type=bind,source="$(pwd)",target=/mount taghistory

# install dependencies
RUN apk add --no-cache \
    cmake \
    curl \
    doxygen \
    g++ \
    git \
    graphviz \
    boost-dev \
    bzip2-dev \
    expat-dev \
    make \
    ruby \
    ruby-json \
    sparsehash \
    zlib-dev

# in the testing repo
RUN apk add --no-cache \
    --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing \
    gdal-dev \
    geos-dev \
    proj4-dev \
    libspatialite \
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

# add tini so our executable obeys signal calls and we can exit
ENV TINI_VERSION v0.17.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini

# /mount is a the same directory the Dockerfile is contained within
# so you can do something like:
    # docker run -it --mount type=bind,source="$(pwd)",target=/mount taghistory california.pbf
# and it should work seamlessly
WORKDIR /mount
ENTRYPOINT ["/tini", "--", "taghistory"]

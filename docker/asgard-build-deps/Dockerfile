ARG VALHALLA_VERSION="3.1.4"

FROM valhalla/valhalla:build-${VALHALLA_VERSION}

ARG VALHALLA_VERSION="3.1.4"

RUN apt update --assume-yes && apt install -y wget

RUN git clone --branch $VALHALLA_VERSION --depth=1 https://github.com/valhalla/valhalla.git libvalhalla \
  && cd libvalhalla \
  && git submodule update --init --recursive --depth=1 \
  && cd -

RUN mkdir -p libvalhalla/build \
  && cd libvalhalla/build \
  && cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SERVICES=Off -DENABLE_NODE_BINDINGS=Off -DENABLE_PYTHON_BINDINGS=Off \
              -DBUILD_SHARED_LIBS=Off -DBoost_USE_STATIC_LIBS=ON -DENABLE_THREAD_SAFE_TILE_REF_COUNT=On\
  && make -j$(nproc) install && ldconfig
 

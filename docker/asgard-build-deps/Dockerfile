ARG NAVITIA_VALHALLA_TAG

FROM navitia/valhalla-build:${NAVITIA_VALHALLA_TAG}

ARG VALHALLA_BRANCH_TAG

RUN apt update --assume-yes \
    && apt install -y wget libboost1.71-all-dev python3-shapely

RUN git clone --branch ${VALHALLA_BRANCH_TAG} --depth=1 https://github.com/CanalTP/valhalla.git libvalhalla \
  && cd libvalhalla \
  && git submodule update --init --recursive --depth=1 \
  && cd -

RUN mkdir -p libvalhalla/build \
  && cd libvalhalla/build \
  && cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SERVICES=Off -DENABLE_NODE_BINDINGS=Off -DENABLE_PYTHON_BINDINGS=Off \
              -DBUILD_SHARED_LIBS=Off -DBoost_USE_STATIC_LIBS=ON -DENABLE_THREAD_SAFE_TILE_REF_COUNT=On\
  && make -j$(nproc) install && ldconfig

ARG NAVITIA_VALHALLA_TAG
ARG ASGARD_GIT_BRANCH=master

FROM navitia/asgard-build-deps:${NAVITIA_VALHALLA_TAG} as builder

RUN wget --progress=bar:force:noscroll https://github.com/CanalTP/asgard-query/releases/download/v1.0.0/asgard-query

RUN git clone --branch ${ASGARD_GIT_BRANCH} --depth=1 https://github.com/canaltp/asgard asgard

WORKDIR /asgard

# create a sym link libvalhalla in to asgard so that rapidjson is placed in the right place
RUN ln -s ../libvalhalla libvalhalla
RUN sed -i 's,git\@github.com:\([^/]*\)/\(.*\).git,https://github.com/\1/\2,' .gitmodules && git submodule update --init --recursive \
  && mkdir build && cd build \
  && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)

FROM ubuntu:20.04

# we install only the depencies that are needed
RUN apt update && apt install -y wget           \
    && apt install -y libboost-regex1.71.0      \
                      libboost-thread1.71.0     \
                      libboost-filesystem1.71.0 \
                      libcurl4                  \
                      libzmq3-dev               \
                      libprotobuf-dev

COPY wait-for-data.sh /usr/bin/wait-for-data.sh
RUN chmod +x /usr/bin/wait-for-data.sh

# We have to create a group and a user with the same gid and uid than the docker-data
RUN groupadd -g 1234 -r asgard-user && useradd --no-log-init -r -g asgard-user -u 1234 asgard-user
COPY --from=builder /asgard-query /usr/bin/asgard-query
RUN chmod +x /usr/bin/asgard-query
USER asgard-user
COPY --from=builder /asgard/build/asgard/asgard /usr/bin/asgard
EXPOSE 6000 8080
HEALTHCHECK --interval=60s --timeout=5s --retries=3 \
CMD /data/valhalla/healthcheck.sh || exit 1

ENTRYPOINT ["/usr/bin/asgard"]

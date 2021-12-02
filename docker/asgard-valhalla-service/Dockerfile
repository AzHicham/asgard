ARG VALHALLA_VERSION=3.1.4

FROM valhalla/valhalla:run-${VALHALLA_VERSION} as builder


FROM ubuntu:20.04

RUN apt update && apt install -y software-properties-common

RUN add-apt-repository -y ppa:valhalla-core/valhalla

# we install only the depencies that are needed
RUN apt install -y libprotobuf-lite17 \
                   libcurl4 \
                   prime-server-bin

RUN apt purge -y software-properties-common
RUN apt autoremove -y
                                                 
COPY --from=builder /usr/local/bin/valhalla_service /usr/local/bin/valhalla_service

EXPOSE 8002

COPY wait-for-data.sh /usr/bin/wait-for-data.sh
RUN chmod +x /usr/bin/wait-for-data.sh

ENTRYPOINT ["sh", "-c", "valhalla_service /data/valhalla/valhalla.json 1"]

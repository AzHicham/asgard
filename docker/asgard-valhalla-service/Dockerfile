ARG NAVITIA_VALHALLA_TAG

FROM navitia/valhalla-run:${NAVITIA_VALHALLA_TAG} as builder

FROM ubuntu:20.04


# we install only the depencies that are needed
# install the dependencies needed by libprime-server runtime
# apt show libprime-server0
RUN apt update && apt install -y libprotobuf-lite17 libcurl4 libczmq4 libzmq5

# now we copy libprime-server
COPY --from=builder /usr/lib/x86_64-linux-gnu/libprime_server.a /usr/lib/x86_64-linux-gnu/libprime_server.a
COPY --from=builder /usr/lib/x86_64-linux-gnu/libprime_server.so /usr/lib/x86_64-linux-gnu/libprime_server.so
COPY --from=builder /usr/lib/x86_64-linux-gnu/libprime_server.so.0 /usr/lib/x86_64-linux-gnu/libprime_server.so.0
COPY --from=builder /usr/lib/x86_64-linux-gnu/libprime_server.so.0.0.0 /usr/lib/x86_64-linux-gnu/libprime_server.0.0.0

COPY --from=builder /usr/local/bin/valhalla_service /usr/local/bin/valhalla_service

EXPOSE 8002

COPY wait-for-data.sh /usr/bin/wait-for-data.sh
RUN chmod +x /usr/bin/wait-for-data.sh

ENTRYPOINT ["sh", "-c", "wait-for-data.sh && valhalla_service /data/valhalla/valhalla.json 1"]

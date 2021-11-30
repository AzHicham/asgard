FROM python:3.9-alpine

RUN apk update && apk upgrade && \
    apk add --no-cache git tar

WORKDIR /data/valhalla

# We have to create a group and a user with the same gid and uid than the docker-app
RUN addgroup -S asgard-user --gid 1234 && adduser -S asgard-user -G asgard-user --uid 1234
RUN chown -R asgard-user:asgard-user /data/valhalla
USER asgard-user

# Setup minio from asgard
RUN git clone --depth=1 https://github.com/canaltp/asgard asgard
RUN pip install -U pip && pip install -r asgard/scripts/minio/requirements.txt

VOLUME [ "/data/valhalla"]
# Download valhalla_tiles.tar valhalla.json of latest versions
# Once the downloading is finished, the elevation_tiles.tar will be decompressed ONLY if the elevation_tiles doesn't exist
# Note: after extraction of elevation_tiles.tar, we remove the content in elevation_tiles.tar to reduce disk usage,
# but still save the file so it won't be edownloaded when rebooting
ENTRYPOINT ["sh", "-c", "! [ -e /data/valhalla/elevation_tiles ] || ! [ -e /data/valhalla/valhalla_tiles.tar ] || ! [ -e /data/valhalla/valhalla.json ] && \ 
                         python3 asgard/scripts/minio/asgard_minio.py download && \
                         ! [ -e /data/valhalla/elevation_tiles ] && \
                         rm -rf tmp_tiles && mkdir tmp_tiles && tar -xf elevation_tiles.tar -C tmp_tiles && \
                         mv tmp_tiles/elevation_tiles /data/valhalla/elevation_tiles && \
                         tar -vf elevation_tiles.tar --delete elevation_tiles"]


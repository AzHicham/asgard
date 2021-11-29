FROM python:3.9-alpine

RUN apk update && apk upgrade && \
    apk add --no-cache git tar

WORKDIR /data/valhalla

# We have to create a group and a user with the same gid and uid than the docker-app
RUN addgroup -S asgard-user --gid 1234 && adduser -S asgard-user -G asgard-user --uid 1234
RUN chown -R asgard-user:asgard-user /data/valhalla
USER asgard-user

# Setup minio from asgard
RUN git clone --depth=1 --branch minio-upload-script https://github.com/canaltp/asgard asgard
RUN pip install -U pip && pip install -r asgard/scripts/minio/requirements.txt

VOLUME [ "/data/valhalla"]
# Donwload valhalla_tiles.tar valhalla.json of latest versions 
ENTRYPOINT ["sh", "-c", "python3 asgard/scripts/minio/asgard_minio.py download && \
                         ! [ -e /data/valhalla/elevation_tiles ] && \
                         rm -rf tmp_tiles && mkdir tmp_tiles && tar -xf elevation_tiles.tar -C tmp_tiles && \
                         mv tmp_tiles/elevation_tiles /data/valhalla/elevation_tiles"]


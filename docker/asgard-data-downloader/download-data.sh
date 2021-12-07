#!/bin/sh

# Download valhalla_tiles.tar valhalla.json of latest versions
# Once the downloading is finished, the elevation_tiles.tar will be decompressed ONLY if the elevation_tiles doesn't exist
# Note: after extraction of elevation_tiles.tar, we remove the content in elevation_tiles.tar to reduce disk usage,
# but still save elevation_tiles.tar so it won't be downloaded when rebooting
if ! [ -e /data/valhalla/elevation_tiles ] || ! [ -e /data/valhalla/valhalla.json ] || ! [ -e /data/valhalla/valhalla_tiles.tar ] || ! [ -e /data/valhalla/healthcheck.sh ]
then
  python3 -u /asgard/scripts/minio/asgard_minio.py download   && \
  ! [ -e /data/valhalla/elevation_tiles ]                     && \
  rm -rf tmp_tiles && mkdir tmp_tiles                         && \
  tar -xf elevation_tiles.tar -C tmp_tiles                    && \
  mv tmp_tiles/elevation_tiles /data/valhalla/elevation_tiles && \
  echo "cleaning up elevation_tiles.tar"                      && \
  tar -vf elevation_tiles.tar --delete elevation_tiles        && \
  chmod +x /data/valhalla/healthcheck.sh                      && \
  echo "DONE"
else
  echo "I've got everything needed"
fi


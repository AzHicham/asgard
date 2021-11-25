FROM python:3.9-slim

RUN apt update && apt install -y --no-install-recommends git

WORKDIR /data/valhalla

RUN git clone --depth=1 --branch minio-upload-script https://github.com/canaltp/asgard asgard
RUN apt remove -y git  && apt autoremove -y
RUN pip install -U pip && pip install -r asgard/scripts/minio/requirements.txt


USER asgard-user
VOLUME [ "/data/valhalla"]
# Donwload valhalla_tiles.tar valhalla.json of latest versions 
ENTRYPOINT ["python3 asgard/scripts/minio/asgard_minio.py download"]

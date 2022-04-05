FROM python:3.9-alpine

RUN apk update && apk upgrade && \
    apk add --no-cache git tar

# Setup minio from asgard
RUN git clone --depth=1 https://github.com/hove-io/asgard /asgard
RUN pip install -U pip && pip install -r asgard/scripts/minio/requirements.txt

COPY download-data.sh /usr/bin/download-data.sh
RUN chmod +x /usr/bin/download-data.sh

WORKDIR /data/valhalla

# We have to create a group and a user with the same gid and uid than the docker-app
RUN addgroup -S asgard-user --gid 1234 && adduser -S asgard-user -G asgard-user --uid 1234
RUN chown -R asgard-user:asgard-user /data/valhalla
USER asgard-user


VOLUME [ "/data/valhalla"]
ENTRYPOINT ["/usr/bin/download-data.sh"]


FROM navitia/asgard-build-deps:latest as builder
WORKDIR /data/valhalla

RUN apt-get update && apt-get install -y --no-install-recommends \
      tar parallel curl

# Download the pbfs given in argument, download links are seperated by ;
ARG pbf_url="http://download.geofabrik.de/europe/france-latest.osm.pbf;https://download.geofabrik.de/europe/spain-latest.osm.pbf"
ARG elevation_min_x_max_x_min_y_max_y='-5.1972 8.3483 42.2646 51.1116'

RUN mkdir osmpbf
RUN cd osmpbf \
    && IFS=";" \
    && for pbf in $pbf_url; do (wget --progress=bar:force:noscroll $pbf) ; done 

RUN valhalla_build_config --mjolnir-tile-dir ${PWD}/valhalla_tiles --mjolnir-tile-extract ${PWD}/valhalla_tiles.tar --mjolnir-timezone ${PWD}/valhalla_tiles/timezones.sqlite --mjolnir-admin ${PWD}/valhalla_tiles/admins.sqlite --additional-data-elevation ${PWD}/elevation_tiles > valhalla.json

# These are default values set after empirically testing asgard with distributed.
# They seem to give the most coherent results between the time of the matrix and the direct_path
# And fixing some projection problems in dead-ends or isolated places.
RUN sed -i 's,\"minimum_reachability\"\: [[:digit:]]*,\"minimum_reachability\"\: 30,' valhalla.json \
  && sed -i 's,\"radius\"\: [[:digit:]]*,\"radius\"\: 20,' valhalla.json \
  && sed -i 's,\"shortcuts\"\: [^,]*,\"shortcuts\"\: false,' valhalla.json \
  && sed -i 's,\"hierarchy\"\: [^,]*,\"hierarchy\"\: false,' valhalla.json \
  && sed -i 's,\"import_bike_share_stations\"\: [^,]*,\"import_bike_share_stations\"\: true,' valhalla.json

RUN valhalla_build_elevation $elevation_min_x_max_x_min_y_max_y ${PWD}/elevation_tiles $(nproc)

RUN valhalla_build_tiles -c valhalla.json `find ${PWD}/osmpbf -type f`
RUN find valhalla_tiles | sort -n | tar cf valhalla_tiles.tar --no-recursion -T - \
  # Make the graph shareable between threads in a thread-safe way
  # This has to be set after the creation of the tiles to avoid seg faults
  && sed -i '/"hierarchy":/a \\t"global_synchronized_cache": true,' valhalla.json

# Create an healthcheck script which will be run by asgard-app
ARG from_coords="2.32811;48.89283"
ARG to_coords="2.37715;48.846781"
RUN echo "/usr/bin/asgard-query directpath \"$from_coords\" \"$to_coords\"" >> healthcheck.sh \
  && chmod +x healthcheck.sh

FROM alpine:latest
# We have to create a group and a user with the same gid and uid than the docker-app
RUN addgroup -S asgard-user --gid 1234 && adduser -S asgard-user -G asgard-user --uid 1234
COPY --from=builder /data/valhalla/valhalla_tiles.tar /data/valhalla/
COPY --from=builder /data/valhalla/valhalla.json /data/valhalla/
COPY --from=builder /data/valhalla/healthcheck.sh /data/valhalla/
RUN chown -R asgard-user:asgard-user /data/valhalla
USER asgard-user
VOLUME [ "/data/valhalla" ]
ENTRYPOINT date

FROM valhalla/valhalla:run-3.1.4

EXPOSE 8002

COPY wait-for-data.sh /usr/bin/wait-for-data.sh
RUN chmod +x /usr/bin/wait-for-data.sh

ENTRYPOINT ["sh", "-c", "wait-for-data.sh && valhalla_service /data/valhalla/valhalla.json 1"]

from minio_common import *
import minio

def listall(config):
    # Create client with access and secret key.
    client = minio.Minio(endpoint=config.host, access_key=config.key, secret_key=config.secret, secure=False)

    # Scan available files from prefix /valhalla_version/coverage/
    objects = scan(client, config.bucket, prefix=f"{config.valhalla_version}/{config.coverage}", recursive=True)
    for obj in objects:
        print(f"object_name: {obj.object_name} \t version_id: {obj._version_id} \t "
              f"is_delete_marker: {obj.is_delete_marker} \t size: {obj.size} \t is_latest {obj.is_latest} ")

from minio_common import *
import minio
import minio_common


def remove(config, minio_path, version_id):
    # Create client with access and secret key.
    client = minio.Minio(endpoint=config.host, access_key=config.key, secret_key=config.secret, secure=False)
    minio_common.delete(client, config.bucket, minio_path, version_id)

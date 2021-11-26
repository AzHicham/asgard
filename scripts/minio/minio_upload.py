#!/usr/bin/env python3
""" A MinIO Uploader dedicated to upload Valhalla data

Usage:
  minio_upload.py (-h | --help)
  minio_upload.py [-k S3_KEY | --key=S3_KEY] [-s S3_SECRET | --secret=S3_SECRET] [-H HOST | --host=HOST] [-f FILES | --files=FILES ...] [-b BUCKET | --bucket=BUCKET] [-c COVERAGE | --coverage=COVERAGE] [-V VALHALLA_VERSION | --valhalla_version=VALHALLA_VERSION]

Options:
  -h --help                                                     Show this screen.
  -k S3_KEY, --key=S3_KEY                                       Minio/S3 key/username
  -s S3_SECRET, --secret=S3_SECRET                              Minio/S3 secret/token
  -H HOST, --host=HOST                                          Minio/S3 hostname
  -b BUCKET, --bucket=BUCKET                                    Minio/S3 Bucket's name
  -c COVERAGE, --coverage=COVERAGE                              OSM/data_set name
  -f FILES, --files=FILES                                       Files to upload
  -V VALHALLA_VERSION, --valhalla_version=VALHALLA_VERSION      Valhalla version used to build Asgard

Example:
  minio_upload.py -s minioadmin -k minioadmin -H 127.0.0.1:9000 -b asgard -f ./valhalla_tiles.tar ./valhalla.json -c "europe" -V "3.1.2"
"""

import glob
from collections import OrderedDict
from parse import *
from datetime import datetime
from minio_progress import Progress
from minio import Minio
from minio.commonconfig import Tags
from minio.error import MinioException
from minio_common import *
import minio_config


# Return oldest files only if there is enough data_set versions 'nb_version_to_keep'
def get_oldest_data(objects, nb_version_to_keep=0):
    dict = OrderedDict()
    file_list = []
    format_string = common_format_str()
    for obj in objects:
        list_str = parse(format_string, obj.object_name)
        dt = datetime.strptime(list_str[4], "%Y-%m-%d-%H:%M:%S")
        if dt in dict:
            dict[dt].append(obj.object_name)
        else:
            dict[dt] = [obj.object_name]
    if len(dict) > nb_version_to_keep:
        _, file_list = next(iter(dict.items()))
    return file_list


def _upload(client, bucket, minio_filepath, input_filepath, tags=None):
    file_object = client.fput_object(
        bucket, minio_filepath, input_filepath,
        tags=tags, progress=Progress(),
        content_type=get_content_type(input_filepath)
    )
    print(f"uploaded: {file_object.object_name}")
    return file_object


def mk_filelist(list_input):
    files = []

    for f in list_input:
        # note: glob filters out non-existant files
        for fname in glob.glob(f):
            files.append(fname)

    seen = set()
    return [f for f in files if not (f in seen or seen.add(f))]


def parse_args():
    from docopt import docopt

    return docopt(__doc__, version='Asgard Minio Upload v0.0.1')


def upload(config, input_files):
    # Create client with access and secret key.
    client = Minio(endpoint=config.host, access_key=config.key, secret_key=config.secret, secure=False)

    # make list file from input argument
    files = mk_filelist(input_files)
    print(f"Files to be uploaded: {files}")
    # Create Tags
    dt_now_str = datetime.now().strftime("%Y-%m-%d-%H:%M:%S")
    tags = Tags(for_object=True)
    tags["version"] = config.valhalla_version
    tags["coverage"] = config.coverage
    tags["datetime"] = dt_now_str

    for input_filepath in files:
        try:
            minio_filepath = common_format_str().format(config.valhalla_version, config.coverage,
                                                        config.valhalla_version, config.coverage,
                                                        dt_now_str, os.path.basename(input_filepath))
            _upload(client, config.bucket, minio_filepath, input_filepath, tags)
        except MinioException as err:
            print(err)

    # Scan available files from prefix /valhalla_version/coverage/
    objects = scan(client, config.bucket, prefix=f"{config.valhalla_version}/{config.coverage}", recursive=True)
    # Delete oldest data_set (all file with same latest datetime)
    oldest_objects = get_oldest_data(objects, nb_version_to_keep=2)
    for file in oldest_objects:
        try:
            delete(client, config.bucket, file)
        except MinioException as err:
            print(err)


def main():
    args = parse_args()
    check_cmdline(args)
    config = minio_config.Config(args)
    upload(config, args.get("--files"))


if __name__ == '__main__':
    main()

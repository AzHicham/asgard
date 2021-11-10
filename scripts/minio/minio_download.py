#!/usr/bin/env python3
"""Bench Jormungandr

Usage:
  minio_download.py (-h | --help)
  minio_download.py [-k S3_KEY | --key=S3_KEY] [-s S3_SECRET | --secret=S3_SECRET] [-H HOST | --host=HOST] [-F] [-b BUCKET | --bucket=BUCKET] [-c COVERAGE | --coverage=COVERAGE] [-V VALHALLA_VERSION | --valhalla_version=VALHALLA_VERSION]

Options:
  -h --help                                                     Show this screen.
  -k S3_KEY, --key=S3_KEY                                       Minio/S3 key/username
  -s S3_SECRET, --secret=S3_SECRET                              Minio/S3 secret/token
  -H HOST, --host=HOST                                          Minio/S3 hostname
  -b BUCKET, --bucket=BUCKET                                    Minio/S3 Bucket's name
  -c COVERAGE, --coverage=COVERAGE                              OSM/data_set name
  -F --force                                                    Force download data even if present locally
  -V VALHALLA_VERSION, --valhalla_version=VALHALLA_VERSION      Valhalla version used to build Asgard

Example:
  minio_download.py -s minioadmin -k minioadmin -H 127.0.0.1:9000 -b asgard -c "europe" -V "3.1.2"
"""
import os.path
import sys
from collections import OrderedDict
from datetime import datetime
from parse import *
from minio import Minio
from minio.error import MinioException
from minio_common import *


def get_latest_data(file_objects):
    dict = OrderedDict()
    format_string = common_format_str()
    for obj in file_objects:
        list_str = parse(format_string, obj.object_name)
        dt = datetime.strptime(list_str[4], "%Y-%m-%d-%H:%M:%S")
        base_name = f"{list_str[2]}_{list_str[3]}_{list_str[4]}_{list_str[5]}"
        symlink = list_str[5]
        if dt in dict:
            (dict[dt]).append((obj.object_name, base_name, symlink))
        else:
            dict[dt] = [(obj.object_name, base_name, symlink)]
    if len(dict) > 0:
        _, file_list = next(iter(reversed(dict.items())))
        return file_list
    else:
        return []


def download(client, bucket, minio_filepath, output_filepath):
    file_object = client.fget_object(bucket, minio_filepath, output_filepath)
    print(f"downloaded: {file_object.object_name}")
    return file_object


def parse_args():
    from docopt import docopt

    return docopt(__doc__, version='Asgard Minio Download v0.0.1')


def main():
    args = parse_args()
    check_cmdline(args)

    # Create client with access and secret key.
    client = Minio(endpoint=args['--host'], access_key=args['--key'], secret_key=args['--secret'], secure=False)

    # Scan available files from prefix /valhalla_version/coverage/
    objects = scan(client, args['--bucket'], prefix=f"{args['--valhalla_version']}/{args['--coverage']}", recursive=True)
    # Get latest data_set (all file with same latest datetime)
    latest_objects = get_latest_data(objects)
    print("Latest data set: ", latest_objects)

    for obj, basename, symlink in latest_objects:
        try:
            if not os.path.exists(basename) or args['--force']:
                download(client, args['--bucket'], obj, basename)
                create_symlink(f"./{basename}", f"{symlink}")
        except MinioException as err:
            print(err)


if __name__ == '__main__':
    main()

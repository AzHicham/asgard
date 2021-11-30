#!/usr/bin/env python3
""" A MinIO Downloader dedicated to download Valhalla data

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
import glob
from collections import defaultdict
from datetime import datetime
from parse import *
from minio import Minio
from minio.error import MinioException
from minio_progress import Progress
from minio.commonconfig import Tags
from minio_common import *
import minio_config


def get_latest_data(file_objects):
    dic = defaultdict(list)
    format_string = common_format_str()
    for obj in file_objects:
        # we want the real object, not the delete marker
        if obj.is_delete_marker:
            continue
        list_str = parse(format_string, obj.object_name)
        dt = datetime.strptime(list_str[4], "%Y-%m-%d-%H:%M:%S")
        base_name = f"{list_str[2]}_{list_str[3]}_{list_str[4]}_{list_str[5]}"
        symlink = list_str[5]
        dic[dt].append((obj.object_name, base_name, symlink))
    if dic:
        _, file_list = next(iter(sorted(dic.items(), reverse=True)))
        return file_list
    else:
        return []


def get_target_files(target_files):
    # target_files format must be 3.1.2_europe_2021-11-25-12:02:41_toto.tar
    res = []
    target_format = "{}_{}_{}_{}"
    for target in target_files:
        try:
            valhalla_version, coverage, datetime, symlink = parse(target_format, target)
        except Exception:
            print(f"cannot parse the given file, bad foramt:{target}")
            raise

        res.append((f"{valhalla_version}/{coverage}/{target}", target, symlink))
    return res


def _download(client, bucket, minio_filepath, output_filepath):
    file_object = client.fget_object(bucket, minio_filepath, output_filepath)
    print(f"downloaded: {file_object.object_name}")
    return file_object


def parse_args():
    from docopt import docopt

    return docopt(__doc__, version='Asgard Minio Download v0.0.1')


def download(config, target_files=[]):
    # Create client with access and secret key.
    client = Minio(endpoint=config.host, access_key=config.key, secret_key=config.secret, secure=False)

    # Scan available files from prefix /valhalla_version/coverage/
    objects = scan(client, config.bucket, prefix=f"{config.valhalla_version}/{config.coverage}", recursive=True)
    # Get latest data_set (all file with same latest datetime)
    if target_files:
        latest_objects = get_target_files(target_files)
    else:
        latest_objects = get_latest_data(objects)
    print("Latest data set: ", latest_objects)

    for obj, basename, symlink in latest_objects:
        try:
            if not os.path.exists(basename) or config.force:
                _download(client, config.bucket, obj, basename)
            else:
                print(f"{basename} exists")
            create_symlink(f"./{basename}", f"{symlink}")
        except MinioException as err:
            print(err)


def main():
    args = parse_args()
    check_cmdline(args)

    config = minio_config.Config(args)
    download(config)


if __name__ == '__main__':
    main()

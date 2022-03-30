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
from collections import defaultdict
from datetime import datetime
from parse import *
from minio import Minio
from minio.error import MinioException
from minio_common import *
import minio_config
import time
from threading import Thread


def get_latest_data(file_objects):
    dic = defaultdict(list)
    format_string = common_format_str()
    delete_markers = set()
    real_objects = []

    for obj in file_objects:
        # we store the delete_markers names
        if obj.is_delete_marker:
            delete_markers.add(obj.object_name)
        else:
            real_objects.append(obj)

    for obj in real_objects:
        # is this object already present in delete_markers?
        if obj.object_name in delete_markers:
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


class WatchDog(Thread):
    def __init__(self, file_name, file_total_size):
        super().__init__()
        self.keep_going = True
        self.file_name = file_name
        self.file_total_size = file_total_size

    def _file_size(self):
        from os import listdir
        for f in listdir("."):
            if f.startswith(self.file_name):
                return os.path.getsize(f)
        return 0

    def run(self):
        try:
            while self._file_size() < self.file_total_size and self.keep_going:
                print(f"{self.file_name}: {self._file_size()} / {self.file_total_size}")
                time.sleep(5)
            print(f"{self.file_name}: {self._file_size()} / {self.file_total_size}")
        except Exception as e:
            print(e)


def _download(client, bucket, minio_filepath, output_filepath):
    result = client.stat_object(bucket, minio_filepath)
    print(
        f"Downloading: {output_filepath} last-modified: {result.last_modified}, size: {result.size}"
    )
    t = WatchDog(output_filepath, result.size)
    t.start()
    file_object = client.fget_object(bucket, minio_filepath, output_filepath)
    t.keep_going = False
    t.join()
    print(f"Downloaded: {file_object.object_name}")


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

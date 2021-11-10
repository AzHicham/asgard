#!/usr/bin/env python3

import os
import glob
import argparse
import sys
import time
import io
from collections import OrderedDict
from parse import *
from datetime import datetime
from minio_progress import Progress
from minio import Minio
from minio.deleteobjects import DeleteObject
from minio.commonconfig import Tags
from minio.error import MinioException
from minio.retention import Retention, GOVERNANCE
from os.path import relpath, abspath
from minio_common import *


def get_latest_data(objects):
    global FORMAT_STR
    dict = OrderedDict()
    for obj in objects:
        global FORMAT_STR
        format_string = FORMAT_STR
        list_str = parse(format_string, obj.object_name)
        dt = datetime.strptime(list_str[4], "%Y-%m-%d-%H:%M:%S")
        base_name = f"{list_str[2]}_{list_str[3]}_{list_str[4]}_{list_str[5]}"
        symlink = list_str[5]
        if dt in dict:
            (dict[dt]).append((obj.object_name, base_name, symlink))
        else:
            dict[dt] = [(obj.object_name, base_name, symlink)]
    if len(dict):
        _, list = next(iter(reversed(dict.items())))
        return list
    else:
        return []

def download(client, bucket, minio_filepath, output_filepath):
    res = client.fget_object(bucket, minio_filepath, output_filepath)
    print(f"downloaded: {res.object_name}")
    return res

def parse_cmdline(args):
    desc = "upload some files to minio/s3"
    epilog = ""

    parser = argparse.ArgumentParser(description=desc, epilog=epilog)

    parser.add_argument('-k', '--key', default=os.getenv('S3_KEY', ''), type=str,
                        help="minio/s3 key, default envvar S3_KEY")
    parser.add_argument('-s', '--secret', default=os.getenv('S3_SECRET', ''), type=str,
                        help="minio/s3 secret, default envvar S3_SECRET")
    parser.add_argument('-H', '--host', default=os.getenv('S3_HOST', ''), type=str,
                        help="minio/s3 host, default envvar S3_HOST")
    parser.add_argument('-c', '--coverage', help="osm/coverage name")
    parser.add_argument('-v', '--version', help="asgard version")
    parser.add_argument('-b', '--bucket', help="name of bucket")
    parser.add_argument('-o', '--output', help="output folder")

    args = parser.parse_args()

    if not all([args.key, args.secret, args.host, args.bucket]):
        print("must supply a key, secret, server host, and file(s)")
        sys.exit(1)

    if os.path.exists(args.bucket):
        print("it looks like you forgot to give a bucket as given bucket is a file")
        sys.exit(1)

    if args.host.startswith('http'):
        _, args.host = args.host.split('://')

    return args

def main(args):
    args = parse_cmdline(args)

    # Create client with access and secret key.
    client = Minio(endpoint=args.host, access_key=args.key, secret_key=args.secret, secure=False)

    # Scan available files from prefix /valhalla_version/coverage/
    objects = scan(client, args.bucket, prefix=f"{args.version}/{args.coverage}", recursive=True)
    # Get latests data_set (all file with same latest datetime)
    latest_objects = get_latest_data(objects)
    print("Latest data set: ", latest_objects)

    for object, basename, symlink in latest_objects:
        try:
            res = download(client, args.bucket, object, basename)
            create_symlink(f"./{basename}", f"{symlink}")
        except MinioException as err:
            print(err)

if __name__ == '__main__':
    main(sys.argv[1:])

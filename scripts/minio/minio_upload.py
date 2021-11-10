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

# Remove oldest data but keep at least 2 versions
def get_oldest_data(objects):
    global FORMAT_STR
    dict = OrderedDict()
    for obj in objects:
        format_string = FORMAT_STR
        list_str = parse(format_string, obj.object_name)
        dt = datetime.strptime(list_str[4], "%Y-%m-%d-%H:%M:%S")
        if dt in dict:
            (dict[dt]).append(obj.object_name)
        else:
            dict[dt] = [obj.object_name]
    if len(dict) > 2:
        _, list = next(iter(dict.items()))
        return list
    else:
        return []

def upload(client, bucket, minio_filepath, input_filepath, tags=None):
    object = client.fput_object(
        bucket, minio_filepath, input_filepath,
        tags=tags, progress=Progress(),
    )
    print(f"uploaded: {object.object_name}")
    return object

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
    parser.add_argument('-f', '--files', nargs='+', help="file list, wildcards allowed")

    args = parser.parse_args()

    if not all([args.key, args.secret, args.host, args.bucket, args.files]):
        print("must supply a key, secret, server host, and file(s)")
        sys.exit(1)

    if os.path.exists(args.bucket):
        print("it looks like you forgot to give a bucket as given bucket is a file")
        sys.exit(1)

    if args.host.startswith('http'):
        _, args.host = args.host.split('://')

    return args

def tznow():
    def utc_to_local(utc_dt):
        return utc_dt.replace(tzinfo=datetime.timezone.utc).astimezone(tz=None)

    ts = datetime.datetime.utcnow()
    return utc_to_local(ts)

def mk_filelist(list_input):
    files = []

    for f in list_input:
        # note: glob filters out non-existant files
        for fname in glob.glob(f):
            files.append(fname)

    seen = set()
    return [f for f in files if not (f in seen or seen.add(f))]


def main(args):
    args = parse_cmdline(args)

    # Create client with access and secret key.
    client = Minio(endpoint=args.host, access_key=args.key, secret_key=args.secret, secure=False)

    # make list file from input argument
    files = mk_filelist(args.files)

    # Create Tags
    dt_now_str = datetime.now().strftime("%Y-%m-%d-%H:%M:%S")
    tags = Tags(for_object=True)
    tags["version"] = args.version
    tags["coverage"] = args.coverage
    tags["datetime"] = dt_now_str

    for input_filepath in files:
        try:
            minio_filepath = f"{args.version}/{args.coverage}/{args.version}_{args.coverage}_{dt_now_str}_{os.path.basename(input_filepath)}"
            upload(client, args.bucket, minio_filepath, input_filepath, tags)
        except MinioException as err:
            print(err)

    # Scan available files from prefix /valhalla_version/coverage/
    objects = scan(client, args.bucket, prefix=f"{args.version}/{args.coverage}", recursive=True)
    # Delete oldest data_set (all file with same latest datetime)
    oldest_objects = get_oldest_data(objects)
    for file in oldest_objects:
        try:
            res = delete(client, args.bucket, file)
        except MinioException as err:
            print(err)

if __name__ == '__main__':
    main(sys.argv[1:])

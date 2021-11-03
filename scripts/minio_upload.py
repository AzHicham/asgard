#!/usr/bin/env python3

import os
import sys
import glob
import argparse
import time
import io
from datetime import datetime, timedelta
from minio import Minio
from minio.deleteobjects import DeleteObject
from minio.commonconfig import Tags
from minio.error import MinioException
from minio.retention import Retention, GOVERNANCE
from os.path import relpath, abspath


def upload(client, bucket, base_name, filename, retention_date, tags=None):
    """
    """

    result = client.fput_object(
        bucket, base_name, filename,
        tags=tags,
        #retention=Retention(GOVERNANCE, retention_date),
        #legal_hold=True,
    )

    return result


def parse_cmdline(args):
    desc = "upload some files to minio/s3"
    epilog = """
    example:
        ./mc-put.py -k key -s secret -u minio.example.com:8081 mybucket myfiles*.zip
    """

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

    # make list file from input argument (cleaner)
    files = mk_filelist(args.files)

    tags = client.get_bucket_tags(args.bucket)

    # Upload data with tags, retention and legal-hold.
    now = datetime.now()
    retention_date = datetime.now().replace(
        hour=0, minute=0, second=0, microsecond=0,
    ) + timedelta(days=30)
    tags = Tags(for_object=True)
    tags["version"] = args.version
    tags["coverage"] = args.coverage
    tags["datetime"] = now.strftime("%m/%d/%Y, %H:%M:%S")

    for filename in files:
        try:
            base_name = args.coverage + '/' + args.version + '/' + os.path.basename(filename)
            print(base_name)
            res = upload(client, args.bucket, base_name, filename, retention_date, tags)
            print(f"uploaded: {res}")
        except MinioException as err:
            print(err)


if __name__ == '__main__':
    main(sys.argv[1:])

#!/usr/bin/env python3

import os
import sys
import pathlib


def common_format_str():
    return "{}/{}/{}_{}_{}_{}"


def scan(client, bucket, prefix="/", recursive=False):
    obj = client.list_objects(
        bucket, prefix=prefix, recursive=recursive, include_version=True
    )
    return obj


def get_content_type(filename):
    suffix = pathlib.Path(filename).suffix
    if suffix == '.csv':
        return "text/csv"
    if suffix == '.txt':
        return "text/plain"
    elif suffix == '.json':
        return "application/json"
    elif suffix == '.tar':
        return "application/x-tar"
    else:
        return "application/octet-stream"


def create_symlink(src, dst):
    if os.path.exists(dst):
        os.remove(dst)
    os.symlink(f"./{src}", f"{dst}")
    print(f"Symlink created : {dst} -> {src}")


def delete(client, bucket, minio_filepath, version_id):
    objects = client.remove_object(bucket, minio_filepath, version_id)
    print(f"Deleted file: {minio_filepath}, {version_id}")



def check_cmdline(args):
    if not all([args['--key'], args['--secret'], args['--host'], args['--bucket']]):
        print("must supply a key, secret, server host, and file(s)")
        sys.exit(1)

    if args['--host'].startswith('http'):
        _, args['--host'] = args['--host'].split('://')


def check_config(config):
    if not all([config.key, config.secret, config.host, config.bucket]):
        print(config)
        print("must supply a key, secret, server host, and file(s)")
        sys.exit(1)

    if config.host.startswith('http'):
        _, config.host = config.host.split('://')

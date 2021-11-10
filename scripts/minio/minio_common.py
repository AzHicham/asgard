#!/usr/bin/env python3

import os
import sys


def common_format_str():
    return "{}/{}/{}_{}_{}_{}"


def scan(client, bucket, prefix="/", recursive=False):
    obj = client.list_objects(
        bucket, prefix=prefix, recursive=recursive,
    )
    return obj


def create_symlink(src, dst):
    if os.path.exists(dst):
        os.remove(dst)
    os.symlink(f"./{src}", f"{dst}")
    print(f"Symlink created : {dst} -> {src}")


def delete(client, bucket, minio_filepath):
    objects = client.remove_object(bucket, minio_filepath)
    print("Deleted file: ", minio_filepath)
    return objects


def check_cmdline(args):
    if not all([args['--key'], args['--secret'], args['--host'], args['--bucket']]):
        print("must supply a key, secret, server host, and file(s)")
        sys.exit(1)

    if args['--host'].startswith('http'):
        _, args['--host'] = args['--host'].split('://')

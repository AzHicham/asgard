#!/usr/bin/env python3

import os
import glob
import argparse
import sys
import time
import io
from datetime import datetime
from minio_progress import Progress
from minio import Minio
from minio.deleteobjects import DeleteObject
from minio.commonconfig import Tags
from minio.error import MinioException
from minio.retention import Retention, GOVERNANCE
from os.path import relpath, abspath

FORMAT_STR = "{}/{}/{}_{}_{}_{}"

def scan(client, bucket, prefix="/", recursive=False):
    objects = client.list_objects(
        bucket, prefix=prefix, recursive=recursive,
    )
    return objects

def create_symlink(src, dst):
    if os.path.exists(dst):
      os.remove(dst)
    os.symlink(f"./{src}", f"{dst}")
    print(f"Symlink created : {dst} -> {src}")

def delete(client, bucket, minio_filepath):
    objects = client.remove_object(bucket, minio_filepath)
    print("Deleted file: ", minio_filepath)
    return objects

#!/usr/bin/env python3
""" A MinIO Downloader dedicated to download Valhalla data

Usage:
  asgard_minio.py (-h | --help)
  asgard_minio.py download [<files>...] [-k S3_KEY | --key=S3_KEY] [-s S3_SECRET | --secret=S3_SECRET] [-H HOST | --host=HOST] [-F] [-b BUCKET | --bucket=BUCKET] [-c COVERAGE | --coverage=COVERAGE] [-V VALHALLA_VERSION | --valhalla_version=VALHALLA_VERSION]
  asgard_minio.py upload <files>... [-k S3_KEY | --key=S3_KEY] [-s S3_SECRET | --secret=S3_SECRET] [-H HOST | --host=HOST] [-F] [-b BUCKET | --bucket=BUCKET] [-c COVERAGE | --coverage=COVERAGE] [-V VALHALLA_VERSION | --valhalla_version=VALHALLA_VERSION]
  asgard_minio.py list-all [-k S3_KEY | --key=S3_KEY] [-s S3_SECRET | --secret=S3_SECRET] [-H HOST | --host=HOST] [-F] [-b BUCKET | --bucket=BUCKET] [-c COVERAGE | --coverage=COVERAGE] [-V VALHALLA_VERSION | --valhalla_version=VALHALLA_VERSION]

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
  asgard_minio.py download -s minioadmin -k minioadmin -H 127.0.0.1:9000 -b asgard -c "europe" -V "3.1.2"
  asgard_minio.py upload toto.py titi.py -s minioadmin -k minioadmin -H 127.0.0.1:9000 -b asgard -c "europe" -V "3.1.2"
  asgard_minio.py list-all toto.py titi.py -s minioadmin -k minioadmin -H 127.0.0.1:9000 -b asgard -c "europe" -V "3.1.2"

  MINIO_KEY=my_key MINIO_SECRET=my_secret MINIO_HOST=localhost:9000 MINIO_BUCKET=asgard MINIO_COVERAGE=europe MINIO_VALHALLA_VERSION=3.1.2 asgard_minio.py download
"""

from minio_common import check_config
from minio_download import download
from minio_upload import upload
from minio_listall import listall
import minio_config


def parse_args():
    from docopt import docopt
    return docopt(__doc__, version='Asgard Minio Data Management v0.0.1')


def main():
    args = parse_args()
    config = minio_config.Config(args)
    check_config(config)

    if args.get('download'):
        download(config, args["<files>"])

    if args.get('upload'):
        upload(config, args["<files>"])

    if args.get('list-all'):
        listall(config)


if __name__ == '__main__':
    main()

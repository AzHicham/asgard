# [-k S3_KEY | --key=S3_KEY] [-s S3_SECRET | --secret=S3_SECRET] [-H HOST | --host=HOST] [-F] [-b BUCKET | --bucket=BUCKET] [-c COVERAGE | --coverage=COVERAGE] [-V VALHALLA_VERSION | --valhalla_version=VALHALLA_VERSION]
from dataclasses import dataclass
import os


@dataclass
class Config:
    key: str = os.getenv('MINIO_KEY', "key")
    secret: str = os.getenv('MINIO_SECRET', "secret")
    host: str = os.getenv('MINIO_HOST', "localhost:9000")
    bucket: str = os.getenv('MINIO_BUCKET', "asgard")
    coverage: str = os.getenv('MINIO_COVERAGE', "europe")
    valhalla_version: str = os.getenv('MINIO_VALHALLA_VERSION', "3.1.2")
    force: bool = False

    def __init__(self, args=None):
        args = args or {}
        self.key = args.get('--key') or self.key
        self.secret = args.get('--secret') or self.secret
        self.host = args.get('--host') or self.host
        self.bucket = args.get('--bucket') or self.bucket
        self.coverage = args.get('--coverage') or self.coverage
        self.valhalla_version = args.get('--valhalla_version') or self.valhalla_version
        self.force = args.get('--force') or self.force

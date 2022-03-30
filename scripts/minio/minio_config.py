from dataclasses import dataclass
import os


@dataclass
class Config:
    """
    This class will load the config either from environment variables or command line arguments
    """
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

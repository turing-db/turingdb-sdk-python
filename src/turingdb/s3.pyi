from turingdb.exceptions import TuringDBException as TuringDBException
from turingdb.path import PathType as PathType
from turingdb.turingdb import TuringDB as TuringDB

class S3Client:
    def __init__(self, turingdb_client: TuringDB, bucket_name: str, access_key: str | None = None, secret_key: str | None = None, region: str | None = None, use_scratch: bool = True) -> None: ...
    def transfer(self, src: str, dst: str): ...

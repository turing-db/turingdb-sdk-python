from enum import Enum

class PathType(Enum):
    LOCAL = 'local'
    S3 = 's3'
    TURINGDB = 'turingdb'
    @staticmethod
    def get_type(path: str): ...

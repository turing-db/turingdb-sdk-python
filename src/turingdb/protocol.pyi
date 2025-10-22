from pandas import DataFrame
from typing import Protocol

class QueryProtocol(Protocol):
    def query(self, query: str) -> DataFrame: ...

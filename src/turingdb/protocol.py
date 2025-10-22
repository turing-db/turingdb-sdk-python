from typing import TYPE_CHECKING, Protocol

if TYPE_CHECKING:
    from pandas import DataFrame


class QueryProtocol(Protocol):
    def query(self, query: str) -> DataFrame: ...

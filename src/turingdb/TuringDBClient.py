
class TuringDBClient:
    _host: str
    _port: int

    def __init__(self,
                 host: str = 'http://127.0.0.1',
                 port: int = 6666):
        self._host = host
        self._port = port


    def query(self, query: str, graph_name: str):
        raise NotImplemented

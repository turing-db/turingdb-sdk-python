from typing import Optional

import time


class TuringDBException(Exception):
    def __init__(self, message: str):
        super().__init__(message)


class TuringDB:
    DEFAULT_HEADERS = {
        "Accept": "application/json",
        "Content-Type": "application/json",
    }

    def __init__(
        self,
        instance_id: str = "",
        auth_token: str = "",
        host: str = "https://engines.turingdb.ai/sdk",
        timeout: Optional[int] = None,
    ):
        import httpx
        import copy

        self.host = host
        self._client = httpx.Client(
            auth=None,
            verify=False,
            timeout=timeout,
        )
        self._query_exec_time = None
        self._total_exec_time = None
        self._t0 = 0
        self._t1 = 0
        self._timeout = timeout

        self._params = {
            "graph": "default",
        }

        self._headers = copy.deepcopy(TuringDB.DEFAULT_HEADERS)

        if instance_id != "":
            self._headers["Turing-Instance-Id"] = instance_id

        if auth_token != "":
            self._headers["Authorization"] = f"Bearer {auth_token}"

    def try_reach(self, timeout: int = 5):
        self._client.timeout = timeout
        self.list_available_graphs()
        self._client.timeout = self._timeout

    def warmup(self, timeout: int = 5):
        self._client.timeout = timeout
        self.query("LIST GRAPH")
        self._client.timeout = self._timeout

    def list_available_graphs(self) -> list[str]:
        return self._send_request("list_avail_graphs")["data"]

    def list_loaded_graphs(self) -> list[str]:
        return self._send_request("list_loaded_graphs")["data"][0][0]

    def is_graph_loaded(self) -> bool:
        return self._send_request("is_graph_loaded", params={"graph": self.get_graph()})["data"]

    def load_graph(self, graph_name: str, raise_if_loaded: bool = True):
        try:
            return self._send_request("load_graph", params={"graph": graph_name})
        except TuringDBException as e:
            if raise_if_loaded or e.__str__() != "GRAPH_ALREADY_EXISTS":
                raise e

    def create_graph(self, graph_name: str):
        return self.query(f"create graph {graph_name}")

    def query(self, query: str):
        json = self._send_request("query", data=query, params=self._params)

        if not isinstance(json, dict):
            raise TuringDBException("Invalid response from the server")

        return self._parse_chunks(json)

    def set_commit(self, commit: str):
        self._params["commit"] = commit

    def set_change(self, change: int | str):
        if isinstance(change, int):
            change = f"{change:x}"
        self._params["change"] = change

    def checkout(self, change: int | str = "main", commit: str = "HEAD"):
        if change == "main":
            if "change" in self._params:
                del self._params["change"]
        else:
            self.set_change(change)

        if commit == "HEAD":
            if "commit" in self._params:
                del self._params["commit"]
        else:
            self.set_commit(commit)

    def set_graph(self, graph_name: str):
        self._params["graph"] = graph_name

    def get_graph(self) -> str:
        return self._params["graph"]

    def _send_request(
        self,
        path: str,
        data: Optional[dict | str] = None,
        params: Optional[dict] = None,
    ):
        import orjson

        self._query_exec_time = None
        self._total_exec_time = None
        self._t0 = time.time()

        if data is None:
            data = ""

        url = f"{self.host}/{path}"

        if isinstance(data, dict):
            response = self._client.post(
                url, json=data, params=params, headers=self._headers
            )
        else:
            response = self._client.post(
                url, content=data, params=params, headers=self._headers
            )
        response.raise_for_status()

        json = orjson.loads(response.text)

        if isinstance(json, dict):
            err = json.get("error")
            if err is not None:
                details = json.get("error_details")
                if details is not None:
                    err = f"{err}: {details}"
                raise TuringDBException(err)

        self._t1 = time.time()
        self._total_exec_time = (self._t1 - self._t0) * 1000

        return json

    def _parse_chunks(self, json: dict):
        import pandas as pd
        import numpy as np

        columns: list[list] = []

        self._query_exec_time = json["time"]

        for chunk in json["data"]:
            for i, col in enumerate(chunk):
                if i >= len(columns):
                    columns.append([])

                columns[i] = columns[i] + col

        arrays = [np.array(columns[i]) for i in range(len(columns))]

        self._t1 = time.time()
        self._total_exec_time = (self._t1 - self._t0) * 1000

        return pd.DataFrame(dict(zip(range(len(arrays)), arrays)), index=None)

    def get_query_exec_time(self) -> Optional[float]:
        return self._query_exec_time

    def get_total_exec_time(self) -> Optional[float]:
        return self._total_exec_time

import pandas as pd
from .turingdb_core_pymodule import TuringClient

class turingDB:
    """Python SDK Class To Connect To And Interact With A TuringDB Instance"""

    def __init__(self, url = "http://127.0.0.1:6666", graph = "default", auth_token = ""):
        # Create the underlying C++ object
        self._turing_client = TuringClient(url)
        self._graph = graph
        self._url = url
        self._auth_token = auth_token
        if auth_token:
            self._turing_client.set_auth_token(self._auth_token)

    def load_graph(self,graph):
        """Load a graph from the available graph list onto the turingDB engine"""
        self._turing_client.load_graph(graph)

    def set_graph(self,graph):
        """Set a default graph for the turingDB object"""
        self._graph = graph

    def set_auth_token(self,token):
        """Set a token to authenticate all requests sent to a turingDB engine.
        Running this function repeatedly will reset the authentication token accordingly"""
        self._auth_token = token
        return self._turing_client.set_auth_token(self._auth_token)

    def clear_auth_token(self):
        """Remove the authtentication token from requests being sent to the engines"""
        return self._turing_client.clear_auth_token()

    def list_available_graphs(self):
        """List all the available graphs that a turingDB engine can load and query"""
        return self._turing_client.list_available_graphs()

    def list_loaded_graphs(self):
        """List all the graphs that a turingDB engine has loaded into memory"""
        return self._turing_client.list_loaded_graphs()

    def query(self, query, graph=None):
        """Query a graph loaded into a turingDB engine. If the 'graph' keyword is not 
        specified the turingDB classes default graph is used"""
        if graph is not None:
            return pd.DataFrame(self._turing_client.query(query,graph))

        return pd.DataFrame(self._turing_client.query(query,self._graph))

    def reveal_auth_token(self):
        """prints the authentication token"""
        print(self._auth_token)

    def __enter__(self):
        """Context manager support."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Cleanup when exiting context."""
        if hasattr(self._turing_client, 'cleanup'):
            self._turing_client.cleanup()

    def __repr__(self):
        return f"TuringDB(graph = {self._graph}, url = {self._url})"

__all__ = ["turingDB"]

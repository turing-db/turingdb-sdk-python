import pandas as pd
from .turingdb_core_pymodule import TuringClient

class turingDB:
    """Python wrapper around the C++ TuringRequest class."""
    
    def __init__(self, url = "http://127.0.0.1:6666", graph = "default"):
        # Create the underlying C++ object
        self._turing_client = TuringClient(url)
        self._graph = graph
        self._url = url
    

    def load_graph(self,graph):
        self._turing_client.load_graph(graph)

    def set_graph(self,graph):
        self._graph = graph

    def list_available_graphs(self):
        return self._turing_client.list_available_graphs()

    def list_loaded_graphs(self):
        return self._turing_client.list_loaded_graphs()

    def query(self, query, graph=None):
        if graph is not None:
            return pd.DataFrame(self._turing_client.query(query,graph))

        return pd.DataFrame(self._turing_client.query(query,self._graph))

    def __enter__(self):
        """Context manager support."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Cleanup when exiting context."""
        if hasattr(self._turing_client, 'cleanup'):
            self._turing_client.cleanup()
    
    def __repr__(self):
        return f"TuringDB(graph = {self._graph}, url = {self._url})"

# Export the wrapper, not the C++ class directly
__all__ = ["turingDB"]

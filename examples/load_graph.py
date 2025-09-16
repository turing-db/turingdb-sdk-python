from turingdb import TuringDB

if __name__ == '__main__':
    client = TuringDB(host='http://localhost:1234')
    client.load_graph('reactome')

from turingdb import TuringDB

import time
import pathlib

if __name__ == "__main__":
    client = TuringDB(host="http://localhost:6666")

    folder = pathlib.Path(__file__).parent.absolute()
    create_query = ""

    client.warmup()

    t0 = time.time()
    client.create_graph("stations7")
    client.set_graph("stations7")

    change = client.new_change()

    with open(f"{folder}/stations.cypher", "r") as f:
        create_query = f.read().replace("\n", " ").strip()

    client.query(create_query)
    client.query("CHANGE SUBMIT")
    client.checkout()

    print(f"Created graph in {(time.time() - t0) * 1000:.2f} milliseconds")

    for i in range(1, 13):
        station_count = i
        stations_path = ""
        stations_return = ""

        for j in range(1, station_count):
            stations_path += f"--(s{j}:Station)"
            stations_return += f", s{j}, s{j}.displayName"

        query = f'MATCH (start:Station{{displayName:"Paddington"}}){stations_path}--(end:Station{{displayName:"Blackfriars"}}) RETURN start, start.displayName{stations_return}, end, end.displayName'
        res = client.query(query)

        total_time = client.get_total_exec_time()
        query_time = client.get_query_exec_time()
        print(f"- {station_count} stations: Result={res.shape} Total={total_time:.2f} ms Query={query_time:.2f} ms")

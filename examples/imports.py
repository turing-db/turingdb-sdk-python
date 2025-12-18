from turingdb import TuringDB

if __name__ == "__main__":
    client = TuringDB(host="http://localhost:1234")

    print("- Connecting to S3")
    client.s3_connect("mybucket", region="eu-west-2", use_scratch=True)

    print("- Uploading ./mygraph.gml")
    client.transfer(src="~/mygraph.gml", dst="turingdb://mygraph.gml")

    print("- Loading mygraph.gml")
    client.query('LOAD GML "mygraph.gml" AS mygraph')

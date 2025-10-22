from turingdb import TuringDB

if __name__ == "__main__":
    client = TuringDB(host="http://localhost:1234")

    print("- Connecting to S3")
    client.s3_connect("turing-internal", region="eu-west-2", use_scratch=True)

    print("- Uploading ./test.dump")
    client.transfer(src="~/link.gml", dst="turingdb://sub/sub/sub/link.gml")

    print("- Loading ./test.dump into TuringDB")
    client.query('IMPORT GRAPH pole FROM "sub/sub/sub/pole"')

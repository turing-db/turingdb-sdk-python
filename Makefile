mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(shell dirname $(mkfile_path))

all:
	cd $(current_dir) && uv build

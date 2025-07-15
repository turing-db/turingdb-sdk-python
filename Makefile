SHELL := /bin/bash

BUILD_ENV=
BUILD_DIR=$(abspath build_package)
SRC_DIR=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
TURING_HOME=$(abspath turing_install)

all: build

.PHONY: build
build: JOBS=$(filter -j%,$(MAKEFLAGS))
build:
	@echo "Setting Up Python Environment" 
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) \
	&&if [ ! -d "build_env" ]; then \
	echo "Creating Virtual Environment"; \
	uv venv --python 3.10.12 build_env; \
	else echo "Virtual Environment Already Exists, Skipping Creation"; \
	fi \
	&& source $(BUILD_DIR)/build_env/bin/activate \
	&& uv pip install numpy==1.21.5 
	@echo "Building Module"
	@cd $(BUILD_DIR) \
	&& source build_env/bin/activate \
	&& export NUMPY_INCLUDE=$$(python -c "import numpy; print(numpy.get_include())") \
	&& export PYTHON_INCLUDE=$$(python -c "import sysconfig; print(sysconfig.get_path('include'))") \
	&& export PYTHON_LIB=$$(python -c "import pathlib; print(pathlib.Path('$$PYTHON_INCLUDE').parent.parent.absolute())")/lib \
	&& mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) \
	&& $(BUILD_ENV) cmake -DPYTHON_INCLUDE_DIR=$$PYTHON_INCLUDE -DPYTHON_LIB_DIR=$$PYTHON_LIB -DNUMPY_INCLUDE_DIR=$$NUMPY_INCLUDE -DCMAKE_INSTALL_PREFIX=$(TURING_HOME) $(SRC_DIR) \
	&& make -s $(JOBS) \
	&& uv build $(BUILD_DIR)/pymodule --out-dir $(TURING_HOME)/lib/turingdb --wheel

.PHONY: debug
debug: BUILD_ENV += DEBUG_BUILD=1
debug: build

.PHONY: reldebug
reldebug: BUILD_ENV += RELDEBUG=1
reldebug: build

.PHONY: callgrind
callgrind: BUILD_ENV += CALLGRIND_PROFILE=1
callgrind: build

.PHONY: clean
clean:

check-env:
ifndef TURING_HOME
	$(error TURING_HOME is not defined, please configure your environment.)
endif

.PHONY: test
test:
	cd $(BUILD_DIR) && ctest --output-on-failure --output-junit ../unit_tests.xml --force-new-ctest-process
regress:
	cd $(SRC_DIR)/test/regress && wrt -output-on-failure -j 4
run_samples:
	cd $(TURING_HOME)/samples && wrt -output-on-failure -j 4

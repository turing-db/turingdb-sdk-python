SHELL := /bin/bash
BUILD_ENV=
BUILD_DIR=$(abspath build_package)
SRC_DIR=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
INSTALL_DIR=$(abspath install)

# Default Python version if not specified
PYTHON_VERSION ?= 3.10

all: build
.PHONY: build
build:
	./scripts/build_with_version.sh $(PYTHON_VERSION) 8

.PHONY: build_many
build_many:
	@if [ -z "$(PYTHON_VERSIONS)" ]; then \
		echo "Error: PYTHON_VERSIONS not specified. Usage: make build_many PYTHON_VERSIONS='3.9 3.10 3.11'"; \
		exit 1; \
	fi
	@for version in $(PYTHON_VERSIONS); do \
		echo "Building with Python $$version"; \
		$(MAKE) clean; \
		$(MAKE) build PYTHON_VERSION=$$version; \
	done

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
	rm -rf $(BUILD_DIR)
	rm -rf build_env
	rm -rf test_env
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

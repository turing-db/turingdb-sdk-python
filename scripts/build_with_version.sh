#!/usr/bin/env bash

CURRENT_DIR=$(dirname "$(realpath "$0")")
ROOT_DIR="$CURRENT_DIR/.."

ENV_DIR="$ROOT_DIR"/build_env
BUILD_DIR="$ROOT_DIR/build_package"
INSTALL_DIR="$ROOT_DIR/install"

PYTHON_VERSION=$1
JOBS=$2

if [ -z "$PYTHON_VERSION" ]; then
	PYTHON_VERSION="3.10"
fi

if [ -z "$JOBS" ]; then
	JOBS=4
fi

if [ -z "$CC" ]; then
	CC=clang
fi

if [ -z "$CXX" ]; then
	CXX=clang++
fi

echo "- C Compiler: $CC"
echo "- C++ Compiler: $CXX"

echo "Setting Up Python Environment"
mkdir -p $BUILD_DIR

echo "Creating Virtual Environment"
uv venv --python $PYTHON_VERSION ${ENV_DIR} --clear; 
source ${ENV_DIR}/bin/activate
uv pip install numpy==2.2.6
uv pip install pandas==2.3.1

echo "Building Module"
export NUMPY_INCLUDE=$(python -c "import numpy; print(numpy.get_include())")
export PYTHON_INCLUDE=$(python -c "import sysconfig; print(sysconfig.get_path('include'))")
export PYTHON_LIB=$(python -c "import pathlib; print(pathlib.Path('$PYTHON_INCLUDE').parent.parent.absolute())")/lib

echo "NUMPY_INCLUDE: $NUMPY_INCLUDE"
echo "PYTHON_INCLUDE: $PYTHON_INCLUDE"
echo "PYTHON_LIB: $PYTHON_LIB"

cmake -DPYTHON_INCLUDE_DIR=$PYTHON_INCLUDE \
      -DPYTHON_LIB_DIR=$PYTHON_LIB \
      -DCMAKE_C_COMPILER="$CC" \
      -DCMAKE_CXX_COMPILER="$CXX" \
      -DPYTHON_VERSION=$PYTHON_VERSION \
      -DNUMPY_INCLUDE_DIR=$NUMPY_INCLUDE \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
      -DCMAKE_BUILD_TYPE=Release \
      $ROOT_DIR \
      -B $BUILD_DIR

cmake --build $BUILD_DIR -j $JOBS

uv build $BUILD_DIR/pymodule --out-dir $INSTALL_DIR/lib/turingdb --wheel
wheel_path=$(ls -t $INSTALL_DIR/lib/turingdb/*.whl | head -1)

echo "Testing Module"
uv venv --python $PYTHON_VERSION test_env --clear
source test_env/bin/activate
uv pip install "$wheel_path"
python3 -c "from turingdb import turingDB"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL NPY_TuringDB

#include "PyUtils.h"

#include <numpy/ndarrayobject.h>

#include "Profiler.h"
#include "TypedColumn.h"

using namespace turingClient;

static PyObject* ma_module = NULL;
static PyObject* ma_array_func = NULL;

static int init_numpy_ma() {
    if (!ma_module) {
        ma_module = PyImport_ImportModule("numpy.ma");
        if (!ma_module) {
            return -1;
        }

        ma_array_func = PyObject_GetAttrString(ma_module, "array");
        if (!ma_array_func) {
            Py_DECREF(ma_module);
            ma_module = NULL;
            return -1;
        }
    }
    return 0;
}

void destroyTypedColumns(PyObject* capsule) {
    std::vector<std::unique_ptr<turingClient::TypedColumn>>* col = static_cast<std::vector<std::unique_ptr<turingClient::TypedColumn>>*>(
        PyCapsule_GetPointer(capsule, NULL));
    delete col;
}

PyObject* turingPyModule::createMaskedArray(PyObject* dataArray, PyObject* maskArray) {
    Profile profile {"turingPyModule::createMaskedArray"};

    if (init_numpy_ma() < 0) {
        return NULL;
    }
    PyObject* kwargs = PyDict_New();
    if (!kwargs) {
        return NULL;
    }

    PyDict_SetItemString(kwargs, "mask", maskArray);

    PyObject* args = PyTuple_Pack(1, dataArray);
    if (!args) {
        Py_DECREF(kwargs);
        return NULL;
    }

    PyObject* maskedArray = PyObject_Call(ma_array_func, args, kwargs);

    Py_DECREF(args);
    Py_DECREF(kwargs);

    return maskedArray;
}

PyObject* turingPyModule::vectorToList(std::vector<std::string>& vec) {
    Profile profile {"turingPyModule::vectorToList"};

    PyObject* pyList = PyList_New(vec.size());
    if (!pyList) {
        return NULL;
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        PyObject* pyString = PyUnicode_FromString(vec[i].c_str());
        if (!pyString) {
            Py_DECREF(pyList);
            return NULL;
        }

        PyList_SetItem(pyList, i, pyString);
    }

    return pyList;
}

PyObject* turingPyModule::typedColumnToNumpyArray(turingClient::TypedColumn* typedCol, PyObject* backingData) {
    Profile profile {"turingPyModule::typedColumnToNumpyArray"};

    switch (typedCol->columnType()) {
        case INT: {
            auto* col = static_cast<Column<int64_t>*>(typedCol);

            npy_intp dims[] = {static_cast<npy_intp>(col->size())};

            PyObject* dataArray = PyArray_SimpleNewFromData(
                1,
                dims,
                NPY_INT64,
                col->data());

            if (!dataArray) {
                return NULL;
            }

            PyObject* maskArray = PyArray_SimpleNewFromData(
                1,
                dims,
                NPY_BOOL,
                col->mask());

            if (!maskArray) {
                Py_DECREF(dataArray);
                return NULL;
            }

            Py_INCREF(backingData);
            PyArray_SetBaseObject((PyArrayObject*)dataArray, backingData);
            Py_INCREF(backingData);
            PyArray_SetBaseObject((PyArrayObject*)maskArray, backingData);

            return createMaskedArray(dataArray, maskArray);
        }
        case UINT: {
            auto* col = static_cast<Column<uint64_t>*>(typedCol);

            npy_intp dims[] = {static_cast<npy_intp>(col->size())};

            PyObject* dataArray = PyArray_SimpleNewFromData(
                1,
                dims,
                NPY_UINT64,
                (col->data()));

            if (!dataArray) {
                return NULL;
            }

            PyObject* maskArray = PyArray_SimpleNewFromData(
                1,
                dims,
                NPY_BOOL,
                col->mask());

            if (!maskArray) {
                Py_DECREF(dataArray);
                return NULL;
            }

            Py_INCREF(backingData);
            PyArray_SetBaseObject((PyArrayObject*)dataArray, backingData);
            Py_INCREF(backingData);
            PyArray_SetBaseObject((PyArrayObject*)maskArray, backingData);

            return createMaskedArray(dataArray, maskArray);
        }
        case STRING: {
            auto* col = static_cast<Column<std::string>*>(typedCol);

            PyObject* pyList = PyList_New(col->size());

            for (size_t i = 0; i < col->size(); ++i) {
                if (!col->getMask()[i]) {

                    PyObject* pyString = PyUnicode_FromString((*col)[i].c_str());
                    if (!pyString) {
                        Py_DECREF(pyList);
                        return NULL;
                    }

                    PyList_SetItem(pyList, i, pyString);
                } else {
                    PyList_SetItem(pyList, i, Py_None);
                }
            }
            return pyList;
        }
        case BOOL: {
            auto* col = static_cast<Column<CustomBool>*>(typedCol);

            npy_intp dims[] = {static_cast<npy_intp>(col->size())};

            PyObject* dataArray = PyArray_SimpleNew(
                1,
                dims,
                NPY_BOOL);

            if (!dataArray) {
                return NULL;
            }

            bool* dataPtr = (bool*)PyArray_DATA((PyArrayObject*)dataArray);

            for (size_t i = 0; i < col->size(); ++i) {
                dataPtr[i] = (*col)[i];
            }

            PyObject* maskArray = PyArray_SimpleNewFromData(
                1,
                dims,
                NPY_BOOL,
                col->mask());

            if (!maskArray) {
                Py_DECREF(dataArray);
                return NULL;
            }

            Py_INCREF(backingData);
            PyArray_SetBaseObject((PyArrayObject*)maskArray, backingData);

            return createMaskedArray(dataArray, maskArray);
        }
        default:
            PyErr_SetString(PyExc_RuntimeError, "Unrecognized Column");
            return NULL;
    }
}

PyObject* turingPyModule::typedColumnsToNumpyDictionary(std::vector<std::unique_ptr<TypedColumn>>* columns) {
    Profile profile {"turingPyModule::typedColumnsToNumpyDictionary"};

    PyObject* backingData = PyCapsule_New(columns, NULL, destroyTypedColumns);

    if (!backingData) {
        return NULL;
    }

    PyObject* numpyDict = PyDict_New();
    if (!numpyDict) {
        return NULL;
    }

    for (const auto& col : *columns) {
        PyObject* numpyArr = typedColumnToNumpyArray(col.get(), backingData);
        if (!numpyArr) {
            Py_DECREF(backingData);
            Py_DECREF(numpyDict);
            return NULL;
        }
        if (PyDict_SetItemString(numpyDict, col->getColumnName().c_str(), numpyArr) < 0) {
            Py_DECREF(backingData);
            Py_DECREF(numpyArr);
            Py_DECREF(numpyDict);

            return NULL;
        }
        Py_DECREF(numpyArr);
    }
    return numpyDict;
}

#include "PyUtils.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>

#include "TypedColumn.h"

using namespace turingClient;

void turingPyModule::printPythonObject(PyObject* obj, const char* name, bool use_repr) {
    if (!obj) {
        if (name && strlen(name) > 0) {
            std::cout << name << ": NULL" << std::endl;
        } else {
            std::cout << "NULL" << std::endl;
        }
        return;
    }

    // Get Python's print function
    PyObject* builtins = PyImport_ImportModule("builtins");
    if (!builtins) {
        std::cout << "Failed to import builtins" << std::endl;
        return;
    }

    PyObject* print_func = PyObject_GetAttrString(builtins, "print");
    if (!print_func) {
        Py_DECREF(builtins);
        std::cout << "Failed to get print function" << std::endl;
        return;
    }

    if (name && strlen(name) > 0) {
        PyObject* name_str = PyUnicode_FromString((std::string(name) + ":").c_str());
        if (use_repr) {
            PyObject* repr_obj = PyObject_Repr(obj);
            PyObject_CallFunction(print_func, "OO", name_str, repr_obj);
            Py_XDECREF(repr_obj);
        } else {
            PyObject_CallFunction(print_func, "OO", name_str, obj);
        }
        Py_DECREF(name_str);
    } else {
        if (use_repr) {
            PyObject* repr_obj = PyObject_Repr(obj);
            PyObject_CallFunction(print_func, "O", repr_obj);
            Py_XDECREF(repr_obj);
        } else {
            PyObject_CallFunction(print_func, "O", obj);
        }
    }

    Py_DECREF(print_func);
    Py_DECREF(builtins);
}

void destroyTypedColumns(PyObject* capsule) {
    std::vector<std::unique_ptr<turingClient::TypedColumn>>* col = static_cast<std::vector<std::unique_ptr<turingClient::TypedColumn>>*>(
        PyCapsule_GetPointer(capsule, NULL));
    std::cout << "destroying capsule \n";
    delete col;
}

PyObject* turingPyModule::createMaskedArray(PyObject* dataArray, PyObject* maskArray) {
    PyObject* ma_module = PyImport_ImportModule("numpy.ma");
    if (!ma_module) {
        Py_DECREF(dataArray);
        Py_DECREF(maskArray);
        return NULL;
    }

    PyObject* maArgs = PyTuple_Pack(5, dataArray, Py_None, Py_False, Py_None, maskArray);


    // Call numpy.ma.array(data, mask=mask)
    PyObject* maskedArray = PyObject_CallMethod(
        ma_module, "array", "O", maArgs);

    if (!maskedArray) {
        std::cout << "could not make masked array";
        Py_DECREF(ma_module);
        Py_DECREF(maArgs);
        return NULL;
    }

    Py_DECREF(ma_module);
    Py_DECREF(maArgs);

    std::cout << "Made masked array\n";
    return maskedArray;
}

PyObject* turingPyModule::vectorToList(std::vector<std::string>& vec) {
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
    import_array1(NULL);
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
                static_cast<uint64_t*>(col->data()));

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

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_ARRAY_UNIQUE_SYMBOL NPY_TuringDB

#include <Python.h>
#include <numpy/ndarrayobject.h>

#include "PyProfilerWrapper.h"
#include "PyTuringClient.h"

static struct PyModuleDef turingmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "turing",
    .m_doc = "Turing client Python module",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_turingdb_core_pymodule(void) {
    import_array1(NULL);

    if (PyType_Ready(&PyTypeObject_TuringClient) < 0) {
        return NULL;
    }

#ifdef TURING_PROFILE
    if (PyType_Ready(&PyTypeObject_ProfilerWrapper) < 0) {
        return NULL;
    }
#endif

    // Create the module
    PyObject* module = PyModule_Create(&turingmodule);
    if (module == NULL) {
        return NULL;
    }

    // Add the type to the module
    Py_INCREF(&PyTypeObject_TuringClient);
    if (PyModule_AddObject(module, "TuringClient", (PyObject*)&PyTypeObject_TuringClient) < 0) {
        Py_DECREF(&PyTypeObject_TuringClient);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <numpy/arrayobject.h>

#include "PyTuringRequests.h"

static struct PyModuleDef turingmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "turing",
    .m_doc = "Turing client Python module",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_turingdb_core_pymodule(void) {
    // Initialize NumPy
    import_array1(NULL);

    // Prepare the type
    if (PyType_Ready(&PyTypeObject_TuringRequest) < 0) {
        return NULL;
    }

    // Create the module
    PyObject* module = PyModule_Create(&turingmodule);
    if (module == NULL) {
        return NULL;
    }

    // Add the type to the module
    Py_INCREF(&PyTypeObject_TuringRequest);
    if (PyModule_AddObject(module, "TuringRequest", (PyObject*)&PyTypeObject_TuringRequest) < 0) {
        Py_DECREF(&PyTypeObject_TuringRequest);
        Py_DECREF(module);
        return NULL;
    }

    // Add module constants (optional)
    // PyModule_AddStringConstant(module, "__version__", "1.0.0");
    // PyModule_AddIntConstant(module, "DEFAULT_TIMEOUT", 30);

    return module;
}

#include <Python.h>

typedef struct {
    PyObject_HEAD PyObject* originalMethod;
} PyProfilerWrapper;

PyObject* PyProfilerNew(PyTypeObject* type, PyObject* args, PyObject* kwargs);
void PyProfilerDealloc(PyProfilerWrapper* self);
PyObject* PyProfilerFunction(PyProfilerWrapper* self, PyObject* args, PyObject* kwargs);

inline PyTypeObject PyTypeObject_ProfilerWrapper = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ProfilerWrapper",
    .tp_basicsize = sizeof(PyProfilerWrapper),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyProfilerDealloc,
    .tp_call = (ternaryfunc)PyProfilerFunction,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Wrapper for profiling calls",
    .tp_new = PyProfilerNew,
};

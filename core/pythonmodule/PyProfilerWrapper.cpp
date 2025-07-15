#include "PyProfilerWrapper.h"

#include <iostream>
#include <string>

#include "Profiler.h"

PyObject* PyProfilerNew(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
    PyProfilerWrapper* self = (PyProfilerWrapper*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->originalMethod = NULL;
    }
    return (PyObject*)self;
}

void PyProfilerDealloc(PyProfilerWrapper* self) {
    Py_XDECREF(self->originalMethod);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* PyProfilerFunction(PyProfilerWrapper* self, PyObject* args, PyObject* kwargs) {
    PyObject* result = PyObject_Call(self->originalMethod, args, kwargs);
    std::string profileLogs;

    Profiler::dump(profileLogs);

    std::cout << profileLogs << std::endl;

    Profiler::clear();

    return result;
}

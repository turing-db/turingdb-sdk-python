#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
#include "PyTuringClient.h"

#include "Profiler.h"
#include "PyProfilerWrapper.h"
#include "TypedColumn.h"
#include "PyUtils.h"

using namespace turingPyModule;
PyObject* PyObject_TuringClient_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    PyObject_TuringClient* self {};

    self = (PyObject_TuringClient*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->request = NULL;
    }

    return (PyObject*)self;
}
int PyObject_TuringClient_init(PyObject_TuringClient* self, PyObject* args) {
    Profile profile {"PyObject_TuringClient::init"};

    char* url {};

    if (!PyArg_ParseTuple(args, "s", &url)) {
        return -1;
    }

    std::string urlString {url};

    try {
        self->request = new turingClient::TuringClient(urlString);
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return -1;
    }

    return 0;
}
void PyObject_TuringClient_dealloc(PyObject_TuringClient* self) {
    if (self->request) {
        delete self->request;
        self->request = NULL;
    }

    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* listAvailableGraphs(PyObject_TuringClient* self) {
    Profile profile {"PyObject_TuringClient::listAvailableGraphs"};

    std::vector<std::string> availableGraphs;
    if (auto res = self->request->listAvailableGraphs(availableGraphs); !res) {
        PyErr_SetString(PyExc_RuntimeError, res.error().fmtMessage().c_str());
        return NULL;
    }

    return vectorToList(availableGraphs);
}

PyObject* listLoadedGraphs(PyObject_TuringClient* self) {
    Profile profile {"PyObject_TuringClient::listLoadedGraphs"};

    std::vector<std::string> availableGraphs;
    if (auto res = self->request->listLoadedGraphs(availableGraphs); !res) {
        PyErr_SetString(PyExc_RuntimeError, res.error().fmtMessage().c_str());
        return NULL;
    }

    return vectorToList(availableGraphs);
}
PyObject* loadGraph(PyObject_TuringClient* self, PyObject* args) {
    Profile profile {"PyObject_TuringClient::loadGraph"};

    char* graph {};
    if (!PyArg_ParseTuple(args, "s", &graph)) {
        return NULL;
    }

    if (auto res = self->request->loadGraph(graph); !res) {
        PyErr_SetString(PyExc_RuntimeError, res.error().fmtMessage().c_str());
        return NULL;
    }

    return Py_None;
}

PyObject* query(PyObject_TuringClient* self, PyObject* args, PyObject* kwargs) {
    Profile profile {"PyObject_TuringClient::query"};

    char* query {};
    char* graph {};

    if (!PyArg_ParseTuple(args, "ss", &query, &graph)) {
        return NULL;
    }

    auto* queryResult = new std::vector<std::unique_ptr<turingClient::TypedColumn>>;

    if (auto res = self->request->query(query, graph, *queryResult); !res) {
        PyErr_SetString(PyExc_RuntimeError, res.error().fmtMessage().c_str());
        return NULL;
    }

    return typedColumnsToNumpyDictionary(queryResult);
}

PyObject* callProfilingWrapper(PyObject_TuringClient* self, PyObject* name) {
    PyObject* attr = PyObject_GenericGetAttr((PyObject*)self, name);

    if (attr == NULL) {
        return NULL;
    }

    if (PyCallable_Check(attr)) {
        PyProfilerWrapper* wrappedCall = PyObject_New(PyProfilerWrapper, &PyTypeObject_ProfilerWrapper);
        if (wrappedCall == NULL) {
            Py_DECREF(attr);
            return NULL;
        }

        wrappedCall->originalMethod = attr;
        return (PyObject*)wrappedCall;
    }

    return attr;
}

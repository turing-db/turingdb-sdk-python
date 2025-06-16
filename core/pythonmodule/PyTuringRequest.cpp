#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
#include "PyTuringRequests.h"

#include "TypedColumn.h"
#include "PyUtils.h"

using namespace turingPyModule;
PyObject* PyObject_TuringRequest_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    import_array1(NULL);
    PyObject_TuringRequests* self {};

    self = (PyObject_TuringRequests*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->request = NULL;
    }

    return (PyObject*)self;
}
int PyObject_TuringRequest_init(PyObject_TuringRequests* self, PyObject* args) {
    // Parse any constructor arguments
    char* url {};

    if (!PyArg_ParseTuple(args, "s", &url)) {
        return -1;
    }

    std::string urlString {url};

    try {
        self->request = new turingClient::TuringRequest(urlString);
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return -1;
    }

    return 0; // Success
}
void PyObject_TuringRequest_dealloc(PyObject_TuringRequests* self) {
    if (self->request) {
        delete self->request;
        self->request = NULL;
    }

    // Call the base type's dealloc
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* listAvailableGraphs(PyObject_TuringRequests* self) {
    std::vector<std::string> availableGraphs;
    if (auto res = self->request->listAvailableGraphs(availableGraphs); !res) {
        PyErr_SetString(PyExc_RuntimeError, res.error().fmtMessage().c_str());
        return NULL;
    }

    return vectorToList(availableGraphs);
}

PyObject* listLoadedGraphs(PyObject_TuringRequests* self) {
    std::vector<std::string> availableGraphs;
    if (auto res = self->request->listLoadedGraphs(availableGraphs); !res) {
        PyErr_SetString(PyExc_RuntimeError, res.error().fmtMessage().c_str());
        return NULL;
    }

    return vectorToList(availableGraphs);
}
PyObject* loadGraph(PyObject_TuringRequests* self, PyObject* args) {
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

PyObject* query(PyObject_TuringRequests* self, PyObject* args, PyObject* kwargs) {
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

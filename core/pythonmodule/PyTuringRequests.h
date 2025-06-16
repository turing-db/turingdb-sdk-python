#include <Python.h>
#include <structmember.h>

#include "TuringRequest.h"

typedef struct {
    PyObject_HEAD
        turingClient::TuringRequest* request;
} PyObject_TuringRequests;

PyObject* PyObject_TuringRequest_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
int PyObject_TuringRequest_init(PyObject_TuringRequests* self, PyObject* args);
void PyObject_TuringRequest_dealloc(PyObject_TuringRequests* self);


PyObject* listAvailableGraphs(PyObject_TuringRequests* self);
PyObject* listLoadedGraphs(PyObject_TuringRequests* self);
PyObject* loadGraph(PyObject_TuringRequests* self, PyObject* args);
PyObject* query(PyObject_TuringRequests* self, PyObject* args, PyObject* kwargs);


static PyMethodDef PyObject_TuringRequest_methods[] = {
    {"list_available_graphs", (PyCFunction)listAvailableGraphs, METH_NOARGS, "List Avaialble Graphs"},
    {"list_loaded_graphs", (PyCFunction)listLoadedGraphs, METH_NOARGS, "List Loaded Graphs"},
    {"load_graph", (PyCFunction)loadGraph, METH_VARARGS, "Load Graph"},
    {"query", (PyCFunction)query, METH_VARARGS, "Send Query To A Graph"},
    {NULL}  // Sentinel
};

inline PyTypeObject PyTypeObject_TuringRequest = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
                   .tp_name = "core.TuringRequest",
    .tp_basicsize = sizeof(PyObject_TuringRequests),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyObject_TuringRequest_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "TuringRequest object to interact with the graph",
    .tp_methods = PyObject_TuringRequest_methods,
    .tp_init = (initproc)PyObject_TuringRequest_init,
    .tp_new = PyObject_TuringRequest_new,
};

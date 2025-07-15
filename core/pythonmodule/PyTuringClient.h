#include <Python.h>
#include <structmember.h>

#include "TuringClient.h"

typedef struct {
    PyObject_HEAD
        turingClient::TuringClient* client;
} PyObject_TuringClient;

PyObject* PyObject_TuringClient_new(PyTypeObject* type, PyObject* args, PyObject* kwds);
int PyObject_TuringClient_init(PyObject_TuringClient *self, PyObject *args);
void PyObject_TuringClient_dealloc(PyObject_TuringClient *self);

PyObject *listAvailableGraphs(PyObject_TuringClient *self);
PyObject *listLoadedGraphs(PyObject_TuringClient *self);
PyObject *loadGraph(PyObject_TuringClient *self, PyObject *args);

PyObject *setBearerToken(PyObject_TuringClient *self, PyObject *args);
PyObject *clearBearerToken(PyObject_TuringClient *self);

PyObject *query(PyObject_TuringClient *self, PyObject *args, PyObject *kwargs);

PyObject *callProfilingWrapper(PyObject_TuringClient *self, PyObject *name);

static PyMethodDef PyObject_TuringClient_Methods[] = {
    {"list_available_graphs", (PyCFunction)listAvailableGraphs, METH_NOARGS,
     "List Avaialble Graphs"},
    {"list_loaded_graphs", (PyCFunction)listLoadedGraphs, METH_NOARGS,
     "List Loaded Graphs"},
    {"load_graph", (PyCFunction)loadGraph, METH_VARARGS, "Load Graph"},
    {"set_auth_token", (PyCFunction)setBearerToken, METH_VARARGS,
     "Set The Auth Token For Requests"},
    {"clear_auth_token", (PyCFunction)clearBearerToken, METH_NOARGS,
     "Clear The Auth Token For Requests"},
    {"query", (PyCFunction)query, METH_VARARGS, "Send Query To A Graph"},
    {NULL} // Sentinel
};

inline PyTypeObject PyTypeObject_TuringClient = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "core.TuringClient",
    .tp_basicsize = sizeof(PyObject_TuringClient),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyObject_TuringClient_dealloc,
#ifdef TURING_PROFILE
    .tp_getattro = (getattrofunc)callProfilingWrapper,
#endif
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "TuringClient object to interact with the graph",
    .tp_methods = PyObject_TuringClient_Methods,
    .tp_init = (initproc)PyObject_TuringClient_init,
    .tp_new = PyObject_TuringClient_new,
};

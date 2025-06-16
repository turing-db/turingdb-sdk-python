#include <Python.h>
#include <memory>
#include <vector>
#include <string>

namespace turingClient {
class TypedColumn;
}

namespace turingPyModule {

PyObject* createMaskedArray(PyObject* dataArray, PyObject* maskArray);
PyObject* vectorToList(std::vector<std::string>& vec);
PyObject* typedColumnToNumpyArray(turingClient::TypedColumn* col, PyObject* backingData);
PyObject* typedColumnsToNumpyDictionary(std::vector<std::unique_ptr<turingClient::TypedColumn>>* columns);

void printPythonObject(PyObject* obj, const char* name = "", bool use_repr = false);
}

#include <Python.h>

static PyObject *
erlang_b(PyObject *self, PyObject *args)
{
    int idx;
    int servers, traffic;
    float s = 1.0, r = 0.0;
    static PyObject *module;
    static PyObject *decimal = NULL;
    static PyObject *result;
    PyObject *tuple=PyTuple_New(1);

    if (!PyArg_ParseTuple(args, "ii", &servers, &traffic))
        return NULL;
    if (traffic <= 0) {
        r = (float)0.0;
    }
    else if (servers <= 0) {
        r = (float)1.0;
    }
    else {
        for (idx = 1; idx <= servers; idx++) {
            r = (traffic*s)/(idx+(traffic*s));
            s = r;
        }
    }

    // thanks
    // http://src.gnu-darwin.org/ports/databases/py-sqlrelay/work/sqlrelay/src/api/python/CSQLRelay.C
    module = PyImport_ImportModule("decimal");
    decimal = PyObject_GetAttrString(module, "Decimal");
    PyTuple_SetItem(tuple, 0, Py_BuildValue("f", r));
    result = PyObject_CallObject(decimal,tuple);
    return result;
}

static PyObject *
erlang_b_ext(PyObject *self, PyObject *args)
{
    int idx;
    int servers, traffic;
    float retry;
    float s = 1.0, r = 0.0, t = 0.0;
    static PyObject *module;
    static PyObject *decimal = NULL;
    static PyObject *result;
    PyObject *tuple=PyTuple_New(1);

    if (!PyArg_ParseTuple(args, "iif", &servers, &traffic, &retry))
        return NULL;
    if (traffic <= 0) {
        r = (float)0.0;
    }
    else if (servers <= 0) {
        r = (float)1.0;
    }
    else {
        for (idx = 1; idx <= servers; idx++) {
            r = (traffic*s)/(idx+(traffic*s));
            t = 1/(1-(r*retry));
            r = (traffic*s*t)/(idx+(traffic*s*t));
            s = r;
        }
    }

    // thanks
    // http://src.gnu-darwin.org/ports/databases/py-sqlrelay/work/sqlrelay/src/api/python/CSQLRelay.C
    module = PyImport_ImportModule("decimal");
    decimal = PyObject_GetAttrString(module, "Decimal");
    PyTuple_SetItem(tuple, 0, Py_BuildValue("f", r));
    result = PyObject_CallObject(decimal,tuple);
    return result;
}

static PyMethodDef ErlangMethods[] = {
    {"erlang_b",  erlang_b, METH_VARARGS, 
     "Erlang-B is a formula for the blocking probability derived from the \n" 
     "Erlang distribution to describe the probability of call loss on a \n"
     "group of circuits.\n"
     "Parameters:\n"
     "   servers: number of servers (lines)\n"
     "   traffic: offered traffic\n"
     "returns:\n"
     "    GoS: grade of service (blocking probability)"},
    {"erlang_b_ext", erlang_b_ext, METH_VARARGS, 
     "Extended Erlang B"},
    {NULL, NULL, 0, NULL}

};

PyMODINIT_FUNC
initerlang(void) {
    Py_InitModule("erlang", ErlangMethods);
}

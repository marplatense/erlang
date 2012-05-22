#include <Python.h>

static PyObject *
erlang_b(PyObject *self, PyObject *args)
{
    int idx;
    int servers, traffic;
    float s = 1.0, r = 0.0;
    
    if (!PyArg_ParseTuple(args, "ii", &servers, &traffic))
        return NULL;
    if (servers <= 0) {
        return Py_BuildValue("f", 0.0);
    }
    if (traffic <= 0) {
        return Py_BuildValue("f", 0.0);
    }
    for (idx = 1; idx <= servers; idx++) {
        r = (traffic*s)/(idx+(traffic*s));
        s = r;
    } 
    return Py_BuildValue("f", r);
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
    {NULL, NULL, 0, NULL}

};

PyMODINIT_FUNC
initerlang(void) {
    Py_InitModule("erlang", ErlangMethods);
}

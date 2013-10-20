#include <Python.h>


static PyObject *
cast_to_decimal(float value)
{
    static PyObject *module;
    static PyObject *decimal = NULL;
    static PyObject *result;
    PyObject *tuple=PyTuple_New(1);

    // thanks
    // http://src.gnu-darwin.org/ports/databases/py-sqlrelay/work/sqlrelay/src/api/python/CSQLRelay.C
    module = PyImport_ImportModule("decimal");
    decimal = PyObject_GetAttrString(module, "Decimal");
    PyTuple_SetItem(tuple, 0, Py_BuildValue("f", value));
    result = PyObject_CallObject(decimal,tuple);
    return result;
}

static PyObject *
erlang_b(PyObject *self, PyObject *args)
{
    int idx;
    float servers, traffic;
    float s = 1.0, r = 0.0;

    if (!PyArg_ParseTuple(args, "ff", &servers, &traffic))
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

    return cast_to_decimal(r);
}

static PyObject *
erlang_b_ext(PyObject *self, PyObject *args)
{
    int idx;
    float servers, traffic;
    float retry;
    float s = 1.0, r = 0.0, t = 0.0;

    if (!PyArg_ParseTuple(args, "fff", &servers, &traffic, &retry))
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

    return cast_to_decimal(r);
}

static PyObject *
engset(PyObject *self, PyObject *args)
{

    int idx;
    float servers, traffic;
    float intensity;
    float s = 1.0, r = 0.0;

    if (!PyArg_ParseTuple(args, "fff", &servers, &traffic, &intensity))
        return NULL;
    if ((servers<0) || (intensity<0)) {
        r =  (float)0.0;
    }
    else {
        for (idx = 1; idx <= servers; idx++) {
            r = (s*(idx/((traffic-idx)*intensity))) +1;
            s = r;
        }
    }

    return cast_to_decimal(r);
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
    {"erlang_b_ext", erlang_b_ext, METH_VARARGS, "Extended Erlang B"},
    {"engset", engset, METH_VARARGS, "Engset B"},
    {NULL, NULL, 0, NULL}

};

static struct PyModuleDef erlangmodule = {
    PyModuleDef_HEAD_INIT,
    "erlang",
    NULL,
    -1,
    ErlangMethods
};

PyMODINIT_FUNC
PyInit_erlang(void) {
    return PyModule_Create(&erlangmodule);
    //Py_InitModule("erlang", ErlangMethods);
}

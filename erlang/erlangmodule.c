#include <Python.h>
#include <limits.h>

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

static float 
ErlangB(float servers, float intensity)
{
    int idx;
    float s = 1.0, r = 0.0;
    if (intensity <= 0) {
        r = (float)0.0;
    }
    else if (servers <= 0) {
        r = (float)1.0;
    }
    else {
        for (idx = 1; idx <= servers; idx++) {
            r = (intensity*s)/(idx+(intensity*s));
            s = r;
        }
    }
    return r;
}

static PyObject *
erlang_b(PyObject *self, PyObject *args)
{
    float servers, intensity;
    float r = 0.0;

    if (!PyArg_ParseTuple(args, "ff", &servers, &intensity))
        return NULL;
    r = ErlangB(servers, intensity);
    return cast_to_decimal(r);
}

static float 
ErlangBExt(float servers, float intensity, float retry)
{
    int idx;
    float s = 1.0, r = 0.0, t = 0.0;
    if (intensity<= 0) {
        r = (float)0.0;
    }
    else if (servers <= 0) {
        r = (float)1.0;
    }
    else {
        for (idx = 1; idx <= servers; idx++) {
            r = (intensity*s)/(idx+(intensity*s));
            t = 1/(1-(r*retry));
            r = (intensity*s*t)/(idx+(intensity*s*t));
            s = r;
        }
    }
    return r;
}

static PyObject *
erlang_b_ext(PyObject *self, PyObject *args)
{
    float servers, intensity, retry;
    float r = 0.0;

    if (!PyArg_ParseTuple(args, "fff", &servers, &intensity, &retry))
        return NULL;
    r = ErlangBExt(servers, intensity, retry);
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

static float
ErlangC(float servers, float intensity)
{
    float s =  0.0, r = 0.0;
    s = ErlangB(servers, intensity);
    r = s/(((intensity/servers)*s)+(1-(intensity/servers)));
    return r;
}

static PyObject *
erlangc(PyObject *self, PyObject *args)
{
    float servers, intensity;
    float r = 0.0;
    if (!PyArg_ParseTuple(args, "ff", &servers, &intensity))
        return NULL;
    if ((servers<0) || (intensity<0)) {
        r =  (float)0.0;
    }
    else {
        r = ErlangC(servers, intensity);
    }
    return cast_to_decimal(r);
}

static PyObject *
nb_trunks(PyObject *self, PyObject *args)
{
    
    int idx = 1, sngcount;
    float intensity, blocking;
    float r = 0.0, b = 0.0;
    if (!PyArg_ParseTuple(args, "ff", &intensity, &blocking))
        return NULL;
    if ((blocking<0) || (intensity<0)) {
        r =  (float)0.0;
    }
    else {
        for (idx = 1; idx <= SHRT_MAX; idx++) {
            sngcount = idx;
            b = ErlangB(sngcount, intensity);
            if (b <= blocking) {
                break;
            }
        }
    }
    if (idx<SHRT_MAX) {
        r = idx;
    }
    return cast_to_decimal(r);
}

static PyMethodDef ErlangMethods[] = {
    {"erlang_b",  erlang_b, METH_VARARGS, 
     "The Erlang B formula calculates the percentage likelyhood of the call"
     " being blocked, that is that all the trunks are in use and the caller"
     " will receive a busy signal.\n"
     " Servers = Number of telephone lines\n"
     " Intensity = Arrival rate of calls / Completion rate of calls\n"
     "   Arrival rate = the number of calls arriving per hour\n"
     "   Completion rate = the number of calls completed per hour\n\n"
     "Original version of this function Copyright © T&C Limited 1996, 1999"},
    {"erlang_b_ext", erlang_b_ext, METH_VARARGS, 
     "The Extended Erlang B formula calculates the percentage likelyhood of the call"
     " being blocked, that is that all the trunks are in use and the caller"
     " will receive a busy signal. The Extended version allows input of a percentage"
     " figure for those blocked callers who will immediately retry.\n"
     " Servers = Number of telephone lines\n"
     " Intensity = Arrival rate of calls / Completion rate of calls\n"
     "   Arrival rate = the number of calls arriving per hour\n"
     "   Completion rate = the number of calls completed per hour\n"
     " Retry = Number of blocked callers who will retry immediately (0.1 = 10%)\n\n"
     "Original version of this function Copyright © T&C Limited 1996, 1999"},
    {"engset", engset, METH_VARARGS,
     "The Engset B formula calculates the percentage likelyhood of the call"
     " being blocked, that is that all the trunks are in use and the caller"
     " will receive a busy signal. This uses the Engset model, based on the"
     " hindrance formula.\n"
     " Servers = Number of telephone lines\n"
     " Events = Number of calls\n"
     " Intensity = average intensity per call\n\n"
     "Original version of this function Copyright © T&C Limited 1996, 1999"},
    {"erlang_c", erlangc, METH_VARARGS,
     "This formula gives the percentage likelyhood of the caller being"
     " placed in a queue.\n"
     " Servers = Number of agents\n"
     " Intensity = Arrival rate of calls / Completion rate of calls\n"
     "   Arrival rate = the number of calls arriving per hour\n"
     "  Completion rate = the number of calls completed per hour\n\n"
     "Original version of this function Copyright © T&C Limited 1996, 1999"},
    {"nb_trunks", nb_trunks, METH_VARARGS,
     "This formula gives the number of telephone lines required to"
     " handle the Busyhour traffic in Erlang against a required blocking factor\n"
     " Intensity = Busyhour Traffic in erlangs\n"
     " Blocking = blocking factor percentage e.g. 0.10  (10% of calls may"
     " receive busy tone)\n\n"
     "Original version of this function Copyright © T&C Limited 2000, 2001\n"
     "This function has been supplied by Edwin Barendse"},
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

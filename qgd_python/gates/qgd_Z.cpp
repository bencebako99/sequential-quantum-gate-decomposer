/*
Created on Fri Jun 26 14:42:56 2020
Copyright (C) 2020 Peter Rakyta, Ph.D.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.

@author: Peter Rakyta, Ph.D.
*/
/*
\file qgd_Z.cpp
\brief Python interface for the Z gate class
*/

#define PY_SSIZE_T_CLEAN


#include <Python.h>
#include "structmember.h"
#include "Z.h"
#include "numpy_interface.h"




/**
@brief Type definition of the qgd_Z Python class of the qgd_Z module
*/
typedef struct {
    PyObject_HEAD
    /// Pointer to the C++ class of the X gate
    Z* gate;
} qgd_Z;


/**
@brief Creates an instance of class N_Qubit_Decomposition and return with a pointer pointing to the class instance (C++ linking is needed)
@param qbit_num The number of qubits spanning the operation.
@param target_qbit The 0<=ID<qbit_num of the target qubit.
*/
Z* 
create_Z( int qbit_num, int target_qbit ) {

    return new Z( qbit_num, target_qbit );
}


/**
@brief Call to deallocate an instance of N_Qubit_Decomposition class
@param ptr A pointer pointing to an instance of N_Qubit_Decomposition class.
*/
void
release_Z( Z*  instance ) {
    delete instance;
    return;
}





extern "C"
{


/**
@brief Method called when a python instance of the class qgd_Z is destroyed
@param self A pointer pointing to an instance of class qgd_Z.
*/
static void
qgd_Z_dealloc(qgd_Z *self)
{

    // release the X gate
    release_Z( self->gate );

    Py_TYPE(self)->tp_free((PyObject *) self);
}


/**
@brief Method called when a python instance of the class qgd_Z is allocated
@param type A pointer pointing to a structure describing the type of the class qgd_Z.
*/
static PyObject *
qgd_Z_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    qgd_Z *self;
    self = (qgd_Z *) type->tp_alloc(type, 0);
    if (self != NULL) {}
    return (PyObject *) self;
}


/**
@brief Method called when a python instance of the class qgd_Z is initialized
@param self A pointer pointing to an instance of the class qgd_Z.
@param args A tuple of the input arguments: qbit_num (int), target_qbit (int), Theta (bool) , Phi (bool), Lambda (bool)
@param kwds A tuple of keywords
*/
static int
qgd_Z_init(qgd_Z *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {(char*)"qbit_num", (char*)"target_qbit", NULL};
    int  qbit_num = -1; 
    int target_qbit = -1;

    if (PyArray_API == NULL) {
        import_array();
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii", kwlist,
                                     &qbit_num, &target_qbit))
        return -1;

    if (qbit_num != -1 && target_qbit != -1) {
        self->gate = create_Z( qbit_num, target_qbit );
    }
    return 0;
}

/**
@brief Extract the optimized parameters
@param start_index The index of the first inverse gate
*/
/**
static PyObject *
qgd_Z_get_Matrix( qgd_Z *self, PyObject *args ) {

    PyObject * parameters_arr = NULL;


    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &parameters_arr )) 
        return Py_BuildValue("i", -1);

    
    if ( PyArray_IS_C_CONTIGUOUS(parameters_arr) ) {
        Py_INCREF(parameters_arr);
    }
    else {
        parameters_arr = PyArray_FROM_OTF(parameters_arr, NPY_DOUBLE, NPY_ARRAY_IN_ARRAY);
    }


    // get the C++ wrapper around the data
    Matrix_real&& parameters_mtx = numpy2matrix_real( parameters_arr );


    Matrix Z_mtx = self->gate->get_matrix(  );
    
    // convert to numpy array
    Z_mtx.set_owner(false);
    PyObject *Z_py = matrix_to_numpy( Z_mtx );


    Py_DECREF(parameters_arr);

    return Z_py;
}

*/
/**
@brief Extract the optimized parameters
@param start_index The index of the first inverse gate
*/
static PyObject *
qgd_Z_get_Matrix( qgd_Z *self ) {

   
    Matrix Z_mtx = self->gate->get_matrix(  );
    
    // convert to numpy array
    Z_mtx.set_owner(false);
    PyObject *Z_py = matrix_to_numpy( Z_mtx );


    return Z_py;
}

/**
@brief Call to apply the gate operation on the inut matrix
*/
static PyObject *
qgd_Z_apply_to( qgd_Z *self, PyObject *args ) {

    PyObject * unitary_arg = NULL;


    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &unitary_arg )) 
        return Py_BuildValue("i", -1);


    // convert python object array to numpy C API array
    if ( unitary_arg == NULL ) {
        PyErr_SetString(PyExc_Exception, "Input matrix was not given");
        return NULL;
    }

    PyObject* unitary = PyArray_FROM_OTF(unitary_arg, NPY_COMPLEX128, NPY_ARRAY_IN_ARRAY);

    // test C-style contiguous memory allocation of the array
    if ( !PyArray_IS_C_CONTIGUOUS(unitary) ) {
        PyErr_SetString(PyExc_Exception, "input mtrix is not memory contiguous");
        return NULL;
    }


    // create QGD version of the input matrix
    Matrix unitary_mtx = numpy2matrix(unitary);

    self->gate->apply_to( unitary_mtx );
    
    Py_DECREF(unitary);

    return Py_BuildValue("i", 0);
}



/**
@brief Structure containing metadata about the members of class qgd_Z.
*/
static PyMemberDef qgd_Z_members[] = {
    {NULL}  /* Sentinel */
};


/**
@brief Structure containing metadata about the methods of class qgd_Z.
*/
static PyMethodDef qgd_Z_methods[] = {
    {"get_Matrix", (PyCFunction) qgd_Z_get_Matrix, METH_VARARGS,
     "Method to get the matrix of the operation."
    },
    {"apply_to", (PyCFunction) qgd_Z_apply_to, METH_VARARGS,
     "Call to apply the gate on the input matrix."
    },
    {NULL}  /* Sentinel */
};


/**
@brief A structure describing the type of the class qgd_Z.
*/
static PyTypeObject  qgd_Z_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "qgd_Z.qgd_Z", /*tp_name*/
  sizeof(qgd_Z), /*tp_basicsize*/
  0, /*tp_itemsize*/
  (destructor) qgd_Z_dealloc, /*tp_dealloc*/
  #if PY_VERSION_HEX < 0x030800b4
  0, /*tp_print*/
  #endif
  #if PY_VERSION_HEX >= 0x030800b4
  0, /*tp_vectorcall_offset*/
  #endif
  0, /*tp_getattr*/
  0, /*tp_setattr*/
  #if PY_MAJOR_VERSION < 3
  0, /*tp_compare*/
  #endif
  #if PY_MAJOR_VERSION >= 3
  0, /*tp_as_async*/
  #endif
  0, /*tp_repr*/
  0, /*tp_as_number*/
  0, /*tp_as_sequence*/
  0, /*tp_as_mapping*/
  0, /*tp_hash*/
  0, /*tp_call*/
  0, /*tp_str*/
  0, /*tp_getattro*/
  0, /*tp_setattro*/
  0, /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "Object to represent a X gate of the QGD package.", /*tp_doc*/
  0, /*tp_traverse*/
  0, /*tp_clear*/
  0, /*tp_richcompare*/
  0, /*tp_weaklistoffset*/
  0, /*tp_iter*/
  0, /*tp_iternext*/
  qgd_Z_methods, /*tp_methods*/
  qgd_Z_members, /*tp_members*/
  0, /*tp_getset*/
  0, /*tp_base*/
  0, /*tp_dict*/
  0, /*tp_descr_get*/
  0, /*tp_descr_set*/
  0, /*tp_dictoffset*/
  (initproc) qgd_Z_init, /*tp_init*/
  0, /*tp_alloc*/
  qgd_Z_new, /*tp_new*/
  0, /*tp_free*/
  0, /*tp_is_gc*/
  0, /*tp_bases*/
  0, /*tp_mro*/
  0, /*tp_cache*/
  0, /*tp_subclasses*/
  0, /*tp_weaklist*/
  0, /*tp_del*/
  0, /*tp_version_tag*/
  #if PY_VERSION_HEX >= 0x030400a1
  0, /*tp_finalize*/
  #endif
  #if PY_VERSION_HEX >= 0x030800b1
  0, /*tp_vectorcall*/
  #endif
  #if PY_VERSION_HEX >= 0x030800b4 && PY_VERSION_HEX < 0x03090000
  0, /*tp_print*/
  #endif
};


/**
@brief Structure containing metadata about the module.
*/
static PyModuleDef  qgd_Z_Module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "qgd_Z",
    .m_doc = "Python binding for QGD X gate",
    .m_size = -1,
};


/**
@brief Method called when the Python module is initialized
*/
PyMODINIT_FUNC
PyInit_qgd_Z(void)
{
    PyObject *m;
    if (PyType_Ready(& qgd_Z_Type) < 0)
        return NULL;

    m = PyModule_Create(& qgd_Z_Module);
    if (m == NULL)
        return NULL;

    Py_INCREF(& qgd_Z_Type);
    if (PyModule_AddObject(m, "qgd_Z", (PyObject *) & qgd_Z_Type) < 0) {
        Py_DECREF(& qgd_Z_Type);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}




}

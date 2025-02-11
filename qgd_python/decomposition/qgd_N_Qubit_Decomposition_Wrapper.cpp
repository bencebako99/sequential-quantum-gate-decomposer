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
\file qgd_N_Qubit_Decomposition_Wrapper.cpp
\brief Python interface for the N_Qubit_Decomposition class
*/

#define PY_SSIZE_T_CLEAN


#include <Python.h>
#include <numpy/arrayobject.h>
#include "structmember.h"
#include <stdio.h>
#include "N_Qubit_Decomposition.h"

#include "numpy_interface.h"





/**
@brief Type definition of the qgd_gates_Block Python class of the qgd_Gates_Block module
*/
typedef struct qgd_Gates_Block {
    PyObject_HEAD
    Gates_block* gate;
} qgd_Gates_Block;


/**
@brief Type definition of the qgd_N_Qubit_Decomposition_Wrapper Python class of the qgd_N_Qubit_Decomposition_Wrapper module
*/
typedef struct qgd_N_Qubit_Decomposition_Wrapper {
    PyObject_HEAD
    /// pointer to the unitary to be decomposed to keep it alive
    PyObject *Umtx;
    /// An object to decompose the unitary
    N_Qubit_Decomposition* decomp;

} qgd_N_Qubit_Decomposition_Wrapper;



/**
@brief Creates an instance of class N_Qubit_Decomposition and return with a pointer pointing to the class instance (C++ linking is needed)
@param Umtx An instance of class Matrix containing the unitary to be decomposed
@param qbit_num Number of qubits spanning the unitary
@param optimize_layer_num Logical value. Set true to optimize the number of decomposing layers during the decomposition procedure, or false otherwise.
@param initial_guess Type to guess the initial values for the optimization. Possible values: ZEROS=0, RANDOM=1, CLOSE_TO_ZERO=2
@return Return with a void pointer pointing to an instance of N_Qubit_Decomposition class.
*/
N_Qubit_Decomposition* 
create_N_Qubit_Decomposition( Matrix& Umtx, int qbit_num, bool optimize_layer_num, guess_type initial_guess ) {

    return new N_Qubit_Decomposition( Umtx, qbit_num, optimize_layer_num, initial_guess );
}


/**
@brief Call to deallocate an instance of N_Qubit_Decomposition class
@param ptr A pointer pointing to an instance of N_Qubit_Decomposition class.
*/
void
release_N_Qubit_Decomposition( N_Qubit_Decomposition*  instance ) {

    if (instance != NULL ) {
        delete instance;
    }
    return;
}



extern "C"
{


/**
@brief Method called when a python instance of the class qgd_N_Qubit_Decomposition_Wrapper is destroyed
@param self A pointer pointing to an instance of class qgd_N_Qubit_Decomposition_Wrapper.
*/
static void
qgd_N_Qubit_Decomposition_Wrapper_dealloc(qgd_N_Qubit_Decomposition_Wrapper *self)
{

    if ( self->decomp != NULL ) {
        // deallocate the instance of class N_Qubit_Decomposition
        release_N_Qubit_Decomposition( self->decomp );
        self->decomp = NULL;
    }

    if ( self->Umtx != NULL ) {
        // release the unitary to be decomposed
        Py_DECREF(self->Umtx);    
        self->Umtx = NULL;
    }
    
    Py_TYPE(self)->tp_free((PyObject *) self);

}

/**
@brief Method called when a python instance of the class qgd_N_Qubit_Decomposition_Wrapper is allocated
@param type A pointer pointing to a structure describing the type of the class qgd_N_Qubit_Decomposition_Wrapper.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    qgd_N_Qubit_Decomposition_Wrapper *self;
    self = (qgd_N_Qubit_Decomposition_Wrapper *) type->tp_alloc(type, 0);
    if (self != NULL) {}

    self->decomp = NULL;
    self->Umtx = NULL;

    return (PyObject *) self;
}


/**
@brief Method called when a python instance of the class qgd_N_Qubit_Decomposition_Wrapper is initialized
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: Umtx (numpy array), qbit_num (integer), optimize_layer_num (bool), initial_guess (string PyObject 
@param kwds A tuple of keywords
*/
static int
qgd_N_Qubit_Decomposition_Wrapper_init(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args, PyObject *kwds)
{
    // The tuple of expected keywords
    static char *kwlist[] = {(char*)"Umtx", (char*)"qbit_num", (char*)"optimize_layer_num", (char*)"initial_guess", NULL};
 
    // initiate variables for input arguments
    PyObject *Umtx_arg = NULL;
    int  qbit_num = -1; 
    bool optimize_layer_num = false;
    PyObject *initial_guess = NULL;

    // parsing input arguments
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OibO", kwlist,
                                     &Umtx_arg, &qbit_num, &optimize_layer_num, &initial_guess))
        return -1;

    // convert python object array to numpy C API array
    if ( Umtx_arg == NULL ) return -1;
    self->Umtx = PyArray_FROM_OTF(Umtx_arg, NPY_COMPLEX128, NPY_ARRAY_IN_ARRAY);

    // test C-style contiguous memory allocation of the array
    if ( !PyArray_IS_C_CONTIGUOUS(self->Umtx) ) {
        std::cout << "Umtx is not memory contiguous" << std::endl;
    }


    // create QGD version of the Umtx
    Matrix Umtx_mtx = numpy2matrix(self->Umtx);  


    // determine the initial guess type
    PyObject* initial_guess_string = PyObject_Str(initial_guess);
    PyObject* initial_guess_string_unicode = PyUnicode_AsEncodedString(initial_guess_string, "utf-8", "~E~");
    const char* initial_guess_C = PyBytes_AS_STRING(initial_guess_string_unicode);

    guess_type qgd_initial_guess;
    if ( strcmp("zeros", initial_guess_C) == 0 or strcmp("ZEROS", initial_guess_C) == 0) {
        qgd_initial_guess = ZEROS;        
    }
    else if ( strcmp("random", initial_guess_C)==0 or strcmp("RANDOM", initial_guess_C)==0) {
        qgd_initial_guess = RANDOM;        
    }
    else if ( strcmp("close_to_zero", initial_guess_C)==0 or strcmp("CLOSE_TO_ZERO", initial_guess_C)==0) {
        qgd_initial_guess = CLOSE_TO_ZERO;        
    }
    else {
        std::cout << "Wrong initial guess format. Using default ZEROS." << std::endl; 
        qgd_initial_guess = ZEROS;     
    }

    // create an instance of the class N_Qubit_Decomposition
    if (qbit_num > 0 ) {
        self->decomp =  create_N_Qubit_Decomposition( Umtx_mtx, qbit_num, optimize_layer_num, qgd_initial_guess);
    }
    else {
        std::cout << "The number of qubits should be given as a positive integer, " << qbit_num << "  was given" << std::endl;
        return -1;
    }



    Py_XDECREF(initial_guess_string);
    Py_XDECREF(initial_guess_string_unicode);

    return 0;
}

/**
@brief Wrapper function to call the start_decomposition method of C++ class N_Qubit_Decomposition
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: prepare_export (bool)
@param kwds A tuple of keywords
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_Start_Decomposition(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args, PyObject *kwds)
{

    // The tuple of expected keywords
    static char *kwlist[] = {(char*)"prepare_export", NULL};

    // initiate variables for input arguments
    bool  prepare_export = true; 

    // parsing input arguments
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|b", kwlist,
                                     &prepare_export))
        return Py_BuildValue("i", -1);


    // starting the decomposition
    self->decomp->start_decomposition(true, prepare_export);


    return Py_BuildValue("i", 0);

}





/**
@brief Wrapper function to get the number of decomposing gates.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@return Returns with the number of gates
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_get_gate_num( qgd_N_Qubit_Decomposition_Wrapper *self ) {

    // get the number of gates
    int ret = self->decomp->get_gate_num();


    return Py_BuildValue("i", ret);

}



/**
@brief Call to get the metadata organised into Python dictionary of the idx-th gate
@param decomp A pointer pointing to an instance of the class N_Qubit_Decomposition.
@param idx Labels the idx-th decomposing gate.
@return Returns with a python dictionary containing the metadata of the idx-th gate
*/
static PyObject *
get_gate( N_Qubit_Decomposition* decomp, int &idx ) {


    // create dictionary conatining the gate data
    PyObject* py_gate = PyDict_New();

    Gate* gate = decomp->get_gate( idx );

    if (gate->get_type() == CNOT_OPERATION) {

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "CNOT" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* control_qbit = Py_BuildValue("i",  gate->get_control_qbit() );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "control_qbit", control_qbit );            

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(control_qbit);

    }
    else if (gate->get_type() == CZ_OPERATION) {

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "CZ" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* control_qbit = Py_BuildValue("i",  gate->get_control_qbit() );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "control_qbit", control_qbit );            

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(control_qbit);

    }
    else if (gate->get_type() == CH_OPERATION) {

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "CH" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* control_qbit = Py_BuildValue("i",  gate->get_control_qbit() );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "control_qbit", control_qbit );            

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(control_qbit);

    }
    else if (gate->get_type() == SYC_OPERATION) {

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "SYC" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* control_qbit = Py_BuildValue("i",  gate->get_control_qbit() );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "control_qbit", control_qbit );            

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(control_qbit);

    }
    else if (gate->get_type() == U3_OPERATION) {

        // get U3 parameters
        U3* u3_gate = static_cast<U3*>(gate);
        Matrix_real&& parameters = u3_gate->get_optimized_parameters();
 

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "U3" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* Theta = Py_BuildValue("f",  parameters[0] );
        PyObject* Phi = Py_BuildValue("f",  parameters[1] );
        PyObject* Lambda = Py_BuildValue("f",  parameters[2] );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "Theta", Theta );
        PyDict_SetItemString(py_gate, "Phi", Phi );
        PyDict_SetItemString(py_gate, "Lambda", Lambda );

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(Theta);
        Py_XDECREF(Phi);
        Py_XDECREF(Lambda);


    }
    else if (gate->get_type() == RX_OPERATION) {

        // get U3 parameters
        RX* rx_gate = static_cast<RX*>(gate);
        Matrix_real&& parameters = rx_gate->get_optimized_parameters();
 

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "RX" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* Theta = Py_BuildValue("f",  parameters[0] );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "Theta", Theta );

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(Theta);


    }
    else if (gate->get_type() == RY_OPERATION) {

        // get U3 parameters
        RY* ry_gate = static_cast<RY*>(gate);
        Matrix_real&& parameters = ry_gate->get_optimized_parameters();
 

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "RY" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* Theta = Py_BuildValue("f",  parameters[0] );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "Theta", Theta );

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(Theta);


    }
    else if (gate->get_type() == RZ_OPERATION) {

        // get U3 parameters
        RZ* rz_gate = static_cast<RZ*>(gate);
        Matrix_real&& parameters = rz_gate->get_optimized_parameters();
 

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "RZ" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );
        PyObject* Phi = Py_BuildValue("f",  parameters[0] );


        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );
        PyDict_SetItemString(py_gate, "Phi", Phi );

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);
        Py_XDECREF(Phi);

    }
    else if (gate->get_type() == X_OPERATION) {

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "X" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );

        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);

    }
    else if (gate->get_type() == SX_OPERATION) {

        // create gate parameters
        PyObject* type = Py_BuildValue("s",  "SX" );
        PyObject* target_qbit = Py_BuildValue("i",  gate->get_target_qbit() );

        PyDict_SetItemString(py_gate, "type", type );
        PyDict_SetItemString(py_gate, "target_qbit", target_qbit );

        Py_XDECREF(type);
        Py_XDECREF(target_qbit);

    }
    else {
  
    }

    return py_gate;

}



/**
@brief Wrapper function to set the number of identical successive blocks during the subdecomposition of the qbit-th qubit.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: idx (int)
idx: labels the idx-th gate.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_get_gate( qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    int  idx; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|i", &idx )) return Py_BuildValue("i", -1);


    return get_gate( self->decomp, idx );


}







/**
@brief Wrapper function to set the number of identical successive blocks during the subdecomposition of the qbit-th qubit.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: qbit (bool), identical_blocks (bool)
qbit: The number of qubits for which the subdecomposition should contain identical_blocks successive identical blocks.
identical_blocks: Number of successive identical blocks in the decomposition.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_get_gates( qgd_N_Qubit_Decomposition_Wrapper *self ) {


    // get the number of gates
    int op_num = self->decomp->get_gate_num();

    // preallocate Python tuple for the output
    PyObject* ret = PyTuple_New( (Py_ssize_t) op_num );



    // iterate over the gates to get the gate list
    for (int idx = 0; idx < op_num; idx++ ) {

        // get metadata about the idx-th gate
        PyObject* gate = get_gate( self->decomp, idx );

        // adding gate information to the tuple
        PyTuple_SetItem( ret, (Py_ssize_t) idx, gate );

    }


    return ret;

}


/**
@brief Lists the gates decomposing the initial unitary. (These gates are the inverse gates of the gates bringing the intial matrix into unity.)
@param start_index The index of the first inverse gate
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_List_Gates( qgd_N_Qubit_Decomposition_Wrapper *self ) {

    self->decomp->list_gates( 0 );

    return Py_None;
}




/**
@brief Extract the optimized parameters
@param start_index The index of the first inverse gate
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_get_Optimized_Parameters( qgd_N_Qubit_Decomposition_Wrapper *self ) {

    int parameter_num = self->decomp->get_parameter_num();

    Matrix_real parameters_mtx(1, parameter_num);
    double* parameters = parameters_mtx.get_data();
    self->decomp->get_optimized_parameters(parameters);

    // reversing the order
    Matrix_real parameters_mtx_reversed(1, parameter_num);
    double* parameters_reversed = parameters_mtx_reversed.get_data();
    for (int idx=0; idx<parameter_num; idx++ ) {
        parameters_reversed[idx] = parameters[parameter_num-1-idx];
    }

    // convert to numpy array
    parameters_mtx_reversed.set_owner(false);
    PyObject * parameter_arr = matrix_real_to_numpy( parameters_mtx_reversed );

    return parameter_arr;
}






/**
@brief Set the maximal number of layers used in the subdecomposition of the qbit-th qubit.
@param max_layer_num A dictionary {'n': max_layer_num} labeling the maximal number of the gate layers used in the subdecomposition.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Max_Layer_Num(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    PyObject* max_layer_num; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &max_layer_num )) return Py_BuildValue("i", -1);

    // Check whether input is dictionary
    if (!PyDict_Check(max_layer_num)) {
        printf("Input must be dictionary!\n");
        return Py_BuildValue("i", -1);
    }


    PyObject* key = NULL;
    PyObject* value = NULL;
    Py_ssize_t pos = 0;


    while (PyDict_Next(max_layer_num, &pos, &key, &value)) {

        // convert value fron PyObject to int
        assert(PyLong_Check(value) == 1);
        int value_int = (int) PyLong_AsLong(value);

        // convert keylue fron PyObject to int
        assert(PyLong_Check(key) == 1);
        int key_int = (int) PyLong_AsLong(key);

        // set maximal layer nums on the C++ side
        self->decomp->set_max_layer_num( key_int, value_int );
    }

    return Py_BuildValue("i", 0);
}




/**
@brief Set the number of identical successive blocks during the subdecomposition of the qbit-th qubit.
@param identical_blocks A dictionary {'n': identical_blocks} labeling the number of successive identical layers used in the subdecomposition at the disentangling of the n-th qubit.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Identical_Blocks(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    PyObject* identical_blocks; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &identical_blocks )) return Py_BuildValue("i", -1);

    // Check whether input is dictionary
    if (!PyDict_Check(identical_blocks)) {
        printf("Input must be dictionary!\n");
        return Py_BuildValue("i", -1);
    }


    PyObject* key = NULL;
    PyObject* value = NULL;
    Py_ssize_t pos = 0;


    while (PyDict_Next(identical_blocks, &pos, &key, &value)) {

        // convert value fron PyObject to int
        assert(PyLong_Check(value) == 1);
        int value_int = (int) PyLong_AsLong(value);

        // convert keylue fron PyObject to int
        assert(PyLong_Check(key) == 1);
        int key_int = (int) PyLong_AsLong(key);

        // set maximal layer nums on the C++ side
        self->decomp->set_identical_blocks( key_int, value_int );
    }

    return Py_BuildValue("i", 0);
}





/**
@brief Set the number of iteration loops during the subdecomposition of the qbit-th qubit.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: identical_blocks (PyDict)
identical_blocks: A dictionary {'n': iteration_loops} labeling the number of successive identical layers used in the subdecomposition at the disentangling of the n-th qubit.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Iteration_Loops(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    PyObject* iteration_loops; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &iteration_loops )) return Py_BuildValue("i", -1);

    // Check whether input is dictionary
    if (!PyDict_Check(iteration_loops)) {
        printf("Input must be dictionary!\n");
        return Py_BuildValue("i", -1);
    }


    PyObject* key = NULL;
    PyObject* value = NULL;
    Py_ssize_t pos = 0;


    while (PyDict_Next(iteration_loops, &pos, &key, &value)) {

        // convert value fron PyObject to int
        assert(PyLong_Check(value) == 1);
        int value_int = (int) PyLong_AsLong(value);

        // convert keylue fron PyObject to int
        assert(PyLong_Check(key) == 1);
        int key_int = (int) PyLong_AsLong(key);

        // set maximal layer nums on the C++ side
        self->decomp->set_iteration_loops( key_int, value_int );
    }

    return Py_BuildValue("i", 0);
}



/**
@brief Set the verbosity of the N_Qubit_Decomposition class
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: verbose (int)
verbose: Set False to suppress the output messages of the decompostion, or True (deafult) otherwise.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Verbose(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    int verbose; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|i", &verbose )) return Py_BuildValue("i", -1);


    // set maximal layer nums on the C++ side
    self->decomp->set_verbose( verbose );

    return Py_BuildValue("i", 0);
}

/**
@brief Set the debugfile name of the N_Qubit_Decomposition class
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_adaptive_Wrapper.
@param args A tuple of the input arguments: debugfile_name (string)
debug: Set True to suppress the output messages of the decompostion into a file named debugfile_name, or False (deafult) otherwise.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Debugfile(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args  ) {


    PyObject *debugfile = NULL;
 
    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &debugfile )) return Py_BuildValue("s", -1);

    // determine the debugfile name type
    PyObject* debugfile_string = PyObject_Str(debugfile);
    PyObject* debugfile_string_unicode = PyUnicode_AsEncodedString(debugfile_string, "utf-8", "~E~");
    const char* debugfile_C = PyBytes_AS_STRING(debugfile_string_unicode);

    
    Py_XDECREF(debugfile_string);
    Py_XDECREF(debugfile_string_unicode);

    // determine the length of the filename and initialize C++ variant of the string
    Py_ssize_t string_length = PyBytes_Size(debugfile_string_unicode);
    std::string debugfile_Cpp(debugfile_C, string_length);

    // set the name of the debugfile on the C++ side
    self->decomp->set_debugfile( debugfile_Cpp );


    return Py_BuildValue("s", NULL);
}

/**
@brief Wrapper method to set the optimization tolerance of the optimization process during the decomposition.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: tolerance (double)
tolerance: The maximal allowed error of the optimization problem
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Optimization_Tolerance(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    double tolerance; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|d", &tolerance )) return Py_BuildValue("i", -1);


    // set maximal layer nums on the C++ side
    self->decomp->set_optimization_tolerance( tolerance );

    return Py_BuildValue("i", 0);
}


/**
@brief Wrapper method to set the threshold of convergence in the optimization processes.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: tolerance (double)
tolerance: The maximal allowed error of the optimization problem
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Convergence_Threshold(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    double threshold; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|d", &threshold )) return Py_BuildValue("i", -1);


    // set maximal layer nums on the C++ side
    self->decomp->set_convergence_threshold( threshold );

    return Py_BuildValue("i", 0);
}



/**
@brief Wrapper method to to set the number of gate blocks to be optimized in one shot
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: tolerance (double)
optimization_block: number of operators in one sub-layer of the optimization process
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Optimization_Blocks(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    double optimization_block; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|d", &optimization_block )) return Py_BuildValue("i", -1);


    // set maximal layer nums on the C++ side
    self->decomp->set_optimization_blocks( optimization_block );

    return Py_BuildValue("i", 0);
}



/**
@brief Wrapper function to set custom gate structure for the decomposition.
@param self A pointer pointing to an instance of the class qgd_N_Qubit_Decomposition_Wrapper.
@param args A tuple of the input arguments: gate_structure_dict (PyDict)
gate_structure_dict: ?????????????????????????????
@return Returns with zero on success.
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_set_Gate_Structure( qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {


    // initiate variables for input arguments
    PyObject* gate_structure_dict; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &gate_structure_dict )) return Py_BuildValue("i", -1);

    // Check whether input is dictionary
    if (!PyDict_Check(gate_structure_dict)) {
        printf("Input must be dictionary!\n");
        return Py_BuildValue("i", -1);
    }


    PyObject* key = NULL;
    PyObject* value = NULL;
    Py_ssize_t pos = 0;

    std::map< int, Gates_block* > gate_structure;


    while (PyDict_Next(gate_structure_dict, &pos, &key, &value)) {

        // convert keylue from PyObject to int
        assert(PyLong_Check(key) == 1);
        int key_int = (int) PyLong_AsLong(key);

        // convert keylue from PyObject to qgd_Gates_Block
        qgd_Gates_Block* qgd_op_block = (qgd_Gates_Block*) value;

        gate_structure.insert( std::pair<int, Gates_block*>( key_int, qgd_op_block->gate ));

    }

    self->decomp->set_custom_gate_structure( gate_structure );

    return Py_BuildValue("i", 0);


}

/**
@brief Wrapper method to reorder the qubits in the decomposition class.
@param 
*/
static PyObject *
qgd_N_Qubit_Decomposition_Wrapper_Reorder_Qubits(qgd_N_Qubit_Decomposition_Wrapper *self, PyObject *args ) {

    // initiate variables for input arguments
    PyObject* qbit_list; 

    // parsing input arguments
    if (!PyArg_ParseTuple(args, "|O", &qbit_list )) return Py_BuildValue("i", -1);

    bool is_tuple = PyTuple_Check(qbit_list);
    bool is_list = PyList_Check(qbit_list);

    // Check whether input is dictionary
    if (!is_list && !is_tuple) {
        printf("Input must be tuple or list!\n");
        return Py_BuildValue("i", -1);
    }

    // get the number of qbubits
    Py_ssize_t element_num;

    if (is_tuple) {
        element_num = PyTuple_GET_SIZE(qbit_list);
    }
    else {
        element_num = PyList_GET_SIZE(qbit_list);
    }


    // create C++ variant of the tuple/list
    std::vector<int> qbit_list_C( (int) element_num);
    for ( Py_ssize_t idx=0; idx<element_num; idx++ ) {
        if (is_tuple) {        
            qbit_list_C[(int) idx] = (int) PyLong_AsLong( PyTuple_GetItem(qbit_list, idx ) );
        }
        else {
            qbit_list_C[(int) idx] = (int) PyLong_AsLong( PyList_GetItem(qbit_list, idx ) );
        }

    }


    // reorder the qubits in the decomposition class
    self->decomp->reorder_qubits( qbit_list_C );

    

    return Py_BuildValue("i", 0);
}



/**
@brief Structure containing metadata about the members of class qgd_N_Qubit_Decomposition_Wrapper.
*/
static PyMemberDef qgd_N_Qubit_Decomposition_Wrapper_members[] = {
    {NULL}  /* Sentinel */
};

/**
@brief Structure containing metadata about the methods of class qgd_N_Qubit_Decomposition_Wrapper.
*/
static PyMethodDef qgd_N_Qubit_Decomposition_Wrapper_methods[] = {
    {"Start_Decomposition", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_Start_Decomposition, METH_VARARGS | METH_KEYWORDS,
     "Method to start the decomposition."
    },
    {"get_Gate_Num", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_get_gate_num, METH_NOARGS,
     "Method to get the number of decomposing gates."
    },
    {"get_Optimized_Parameters", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_get_Optimized_Parameters, METH_NOARGS,
     "Method to get the array of optimized parameters."
    },
    {"get_Gate", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_get_gate, METH_VARARGS,
     "Method to get the i-th decomposing gates."
    },
    {"get_Gates", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_get_gates, METH_NOARGS,
     "Method to get the tuple of decomposing gates."
    },
    {"List_Gates", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_List_Gates, METH_NOARGS,
     "Call to print the decomposing nitaries on standard output"
    },
    {"set_Max_Layer_Num", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Max_Layer_Num, METH_VARARGS,
     "Call to set the maximal number of layers used in the subdecomposition of the qbit-th qubit."
    },
    {"set_Identical_Blocks", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Identical_Blocks, METH_VARARGS,
     "Call to set the number of identical successive blocks during the subdecomposition of the qbit-th qubit."
    },
    {"set_Iteration_Loops", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Iteration_Loops, METH_VARARGS,
     "Call to set the number of iteration loops during the subdecomposition of the qbit-th qubit."
    },
    {"set_Verbose", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Verbose, METH_VARARGS,
     "Call to set the verbosity of the qgd_N_Qubit_Decomposition class."
    },
    {"set_Debugfile", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Debugfile, METH_VARARGS,
     "Set the debugfile name of the N_Qubit_Decomposition class."
    },
    {"set_Gate_Structure", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Gate_Structure, METH_VARARGS,
     "Call to set custom gate structure in the decomposition."
    },
    {"Reorder_Qubits", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_Reorder_Qubits, METH_VARARGS,
     "Wrapper method to reorder the qubits in the decomposition class."
    },
    {"set_Optimization_Tolerance", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Optimization_Tolerance, METH_VARARGS,
     "Wrapper method to set the optimization tolerance of the optimization process during the decomposition."
    },
    {"set_Convergence_Threshold", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Convergence_Threshold, METH_VARARGS,
     "Wrapper method to set the threshold of convergence in the optimization processes."
    },
    {"set_Optimization_Blocks", (PyCFunction) qgd_N_Qubit_Decomposition_Wrapper_set_Optimization_Blocks, METH_VARARGS,
     "Wrapper method to to set the number of gate blocks to be optimized in one shot."
    },
    {NULL}  /* Sentinel */
};

/**
@brief A structure describing the type of the class qgd_N_Qubit_Decomposition_Wrapper.
*/
static PyTypeObject qgd_N_Qubit_Decomposition_Wrapper_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "qgd_N_Qubit_Decomposition_Wrapper.qgd_N_Qubit_Decomposition_Wrapper", /*tp_name*/
  sizeof(qgd_N_Qubit_Decomposition_Wrapper), /*tp_basicsize*/
  0, /*tp_itemsize*/
  (destructor) qgd_N_Qubit_Decomposition_Wrapper_dealloc, /*tp_dealloc*/
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
  "Object to represent a Gates_block class of the QGD package.", /*tp_doc*/
  0, /*tp_traverse*/
  0, /*tp_clear*/
  0, /*tp_richcompare*/
  0, /*tp_weaklistoffset*/
  0, /*tp_iter*/
  0, /*tp_iternext*/
  qgd_N_Qubit_Decomposition_Wrapper_methods, /*tp_methods*/
  qgd_N_Qubit_Decomposition_Wrapper_members, /*tp_members*/
  0, /*tp_getset*/
  0, /*tp_base*/
  0, /*tp_dict*/
  0, /*tp_descr_get*/
  0, /*tp_descr_set*/
  0, /*tp_dictoffset*/
  (initproc) qgd_N_Qubit_Decomposition_Wrapper_init, /*tp_init*/
  0, /*tp_alloc*/
  qgd_N_Qubit_Decomposition_Wrapper_new, /*tp_new*/
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
static PyModuleDef qgd_N_Qubit_Decomposition_Wrapper_Module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "qgd_N_Qubit_Decomposition_Wrapper",
    .m_doc = "Python binding for QGD N_Qubit_Decomposition class",
    .m_size = -1,
};


/**
@brief Method called when the Python module is initialized
*/
PyMODINIT_FUNC
PyInit_qgd_N_Qubit_Decomposition_Wrapper(void)
{
    // initialize Numpy API
    import_array();

    PyObject *m;
    if (PyType_Ready(&qgd_N_Qubit_Decomposition_Wrapper_Type) < 0)
        return NULL;

    m = PyModule_Create(&qgd_N_Qubit_Decomposition_Wrapper_Module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&qgd_N_Qubit_Decomposition_Wrapper_Type);
    if (PyModule_AddObject(m, "qgd_N_Qubit_Decomposition_Wrapper", (PyObject *) &qgd_N_Qubit_Decomposition_Wrapper_Type) < 0) {
        Py_DECREF(&qgd_N_Qubit_Decomposition_Wrapper_Type);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}


} //extern C


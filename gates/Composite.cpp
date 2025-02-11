/*
Created on Fri Jun 26 14:13:26 2020
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
/*! \file Composite.cpp
    \brief Class for the representation of general unitary operation on the first qbit_num-1 qubits.
*/


#include "Composite.h"
#include "common.h"
#include "dot.h"
#include "Random_Unitary.h"



/**
@brief Deafult constructor of the class.
@return An instance of the class
*/
Composite::Composite() {

    // number of qubits spanning the matrix of the operation
    qbit_num = -1;
    // The size N of the NxN matrix associated with the operations.
    matrix_size = -1;
    // The type of the operation (see enumeration gate_type)
    type = COMPOSITE_OPERATION;
    // The index of the qubit on which the operation acts (target_qbit >= 0)
    target_qbit = -1;
    // The index of the qubit which acts as a control qubit (control_qbit >= 0) in controlled operations
    control_qbit = -1;
    // the number of free parameters of the operation
    parameter_num = 0;
}



/**
@brief Constructor of the class.
@param qbit_num_in The number of qubits spanning the unitaries
@return An instance of the class
*/
Composite::Composite(int qbit_num_in) {

    // number of qubits spanning the matrix of the operation
    qbit_num = qbit_num_in;
    // the size of the matrix
    matrix_size = Power_of_2(qbit_num);
    // A string describing the type of the operation
    type = COMPOSITE_OPERATION;
    // The index of the qubit on which the operation acts (target_qbit >= 0)
    target_qbit = -1;
    // The index of the qubit which acts as a control qubit (control_qbit >= 0) in controlled operations
    control_qbit = -1;
    // The number of parameters
    parameter_num = (matrix_size)*(matrix_size-1) + (matrix_size-1);
}


/**
@brief Destructor of the class
*/
Composite::~Composite() {
}

/**
@brief Set the number of qubits spanning the matrix of the operation
@param qbit_num_in The number of qubits spanning the matrix
*/
void Composite::set_qbit_num( int qbit_num_in ) {
    // setting the number of qubits
    qbit_num = qbit_num_in;

    // update the size of the matrix
    matrix_size = Power_of_2(qbit_num);

    // Update the number of the parameters
    parameter_num = (matrix_size)*(matrix_size-1) + (matrix_size-1);


}


/**
@brief Call to retrieve the operation matrix
@return Returns with a matrix of the operation
*/
Matrix
Composite::get_matrix( Matrix_real& parameters ) {


    // create array of random parameters to construct random unitary
    double* vartheta = parameters.get_data();//(double*) qgd_calloc( int(dim*(dim-1)/2),sizeof(double), 64);
    double* varphi = parameters.get_data()+int((matrix_size*(matrix_size-1))/2);//(double*) qgd_calloc( int(dim*(dim-1)/2),sizeof(double), 64);
    double* varkappa = parameters.get_data()+matrix_size*(matrix_size-1);//(double*) qgd_calloc( (dim-1),sizeof(double), 64);

    Random_Unitary ru(matrix_size);
    Matrix com_matrix  = ru.Construct_Unitary_Matrix( vartheta, varphi, varkappa );
//com_matrix.print_matrix();
#ifdef DEBUG
        if (com_matrix.isnan()) {
	    std::stringstream sstream;
	    sstream << "Composite::get_matrix: UN_matrix contains NaN." << std::endl;
            print(sstream, 1);	           
        }
#endif

        return com_matrix;
}


/**
@brief Call to apply the gate on the input array/matrix
@param input The input array on which the gate is applied
*/
void 
Composite::apply_to( Matrix_real& parameters, Matrix& input ) {


    if (input.rows != matrix_size ) {
	std::stringstream sstream;
        sstream << "Wrong matrix size in Composite gate apply" << std::endl;
        print(sstream, 0);	
        exit(-1);
    }

    if (parameters.size() < parameter_num) {
	std::stringstream sstream;
	sstream << "Not enough parameters given for the Composite gate" << std::endl;
        print(sstream, 0);	
        exit(-1);
    }


    Matrix com_matrix = get_matrix( parameters );
    Matrix transformed = dot( com_matrix, input );
    memcpy( input.get_data(), transformed.get_data(), input.size()*sizeof(QGD_Complex16) );

//std::cout << "Composite::apply_to" << std::endl;
//exit(-1);
}


/**
@brief Call to apply the gate on the input array/matrix by input*Gate
@param input The input array on which the gate is applied
*/
void 
Composite::apply_from_right( Matrix_real& parameters, Matrix& input ) {


    if (input.rows != matrix_size ) {
	std::stringstream sstream;
	sstream << "Wrong matrix size in Composite gate apply" << std::endl;
        print(sstream, 0);	        
        exit(-1);
    }

    if (parameters.size() < parameter_num) {
	std::stringstream sstream;
        sstream << "Not enough parameters given for the Composite gate" << std::endl;
        print(sstream, 0);	 
        exit(-1);
    }

    Matrix com_matrix = get_matrix( parameters );
    Matrix transformed = dot( input, com_matrix );
    memcpy( input.get_data(), transformed.get_data(), input.size()*sizeof(QGD_Complex16) );

//std::cout << "Composite::apply_to" << std::endl;
//exit(-1);

}




/**
@brief Call to reorder the qubits in the matrix of the operation
@param qbit_list The reordered list of qubits spanning the matrix
*/
void Composite::reorder_qubits( std::vector<int> qbit_list ) {

    // check the number of qubits
    if ((int)qbit_list.size() != qbit_num ) {
	std::stringstream sstream;
	sstream << "Wrong number of qubits" << std::endl;
	print(sstream, 0);	    	
        exit(-1);
    }


    int control_qbit_new = control_qbit;
    int target_qbit_new = target_qbit;

    // setting the new value for the target qubit
    for (int idx=0; idx<qbit_num; idx++) {
        if (target_qbit == qbit_list[idx]) {
            target_qbit_new = qbit_num-1-idx;
        }
        if (control_qbit == qbit_list[idx]) {
            control_qbit_new = qbit_num-1-idx;
        }
    }

    control_qbit = control_qbit_new;
    target_qbit = target_qbit_new;
}

/**
@brief Call to set the final optimized parameters of the gate.
@param parameters_ Real array of the optimized parameters
*/
void 
Composite::set_optimized_parameters( Matrix_real parameters_ ) {

    parameters = parameters_.copy();

}


/**
@brief Call to get the final optimized parameters of the gate.
*/
Matrix_real 
Composite::get_optimized_parameters() {

    return parameters.copy();

}

/**
@brief Call to get the number of free parameters
@return Return with the number of the free parameters
*/
int 
Composite::get_parameter_num() {
    return parameter_num;
}


/**
@brief Call to get the type of the operation
@return Return with the type of the operation (see gate_type for details)
*/
gate_type 
Composite::get_type() {
    return type;
}


/**
@brief Call to get the number of qubits composing the unitary
@return Return with the number of qubits composing the unitary
*/
int 
Composite::get_qbit_num() {
    return qbit_num;
}


/**
@brief Call to create a clone of the present class
@return Return with a pointer pointing to the cloned object
*/
Composite* Composite::clone() {

    Composite* ret = new Composite( qbit_num );

    if ( parameters.size() > 0 ) {
        ret->set_optimized_parameters( parameters );
    }

    return ret;

}




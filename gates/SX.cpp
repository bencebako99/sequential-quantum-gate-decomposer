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
/*! \file SX.cpp
    \brief Class representing a SX gate.
*/

#include "SX.h"




//static tbb::spin_mutex my_mutex;
/**
@brief Nullary constructor of the class.
*/
SX::SX() {

        // number of qubits spanning the matrix of the gate
        qbit_num = -1;
        // the size of the matrix
        matrix_size = -1;
        // A string describing the type of the gate
        type = SX_OPERATION;

        // The index of the qubit on which the gate acts (target_qbit >= 0)
        target_qbit = -1;
        // The index of the qubit which acts as a control qubit (control_qbit >= 0) in controlled gates
        control_qbit = -1;

        parameter_num = 0;



}



/**
@brief Constructor of the class.
@param qbit_num_in The number of qubits spanning the gate.
@param target_qbit_in The 0<=ID<qbit_num of the target qubit.
@param theta_in logical value indicating whether the matrix creation takes an argument theta.
@param phi_in logical value indicating whether the matrix creation takes an argument phi
@param lambda_in logical value indicating whether the matrix creation takes an argument lambda
*/
SX::SX(int qbit_num_in, int target_qbit_in) {

        // number of qubits spanning the matrix of the gate
        qbit_num = qbit_num_in;
        // the size of the matrix
        matrix_size = Power_of_2(qbit_num);
        // A string describing the type of the gate
        type = SX_OPERATION;


        if (target_qbit_in >= qbit_num) {
            std::stringstream sstream;
	    sstream << "The index of the target qubit is larger than the number of qubits" << std::endl;
	    print(sstream, 0);
            throw "The index of the target qubit is larger than the number of qubits";
        }
	
        // The index of the qubit on which the gate acts (target_qbit >= 0)
        target_qbit = target_qbit_in;
        // The index of the qubit which acts as a control qubit (control_qbit >= 0) in controlled gates
        control_qbit = -1;

        parameter_num = 0;


}


/**
@brief Destructor of the class
*/
SX::~SX() {

}



/**
@brief Call to retrieve the gate matrix
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@return Returns with a matrix of the gate
*/
Matrix
SX::get_matrix( ) {

        Matrix SX_matrix = create_identity(matrix_size);
        apply_to(SX_matrix);

#ifdef DEBUG
        if (SX_matrix.isnan()) {
            std::stringstream sstream;
	    sstream << "SX::get_matrix: SX_matrix contains NaN." << std::endl;
            print(sstream, 1);	  
        }
#endif

        return SX_matrix;

}



/**
@brief Call to apply the gate on the input array/matrix by U3*input
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
void 
SX::apply_to( Matrix& input ) {

    if (input.rows != matrix_size ) {
        std::stringstream sstream;
	sstream << "Wrong matrix size in X gate apply" << std::endl;
        print(sstream, 0);	 
        exit(-1);
    }


    // the SX gate of one qubit
    Matrix sx_1qbit(2,2);
    sx_1qbit[0].real = 0.5; sx_1qbit[0].imag = 0.5;
    sx_1qbit[1].real = 0.5; sx_1qbit[1].imag = -0.5;
    sx_1qbit[2].real = 0.5; sx_1qbit[2].imag = -0.5;
    sx_1qbit[3].real = 0.5; sx_1qbit[3].imag = 0.5;

   
    //apply_kernel_to function to SX gate 
    apply_kernel_to( sx_1qbit, input );
   
  /*  int index_step = Power_of_2(target_qbit);
    int current_idx = 0;
    int current_idx_pair = current_idx+index_step;

//std::cout << "target qbit: " << target_qbit << std::endl;

    while ( current_idx_pair < matrix_size ) {


        tbb::parallel_for(0, index_step, 1, [&](int idx) {  

            int current_idx_loc = current_idx + idx;
            int current_idx_pair_loc = current_idx_pair + idx;

            int row_offset = current_idx_loc*input.stride;
            int row_offset_pair = current_idx_pair_loc*input.stride;

            for ( int col_idx=0; col_idx<matrix_size; col_idx++) {
                int index      = row_offset+col_idx;
                int index_pair = row_offset_pair+col_idx;

                QGD_Complex16 element      = input[index];
                QGD_Complex16 element_pair = input[index_pair];

                QGD_Complex16 tmp1 = mult(sx_1qbit[0], element);
                QGD_Complex16 tmp2 = mult(sx_1qbit[1], element_pair);
                input[index].real = tmp1.real + tmp2.real;
                input[index].imag = tmp1.imag + tmp2.imag;

                tmp1 = mult(sx_1qbit[2], element);
                tmp2 = mult(sx_1qbit[3], element_pair);
                input[index_pair].real = tmp1.real + tmp2.real;
                input[index_pair].imag = tmp1.imag + tmp2.imag;

            };         

//std::cout << current_idx << " " << current_idx_pair << std::endl;

        });


        current_idx = current_idx + 2*index_step;
        current_idx_pair = current_idx_pair + 2*index_step;


    }
*/

}




/**
@brief Call to apply the gate on the input array/matrix by input*U3
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
void 
SX::apply_from_right( Matrix& input ) {


    if (input.cols != matrix_size ) {
        std::stringstream sstream;
	sstream << "Wrong matrix size in U3 apply_from_right" << std::endl;
        print(sstream, 0);	
        exit(-1);
    }

    // the SX gate of one qubit
    Matrix sx_1qbit(2,2);
    sx_1qbit[0].real = 0.5; sx_1qbit[0].imag = 0.5;
    sx_1qbit[1].real = 0.5; sx_1qbit[1].imag = -0.5;
    sx_1qbit[2].real = 0.5; sx_1qbit[2].imag = -0.5;
    sx_1qbit[3].real = 0.5; sx_1qbit[3].imag = 0.5;

    
    //apply_kernel_from_right function to SX gate 
    apply_kernel_from_right(sx_1qbit, input);

  /*  int index_step = Power_of_2(target_qbit);
    int current_idx = 0;
    int current_idx_pair = current_idx+index_step;

//std::cout << "target qbit: " << target_qbit << std::endl;

    while ( current_idx_pair < matrix_size ) {


        tbb::parallel_for(0, index_step, 1, [&](int idx) {  

            int current_idx_loc = current_idx + idx;
            int current_idx_pair_loc = current_idx_pair + idx;


            for ( int row_idx=0; row_idx<matrix_size; row_idx++) {

                int row_offset = row_idx*input.stride;


                int index      = row_offset+current_idx_loc;
                int index_pair = row_offset+current_idx_pair_loc;

                QGD_Complex16 element      = input[index];
                QGD_Complex16 element_pair = input[index_pair];

                QGD_Complex16 tmp1 = mult(sx_1qbit[0], element);
                QGD_Complex16 tmp2 = mult(sx_1qbit[2], element_pair);
                input[index].real = tmp1.real + tmp2.real;
                input[index].imag = tmp1.imag + tmp2.imag;

                tmp1 = mult(sx_1qbit[1], element);
                tmp2 = mult(sx_1qbit[3], element_pair);
                input[index_pair].real = tmp1.real + tmp2.real;
                input[index_pair].imag = tmp1.imag + tmp2.imag;

            };         

//std::cout << current_idx << " " << current_idx_pair << std::endl;

        });


        current_idx = current_idx + 2*index_step;
        current_idx_pair = current_idx_pair + 2*index_step;


    }
*/

}



/**
@brief Call to create a clone of the present class
@return Return with a pointer pointing to the cloned object
*/
SX* SX::clone() {

    SX* ret = new SX(qbit_num, target_qbit);

    return ret;

}




/**
@brief Call to reorder the qubits in the matrix of the gate
@param qbit_list The reordered list of qubits spanning the matrix
*/
void SX::reorder_qubits( std::vector<int> qbit_list) {

    Gate::reorder_qubits(qbit_list);

}


/**
@brief Call to set the number of qubits spanning the matrix of the gate
@param qbit_num_in The number of qubits
*/
void SX::set_qbit_num(int qbit_num_in) {

        // setting the number of qubits
        Gate::set_qbit_num(qbit_num_in);
}

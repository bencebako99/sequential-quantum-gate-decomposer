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
/*! \file N_Qubit_Decomposition.h
    \brief Header file for a class to determine the decomposition of an N-qubit unitary into a sequence of CNOT and U3 gates.
*/

#ifndef N_Qubit_Decomposition_H
#define N_Qubit_Decomposition_H

#include "N_Qubit_Decomposition_Base.h"
#include "Sub_Matrix_Decomposition.h"

#ifdef __cplusplus
extern "C" 
{
#endif

/// Definition of the zggev function from Lapacke to calculate the eigenvalues of a complex matrix
int LAPACKE_zggev 	( 	int  	matrix_layout,
		char  	jobvl,
		char  	jobvr,
		int  	n,
		QGD_Complex16 *  	a,
		int  	lda,
		QGD_Complex16 *  	b,
		int  	ldb,
		QGD_Complex16 *  	alpha,
		QGD_Complex16 *  	beta,
		QGD_Complex16 *  	vl,
		int  	ldvl,
		QGD_Complex16 *  	vr,
		int  	ldvr 
	); 	

#ifdef __cplusplus
}
#endif


/**
@brief A base class to determine the decomposition of an N-qubit unitary into a sequence of CNOT and U3 gates.
This class contains the non-template implementation of the decomposition class.
*/
class N_Qubit_Decomposition : public N_Qubit_Decomposition_Base {


public:

protected:

/*
    /// logical value. Set true to optimize the minimum number of gate layers required in the decomposition, or false when the predefined maximal number of layer gates is used (ideal for general unitaries).
    bool optimize_layer_num;

    /// A map of <int n: int num> indicating that how many identical successive blocks should be used in the disentanglement of the nth qubit from the others
    std::map<int,int> identical_blocks;

    /// A map of <int n: Gates_block* block> describing custom gate structure to be used in the decomposition. Gate block corresponding to n is used in the subdecomposition of the n-th qubit. The Gate block is repeated periodically.
    Gates_block* gate_structure;


    std::vector<decomposition_tree_node*> root_nodes;
*/

    /// A map of <int n: Gates_block* block> describing custom gate structure to be used in the decomposition. Gate block corresponding to n is used in the subdecomposition of the n-th qubit. The Gate block is repeated periodically.
    std::map<int, Gates_block*> gate_structure;
public:

/**
@brief Nullary constructor of the class.
@return An instance of the class
*/
N_Qubit_Decomposition();



/**
@brief Constructor of the class.
@param Umtx_in The unitary matrix to be decomposed
@param qbit_num_in The number of qubits spanning the unitary Umtx
@param optimize_layer_num_in Optional logical value. If true, then the optimization tries to determine the lowest number of the layers needed for the decomposition. If False (default), the optimization is performed for the maximal number of layers.
@param initial_guess_in Enumeration element indicating the method to guess initial values for the optimization. Possible values: 'zeros=0' ,'random=1', 'close_to_zero=2'
@return An instance of the class
*/
N_Qubit_Decomposition( Matrix Umtx_in, int qbit_num_in, bool optimize_layer_num_in, guess_type initial_guess_in );



/**
@brief Destructor of the class
*/
virtual ~N_Qubit_Decomposition();


/**
@brief Start the disentanglig process of the unitary
@param finalize_decomp Optional logical parameter. If true (default), the decoupled qubits are rotated into state |0> when the disentangling of the qubits is done. Set to False to omit this procedure
@param prepare_export Logical parameter. Set true to prepare the list of gates to be exported, or false otherwise.
*/
virtual void start_decomposition(bool finalize_decomp=true, bool prepare_export=true);



/**
@brief Start the decompostion process to recursively decompose the submatrices.
*/
void  decompose_submatrix();


/**
@brief Call to extract and store the calculated parameters and gates of the sub-decomposition processes
@param cSub_decomposition An instance of class Sub_Matrix_Decomposition used to disentangle the n-th qubit from the others.
*/
void  extract_subdecomposition_results( Sub_Matrix_Decomposition* cSub_decomposition );

/**
@brief Call to simplify the gate structure in the layers if possible (i.e. tries to reduce the number of CNOT gates)
*/
void simplify_layers();

/**
@brief Call to simplify the gate structure in a block of gates (i.e. tries to reduce the number of CNOT gates)
@param layer An instance of class Gates_block containing the 2-qubit gate structure to be simplified
@param parameters An array of parameters to calculate the matrix representation of the gates in the block of gates.
@param parameter_num_block NUmber of parameters in the block of gates to be simplified.
@param max_layer_num_loc A map of <int n: int num> indicating the maximal number of CNOT gates allowed in the simplification.
@param simplified_layer An instance of Gates_block containing the simplified structure of gates.
@param simplified_parameters An array of parameters containing the parameters of the simplified block structure.
@param simplified_parameter_num The number of parameters in the simplified block structure.
@return Returns with 0 if the simplification wa ssuccessful.
*/
int simplify_layer( Gates_block* layer, double* parameters, int parameter_num_block, std::map<int,int> max_layer_num_loc, Gates_block* &simplified_layer, double* &simplified_parameters, int &simplified_parameter_num);


/**
@brief Call to set custom layers to the gate structure that are intended to be used in the subdecomposition.
@param gate_structure An <int, Gates_block*> map containing the gate structure used in the individual subdecomposition (default is used, if a gate structure for specific subdecomposition is missing).
*/
void set_custom_gate_structure( std::map<int, Gates_block*> gate_structure_in );

/**
@brief Set the number of identical successive blocks during the subdecomposition of the n-th qubit.
@param n The number of qubits for which the maximal number of layers should be used in the subdecomposition.
@param identical_blocks_in The number of successive identical layers used in the subdecomposition.
@return Returns with zero in case of success.
*/
int set_identical_blocks( int n, int identical_blocks_in );

/**
@brief Set the number of identical successive blocks during the subdecomposition of the n-th qubit.
@param identical_blocks_in An <int,int> map containing the number of successive identical layers used in the subdecompositions.
@return Returns with zero in case of success.
*/
int set_identical_blocks( std::map<int, int> identical_blocks_in );




};


#endif

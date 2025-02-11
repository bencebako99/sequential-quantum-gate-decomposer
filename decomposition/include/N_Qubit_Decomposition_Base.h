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

#ifndef N_Qubit_Decomposition_Base_H
#define N_Qubit_Decomposition_Base_H

#include "Decomposition_Base.h"

/// @brief Type definition of the fifferent types of the cost function
typedef enum cost_function_type {FROBENIUS_NORM, FROBENIUS_NORM_CORRECTION1, FROBENIUS_NORM_CORRECTION2, HILBERT_SCHMIDT_TEST, HILBERT_SCHMIDT_TEST_CORRECTION1, HILBERT_SCHMIDT_TEST_CORRECTION2} cost_function_type;



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


/// implemented optimization algorithms
enum optimization_aglorithms{ ADAM, BFGS, BFGS2, ADAM_BATCHED };


/**
@brief A base class to determine the decomposition of an N-qubit unitary into a sequence of CNOT and U3 gates.
This class contains the non-template implementation of the decomposition class.
*/
class N_Qubit_Decomposition_Base : public Decomposition_Base {


public:

    ///
    int iter_max;
    ///
    int gradient_threshold;
    /// 
    int random_shift_count_max;
    ///    
    double eta;
    /// unique id indentifying the instance of the class
    int id;
protected:


    /// logical value. Set true to optimize the minimum number of gate layers required in the decomposition, or false when the predefined maximal number of layer gates is used (ideal for general unitaries).
    bool optimize_layer_num;

    /// A map of <int n: int num> indicating that how many identical successive blocks should be used in the disentanglement of the nth qubit from the others
    std::map<int,int> identical_blocks;
    ///
    optimization_aglorithms alg;
    /// The chosen variant of the cost function
    cost_function_type cost_fnc;
    ///
    double prev_cost_fnv_val;
    ///
    double correction1_scale;
    ///
    double correction2_scale;    
    
    /// number of iterations
    int number_of_iters;

    /// logical variable indicating whether adaptive learning reate is used in the ADAM algorithm
    bool adaptive_eta;
    /// parameter to contron the radius of parameter randomization around the curren tminimum
    double radius;
    /// randomization rate
    double randomization_rate;
    /// threashold of count of iterations after what the parameters are randomized if the cost function does not deacrese fast enough
    unsigned long long iteration_threshold_of_randomization;
    /// number of utilized accelerators
    int accelerator_num;

    /// The offset in the first columns from which the "trace" is calculated. In this case Tr(A) = sum_(i-offset=j) A_{ij}
    int trace_offset;


    Matrix_real randomization_probs;
    matrix_base<int> randomized_probs;

    




public:

/**
@brief Nullary constructor of the class.
@return An instance of the class
*/
N_Qubit_Decomposition_Base();



/**
@brief Constructor of the class.
@param Umtx_in The unitary matrix to be decomposed
@param qbit_num_in The number of qubits spanning the unitary Umtx
@param optimize_layer_num_in Optional logical value. If true, then the optimization tries to determine the lowest number of the layers needed for the decomposition. If False (default), the optimization is performed for the maximal number of layers.
@param initial_guess_in Enumeration element indicating the method to guess initial values for the optimization. Possible values: 'zeros=0' ,'random=1', 'close_to_zero=2'
@return An instance of the class
*/
N_Qubit_Decomposition_Base( Matrix Umtx_in, int qbit_num_in, bool optimize_layer_num_in, guess_type initial_guess_in, int accelerator_num_in=0 );



/**
@brief Destructor of the class
*/
virtual ~N_Qubit_Decomposition_Base();

/**
@brief Calculate the error of the decomposition according to the spectral norm of \f$ U-U_{approx} \f$, where \f$ U_{approx} \f$ is the unitary produced by the decomposing quantum cirquit. The calculated error is stored in the attribute decomposition_error.
@param decomposed_matrix The decomposed matrix, i.e. the result of the decomposing gate structure applied on the initial unitary.
@return Returns with the calculated spectral norm.
*/
void calc_decomposition_error(Matrix& decomposed_matrix );


/**
@brief Call to add further layer to the gate structure used in the subdecomposition.
*/
virtual void add_finalyzing_layer();


/**
@brief final optimization procedure improving the accuracy of the decompositin when all the qubits were already disentangled.
*/
void final_optimization();

/**
@brief Call to solve layer by layer the optimization problem via calling one of the implemented algorithms. The optimalized parameters are stored in attribute optimized_parameters.
@param num_of_parameters Number of parameters to be optimized
@param solution_guess_gsl A GNU Scientific Library vector containing the solution guess.
*/
void solve_layer_optimization_problem( int num_of_parameters, gsl_vector *solution_guess_gsl);




/**
@brief Call to solve layer by layer the optimization problem via BBFG algorithm. (optimal for smaller problems) The optimalized parameters are stored in attribute optimized_parameters.
@param num_of_parameters Number of parameters to be optimized
@param solution_guess_gsl A GNU Scientific Library vector containing the solution guess.
*/
void solve_layer_optimization_problem_BFGS( int num_of_parameters, gsl_vector *solution_guess_gsl);



/**
@brief Call to solve layer by layer the optimization problem via BBFG algorithm. The optimalized parameters are stored in attribute optimized_parameters.
@param num_of_parameters Number of parameters to be optimized
@param solution_guess_gsl A GNU Scientific Library vector containing the solution guess.
*/
void solve_layer_optimization_problem_BFGS2( int num_of_parameters, gsl_vector *solution_guess_gsl);

/**
@brief Call to solve layer by layer the optimization problem via batched ADAM algorithm. (optimal for larger problems) The optimalized parameters are stored in attribute optimized_parameters.
@param num_of_parameters Number of parameters to be optimized
@param solution_guess_gsl A GNU Scientific Library vector containing the solution guess.
*/
void solve_layer_optimization_problem_ADAM_BATCHED( int num_of_parameters, gsl_vector *solution_guess_gsl);

/**
@brief Call to solve layer by layer the optimization problem via ADAM algorithm. (optimal for larger problems) The optimalized parameters are stored in attribute optimized_parameters.
@param num_of_parameters Number of parameters to be optimized
@param solution_guess_gsl A GNU Scientific Library vector containing the solution guess.
*/
void solve_layer_optimization_problem_ADAM( int num_of_parameters, gsl_vector *solution_guess_gsl);

/**
@brief ?????????????
*/
void randomize_parameters( Matrix_real& input, gsl_vector* output, const int randomization_succesful, const double& f0 );

/**
@brief The optimization problem of the final optimization
@param parameters An array of the free parameters to be optimized. (The number of teh free paramaters should be equal to the number of parameters in one sub-layer)
@return Returns with the cost function. (zero if the qubits are desintangled.)
*/
double optimization_problem( double* parameters);


/**
@brief The optimization problem of the final optimization
@param parameters An array of the free parameters to be optimized. (The number of teh free paramaters should be equal to the number of parameters in one sub-layer)
@return Returns with the cost function. (zero if the qubits are desintangled.)
*/
double optimization_problem( Matrix_real& parameters);


/**
@brief The optimization problem of the final optimization useful for gradient
@param parameters A GNU Scientific Library containing the parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@param ret_temp A matrix to store trace in for gradient
@return Returns with the cost function. (zero if the qubits are desintangled.)
*/
double optimization_problem( const gsl_vector* parameters, void* void_instance, Matrix ret_temp );


/**
@brief The optimization problem of the final optimization
@param parameters A GNU Scientific Library containing the parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@return Returns with the cost function. (zero if the qubits are desintangled.)
*/
static double optimization_problem( const gsl_vector* parameters, void* void_instance );


/**
@brief The optimization problem of the final optimization
@param parameters A GNU Scientific Library vector containing the free parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@param grad A GNU Scientific Library vector containing the calculated gradient components.
*/
static Matrix_real optimization_problem_batch( int batchsize, const gsl_vector* parameters, void* void_instance );


/**
@brief Calculate the approximate derivative (f-f0)/(x-x0) of the cost function with respect to the free parameters.
@param parameters A GNU Scientific Library vector containing the free parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@param grad A GNU Scientific Library vector containing the calculated gradient components.
*/
static void optimization_problem_grad( const gsl_vector* parameters, void* void_instance, gsl_vector* grad );



/**
@brief Call to calculate both the cost function and the its gradient components.
@param parameters A GNU Scientific Library vector containing the free parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@param f0 The value of the cost function at x0.
@param grad A GNU Scientific Library vector containing the calculated gradient components.
*/
static void optimization_problem_combined( const gsl_vector* parameters, void* void_instance, double* f0, gsl_vector* grad );


/**
@brief Call to calculate both the cost function and the its gradient components.
@param parameters The parameters for which the cost fuction shoule be calculated
@param f0 The value of the cost function at x0.
@param grad An array storing the calculated gradient components
@param onlyCPU Set true to use only CPU in the calculations (has effect if compiled to use accelerator devices)
*/
void optimization_problem_combined( const Matrix_real& parameters, double* f0, Matrix_real& grad );


/**
// @brief The optimization problem of the final optimization
@param parameters A GNU Scientific Library containing the parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@return Returns with the cost function. (zero if the qubits are desintangled.)
*/
double optimization_problem_panelty( double* parameters, Gates_block* gates_block );


/**
@brief Call to get the variant of the cost function used in the calculations
*/
cost_function_type get_cost_function_variant();


/**
@brief ???????????
*/
double get_previous_cost_function_value();


/**
@brief Call to set the variant of the cost function used in the calculations
@param variant The variant of the cost function from the enumaration cost_function_type
*/
void set_cost_function_variant( cost_function_type variant  );


/**
@brief ???????????
*/
double get_correction1_scale();


/**
@brief ??????????????
@param ?????????
*/
void get_correction1_scale( const double& scale  );



/**
@brief ???????????
*/
double get_correction2_scale();


/**
@brief ??????????????
@param ?????????
*/
void get_correction2_scale( const double& scale  );


/**
@brief ???????????
*/
long get_iteration_threshold_of_randomization();


/**
@brief ??????????????
@param ?????????
*/
void set_iteration_threshold_of_randomization( const unsigned long long& threshold  );



/**
@brief ?????????????
*/
void set_iter_max( int iter_max_in  );


/**
@brief ?????????????
*/
void set_random_shift_count_max( int random_shift_count_max_in  );


/**
@brief ?????????????
*/
void set_optimizer( optimization_aglorithms alg_in );


/**
@brief ?????????????
*/
void set_adaptive_eta( bool adaptive_eta_in  );


/**
@brief ?????????????
*/
void set_randomized_radius( double radius_in  );


/**
@brief Get the trace ffset used in the evaluation of the cost function
*/
int get_trace_offset();

/**
@brief Set the trace offset used in the evaluation of the cost function
*/
void set_trace_offset(int trace_offset_in);


/**
@brief Get the number of processed iterations during the optimization process
*/
int get_num_iters();

#ifdef __DFE__

/**
@brief ?????????????
*/
void upload_Umtx_to_DFE();


/**
@brief Get the number of accelerators to be reserved on DFEs on users demand.
*/
int get_accelerator_num();



#endif

};


#endif

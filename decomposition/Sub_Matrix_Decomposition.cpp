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
/*! \file Sub_Matrix_Decomposition.cpp
    \brief Class responsible for the disentanglement of one qubit from the others.
*/

#include "Sub_Matrix_Decomposition.h"
#include "Sub_Matrix_Decomposition_Cost_Function.h"


#ifdef __MPI__
#include <mpi.h>
#endif // MPI

//tbb::spin_mutex my_mutex;

/**
@brief Nullary constructor of the class.
@return An instance of the class
*/
Sub_Matrix_Decomposition::Sub_Matrix_Decomposition( ) {

    // logical value. Set true if finding the minimum number of gate layers is required (default), or false when the maximal number of CNOT gates is used (ideal for general unitaries).
    optimize_layer_num  = false;

    // A string describing the type of the class
    type = SUB_MATRIX_DECOMPOSITION_CLASS;

    // The global minimum of the optimization problem
    global_target_minimum = 0;

    // logical value indicating whether the quasi-unitarization of the submatrices was done or not
    subdisentaglement_done = false;

    // custom gate structure used in the decomposition
    unit_gate_structure = NULL;

}


/**
@brief Constructor of the class.
@param Umtx_in The unitary matrix to be decomposed
@param qbit_num_in The number of qubits spanning the unitary Umtx
@param optimize_layer_num_in Optional logical value. If true, then the optimization tries to determine the lowest number of the layers needed for the decomposition. If False (default), the optimization is performed for the maximal number of layers.
@param initial_guess_in Enumeration element indicating the method to guess initial values for the optimization. Possible values: 'zeros=0' ,'random=1', 'close_to_zero=2'
@return An instance of the class
*/
Sub_Matrix_Decomposition::Sub_Matrix_Decomposition( Matrix Umtx_in, int qbit_num_in, bool optimize_layer_num_in=false, guess_type initial_guess_in= CLOSE_TO_ZERO ) : Decomposition_Base(Umtx_in, qbit_num_in, initial_guess_in) {

    // logical value. Set true if finding the minimum number of gate layers is required (default), or false when the maximal number of CNOT gates is used (ideal for general unitaries).
    optimize_layer_num  = optimize_layer_num_in;

    // A string describing the type of the class
    type = SUB_MATRIX_DECOMPOSITION_CLASS;

    // The global minimum of the optimization problem
    global_target_minimum = 0;

    // number of iteratrion loops in the optimization
    iteration_loops[2] = 3;

    // logical value indicating whether the quasi-unitarization of the submatrices was done or not
    subdisentaglement_done = false;

    // filling in numbers that were not given in the input
    for ( std::map<int,int>::iterator it = max_layer_num_def.begin(); it!=max_layer_num_def.end(); it++) {
        if ( max_layer_num.count( it->first ) == 0 ) {
            max_layer_num.insert( std::pair<int, int>(it->first,  it->second) );
        }
    }

    // custom gate structure used in the decomposition
    unit_gate_structure = NULL;


}


/**
@brief Destructor of the class
*/
Sub_Matrix_Decomposition::~Sub_Matrix_Decomposition() {

    if ( unit_gate_structure != NULL ) {
        delete unit_gate_structure;
    }
    unit_gate_structure = NULL;

}






/**
@brief Start the optimization process to disentangle the most significant qubit from the others. The optimized parameters and gates are stored in the attributes optimized_parameters and gates.
*/
void  Sub_Matrix_Decomposition::disentangle_submatrices() {

    if (subdisentaglement_done) {        
	std::stringstream sstream;
	sstream << "Sub-disentaglement already done." << std::endl;
	print(sstream, 2);	    	
        return;
    }   

    std::stringstream sstream;
    sstream << std::endl << "Disentagling submatrices." << std::endl;
    print(sstream, 2);	    	
	 
    
    // setting the global target minimum
    global_target_minimum = 0;
    current_minimum = optimization_problem(NULL);

    // check if it needed to do the subunitarization
    if (check_optimization_solution()) {
        std::stringstream sstream;
        sstream << "Disentanglig not needed" << std::endl;
        print(sstream, 2);	    	                    
        subdecomposed_mtx = Umtx;
        subdisentaglement_done = true;
        return;
    }


    if ( !check_optimization_solution() ) {
        // Adding the gates of the successive layers

        //measure the time for the decompositin
        tbb::tick_count start_time = tbb::tick_count::now();

        // the maximal number of layers in the subdeconposition
        int max_layer_num_loc;
        try {
            max_layer_num_loc = max_layer_num[qbit_num];
        }
        catch (...) {
            std::stringstream sstream;
	    sstream << "Layer number not given" << std::endl;
	    print(sstream, 0);	    
            throw sstream.str();
        }

        while ( layer_num < max_layer_num_loc ) {

            // add another gate layers to the gate structure used in the decomposition
            add_gate_layers();

            // get the number of blocks
            layer_num = gates.size();

            // Do the optimization
            if (optimize_layer_num || layer_num >= max_layer_num_loc ) {
/*
                // release optimized parameters if necessary
                if (optimized_parameters != NULL) {
                    qgd_free(optimized_parameters);
                    optimized_parameters = NULL;
                }
*/
                // solve the optimization problem to find the correct minimum
                solve_optimization_problem( NULL, 0);


                if (check_optimization_solution()) {
                    break;
                }
            }

        }

        
        tbb::tick_count current_time = tbb::tick_count::now();

	std::stringstream sstream;
	sstream << "--- " << (current_time - start_time).seconds() << " seconds elapsed during the decomposition ---" << std::endl << std::endl;
	print(sstream, 2);	    	
	
    }



    if (check_optimization_solution()) {
       std::stringstream sstream;
       sstream << "Sub-disentaglement was succesfull." << std::endl << std::endl;
       print(sstream, 1);	    	    
    }
    else {
       std::stringstream sstream;
       sstream << "Sub-disentaglement did not reach the tolerance limit." << std::endl << std::endl;
       print(sstream, 1);	    	
    }


    // indicate that the unitarization of the sumbatrices was done
    subdisentaglement_done = true;

    // The subunitarized matrix
//Matrix_real optimized_parameters_mtx_tmp(optimized_parameters, 1, parameter_num );
    subdecomposed_mtx = get_transformed_matrix( optimized_parameters_mtx, gates.begin(), gates.size(), Umtx );

}

/**
@brief Call to add further layer to the gate structure used in the subdecomposition.
*/
void Sub_Matrix_Decomposition::add_gate_layers() {

    if ( unit_gate_structure == NULL ) {
        // add default gate structure if custom is not given
        add_default_gate_layers();
        return;
    }

    //////////////////////////////////////
    // add custom gate structure

    // the  number of succeeding identical layers in the subdecomposition
    int identical_blocks_loc;
    try {
        identical_blocks_loc = identical_blocks[qbit_num];
        if (identical_blocks_loc==0) {
            identical_blocks_loc = 1;
        }
    }
    catch (...) {
        identical_blocks_loc=1;
    }

    // get the list of sub-blocks in the custom gate structure
    std::vector<Gate*> gates = unit_gate_structure->get_gates();

    for (std::vector<Gate*>::iterator gates_it = gates.begin(); gates_it != gates.end(); gates_it++ ) {

        // The current gate
        Gate* gate = *gates_it;

        for (int idx=0;  idx<identical_blocks_loc; idx++) {
            if (gate->get_type() == CNOT_OPERATION ) {
                CNOT* cnot_gate = static_cast<CNOT*>( gate );
                add_gate_to_end( (Gate*)cnot_gate->clone() );
            }
            else if (gate->get_type() == CZ_OPERATION ) {
                CZ* cz_gate = static_cast<CZ*>( gate );
                add_gate_to_end( (Gate*)cz_gate->clone() );
            }
            else if (gate->get_type() == CH_OPERATION ) {
                CH* ch_gate = static_cast<CH*>( gate );
                add_gate_to_end( (Gate*)ch_gate->clone() );
            }
            else if (gate->get_type() == SYC_OPERATION ) {
                SYC* syc_gate = static_cast<SYC*>( gate );
                add_gate_to_end( (Gate*)syc_gate->clone() );
            }
            else if (gate->get_type() == GENERAL_OPERATION ) {
                add_gate_to_end( gate->clone() );
            }
            else if (gate->get_type() == U3_OPERATION ) {
                U3* u3_gate = static_cast<U3*>( gate );
                add_gate_to_end( (Gate*)u3_gate->clone() );
            }
            else if (gate->get_type() == RX_OPERATION ) {
                RX* rx_gate = static_cast<RX*>( gate );
                add_gate_to_end( (Gate*)rx_gate->clone() );
            }
            else if (gate->get_type() == RY_OPERATION ) {
                RY* ry_gate = static_cast<RY*>( gate );
                add_gate_to_end( (Gate*)ry_gate->clone() );
            }
            else if (gate->get_type() == CRY_OPERATION ) {
                CRY* cry_gate = static_cast<CRY*>( gate );
                add_gate_to_end( (Gate*)cry_gate->clone() );
            }
            else if (gate->get_type() == RZ_OPERATION ) {
                RZ* rz_gate = static_cast<RZ*>( gate );
                add_gate_to_end( (Gate*)rz_gate->clone() );
            }
            else if (gate->get_type() == X_OPERATION ) {
                X* x_gate = static_cast<X*>( gate );
                add_gate_to_end( (Gate*)x_gate->clone() );
            }
            else if (gate->get_type() == Y_OPERATION ) {
                Y* y_gate = static_cast<Y*>( gate );
                add_gate_to_end( (Gate*)y_gate->clone() );
            }
            else if (gate->get_type() == Z_OPERATION ) {
                Z* z_gate = static_cast<Z*>( gate );
                add_gate_to_end( (Gate*)z_gate->clone() );
            }
            else if (gate->get_type() == Y_OPERATION ) {
                Y* y_gate = static_cast<Y*>( gate );
                add_gate_to_end( (Gate*)y_gate->clone() );
            }
            else if (gate->get_type() == SX_OPERATION ) {
                SX* sx_gate = static_cast<SX*>( gate );
                add_gate_to_end( (Gate*)sx_gate->clone() );
            }
            else if (gate->get_type() == ADAPTIVE_OPERATION ) {
                Adaptive* ad_gate = static_cast<Adaptive*>( gate );
                add_gate_to_end( (Gate*)ad_gate->clone() );
            }
            else if (gate->get_type() == BLOCK_OPERATION ) {
                Gates_block* block_gate = static_cast<Gates_block*>( gate );
                add_gate_to_end( (Gate*)block_gate->clone() );
            }
            else {
                std::string err("Sub_Matrix_Decomposition::add_gate_layers: Unimplemented gate");
                throw err;
            }

        }
    }


}



/**
@brief Call to add default gate layers to the gate structure used in the subdecomposition.
*/
void Sub_Matrix_Decomposition::add_default_gate_layers() {

    int control_qbit_loc = qbit_num-1;

    // the  number of succeeding identical layers in the subdecomposition
    int identical_blocks_loc;
    try {
        identical_blocks_loc = identical_blocks[qbit_num];
        if (identical_blocks_loc==0) {
            identical_blocks_loc = 1;
        }
    }
    catch (...) {
        identical_blocks_loc=1;
    }


    for (int target_qbit_loc = 0; target_qbit_loc<control_qbit_loc; target_qbit_loc++ ) {

        for (int idx=0;  idx<identical_blocks_loc; idx++) {

            // creating block of gates
            Gates_block* block = new Gates_block( qbit_num );

            // adding U3 gate to the block
            bool Theta = true;
            bool Phi = false;
            bool Lambda = true;
            block->add_u3(target_qbit_loc, Theta, Phi, Lambda);
            block->add_u3(control_qbit_loc, Theta, Phi, Lambda);


            // add CNOT gate to the block
            block->add_cnot(target_qbit_loc, control_qbit_loc);

            // adding the opeartion block to the gates
            add_gate( block );

        }
    }


}



/**
@brief Call to set custom layers to the gate structure that are intended to be used in the subdecomposition.
@param block_in A pointer to the block of gates to be used in the decomposition
*/
void Sub_Matrix_Decomposition::set_custom_gate_layers( Gates_block* block_in) {

    unit_gate_structure = block_in->clone();

}





/**
@brief Call to solve layer by layer the optimization problem. The optimalized parameters are stored in attribute optimized_parameters.
@param num_of_parameters Number of parameters to be optimized
@param solution_guess_gsl A GNU Scientific Library vector containing the solution guess.
*/
void Sub_Matrix_Decomposition::solve_layer_optimization_problem( int num_of_parameters, gsl_vector *solution_guess_gsl) {


        if (gates.size() == 0 ) {
            return;
        }


        if (solution_guess_gsl == NULL) {
            solution_guess_gsl = gsl_vector_alloc(num_of_parameters);
        }
/*
        if (optimized_parameters == NULL) {
            optimized_parameters = (double*)qgd_calloc(num_of_parameters,sizeof(double), 64);
            memcpy(optimized_parameters, solution_guess_gsl->data, num_of_parameters*sizeof(double) );
        }
*/
        if (optimized_parameters_mtx.size() == 0) {
            optimized_parameters_mtx = Matrix_real(1, num_of_parameters);
            memcpy(optimized_parameters_mtx.get_data(), solution_guess_gsl->data, num_of_parameters*sizeof(double) );
        }


        // maximal number of iteration loops
        int iteration_loops_max;
        try {
            iteration_loops_max = std::max(iteration_loops[qbit_num], 1);
        }
        catch (...) {
            iteration_loops_max = 1;
        }

        // random generator of real numbers   
        std::uniform_real_distribution<> distrib_real(0.0, 2*M_PI);

        // do the optimization loops
        for (int idx=0; idx<iteration_loops_max; idx++) {

            int iter = 0;
            int status;

            const gsl_multimin_fdfminimizer_type *T;
            gsl_multimin_fdfminimizer *s;

            Sub_Matrix_Decomposition* par = this;


            gsl_multimin_function_fdf my_func;


            my_func.n = num_of_parameters;
            my_func.f = optimization_problem;
            my_func.df = optimization_problem_grad;
            my_func.fdf = optimization_problem_combined;
            my_func.params = par;


            T = gsl_multimin_fdfminimizer_vector_bfgs2;
            s = gsl_multimin_fdfminimizer_alloc (T, num_of_parameters);


            gsl_multimin_fdfminimizer_set (s, &my_func, solution_guess_gsl, 0.1, 0.1);

            do {
                iter++;

                status = gsl_multimin_fdfminimizer_iterate (s);

                if (status) {
                  break;
                }

                status = gsl_multimin_test_gradient (s->gradient, 1e-1);
                /*if (status == GSL_SUCCESS) {
                    printf ("Minimum found\n");
                }*/

            } while (status == GSL_CONTINUE && iter < 100);

            if (current_minimum > s->f) {
                current_minimum = s->f;
                memcpy( optimized_parameters_mtx.get_data(), s->x->data, num_of_parameters*sizeof(double) );
                gsl_multimin_fdfminimizer_free (s);

                for ( int jdx=0; jdx<num_of_parameters; jdx++) {
                    solution_guess_gsl->data[jdx] = solution_guess_gsl->data[jdx] + distrib_real(gen)/100;
                }
            }
            else {
                for ( int jdx=0; jdx<num_of_parameters; jdx++) {
                    solution_guess_gsl->data[jdx] = solution_guess_gsl->data[jdx] + distrib_real(gen);
                }

                gsl_multimin_fdfminimizer_free (s);
            }

#ifdef __MPI__        
            MPI_Bcast( (void*)solution_guess_gsl->data, num_of_parameters, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#endif



        }


}




/**
@brief The optimization problem of the final optimization
@param parameters An array of the free parameters to be optimized. (The number of teh free paramaters should be equal to the number of parameters in one sub-layer)
@return Returns with the cost function. (zero if the qubits are desintangled.)
*/
double Sub_Matrix_Decomposition::optimization_problem( double* parameters ) {

        // get the transformed matrix with the gates in the list
        Matrix_real parameters_mtx(parameters, 1, parameter_num );
        Matrix matrix_new = get_transformed_matrix( parameters_mtx, gates.begin(), gates.size(), Umtx );

#ifdef DEBUG
        if (matrix_new.isnan()) {
           std::stringstream sstream;
	   sstream << "Sub_Matrix_Decomposition::optimization_problem: matrix_new contains NaN a. Exiting" << std::endl;
	   print(sstream, 1);	
        }
#endif

        double cost_function = get_submatrix_cost_function(matrix_new);


        return cost_function;
}


/**
@brief The optimization problem of the final optimization
@param parameters A GNU Scientific Library containing the parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@return Returns with the cost function. (zero if the qubits are disentangled.)
*/
double Sub_Matrix_Decomposition::optimization_problem( const gsl_vector* parameters, void* void_instance ) {

    Sub_Matrix_Decomposition* instance = reinterpret_cast<Sub_Matrix_Decomposition*>(void_instance);
    std::vector<Gate*> gates_loc = instance->get_gates();

    Matrix Umtx_loc, matrix_new;

//{
//tbb::spin_mutex::scoped_lock my_lock{my_mutex};

    Umtx_loc = instance->get_Umtx();
    Matrix_real parameters_mtx(parameters->data, 1, instance->get_parameter_num() );
    matrix_new = instance->get_transformed_matrix( parameters_mtx, gates_loc.begin(), gates_loc.size(), Umtx_loc );

#ifdef DEBUG
        if (matrix_new.isnan()) {
           std::stringstream sstream;
	   sstream << "Sub_Matrix_Decomposition::optimization_problem matrix_new contains NaN b." << std::endl;
	   print(sstream, 1);	 
        }
#endif

//}


    double cost_function = get_submatrix_cost_function(matrix_new);  //NEW METHOD


    return cost_function;
}





/**
@brief Calculate the approximate derivative (f-f0)/(x-x0) of the cost function with respect to the free parameters.
@param parameters A GNU Scientific Library vector containing the free parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@param grad A GNU Scientific Library vector containing the calculated gradient components.
*/
void Sub_Matrix_Decomposition::optimization_problem_grad( const gsl_vector* parameters, void* void_instance, gsl_vector* grad ) {

    // The function value at x0
    double f0;

    // calculate the approximate gradient
    optimization_problem_combined( parameters, void_instance, &f0, grad);

}



/**
@brief Call to calculate both the cost function and the its gradient components.
@param parameters A GNU Scientific Library vector containing the free parameters to be optimized.
@param void_instance A void pointer pointing to the instance of the current class.
@param f0 The value of the cost function at x0.
@param grad A GNU Scientific Library vector containing the calculated gradient components.
*/
void Sub_Matrix_Decomposition::optimization_problem_combined( const gsl_vector* parameters, void* void_instance, double* f0, gsl_vector* grad ) {

    Sub_Matrix_Decomposition* instance = reinterpret_cast<Sub_Matrix_Decomposition*>(void_instance);

    int parameter_num_loc = instance->get_parameter_num();

    // storage for the function values calculated at the displaced points x
    gsl_vector* f = gsl_vector_alloc(grad->size);

    // the difference in one direction in the parameter for the gradient calculation
    double dparam = 1e-8;

    // calculate the function values at displaced x and the central x0 points through TBB parallel for
    tbb::parallel_for(0, parameter_num_loc+1, 1, [&](int i) {

        if (i == (int)parameters->size) {
            // calculate function value at x0
            *f0 = instance->optimization_problem(parameters, reinterpret_cast<void*>(instance));
        }
        else {

            gsl_vector* parameters_d = gsl_vector_calloc(parameters->size);
            memcpy( parameters_d->data, parameters->data, parameters->size*sizeof(double) );
            parameters_d->data[i] = parameters_d->data[i] + dparam;

            // calculate the cost function at the displaced point
            f->data[i] = instance->optimization_problem(parameters_d, reinterpret_cast<void*>(instance));

            // release vectors
            gsl_vector_free(parameters_d);
            parameters_d = NULL;

        }
    });


/*
    // sequential version
    functor_sub_optimization_grad<Sub_Matrix_Decomposition> tmp = functor_grad<Sub_Matrix_Decomposition>( parameters, instance, f, f0, dparam );
    #pragma omp parallel for
    for (int idx=0; idx<parameter_num_loc+1; idx++) {
        tmp(idx);
    }
*/


    for (int idx=0; idx<parameter_num_loc; idx++) {
        // set the gradient
#ifdef DEBUG
        if (isnan(f->data[idx])) {
	  sstream << "Sub_Matrix_Decomposition::optimization_problem_combined: f->data[i] is NaN " << std::endl;
	  print(sstream, 0);	  
          exit(-1);
        }
#endif // DEBUG
        gsl_vector_set(grad, idx, (f->data[idx]-(*f0))/dparam);
    }


    gsl_vector_free(f);

}










/**
@brief Set the number of identical successive blocks during the subdecomposition of the qbit-th qubit.
@param qbit The number of qubits for which the maximal number of layers should be used in the subdecomposition.
@param identical_blocks_in The number of successive identical layers used in the subdecomposition.
@return Returns with zero in case of success.
*/
int Sub_Matrix_Decomposition::set_identical_blocks( int qbit, int identical_blocks_in )  {

    std::map<int,int>::iterator key_it = identical_blocks.find( qbit );

    if ( key_it != identical_blocks.end() ) {
        identical_blocks.erase( key_it );
    }

    identical_blocks.insert( std::pair<int, int>(qbit,  identical_blocks_in) );

    return 0;

}


/**
@brief Set the number of identical successive blocks during the subdecomposition of the n-th qubit.
@param identical_blocks_in An <int,int> map containing the number of successive identical layers used in the subdecompositions.
@return Returns with zero in case of success.
*/
int Sub_Matrix_Decomposition::set_identical_blocks( std::map<int, int> identical_blocks_in )  {

    for ( std::map<int,int>::iterator it=identical_blocks_in.begin(); it!= identical_blocks_in.end(); it++ ) {
        set_identical_blocks( it->first, it->second );
    }

    return 0;

}


/**
@brief Create a clone of the present class.
@return Return with a pointer pointing to the cloned object.
*/
Sub_Matrix_Decomposition* Sub_Matrix_Decomposition::clone() {


    Sub_Matrix_Decomposition* ret = new Sub_Matrix_Decomposition(Umtx, qbit_num, optimize_layer_num, initial_guess);

    // setting computational parameters
    ret->set_identical_blocks( identical_blocks );
    ret->set_max_iteration( max_iterations );
    ret->set_optimization_blocks( optimization_block );
    ret->set_max_layer_num( max_layer_num );
    ret->set_iteration_loops( iteration_loops );

    if ( extract_gates(static_cast<Gates_block*>(ret)) != 0 ) {
	std::stringstream sstream;
	sstream << "Sub_Matrix_Decomposition::clone(): extracting gates was not succesfull\n" << std::endl;
	print(sstream, 0);	    	
        exit(-1);
    }

    return ret;

}



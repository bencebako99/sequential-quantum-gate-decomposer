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
/*! \file Gates_block.cpp
    \brief Class responsible for grouping two-qubit (CNOT,CZ,CH) and one-qubit gates into layers
*/

#include "CZ.h"
#include "CH.h"
#include "CNOT.h"
#include "U3.h"
#include "RX.h"
#include "RY.h"
#include "CRY.h"
#include "RZ.h"
#include "X.h"
#include "Y.h"
#include "Z.h"
#include "SX.h"
#include "SYC.h"
#include "UN.h"
#include "ON.h"
#include "Adaptive.h"
#include "Composite.h"
#include "Gates_block.h"




//static tbb::spin_mutex my_mutex;
/**
@brief Default constructor of the class.
*/
Gates_block::Gates_block() : Gate() {

    // A string describing the type of the operation
    type = BLOCK_OPERATION;
    // number of operation layers
    layer_num = 0;
}



/**
@brief Constructor of the class.
@param qbit_num_in The number of qubits in the unitaries
*/
Gates_block::Gates_block(int qbit_num_in) : Gate(qbit_num_in) {

    // A string describing the type of the operation
    type = BLOCK_OPERATION;
    // number of operation layers
    layer_num = 0;
}


/**
@brief Destructor of the class.
*/
Gates_block::~Gates_block() {

    release_gates();

}

/**
@brief Call to release the stored gates
*/
void
Gates_block::release_gates() {

    //free the alloctaed memory of the stored gates
    for(std::vector<Gate*>::iterator it = gates.begin(); it != gates.end(); ++it) {

        Gate* operation = *it;

        if (operation->get_type() == CNOT_OPERATION) {
            CNOT* cnot_operation = static_cast<CNOT*>(operation);
            delete cnot_operation;
        }
        else if (operation->get_type() == CZ_OPERATION) {
            CZ* cz_operation = static_cast<CZ*>(operation);
            delete cz_operation;
        }
        else if (operation->get_type() == CH_OPERATION) {
            CH* ch_operation = static_cast<CH*>(operation);
            delete ch_operation;
        }
        else if (operation->get_type() == SYC_OPERATION) {
            SYC* syc_operation = static_cast<SYC*>(operation);
            delete syc_operation;
        }
        else if (operation->get_type() == U3_OPERATION) {

            U3* u3_operation = static_cast<U3*>(operation);
            delete u3_operation;

        }
        else if (operation->get_type() == RY_OPERATION) {

            RY* ry_operation = static_cast<RY*>(operation);
            delete ry_operation;

        }
        else if (operation->get_type() == CRY_OPERATION) {

            CRY* cry_operation = static_cast<CRY*>(operation);
            delete cry_operation;

        }
        else if (operation->get_type() == RX_OPERATION) {

            RX* rx_operation = static_cast<RX*>(operation);
            delete rx_operation;

        }
        else if (operation->get_type() == RZ_OPERATION) {

            RZ* rz_operation = static_cast<RZ*>(operation);
            delete rz_operation;

        }
        else if (operation->get_type() == X_OPERATION) {

            X* x_operation = static_cast<X*>(operation);
            delete x_operation;

        }
        else if (operation->get_type() == Y_OPERATION) {

            Y* y_operation = static_cast<Y*>(operation);
            delete y_operation;

        }
        else if (operation->get_type() == Z_OPERATION) {

            Z* z_operation = static_cast<Z*>(operation);
            delete z_operation;

        }
        else if (operation->get_type() == SX_OPERATION) {

            SX* sx_operation = static_cast<SX*>(operation);            
            delete sx_operation;

        }
        else if (operation->get_type() == BLOCK_OPERATION) {

            Gates_block* block_operation = static_cast<Gates_block*>(operation);
            delete block_operation;

        }
        else if (operation->get_type() == GENERAL_OPERATION) {
            delete operation;
        }
        else if (operation->get_type() == UN_OPERATION) {
            UN* un_operation = static_cast<UN*>(operation);
            delete un_operation;
        }
        else if (operation->get_type() == ON_OPERATION) {
            ON* on_operation = static_cast<ON*>(operation);
            delete on_operation;
        }
        else if (operation->get_type() == COMPOSITE_OPERATION) {
            Composite* com_operation = static_cast<Composite*>(operation);
            delete com_operation;
        }
        else if (operation->get_type() == ADAPTIVE_OPERATION) {

            Adaptive* ad_operation = static_cast<Adaptive*>(operation);
            delete ad_operation;

        }
        else {
            std::string err("Gates_block::release_gates(): unimplemented gate"); 
            throw err;
        }
    }
    
    gates.clear();
    layer_num = 0;
    parameter_num = 0;

}


/**
@brief Call to release one gate in the list
*/
void
Gates_block::release_gate( int idx) {

    if ( idx>= (int)gates.size() ) return;

    // fist decrese the number of parameters
    Gate* gate = gates[idx];
    parameter_num -= gate->get_parameter_num();

    gates.erase( gates.begin() + idx );

}

/**
@brief Call to retrieve the operation matrix (Which is the product of all the operation matrices stored in the operation block)
@param parameters An array pointing to the parameters of the gates
@return Returns with the operation matrix
*/
Matrix
Gates_block::get_matrix( Matrix_real& parameters ) {

    //The stringstream input to store the output messages.
    std::stringstream sstream;

    // create matrix representation of the gate operations
    Matrix block_mtx = create_identity(matrix_size);
    apply_to(parameters, block_mtx);

#ifdef DEBUG
    if (block_mtx.isnan()) {
        std::stringstream sstream;
	sstream << "Gates_block::get_matrix: block_mtx contains NaN." << std::endl;
        print(sstream, 0);		       
    }
#endif

    return block_mtx;


}



/**
@brief Call to apply the gate on the input array/matrix by U3*input
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
void 
Gates_block::apply_to_list( Matrix_real& parameters_mtx, std::vector<Matrix> input ) {


    for ( std::vector<Matrix>::iterator it=input.begin(); it != input.end(); it++ ) {
        apply_to( parameters_mtx, *it );
    }

}


/**
@brief Call to apply the gate on the input array/matrix Gates_block*input
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
void 
Gates_block::apply_to( Matrix_real& parameters_mtx, Matrix& input ) {

    double* parameters = parameters_mtx.get_data();

    parameters = parameters + parameter_num;

    for( int idx=gates.size()-1; idx>=0; idx--) {

        Gate* operation = gates[idx];
        parameters = parameters - operation->get_parameter_num();
        Matrix_real parameters_mtx(parameters, 1, operation->get_parameter_num());

        if (operation->get_type() == CNOT_OPERATION) {
            CNOT* cnot_operation = static_cast<CNOT*>(operation);
            cnot_operation->apply_to(input);
        }
        else if (operation->get_type() == CZ_OPERATION) {
            CZ* cz_operation = static_cast<CZ*>(operation);
            cz_operation->apply_to(input);
        }
        else if (operation->get_type() == CH_OPERATION) {
            CH* ch_operation = static_cast<CH*>(operation);
            ch_operation->apply_to(input);
        }
        else if (operation->get_type() == SYC_OPERATION) {
            SYC* syc_operation = static_cast<SYC*>(operation);
            syc_operation->apply_to(input);
        }
        else if (operation->get_type() == U3_OPERATION) {
            U3* u3_operation = static_cast<U3*>(operation);
            u3_operation->apply_to( parameters_mtx, input );    
        }
        else if (operation->get_type() == RX_OPERATION) {
            RX* rx_operation = static_cast<RX*>(operation);
            rx_operation->apply_to( parameters_mtx, input ); 
        }
        else if (operation->get_type() == RY_OPERATION) {
            RY* ry_operation = static_cast<RY*>(operation);
            ry_operation->apply_to( parameters_mtx, input ); 
        }
        else if (operation->get_type() == CRY_OPERATION) {
            CRY* cry_operation = static_cast<CRY*>(operation);
            cry_operation->apply_to( parameters_mtx, input ); 
        }
        else if (operation->get_type() == RZ_OPERATION) {
            RZ* rz_operation = static_cast<RZ*>(operation);
            rz_operation->apply_to( parameters_mtx, input ); 
        }
        else if (operation->get_type() == X_OPERATION) {
            X* x_operation = static_cast<X*>(operation);
            x_operation->apply_to( input ); 
        }
        else if (operation->get_type() == Y_OPERATION) {
            Y* y_operation = static_cast<Y*>(operation);
            y_operation->apply_to( input ); 
        }
        else if (operation->get_type() == Z_OPERATION) {
            Z* z_operation = static_cast<Z*>(operation);
            z_operation->apply_to( input ); 
        }
        else if (operation->get_type() == SX_OPERATION) {
            SX* sx_operation = static_cast<SX*>(operation);
            sx_operation->apply_to( input ); 
        }
        else if (operation->get_type() == GENERAL_OPERATION) {
            operation->apply_to(input);
        }
        else if (operation->get_type() == UN_OPERATION) {
            UN* un_operation = static_cast<UN*>(operation);
            un_operation->apply_to(parameters_mtx, input);
        }
        else if (operation->get_type() == ON_OPERATION) {
            ON* on_operation = static_cast<ON*>(operation);
            on_operation->apply_to(parameters_mtx, input);
        }
        else if (operation->get_type() == BLOCK_OPERATION) {
            Gates_block* block_operation = static_cast<Gates_block*>(operation);
            block_operation->apply_to(parameters_mtx, input);
        }
        else if (operation->get_type() == COMPOSITE_OPERATION) {
            Composite* com_operation = static_cast<Composite*>(operation);
            com_operation->apply_to(parameters_mtx, input);
        }
        else if (operation->get_type() == ADAPTIVE_OPERATION) {
            Adaptive* ad_operation = static_cast<Adaptive*>(operation);
            ad_operation->apply_to( parameters_mtx, input ); 
        }
        else {
            std::string err("Gates_block::apply_to: unimplemented gate"); 
            throw err;
        }

#ifdef DEBUG
        if (input.isnan()) {
            std::stringstream sstream;
	    sstream << "Gates_block::apply_to: transformed matrix contains NaN." << std::endl;
            print(sstream, 0);	
        }
#endif


    }



}


/**
@brief Call to apply the gate on the input array/matrix by input*Gate_block
@param input The input array on which the gate is applied
*/
void 
Gates_block::apply_from_right( Matrix_real& parameters_mtx, Matrix& input ) {


    //The stringstream input to store the output messages.
    std::stringstream sstream;

    double* parameters = parameters_mtx.get_data();

    for( int idx=0; idx<(int)gates.size(); idx++) {

        Gate* operation = gates[idx];
        Matrix_real parameters_mtx(parameters, 1, operation->get_parameter_num());

        if (operation->get_type() == CNOT_OPERATION) {
            CNOT* cnot_operation = static_cast<CNOT*>(operation);
            cnot_operation->apply_from_right(input);
        }
        else if (operation->get_type() == CZ_OPERATION) {
            CZ* cz_operation = static_cast<CZ*>(operation);
            cz_operation->apply_from_right(input);
        }
        else if (operation->get_type() == CH_OPERATION) {
            CH* ch_operation = static_cast<CH*>(operation);
            ch_operation->apply_from_right(input);
        }
        else if (operation->get_type() == SYC_OPERATION) {
            SYC* syc_operation = static_cast<SYC*>(operation);
            syc_operation->apply_from_right(input);
        }
        else if (operation->get_type() == U3_OPERATION) {
            U3* u3_operation = static_cast<U3*>(operation);
            u3_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == RX_OPERATION) {
            RX* rx_operation = static_cast<RX*>(operation);
            rx_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == RY_OPERATION) {
            RY* ry_operation = static_cast<RY*>(operation);
            ry_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == CRY_OPERATION) {
            CRY* cry_operation = static_cast<CRY*>(operation);
            cry_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == RZ_OPERATION) {
            RZ* rz_operation = static_cast<RZ*>(operation);
            rz_operation->apply_from_right( parameters_mtx, input );         
        }
        else if (operation->get_type() == X_OPERATION) {
            X* x_operation = static_cast<X*>(operation);
            x_operation->apply_from_right( input ); 
        }
        else if (operation->get_type() == Y_OPERATION) {
            Y* y_operation = static_cast<Y*>(operation);
            y_operation->apply_from_right( input ); 
        }
        else if (operation->get_type() == Z_OPERATION) {
            Z* z_operation = static_cast<Z*>(operation);
            z_operation->apply_from_right( input ); 
        }
        else if (operation->get_type() == SX_OPERATION) {
            SX* sx_operation = static_cast<SX*>(operation);
            sx_operation->apply_from_right( input ); 
        }
        else if (operation->get_type() == GENERAL_OPERATION) {
            operation->apply_from_right(input);
        }
        else if (operation->get_type() == UN_OPERATION) {
            UN* un_operation = static_cast<UN*>(operation);
            un_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == ON_OPERATION) {
            ON* on_operation = static_cast<ON*>(operation);
            on_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == BLOCK_OPERATION) {
            Gates_block* block_operation = static_cast<Gates_block*>(operation);
            block_operation->apply_from_right(parameters_mtx, input);
        }
        else if (operation->get_type() == COMPOSITE_OPERATION) {
            Composite* com_operation = static_cast<Composite*>(operation);
            com_operation->apply_from_right( parameters_mtx, input ); 
        }
        else if (operation->get_type() == ADAPTIVE_OPERATION) {
            Adaptive* ad_operation = static_cast<Adaptive*>(operation);
            ad_operation->apply_from_right( parameters_mtx, input ); 
        }
        else {
            std::string err("Gates_block::apply_from_right: unimplemented gate"); 
            throw err;
        }

        parameters = parameters + operation->get_parameter_num();

#ifdef DEBUG
        if (input.isnan()) { 
            std::stringstream sstream;
	    sstream << "Gates_block::apply_from_right: transformed matrix contains NaN." << std::endl;
            print(sstream, 0);	            
        }
#endif


    }


}



/**
@brief ???????????????
*/
std::vector<Matrix> 
Gates_block::apply_derivate_to( Matrix_real& parameters_mtx_in, Matrix& input ) {

    //The stringstream input to store the output messages.
    std::stringstream sstream;
  
    std::vector<Matrix> grad(parameter_num, Matrix(0,0));

    // deriv_idx ... the index of the gate block for which the gradient is to be calculated
    tbb::parallel_for( tbb::blocked_range<int>(0,gates.size()), [&](tbb::blocked_range<int> r) {
        for (int deriv_idx=r.begin(); deriv_idx<r.end(); ++deriv_idx) { 


            Gate* gate_deriv = gates[deriv_idx];

            // for constant gate no gardient component is calculated
            if ( gate_deriv->get_parameter_num() == 0 ) {
                continue;
            }

            int deriv_parameter_idx = 0;
            for ( int idx=0; idx<deriv_idx; idx++ ) {
                deriv_parameter_idx += gates[idx]->get_parameter_num();
            }

            Matrix&& input_loc = input.copy();

            double* parameters = parameters_mtx_in.get_data();
            parameters = parameters + parameter_num;


            std::vector<Matrix> grad_loc;

            for( int idx=gates.size()-1; idx>=0; idx--) {

                Gate* operation = gates[idx];
                parameters = parameters - operation->get_parameter_num();
                Matrix_real parameters_mtx(parameters, 1, operation->get_parameter_num());

                if (operation->get_type() == CNOT_OPERATION) {
                    CNOT* cnot_operation = static_cast<CNOT*>(operation);
                    if ( deriv_idx < idx ) {
                        cnot_operation->apply_to( input_loc );    
                    }
                    else {
                        cnot_operation->Gate::apply_to_list(grad_loc );
                    }
                }
                else if (operation->get_type() == CZ_OPERATION) {
                    CZ* cz_operation = static_cast<CZ*>(operation);
                    if ( deriv_idx < idx ) {
                        cz_operation->apply_to( input_loc );    
                    }
                    else {
                        cz_operation->Gate::apply_to_list(grad_loc );
                    }
                }
                else if (operation->get_type() == CH_OPERATION) {
                    CH* ch_operation = static_cast<CH*>(operation);
                    if ( deriv_idx < idx ) {
                        ch_operation->apply_to( input_loc );    
                    }
                    else {
                        ch_operation->Gate::apply_to_list(grad_loc );
                    }
                }    
    
                else if (operation->get_type() == SYC_OPERATION) {
                        std::stringstream sstream;
	    	        sstream << "Sycamore operation not supported in gardient calculation" << std::endl;			
			print(sstream, 0);	                    
			exit(-1);
                }

                else if (operation->get_type() == U3_OPERATION) {
    
                    U3* u3_operation = static_cast<U3*>(operation);
                    if ( deriv_idx < idx ) {
                        u3_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
    
                        grad_loc = u3_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        u3_operation->apply_to_list( parameters_mtx, grad_loc );
                    }
                }

                else if (operation->get_type() == RX_OPERATION) {
                    RX* rx_operation = static_cast<RX*>(operation);
                    if ( deriv_idx < idx ) {
                        rx_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
    
                        grad_loc = rx_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        rx_operation->apply_to_list( parameters_mtx, grad_loc );
                    }
                }


                else if (operation->get_type() == RY_OPERATION) {
                    RY* ry_operation = static_cast<RY*>(operation);
                    if ( deriv_idx < idx ) {
                        ry_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
    
                        grad_loc = ry_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        ry_operation->apply_to_list( parameters_mtx, grad_loc );    
                    }    
                }

                else if (operation->get_type() == CRY_OPERATION) {
                    CRY* cry_operation = static_cast<CRY*>(operation);
                    if ( deriv_idx < idx ) {
                        cry_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
                        grad_loc = cry_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        cry_operation->apply_to_list( parameters_mtx, grad_loc );
                    }
                }

                else if (operation->get_type() == RZ_OPERATION) {
                    RZ* rz_operation = static_cast<RZ*>(operation);
                    if ( deriv_idx < idx ) {
                        rz_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
    
                        grad_loc = rz_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        rz_operation->apply_to_list( parameters_mtx, grad_loc );
                    }
                }

                else if (operation->get_type() == X_OPERATION) {
                    X* x_operation = static_cast<X*>(operation);
                    if ( deriv_idx < idx ) {
                        x_operation->apply_to( input_loc );    
                    }
                    else {
                        x_operation->Gate::apply_to_list( grad_loc );
                    }
                }
                else if (operation->get_type() == Y_OPERATION) {
                    Y* y_operation = static_cast<Y*>(operation);
                    if ( deriv_idx < idx ) {
                        y_operation->apply_to( input_loc );    
                    }
                    else {
                        y_operation->Gate::apply_to_list( grad_loc );
                    }
                }
                else if (operation->get_type() == Z_OPERATION) {
                    Z* z_operation = static_cast<Z*>(operation);
                    if ( deriv_idx < idx ) {
                        z_operation->apply_to( input_loc );    
                    }
                    else {
                        z_operation->Gate::apply_to_list( grad_loc );
                    }
                }
                else if (operation->get_type() == SX_OPERATION) {
                    SX* sx_operation = static_cast<SX*>(operation);
                    if ( deriv_idx < idx ) {
                        sx_operation->apply_to( input_loc );    
                    }
                    else {
                        sx_operation->Gate::apply_to_list(grad_loc );
                    }
                }
                else if (operation->get_type() == GENERAL_OPERATION) {
                    if ( deriv_idx < idx ) {
                        operation->apply_to( input_loc );    
                    }
                    else {
                        operation->apply_to_list( grad_loc );
                    }
                }

                else if (operation->get_type() == UN_OPERATION) {
                        std::stringstream sstream;
	    	        sstream << "UN operation not supported in gardient calculation" << std::endl;
			print(sstream, 0);	
			exit(-1);
                }
                else if (operation->get_type() == ON_OPERATION) {
                        std::stringstream sstream;
		        sstream << "ON operation not supported in gardient calculation" << std::endl;
			print(sstream, 0);	
			exit(-1);
                }

                else if (operation->get_type() == BLOCK_OPERATION) {
                    Gates_block* block_operation = static_cast<Gates_block*>(operation);
                    if ( deriv_idx < idx ) {
                        block_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
                        grad_loc = block_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        block_operation->apply_to_list( parameters_mtx, grad_loc );
                    }
                }

                else if (operation->get_type() == COMPOSITE_OPERATION) {
                        std::stringstream sstream;
	    	        sstream << "Composite  operation not supported in gardient calculation" << std::endl;
			print(sstream, 0);	
	                exit(-1);
                }

                else if (operation->get_type() == ADAPTIVE_OPERATION) {
                    Adaptive* ad_operation = static_cast<Adaptive*>(operation);
                    if ( deriv_idx < idx ) {
                        ad_operation->apply_to( parameters_mtx, input_loc );    
                    }
                    else if ( deriv_idx == idx ) {
                        grad_loc = ad_operation->apply_derivate_to( parameters_mtx, input_loc );    
                    }
                    else {
                        ad_operation->apply_to_list( parameters_mtx, grad_loc );
                    }
                }
                else {
                    std::string err("Gates_block::apply_derivate_to: unimplemented gate"); 
                    throw err;
                }

            }


            for ( int idx = 0; idx<(int)grad_loc.size(); idx++ ) {
                grad[deriv_parameter_idx+idx] = grad_loc[idx];
            }


        } // tbb range end
    
    });

    return grad;

}

/**
@brief Append a U3 gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
@param Theta The Theta parameter of the U3 operation
@param Phi The Phi parameter of the U3 operation
@param Lambda The Lambda parameter of the U3 operation
*/
void Gates_block::add_u3_to_end(int target_qbit, bool Theta, bool Phi, bool Lambda) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new U3( qbit_num, target_qbit, Theta, Phi, Lambda ));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a U3 gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
@param Theta The Theta parameter of the U3 operation
@param Phi The Phi parameter of the U3 operation
@param Lambda The Lambda parameter of the U3 operation
*/
void Gates_block::add_u3(int target_qbit, bool Theta, bool Phi, bool Lambda) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new U3( qbit_num, target_qbit, Theta, Phi, Lambda ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a RX gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_rx_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new RX( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a RX gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_rx(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new RX( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a RY gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_ry_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new RY( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}


/**
@brief Add a RY gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_ry(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new RY( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}




/**
@brief Append a RY gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_cry_to_end(int target_qbit, int control_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new CRY( qbit_num, target_qbit, control_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}



/**
@brief Add a RY gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_cry(int target_qbit, int control_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new CRY( qbit_num, target_qbit, control_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a RZ gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_rz_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new RZ( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a RZ gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_rz(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new RZ( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}

/**
@brief Append a C_NOT gate operation to the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_cnot_to_end(  int target_qbit, int control_qbit) {

        // new cnot operation
        Gate* gate = static_cast<Gate*>(new CNOT(qbit_num, target_qbit, control_qbit ));

        // append the operation to the list
        add_gate_to_end(gate);

}



/**
@brief Add a C_NOT gate operation to the front of the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_cnot( int target_qbit, int control_qbit) {

        // new cnot operation
        Gate* gate = static_cast<Gate*>(new CNOT(qbit_num, target_qbit, control_qbit ));

        // put the operation to tghe front of the list
        add_gate(gate);

}




/**
@brief Append a CZ gate operation to the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_cz_to_end(  int target_qbit, int control_qbit) {

        // new cz operation
        Gate* gate = static_cast<Gate*>(new CZ(qbit_num, target_qbit, control_qbit ));

        // append the operation to the list
        add_gate_to_end(gate);

}



/**
@brief Add a CZ gate operation to the front of the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_cz(  int target_qbit, int control_qbit) {

        // new cz operation
        Gate* gate = static_cast<Gate*>(new CZ(qbit_num, target_qbit, control_qbit ));

        // put the operation to tghe front of the list
        add_gate(gate);

}


/**
@brief Append a X gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_x_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new X( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a X gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_x(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new X( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a Y gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_y_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new Y( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a Y gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_y(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new Y( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a Z gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_z_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new Z( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a Z gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_z(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new Z( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}




/**
@brief Append a SX gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_sx_to_end(int target_qbit) {

        // create the operation
        Gate* operation = static_cast<Gate*>(new SX( qbit_num, target_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a SX gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_sx(int target_qbit ) {

        // create the operation
        Gate* gate = static_cast<Gate*>(new SX( qbit_num, target_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}





/**
@brief Append a Sycamore gate operation to the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_syc_to_end(  int target_qbit, int control_qbit) {

        // new cz operation
        Gate* gate = static_cast<Gate*>(new SYC(qbit_num, target_qbit, control_qbit ));

        // append the operation to the list
        add_gate_to_end(gate);

}



/**
@brief Add a Sycamore gate operation to the front of the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_syc(  int target_qbit, int control_qbit) {

        // new cz operation
        Gate* gate = static_cast<Gate*>(new SYC(qbit_num, target_qbit, control_qbit ));

        // put the operation to tghe front of the list
        add_gate(gate);

}




/**
@brief Append a CH gate (i.e. controlled Hadamard gate) operation to the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_ch_to_end(  int target_qbit, int control_qbit) {

        // new cz operation
        Gate* gate = static_cast<Gate*>(new CH(qbit_num, target_qbit, control_qbit ));

        // append the operation to the list
        add_gate_to_end(gate);

}



/**
@brief Add a CH gate (i.e. controlled Hadamard gate) operation to the front of the list of gates
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
@param target_qbit The identification number of the target qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_ch(  int target_qbit, int control_qbit) {

        // new cz operation
        Gate* gate = static_cast<Gate*>(new CH(qbit_num, target_qbit, control_qbit ));

        // put the operation to tghe front of the list
        add_gate(gate);

}

/**
@brief Append a list of gates to the list of gates
@param gates_in A list of operation class instances.
*/
void Gates_block::add_gates_to_end( std::vector<Gate*> gates_in) {

        for(std::vector<Gate*>::iterator it = gates_in.begin(); it != gates_in.end(); ++it) {
            add_gate_to_end( *it );
        }

}


/**
@brief Add an array of gates to the front of the list of gates
@param gates_in A list of operation class instances.
*/
void Gates_block::add_gates( std::vector<Gate*>  gates_in) {

        // adding gates in reversed order!!
        for(std::vector<Gate*>::iterator it = gates_in.end(); it != gates_in.begin(); --it) {
            add_gate( *it );
        }

}



/**
@brief Append a UN gate to the list of gates
*/
void Gates_block::add_un_to_end() {

        // create the operation
        Gate* operation = static_cast<Gate*>(new UN( qbit_num ));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a UN gate to the front of the list of gates
*/
void Gates_block::add_un() {

        // create the operation
        Gate* gate = static_cast<Gate*>(new UN( qbit_num ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}


/**
@brief Append a ON gate to the list of gates
*/
void Gates_block::add_on_to_end() {

        // create the operation
        Gate* operation = static_cast<Gate*>(new ON( qbit_num ));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a ON gate to the front of the list of gates
*/
void Gates_block::add_on() {

        // create the operation
        Gate* gate = static_cast<Gate*>(new ON( qbit_num ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}


/**
@brief Append a Composite gate to the list of gates
*/
void Gates_block::add_composite_to_end()  {

        // create the operation
        Gate* operation = static_cast<Gate*>(new Composite( qbit_num ));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}

/**
@brief Add a Composite gate to the front of the list of gates
*/
void Gates_block::add_composite()  {

        // create the operation
        Gate* gate = static_cast<Gate*>(new Composite( qbit_num ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a Adaptive gate to the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_adaptive_to_end(int target_qbit, int control_qbit)  {

        // create the operation
        Gate* operation = static_cast<Gate*>(new Adaptive( qbit_num, target_qbit, control_qbit));

        // adding the operation to the end of the list of gates
        add_gate_to_end( operation );
}


/**
@brief Add a Adaptive gate to the front of the list of gates
@param target_qbit The identification number of the targt qubit. (0 <= target_qbit <= qbit_num-1)
@param control_qbit The identification number of the control qubit. (0 <= target_qbit <= qbit_num-1)
*/
void Gates_block::add_adaptive(int target_qbit, int control_qbit)  {

        // create the operation
        Gate* gate = static_cast<Gate*>(new Adaptive( qbit_num, target_qbit, control_qbit ));

        // adding the operation to the front of the list of gates
        add_gate( gate );

}



/**
@brief Append a general gate to the list of gates
@param gate A pointer to a class Gate describing a gate operation.
*/
void Gates_block::add_gate_to_end( Gate* gate ) {

        //set the number of qubit in the gate
        gate->set_qbit_num( qbit_num );

        // append the gate to the list
        gates.push_back(gate);

        // increase the number of parameters by the number of parameters
        parameter_num = parameter_num + gate->get_parameter_num();

        // increase the number of layers if necessary
        if (gate->get_type() == BLOCK_OPERATION) {
            layer_num = layer_num + 1;
        }

}

/**
@brief Add a gate to the front of the list of gates
@param gate A pointer to a class Gate describing a gate.
*/
 void Gates_block::add_gate( Gate* gate) {


        // set the number of qubit in the gate
        gate->set_qbit_num( qbit_num );

        gates.insert( gates.begin(), gate);

        // increase the number of U3 gate parameters by the number of parameters
        parameter_num = parameter_num + gate->get_parameter_num();

        // increase the number of layers if necessary
        if (gate->get_type() == BLOCK_OPERATION) {
            layer_num = layer_num + 1;
        }

}



/**
@brief ??????
@param gate A pointer to a class Gate describing an gate.
*/
void 
Gates_block::insert_gate( Gate* gate, int idx ) {


        // set the number of qubit in the gate
        gate->set_qbit_num( qbit_num );

        gates.insert( gates.begin()+idx, gate);

        // increase the number of U3 gate parameters by the number of parameters
        parameter_num = parameter_num + gate->get_parameter_num();

        // increase the number of layers if necessary
        if (gate->get_type() == BLOCK_OPERATION) {
            layer_num = layer_num + 1;
        }


}



/**
@brief Call to get the number of the individual gate types in the list of gates
@return Returns with an instance gates_num describing the number of the individual gate types
*/
gates_num Gates_block::get_gate_nums() {

        gates_num gate_nums;

        gate_nums.u3      = 0;
        gate_nums.rx      = 0;
        gate_nums.ry      = 0;
        gate_nums.cry      = 0;
        gate_nums.rz      = 0;
        gate_nums.cnot    = 0;
        gate_nums.cz      = 0;
        gate_nums.ch      = 0;
        gate_nums.x       = 0;
        gate_nums.sx      = 0;
        gate_nums.syc     = 0;
        gate_nums.un     = 0;
        gate_nums.on     = 0;
        gate_nums.com     = 0;
        gate_nums.general = 0;
        gate_nums.adap = 0;
        gate_nums.total = 0;

        for(std::vector<Gate*>::iterator it = gates.begin(); it != gates.end(); ++it) {
            // get the specific gate or block of gates
            Gate* gate = *it;

            if (gate->get_type() == BLOCK_OPERATION) {

                Gates_block* block_gate = static_cast<Gates_block*>(gate);
                gates_num gate_nums_loc = block_gate->get_gate_nums();
                gate_nums.u3   = gate_nums.u3 + gate_nums_loc.u3;
                gate_nums.rx   = gate_nums.rx + gate_nums_loc.rx;
                gate_nums.ry   = gate_nums.ry + gate_nums_loc.ry;
                gate_nums.cry   = gate_nums.cry + gate_nums_loc.cry;
                gate_nums.rz   = gate_nums.rz + gate_nums_loc.rz;
                gate_nums.cnot = gate_nums.cnot + gate_nums_loc.cnot;
                gate_nums.cz = gate_nums.cz + gate_nums_loc.cz;
                gate_nums.ch = gate_nums.ch + gate_nums_loc.ch;
                gate_nums.x  = gate_nums.x + gate_nums_loc.x;
                gate_nums.sx = gate_nums.sx + gate_nums_loc.sx;
                gate_nums.syc   = gate_nums.syc + gate_nums_loc.syc;
                gate_nums.un   = gate_nums.un + gate_nums_loc.un;
                gate_nums.on   = gate_nums.on + gate_nums_loc.on;
                gate_nums.com  = gate_nums.com + gate_nums_loc.com;
                gate_nums.adap = gate_nums.adap + gate_nums_loc.adap;
                gate_nums.total = gate_nums.total + gate_nums_loc.total;

            }
            else if (gate->get_type() == U3_OPERATION) {
                gate_nums.u3   = gate_nums.u3 + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == RX_OPERATION) {
                gate_nums.rx   = gate_nums.rx + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == RY_OPERATION) {
                gate_nums.ry   = gate_nums.ry + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == CRY_OPERATION) {
                gate_nums.cry   = gate_nums.cry + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == RZ_OPERATION) {
                gate_nums.rz   = gate_nums.rz + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == CNOT_OPERATION) {
                gate_nums.cnot   = gate_nums.cnot + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == CZ_OPERATION) {
                gate_nums.cz   = gate_nums.cz + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == CH_OPERATION) {
                gate_nums.ch   = gate_nums.ch + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == X_OPERATION) {
                gate_nums.x   = gate_nums.x + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == Y_OPERATION) {
                gate_nums.y   = gate_nums.y + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == Z_OPERATION) {
                gate_nums.z   = gate_nums.z + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == SX_OPERATION) {
                gate_nums.sx   = gate_nums.sx + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == SYC_OPERATION) {
                gate_nums.syc   = gate_nums.syc + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == GENERAL_OPERATION) {
                gate_nums.general   = gate_nums.general + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == UN_OPERATION) {
                gate_nums.un   = gate_nums.un + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == ON_OPERATION) {
                gate_nums.on   = gate_nums.on + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == COMPOSITE_OPERATION) {
                gate_nums.com   = gate_nums.com + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else if (gate->get_type() == ADAPTIVE_OPERATION) {
                gate_nums.adap   = gate_nums.adap + 1;
                gate_nums.total = gate_nums.total + 1;
            }
            else {
                std::string err("Gates_block::get_gate_nums: unimplemented gate"); 
                throw err;
            }

        }


        return gate_nums;

}


/**
@brief Call to get the number of free parameters
@return Return with the number of parameters of the gates grouped in the gate block.
*/
int Gates_block::get_parameter_num() {
    return parameter_num;
}


/**
@brief Call to get the number of gates grouped in the class
@return Return with the number of the gates grouped in the gate block.
*/
int Gates_block::get_gate_num() {
    return gates.size();
}


/**
@brief Call to print the list of gates stored in the block of gates for a specific set of parameters
@param parameters The parameters of the gates that should be printed.
@param start_index The ordinal number of the first gate.
*/
void Gates_block::list_gates( const Matrix_real &parameters, int start_index ) {


	//The stringstream input to store the output messages.
	std::stringstream sstream;
	sstream << std::endl << "The gates in the list of gates:" << std::endl;
	print(sstream, 1);	    	
		
        int gate_idx = start_index;
        int parameter_idx = parameter_num;
	double *parameters_data = parameters.get_data();
	//const_cast <Matrix_real&>(parameters);


        for(int op_idx = gates.size()-1; op_idx>=0; op_idx--) {

            Gate* gate = gates[op_idx];

            if (gate->get_type() == CNOT_OPERATION) {
                CNOT* cnot_gate = static_cast<CNOT*>(gate);
                std::stringstream sstream;
		sstream << gate_idx << "th gate: CNOT with control qubit: " << cnot_gate->get_control_qbit() << " and target qubit: " << cnot_gate->get_target_qbit() << std::endl;
		print(sstream, 1);   		
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CZ_OPERATION) {
                CZ* cz_gate = static_cast<CZ*>(gate);
		std::stringstream sstream;
		sstream << gate_idx << "th gate: CZ with control qubit: " << cz_gate->get_control_qbit() << " and target qubit: " << cz_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	             
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CH_OPERATION) {
                CH* ch_gate = static_cast<CH*>(gate);
		std::stringstream sstream;
		sstream << gate_idx << "th gate: CH with control qubit: " << ch_gate->get_control_qbit() << " and target qubit: " << ch_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == SYC_OPERATION) {
                SYC* syc_gate = static_cast<SYC*>(gate);
		std::stringstream sstream;
		sstream << gate_idx << "th gate: Sycamore gate with control qubit: " << syc_gate->get_control_qbit() << " and target qubit: " << syc_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == U3_OPERATION) {

                // definig the U3 parameters
                double vartheta;
                double varphi;
                double varlambda;
		
                // get the inverse parameters of the U3 rotation

                U3* u3_gate = static_cast<U3*>(gate);

                if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_theta_parameter()) {
		   
                    varphi = std::fmod( parameters_data[parameter_idx-1], 4*M_PI);
                    varphi = 0;
                    varlambda =0;
                    parameter_idx = parameter_idx - 1;

                }
                else if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_phi_parameter()) {
                    vartheta = 0;
                    varphi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    varlambda =0;
                    parameter_idx = parameter_idx - 1;
                }
                else if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_lambda_parameter()) {
                    vartheta = 0;
                    varphi =  0;
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 1;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_theta_parameter() && u3_gate->is_phi_parameter() ) {
                    vartheta = std::fmod( 2*parameters_data[parameter_idx-2], 4*M_PI);
                    varphi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    varlambda = 0;
                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_theta_parameter() && u3_gate->is_lambda_parameter() ) {
                    vartheta = std::fmod( 2*parameters_data[parameter_idx-2], 4*M_PI);
                    varphi = 0;
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_phi_parameter() && u3_gate->is_lambda_parameter() ) {
                    vartheta = 0;
                    varphi = std::fmod( parameters_data[parameter_idx-2], 2*M_PI);
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 3)) {
                    vartheta = std::fmod( 2*parameters_data[parameter_idx-3], 4*M_PI);
                    varphi = std::fmod( parameters_data[parameter_idx-2], 2*M_PI);
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 3;
                }

//                message = message + "U3 on target qubit %d with parameters theta = %f, phi = %f and lambda = %f";

		std::stringstream sstream;
		sstream << gate_idx << "th gate: U3 on target qubit: " << u3_gate->get_target_qbit() << " and with parameters theta = " << vartheta << ", phi = " << varphi << " and lambda = " << varlambda << std::endl;
		print(sstream, 1);	    	
		                
                gate_idx = gate_idx + 1;

            }
            else if (gate->get_type() == RX_OPERATION) {
	        // definig the rotation parameter
                double vartheta;
                // get the inverse parameters of the U3 rotation
                RX* rx_gate = static_cast<RX*>(gate);		
                vartheta = std::fmod( 2*parameters_data[parameter_idx-1], 4*M_PI);
                parameter_idx = parameter_idx - 1;

		std::stringstream sstream;
		sstream << gate_idx << "th gate: RX on target qubit: " << rx_gate->get_target_qbit() << " and with parameters theta = " << vartheta << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == RY_OPERATION) {
                // definig the rotation parameter
                double vartheta;
                // get the inverse parameters of the U3 rotation
                RY* ry_gate = static_cast<RY*>(gate);
                vartheta = std::fmod( 2*parameters_data[parameter_idx-1], 4*M_PI);
                parameter_idx = parameter_idx - 1;

		std::stringstream sstream;
		sstream << gate_idx << "th gate: RY on target qubit: " << ry_gate->get_target_qbit() << " and with parameters theta = " << vartheta << std::endl; 
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CRY_OPERATION) {
                // definig the rotation parameter
                double Phi;
                // get the inverse parameters of the U3 rotation
                CRY* cry_gate = static_cast<CRY*>(gate);
                Phi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;

		std::stringstream sstream;
		sstream << gate_idx << "th gate: CRY on target qubit: " << cry_gate->get_target_qbit() << ", control qubit" << cry_gate->get_control_qbit() << " and with parameters Phi = " << Phi << std::endl;
		print(sstream, 1);	    		                    
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == RZ_OPERATION) {
                // definig the rotation parameter
                double varphi;
                // get the inverse parameters of the U3 rotation
                RZ* rz_gate = static_cast<RZ*>(gate);
                varphi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;

		std::stringstream sstream;
		sstream << gate_idx << "th gate: RZ on target qubit: " << rz_gate->get_target_qbit() << " and with parameters varphi = " << varphi << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == X_OPERATION) {
                // get the inverse parameters of the U3 rotation
                X* x_gate = static_cast<X*>(gate);
		std::stringstream sstream;
		sstream << gate_idx << "th gate: X on target qubit: " << x_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == Y_OPERATION) {
                // get the inverse parameters of the U3 rotation
                Y* y_gate = static_cast<Y*>(gate);
		std::stringstream sstream;
		sstream << gate_idx << "th gate: Y on target qubit: " << y_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == Z_OPERATION) {
                // get the inverse parameters of the U3 rotation
                Z* z_gate = static_cast<Z*>(gate);
		std::stringstream sstream;
		sstream << gate_idx << "th gate: Z on target qubit: " << z_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == SX_OPERATION) {
                // get the inverse parameters of the U3 rotation
                SX* sx_gate = static_cast<SX*>(gate);

		std::stringstream sstream;
		sstream << gate_idx << "th gate: SX on target qubit: " << sx_gate->get_target_qbit() << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == BLOCK_OPERATION) {
                Gates_block* block_gate = static_cast<Gates_block*>(gate);
                const Matrix_real parameters_layer(parameters.get_data() + parameter_idx - gate->get_parameter_num(), 1, gate->get_parameter_num() );
                block_gate->list_gates( parameters_layer, gate_idx );
                parameter_idx = parameter_idx - block_gate->get_parameter_num();
                gate_idx = gate_idx + block_gate->get_gate_num();
            }
            else if (gate->get_type() == UN_OPERATION) {
                parameter_idx = parameter_idx - gate->get_parameter_num();

		std::stringstream sstream;
		sstream << gate_idx << "th gate: UN " << gate->get_parameter_num() << " parameters" << std::endl;
		print(sstream, 1);	    	         
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == ON_OPERATION) {
                parameter_idx = parameter_idx - gate->get_parameter_num();
		std::stringstream sstream;
		sstream << gate_idx << "th gate: ON " << gate->get_parameter_num() << " parameters" << std::endl;
		print(sstream, 1);	    	 
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == COMPOSITE_OPERATION) {
                parameter_idx = parameter_idx - gate->get_parameter_num();

		std::stringstream sstream;
		sstream << gate_idx << "th gate: Composite " << gate->get_parameter_num() << " parameters" << std::endl;
		print(sstream, 1);	    	               
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == ADAPTIVE_OPERATION) {
                // definig the rotation parameter
                double Theta;
                // get the inverse parameters of the U3 rotation
                Adaptive* ad_gate = static_cast<Adaptive*>(gate);
                Theta = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;

		std::stringstream sstream;
		sstream << gate_idx << "th gate: Adaptive gate on target qubit: " << ad_gate->get_target_qbit() << ", control qubit " << ad_gate->get_control_qbit() << " and with parameters Theta = " << Theta << std::endl;
		print(sstream, 1);	    	
                gate_idx = gate_idx + 1;
            }
            else {
                std::string err("Gates_block::list_gates: unimplemented gate"); 
                throw err;
            }

        }


}


/**
@brief Call to reorder the qubits in the matrix of the gate
@param qbit_list The reordered list of qubits spanning the matrix
*/
void Gates_block::reorder_qubits( std::vector<int>  qbit_list) {

    for(std::vector<Gate*>::iterator it = gates.begin(); it != gates.end(); ++it) {

        Gate* gate = *it;

        if (gate->get_type() == CNOT_OPERATION) {
            CNOT* cnot_gate = static_cast<CNOT*>(gate);
            cnot_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == CZ_OPERATION) {
            CZ* cz_gate = static_cast<CZ*>(gate);
            cz_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == CH_OPERATION) {
            CH* ch_gate = static_cast<CH*>(gate);
            ch_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == SYC_OPERATION) {
            SYC* syc_gate = static_cast<SYC*>(gate);
            syc_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == U3_OPERATION) {
             U3* u3_gate = static_cast<U3*>(gate);
             u3_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == RX_OPERATION) {
             RX* rx_gate = static_cast<RX*>(gate);
             rx_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == RY_OPERATION) {
             RY* ry_gate = static_cast<RY*>(gate);
             ry_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == CRY_OPERATION) {
             CRY* cry_gate = static_cast<CRY*>(gate);
             cry_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == RZ_OPERATION) {
             RZ* rz_gate = static_cast<RZ*>(gate);
             rz_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == X_OPERATION) {
             X* x_gate = static_cast<X*>(gate);
             x_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == Y_OPERATION) {
             Y* y_gate = static_cast<Y*>(gate);
             y_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == Z_OPERATION) {
             Z* z_gate = static_cast<Z*>(gate);
             z_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == SX_OPERATION) {
             SX* sx_gate = static_cast<SX*>(gate);
             sx_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == BLOCK_OPERATION) {
             Gates_block* block_gate = static_cast<Gates_block*>(gate);
             block_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == UN_OPERATION) {
             UN* un_gate = static_cast<UN*>(gate);
             un_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == ON_OPERATION) {
             ON* on_gate = static_cast<ON*>(gate);
             on_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == COMPOSITE_OPERATION) {
             Composite* com_gate = static_cast<Composite*>(gate);
             com_gate->reorder_qubits( qbit_list );
         }
         else if (gate->get_type() == ADAPTIVE_OPERATION) {
             Adaptive* ad_gate = static_cast<Adaptive*>(gate);
             ad_gate->reorder_qubits( qbit_list );
         }
         else {
             std::string err("Gates_block::reorder_qubits: unimplemented gate"); 
             throw err;
         }


    }

}



/**
@brief Call to get the qubits involved in the gates stored in the block of gates.
@return Return with a list of the invovled qubits
*/
std::vector<int> Gates_block::get_involved_qubits() {

    std::vector<int> involved_qbits;

    int qbit;


    for(std::vector<Gate*>::iterator it = gates.begin(); it != gates.end(); ++it) {

        Gate* gate = *it;

        qbit = gate->get_target_qbit();
        if (qbit != -1) {
            add_unique_elelement( involved_qbits, qbit );
        }


        qbit = gate->get_control_qbit();
        if (qbit != -1) {
            add_unique_elelement( involved_qbits, qbit );
        }

    }

    return involved_qbits;
}


/**
@brief Call to get the gates stored in the class.
@return Return with a list of the gates.
*/
std::vector<Gate*> Gates_block::get_gates() {
    return gates;
}


/**
@brief Call to get the gates stored in the class.
@return Return with a list of the gates.
*/
Gate* Gates_block::get_gate(int idx) {

    if (idx > (int)gates.size() ) {
        return NULL;
    }

    return gates[idx];
}


/**
@brief Call to append the gates of a gate block to the current block
@param op_block A pointer to an instance of class Gates_block
*/
void Gates_block::combine(Gates_block* op_block) {

    // getting the list of gates
    std::vector<Gate*> gates_in = op_block->get_gates();

    for(std::vector<Gate*>::iterator it = (gates_in).begin(); it != (gates_in).end(); ++it) {
        Gate* op = *it;

        if (op->get_type() == CNOT_OPERATION) {
            CNOT* cnot_op = static_cast<CNOT*>( op );
            CNOT* cnot_op_cloned = cnot_op->clone();
            Gate* op_cloned = static_cast<Gate*>( cnot_op_cloned );
            add_gate_to_end(op_cloned);
        }
        else if (op->get_type() == CZ_OPERATION) {
            CZ* cz_op = static_cast<CZ*>( op );
            CZ* cz_op_cloned = cz_op->clone();
            Gate* op_cloned = static_cast<Gate*>( cz_op_cloned );
            add_gate_to_end(op_cloned);
        }
        else if (op->get_type() == CH_OPERATION) {
            CH* ch_op = static_cast<CH*>( op );
            CH* ch_op_cloned = ch_op->clone();
            Gate* op_cloned = static_cast<Gate*>( ch_op_cloned );
            add_gate_to_end(op_cloned);
        }
        else if (op->get_type() == SYC_OPERATION) {
            SYC* syc_op = static_cast<SYC*>( op );
            SYC* syc_op_cloned = syc_op->clone();
            Gate* op_cloned = static_cast<Gate*>( syc_op_cloned );
            add_gate_to_end(op_cloned);
        }
        else if (op->get_type() == U3_OPERATION) {
            U3* u3_op = static_cast<U3*>( op );
            U3* u3_op_cloned = u3_op->clone();
            Gate* op_cloned = static_cast<Gate*>( u3_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == RX_OPERATION) {
            RX* rx_op = static_cast<RX*>( op );
            RX* rx_op_cloned = rx_op->clone();
            Gate* op_cloned = static_cast<Gate*>( rx_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == RY_OPERATION) {
            RY* ry_op = static_cast<RY*>( op );
            RY* ry_op_cloned = ry_op->clone();
            Gate* op_cloned = static_cast<Gate*>( ry_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == CRY_OPERATION) {
            CRY* cry_op = static_cast<CRY*>( op );
            CRY* cry_op_cloned = cry_op->clone();
            Gate* op_cloned = static_cast<Gate*>( cry_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == RZ_OPERATION) {
            RZ* rz_op = static_cast<RZ*>( op );
            RZ* rz_op_cloned = rz_op->clone();
            Gate* op_cloned = static_cast<Gate*>( rz_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == X_OPERATION) {
            X* x_op = static_cast<X*>( op );
            X* x_op_cloned = x_op->clone();
            Gate* op_cloned = static_cast<Gate*>( x_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == Y_OPERATION) {
            Y* y_op = static_cast<Y*>( op );
            Y* y_op_cloned = y_op->clone();
            Gate* op_cloned = static_cast<Gate*>( y_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == Z_OPERATION) {
            Z* z_op = static_cast<Z*>( op );
            Z* z_op_cloned = z_op->clone();
            Gate* op_cloned = static_cast<Gate*>( z_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == SX_OPERATION) {
            SX* sx_op = static_cast<SX*>( op );
            SX* sx_op_cloned = sx_op->clone();
            Gate* op_cloned = static_cast<Gate*>( sx_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == BLOCK_OPERATION) {
            Gates_block* block_op = static_cast<Gates_block*>( op );
            Gates_block* block_op_cloned = block_op->clone();
            Gate* op_cloned = static_cast<Gate*>( block_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == GENERAL_OPERATION) {
            Gate* op_cloned = op->clone();
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == UN_OPERATION) {
            UN* un_op = static_cast<UN*>( op );
            UN* un_op_cloned = un_op->clone();
            Gate* op_cloned = static_cast<Gate*>( un_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == ON_OPERATION) {
            ON* on_op = static_cast<ON*>( op );
            ON* on_op_cloned = on_op->clone();
            Gate* op_cloned = static_cast<Gate*>( on_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == COMPOSITE_OPERATION) {
            Composite* com_op = static_cast<Composite*>( op );
            Composite* com_op_cloned = com_op->clone();
            Gate* op_cloned = static_cast<Gate*>( com_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == ADAPTIVE_OPERATION) {
            Adaptive* ad_op = static_cast<Adaptive*>( op );
            Adaptive* ad_op_cloned = ad_op->clone();
            Gate* op_cloned = static_cast<Gate*>( ad_op_cloned );
            add_gate_to_end( op_cloned );
        }
        else {
            std::string err("Gates_block::combine: unimplemented gate"); 
            throw err;
        }

    }

}


/**
@brief Set the number of qubits spanning the matrix of the gates stored in the block of gates.
@param qbit_num_in The number of qubits spanning the matrices.
*/
void Gates_block::set_qbit_num( int qbit_num_in ) {

    // setting the number of qubits
    Gate::set_qbit_num(qbit_num_in);

    // setting the number of qubit in the gates
    for(std::vector<Gate*>::iterator it = gates.begin(); it != gates.end(); ++it) {
        Gate* op = *it;

        if (op->get_type() == CNOT_OPERATION) {
            CNOT* cnot_op = static_cast<CNOT*>( op );
            cnot_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == CZ_OPERATION) {
            CZ* cz_op = static_cast<CZ*>( op );
            cz_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == CH_OPERATION) {
            CH* ch_op = static_cast<CH*>( op );
            ch_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == SYC_OPERATION) {
            SYC* syc_op = static_cast<SYC*>( op );
            syc_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == U3_OPERATION) {
            U3* u3_op = static_cast<U3*>( op );
            u3_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == RX_OPERATION) {
            RX* rx_op = static_cast<RX*>( op );
            rx_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == RY_OPERATION) {
            RY* ry_op = static_cast<RY*>( op );
            ry_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == CRY_OPERATION) {
            CRY* cry_op = static_cast<CRY*>( op );
            cry_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == RZ_OPERATION) {
            RZ* rz_op = static_cast<RZ*>( op );
            rz_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == X_OPERATION) {
            X* x_op = static_cast<X*>( op );
            x_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == Y_OPERATION) {
            Y* y_op = static_cast<Y*>( op );
            y_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == Z_OPERATION) {
            Z* z_op = static_cast<Z*>( op );
            z_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == SX_OPERATION) {
            SX* sx_op = static_cast<SX*>( op );
            sx_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == BLOCK_OPERATION) {
            Gates_block* block_op = static_cast<Gates_block*>( op );
            block_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == UN_OPERATION) {
            UN* un_op = static_cast<UN*>( op );
            un_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == ON_OPERATION) {
            ON* on_op = static_cast<ON*>( op );
            on_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == COMPOSITE_OPERATION) {
            Composite* com_op = static_cast<Composite*>( op );
            com_op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == GENERAL_OPERATION) {
            op->set_qbit_num( qbit_num_in );
        }
        else if (op->get_type() == ADAPTIVE_OPERATION) {
            Adaptive* ad_op = static_cast<Adaptive*>( op );
            ad_op->set_qbit_num( qbit_num_in );
        }
        else {
            std::string err("Gates_block::set_qbit_num: unimplemented gate"); 
            throw err;
        }
    }
}


/**
@brief Create a clone of the present class.
@return Return with a pointer pointing to the cloned object.
*/
Gates_block* Gates_block::clone() {

    // creatign new instance of class Gates_block
    Gates_block* ret = new Gates_block( qbit_num );

    // extracting the gates from the current class
    if (extract_gates( ret ) != 0 ) {
	std::stringstream sstream;
	sstream << "Gates_block::clone(): extracting gates was not succesfull" << std::endl;
	print(sstream, 0);	    	        
        exit(-1);
    };

    return ret;

}


/**
@brief Call to extract the gates stored in the class.
@param op_block An instance of Gates_block class in which the gates will be stored. (The current gates will be erased)
@return Return with 0 on success.
*/
int Gates_block::extract_gates( Gates_block* op_block ) {

    op_block->release_gates();

    for ( std::vector<Gate*>::iterator it=gates.begin(); it != gates.end(); ++it ) {
        Gate* op = *it;

        if (op->get_type() == CNOT_OPERATION) {
            CNOT* cnot_op = static_cast<CNOT*>( op );
            CNOT* cnot_op_cloned = cnot_op->clone();
            Gate* op_cloned = static_cast<Gate*>( cnot_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == CZ_OPERATION) {
            CZ* cz_op = static_cast<CZ*>( op );
            CZ* cz_op_cloned = cz_op->clone();
            Gate* op_cloned = static_cast<Gate*>( cz_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == CH_OPERATION) {
            CH* ch_op = static_cast<CH*>( op );
            CH* ch_op_cloned = ch_op->clone();
            Gate* op_cloned = static_cast<Gate*>( ch_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == SYC_OPERATION) {
            SYC* syc_op = static_cast<SYC*>( op );
            SYC* syc_op_cloned = syc_op->clone();
            Gate* op_cloned = static_cast<Gate*>( syc_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == U3_OPERATION) {
            U3* u3_op = static_cast<U3*>( op );
            U3* u3_op_cloned = u3_op->clone();
            Gate* op_cloned = static_cast<Gate*>( u3_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == RX_OPERATION) {
            RX* rx_op = static_cast<RX*>( op );
            RX* rx_op_cloned = rx_op->clone();
            Gate* op_cloned = static_cast<Gate*>( rx_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == RY_OPERATION) {
            RY* ry_op = static_cast<RY*>( op );
            RY* ry_op_cloned = ry_op->clone();
            Gate* op_cloned = static_cast<Gate*>( ry_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == CRY_OPERATION) {
            CRY* cry_op = static_cast<CRY*>( op );
            CRY* cry_op_cloned = cry_op->clone();
            Gate* op_cloned = static_cast<Gate*>( cry_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == RZ_OPERATION) {
            RZ* rz_op = static_cast<RZ*>( op );
            RZ* rz_op_cloned = rz_op->clone();
            Gate* op_cloned = static_cast<Gate*>( rz_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == X_OPERATION) {
            X* x_op = static_cast<X*>( op );
            X* x_op_cloned = x_op->clone();
            Gate* op_cloned = static_cast<Gate*>( x_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == Y_OPERATION) {
            Y* y_op = static_cast<Y*>( op );
            Y* y_op_cloned = y_op->clone();
            Gate* op_cloned = static_cast<Gate*>( y_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == Z_OPERATION) {
            Z* z_op = static_cast<Z*>( op );
            Z* z_op_cloned = z_op->clone();
            Gate* op_cloned = static_cast<Gate*>( z_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == SX_OPERATION) {
            SX* sx_op = static_cast<SX*>( op );
            SX* sx_op_cloned = sx_op->clone();
            Gate* op_cloned = static_cast<Gate*>( sx_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == BLOCK_OPERATION) {
            Gates_block* block_op = static_cast<Gates_block*>( op );
            Gates_block* block_op_cloned = block_op->clone();
            Gate* op_cloned = static_cast<Gate*>( block_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == UN_OPERATION) {
            UN* un_op = static_cast<UN*>( op );
            UN* un_op_cloned = un_op->clone();
            Gate* op_cloned = static_cast<Gate*>( un_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == ON_OPERATION) {
            ON* on_op = static_cast<ON*>( op );
            ON* on_op_cloned = on_op->clone();
            Gate* op_cloned = static_cast<Gate*>( on_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == COMPOSITE_OPERATION) {
            Composite* com_op = static_cast<Composite*>( op );
            Composite* com_op_cloned = com_op->clone();
            Gate* op_cloned = static_cast<Gate*>( com_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == GENERAL_OPERATION) {
            Gate* op_cloned = op->clone();
            op_block->add_gate_to_end( op_cloned );
        }
        else if (op->get_type() == ADAPTIVE_OPERATION) {
            Adaptive* ad_op = static_cast<Adaptive*>( op );
            Adaptive* ad_op_cloned = ad_op->clone();
            Gate* op_cloned = static_cast<Gate*>( ad_op_cloned );
            op_block->add_gate_to_end( op_cloned );
        }
        else {
            std::string err("Gates_block::extract_gates: unimplemented gate"); 
            throw err;
        }

    }

    return 0;

}



/**
@brief ?????????
@return Return with ?????????
*/
bool Gates_block::contains_adaptive_gate() {

    for ( std::vector<Gate*>::iterator it=gates.begin(); it != gates.end(); ++it ) {
        Gate* op = *it;

        if (op->get_type() == ADAPTIVE_OPERATION) {
            return true;
        }
        else if (op->get_type() == BLOCK_OPERATION) {
            Gates_block* block_op = static_cast<Gates_block*>( op );
            bool ret = block_op->contains_adaptive_gate();
            if ( ret ) return true;
        }
    }

    return false;

}




/**
@brief ?????????
@return Return with ?????????
*/
bool Gates_block::contains_adaptive_gate(int idx) {

    
    Gate* op = gates[idx];

    if (op->get_type() == ADAPTIVE_OPERATION) {
        return true;
    }
    else if (op->get_type() == BLOCK_OPERATION) {
        Gates_block* block_op = static_cast<Gates_block*>( op );
        return block_op->contains_adaptive_gate();
    }

    return false;

}




#ifdef __DFE__


/**
@brief Method to create random initial parameters for the optimization
@return 
*/
DFEgate_kernel_type* Gates_block::convert_to_DFE_gates_with_derivates( Matrix_real& parameters_mtx, int& gatesNum, int& gateSetNum, int& redundantGateSets, bool only_derivates ) {

    int parameter_num = get_parameter_num();
    if ( parameter_num != parameters_mtx.size() ) {
        std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: wrong number of parameters");
        throw error;
    }

    gates_num gate_nums   = get_gate_nums();
    int gates_total_num   = gate_nums.total; 
    int chained_gates_num = get_chained_gates_num();
    int gate_padding      = gates_total_num % chained_gates_num == 0 ? 0 : chained_gates_num - (gates_total_num % chained_gates_num);
    gatesNum              = gates_total_num+gate_padding;
/*
std::cout << "chained gates num: " << chained_gates_num << std::endl;
std::cout << "number of gates: " << gatesNum << std::endl;
*/


    gateSetNum = only_derivates ? parameter_num : parameter_num+1;

#ifdef __MPI__
    int rem = gateSetNum % (4 * world_size );
    if ( rem == 0 ) {
        redundantGateSets = 0;
    }
    else {
        redundantGateSets = (4 * world_size ) - (gateSetNum % (4 * world_size ));
        gateSetNum = gateSetNum + redundantGateSets;
    }
#else
    int rem = gateSetNum % 4;
    if ( rem == 0 ) {
        redundantGateSets = 0;
    }
    else {
        redundantGateSets = 4 - (gateSetNum % 4);
        gateSetNum = gateSetNum + redundantGateSets;
    }
#endif


    DFEgate_kernel_type* DFEgates = new DFEgate_kernel_type[gatesNum*gateSetNum];
    

    int gate_idx = 0;
    convert_to_DFE_gates( parameters_mtx, DFEgates, gate_idx );


    // padding with identity gates
    for (int idx=gate_idx; idx<gatesNum; idx++ ){

        DFEgate_kernel_type& DFEGate = DFEgates[idx];

        
        DFEGate.target_qbit = 0;
        DFEGate.control_qbit = -1;
        DFEGate.gate_type = U3_OPERATION;
        DFEGate.ThetaOver2 = (int32_t)(0);
        DFEGate.Phi = (int32_t)(0);
        DFEGate.Lambda = (int32_t)(0); 
        DFEGate.metadata = 0;

    }
/*
    for ( int idx=0; idx<gatesNum; idx++ ) {

        std::cout << "target qubit: " << (int)DFEgates[idx].target_qbit << " control qubit: " << (int)DFEgates[idx].control_qbit << " gate type: " << (int)DFEgates[idx].gate_type << std::endl; 
    }
*/

    // adjust parameters for derivation
    if (only_derivates ) {
        for (int idx=1; idx<(gateSetNum-1); idx++) {
           memcpy(DFEgates+idx*gatesNum, DFEgates, gatesNum*sizeof(DFEgate_kernel_type));
        }
    }
    else {
        for (int idx=0; idx<(gateSetNum-1); idx++) {
           memcpy(DFEgates+(idx+1)*gatesNum, DFEgates, gatesNum*sizeof(DFEgate_kernel_type));
        }
    }

    gate_idx = 0;
    int gate_set_index = parameter_num-1;
    if (only_derivates) {
        adjust_parameters_for_derivation( DFEgates, gatesNum, gate_idx, gate_set_index );
    } 
    else {
        adjust_parameters_for_derivation( DFEgates+gatesNum, gatesNum, gate_idx, gate_set_index );
    }

/*
    for ( int idx=0; idx<gatesNum*(parameter_num+1); idx++ ) {

        std::cout << "target qubit: " << (int)DFEgates[idx].target_qbit << " control qubit: " << (int)DFEgates[idx].control_qbit << " theta: " << (int)DFEgates[idx].ThetaOver2 << " lambda: " << (int)DFEgates[idx].Lambda << std::endl; 
    }
*/
    return DFEgates;

}


/**
@brief Method to create random initial parameters for the optimization
@return 
*/
void Gates_block::adjust_parameters_for_derivation( DFEgate_kernel_type* DFEgates, const int gatesNum, int& gate_idx, int& gate_set_index) {

        int parameter_idx = parameter_num;
        //int gate_set_index = parameter_num-1;

        int32_t parameter_shift = (int32_t)(M_PI/2*(1<<25));

        for(int op_idx = gates.size()-1; op_idx>=0; op_idx--) { 

            Gate* gate = gates[op_idx];
//std::cout <<   gate_idx << " " <<   gate_set_index << " " << gate->get_type() << std::endl;        

            if (gate->get_type() == CNOT_OPERATION) {
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CZ_OPERATION) {            
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CH_OPERATION) {        	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == SYC_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: SYC_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == U3_OPERATION) {

                // definig the U3 parameters
                double varthetaOver2;
                double varphi;
                double varlambda;
		
                // get the inverse parameters of the U3 rotation

                U3* u3_gate = static_cast<U3*>(gate);

                if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_theta_parameter()) {		   
                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                    DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    parameter_idx                = parameter_idx - 1;

                }
                else if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_phi_parameter()) { // not checked
                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.Phi                  = DFEGate.Phi + parameter_shift;
                    DFEGate.metadata             = 3 + (1<<7); // The 0th and 1st element in kernel matrix should be zero for derivates and 3 = 0011, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    parameter_idx                = parameter_idx - 1;
                }
                else if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_lambda_parameter()) { // not checked
                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.Lambda               = DFEGate.Lambda + parameter_shift;
                    DFEGate.metadata             = 5 + (1<<7); // The 0st and 3nd element in kernel matrix should be zero for derivates and 5 = 0101, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    parameter_idx                = parameter_idx - 1;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_theta_parameter() && u3_gate->is_phi_parameter() ) { //not checked

                    DFEgate_kernel_type& DFEGate2= DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate2.Phi                 = DFEGate2.Phi + parameter_shift;
                    DFEGate2.metadata            = 3 + (1<<7); // The 0th and 1st element in kernel matrix should be zero for derivates and 3 = 0011, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                    DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;



                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_theta_parameter() && u3_gate->is_lambda_parameter() ) {
//////////////////////////////////////////////////////
                    DFEgate_kernel_type& DFEGate2= DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate2.Lambda              = DFEGate2.Lambda + parameter_shift;
                    DFEGate2.metadata            = 5 + (1<<7); // The 0st and 3nd element in kernel matrix should be zero for derivates and 5 = 0101, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                    DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;



                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_phi_parameter() && u3_gate->is_lambda_parameter() ) { // not checked

                    DFEgate_kernel_type& DFEGate2= DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate2.Lambda              = DFEGate2.Lambda + parameter_shift;
                    DFEGate2.metadata            = 5 + (1<<7); // The 0st and 3nd element in kernel matrix should be zero for derivates and 5 = 0101, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.Phi                  = DFEGate.Phi + parameter_shift;
                    DFEGate.metadata             = 3 + (1<<7); // The 0th and 1st element in kernel matrix should be zero for derivates and 3 = 0011, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;


                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 3)) {

                    DFEgate_kernel_type& DFEGate3= DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate3.Lambda              = DFEGate3.Lambda + parameter_shift;
                    DFEGate3.metadata            = 5 + (1<<7); // The 0st and 3nd element in kernel matrix should be zero for derivates and 5 = 0101, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    DFEgate_kernel_type& DFEGate2= DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate2.Phi                 = DFEGate2.Phi + parameter_shift;
                    DFEGate2.metadata            = 3 + (1<<7); // The 0th and 1st element in kernel matrix should be zero for derivates and 3 = 0011, plus the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;


                    DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                    DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                    DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                    gate_set_index               = gate_set_index - 1;

                    parameter_idx = parameter_idx - 3;
                }

                gate_idx = gate_idx + 1;

            }
            else if (gate->get_type() == RX_OPERATION) { // Did not cehcked

                DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                gate_set_index               = gate_set_index - 1;
	
                parameter_idx = parameter_idx - 1;    
	    	 
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == RY_OPERATION) { // Did not cehcked

                DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                gate_set_index               = gate_set_index - 1;

                parameter_idx = parameter_idx - 1;

    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CRY_OPERATION) {

                DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                DFEGate.metadata             = (1<<7); // the leading bit indicates that derivate is processed
                gate_set_index               = gate_set_index - 1;

                parameter_idx = parameter_idx - 1;
	    		                    
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == RZ_OPERATION) { // Did not cehcked

                DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                DFEGate.Phi                  = DFEGate.Phi + parameter_shift;
                DFEGate.metadata             = 3 + (1<<7); // The 0th and 1st element in kernel matrix should be zero for derivates and 3 = 0011, plus the leading bit indicates that derivate is processed
                gate_set_index               = gate_set_index - 1;

                parameter_idx = parameter_idx - 1;
	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == X_OPERATION) {

            }
            else if (gate->get_type() == Y_OPERATION) {

            }
            else if (gate->get_type() == Z_OPERATION) {

            }
            else if (gate->get_type() == SX_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: SX_gate not implemented");
                throw error;	    	
            }
            else if (gate->get_type() == BLOCK_OPERATION) {

                Gates_block* block_gate = static_cast<Gates_block*>(gate);
                block_gate->adjust_parameters_for_derivation( DFEgates, gatesNum, gate_idx, gate_set_index);         
                //gate_set_index               = gate_set_index - block_gate->get_parameter_num();

                parameter_idx = parameter_idx - block_gate->get_parameter_num();
            }
            else if (gate->get_type() == UN_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: UN_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == ON_OPERATION) {

                // THE LAST GATE IS A GENERAL GATE APPENDED IN THE BLOCK-WISE OPTIMISATION ROUTINE OF DECOMPOSITION_BASE
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: ON_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == COMPOSITE_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: Composite_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == GENERAL_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: general_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == ADAPTIVE_OPERATION) {

                DFEgate_kernel_type& DFEGate = DFEgates[gate_set_index*gatesNum + gate_idx];
                DFEGate.metadata              = (1<<7); // the leading bit indicates that derivate is processed
                DFEGate.ThetaOver2           = DFEGate.ThetaOver2 + parameter_shift;
                gate_set_index               = gate_set_index - 1;

                parameter_idx = parameter_idx - 1;
	    		                    
                gate_idx = gate_idx + 1;
            }
            else {
                std::string err("Gates_block::adjust_parameters_for_derivation: unimplemented gate"); 
                throw err;
            }

        }


}


/**
@brief Method to create random initial parameters for the optimization
@return 
*/
DFEgate_kernel_type* Gates_block::convert_to_DFE_gates( Matrix_real& parameters_mtx, int& gatesNum ) {

    int parameter_num = get_parameter_num();
    if ( parameter_num != parameters_mtx.size() ) {
        std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: wrong number of parameters");
        throw error;
    }


    gates_num gate_nums   = get_gate_nums();
    int gates_total_num   = gate_nums.total; 
    int chained_gates_num = get_chained_gates_num();
    int gate_padding      = chained_gates_num - (gates_total_num % chained_gates_num);
    gatesNum              = gates_total_num+gate_padding;



    DFEgate_kernel_type* DFEgates = new DFEgate_kernel_type[gates_total_num+gate_padding];
    
    int gate_idx = 0;
    convert_to_DFE_gates( parameters_mtx, DFEgates, gate_idx );


    // padding with identity gates
    for (int idx=gate_idx; idx<gatesNum; idx++ ){

        DFEgate_kernel_type& DFEGate = DFEgates[idx];

        
        DFEGate.target_qbit = 0;
        DFEGate.control_qbit = -1;
        DFEGate.gate_type = U3_OPERATION;
        DFEGate.ThetaOver2 = (int32_t)(0);
        DFEGate.Phi = (int32_t)(0);
        DFEGate.Lambda = (int32_t)(0); 
        DFEGate.metadata = 0;

    }
/*
    for ( int idx=0; idx<gatesNum; idx++ ) {

        std::cout << "target qubit: " << (int)DFEgates[idx].target_qbit << " control qubit: " << (int)DFEgates[idx].control_qbit << " gate type: " << (int)DFEgates[idx].gate_type << std::endl; 
    }
*/


    return DFEgates;

}



/**
@brief Method to create random initial parameters for the optimization
@return 
*/
void Gates_block::convert_to_DFE_gates( const Matrix_real& parameters_mtx, DFEgate_kernel_type* DFEgates, int& start_index ) {

   	
        int& gate_idx = start_index;
        int parameter_idx = parameter_num;
	double *parameters_data = parameters_mtx.get_data();
	//const_cast <Matrix_real&>(parameters);


        for(int op_idx = gates.size()-1; op_idx>=0; op_idx--) { 

            Gate* gate = gates[op_idx];
            DFEgate_kernel_type& DFEGate = DFEgates[gate_idx];

            if (gate->get_type() == CNOT_OPERATION) {
                CNOT* cnot_gate = static_cast<CNOT*>(gate);
                DFEGate.target_qbit = cnot_gate->get_target_qbit();
                DFEGate.control_qbit = cnot_gate->get_control_qbit();
                DFEGate.gate_type = CNOT_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(M_PI/2*(1<<25));
                DFEGate.Phi = (int32_t)(0);
                DFEGate.Lambda = (int32_t)(M_PI*(1<<25)); 
                DFEGate.metadata = 0;
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CZ_OPERATION) {
                CZ* cz_gate = static_cast<CZ*>(gate);    
                DFEGate.target_qbit = cz_gate->get_target_qbit();
                DFEGate.control_qbit = cz_gate->get_control_qbit();
                DFEGate.gate_type = CZ_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Phi = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Lambda = (int32_t)(M_PI*(1<<25));  // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!	
                DFEGate.metadata = 0;             
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CH_OPERATION) {
                CH* ch_gate = static_cast<CH*>(gate);   
                DFEGate.target_qbit = ch_gate->get_target_qbit();
                DFEGate.control_qbit = ch_gate->get_control_qbit();
                DFEGate.gate_type = CH_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(M_PI/4*(1<<25)); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Phi = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Lambda = (int32_t)(M_PI*(1<<25));  // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!	 
                DFEGate.metadata = 0;        	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == SYC_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: SYC_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == U3_OPERATION) {

                // definig the U3 parameters
                double varthetaOver2;
                double varphi;
                double varlambda;
		
                // get the inverse parameters of the U3 rotation

                U3* u3_gate = static_cast<U3*>(gate);

                if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_theta_parameter()) {
		   
                    varthetaOver2 = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    varphi = 0;
                    varlambda =0;
                    parameter_idx = parameter_idx - 1;

                }
                else if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_phi_parameter()) {
                    varthetaOver2 = 0;
                    varphi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    varlambda =0;
                    parameter_idx = parameter_idx - 1;
                }
                else if ((u3_gate->get_parameter_num() == 1) && u3_gate->is_lambda_parameter()) {
                    varthetaOver2 = 0;
                    varphi =  0;
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 1;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_theta_parameter() && u3_gate->is_phi_parameter() ) {
                    varthetaOver2 = std::fmod( parameters_data[parameter_idx-2], 2*M_PI);
                    varphi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    varlambda = 0;
                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_theta_parameter() && u3_gate->is_lambda_parameter() ) {
                    varthetaOver2 = std::fmod( parameters_data[parameter_idx-2], 2*M_PI);
                    varphi = 0;
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 2) && u3_gate->is_phi_parameter() && u3_gate->is_lambda_parameter() ) {
                    varthetaOver2 = 0;
                    varphi = std::fmod( parameters_data[parameter_idx-2], 2*M_PI);
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 2;
                }
                else if ((u3_gate->get_parameter_num() == 3)) {
                    varthetaOver2 = std::fmod( parameters_data[parameter_idx-3], 2*M_PI);
                    varphi = std::fmod( parameters_data[parameter_idx-2], 2*M_PI);
                    varlambda = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                    parameter_idx = parameter_idx - 3;
                }

                DFEGate.target_qbit = u3_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = U3_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(varthetaOver2*(1<<25)); 
                DFEGate.Phi = (int32_t)(varphi*(1<<25)); 
                DFEGate.Lambda = (int32_t)(varlambda*(1<<25));	
                DFEGate.metadata = 0;
                gate_idx = gate_idx + 1;

            }
            else if (gate->get_type() == RX_OPERATION) {
	        // definig the rotation parameter
                double varthetaOver2;
                // get the inverse parameters of the U3 rotation
                RX* rx_gate = static_cast<RX*>(gate);		
                varthetaOver2 = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;

                DFEGate.target_qbit = rx_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = RX_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(varthetaOver2*(1<<25)); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Phi = (int32_t)(-M_PI/2*(1<<25));  // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Lambda = (int32_t)(M_PI/2*(1<<25)); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!	
                DFEGate.metadata = 0;    
	    	 
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == RY_OPERATION) {
                // definig the rotation parameter
                double varthetaOver2;
                // get the inverse parameters of the U3 rotation
                RY* ry_gate = static_cast<RY*>(gate);
                varthetaOver2 = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;

                DFEGate.target_qbit = ry_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = RY_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(varthetaOver2*(1<<25)); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Phi = (int32_t)(0);  // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Lambda = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!	
                DFEGate.metadata = 0;
    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == CRY_OPERATION) {
                // definig the rotation parameter
                double Phi;
                // get the inverse parameters of the U3 rotation
                CRY* cry_gate = static_cast<CRY*>(gate);
                double varthetaOver2 = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;
                DFEGate.target_qbit = cry_gate->get_target_qbit();
                DFEGate.control_qbit = cry_gate->get_control_qbit();
                DFEGate.gate_type = CRY_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(varthetaOver2*(1<<25)); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Phi = (int32_t)(0);  // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Lambda = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.metadata = 0;
	    		                    
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == RZ_OPERATION) {
                // definig the rotation parameter
                double varphi;
                // get the inverse parameters of the U3 rotation
                RZ* rz_gate = static_cast<RZ*>(gate);
                varphi = std::fmod( parameters_data[parameter_idx-1], 2*M_PI);
                parameter_idx = parameter_idx - 1;

                DFEGate.target_qbit = rz_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = RZ_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Phi = (int32_t)(varphi*(1<<25));  // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.Lambda = (int32_t)(0); // TODO: check !!!!!!!!!!!!!!!!!!!!!!!!!!!
                DFEGate.metadata = 0;
	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == X_OPERATION) {
                // get the inverse parameters of the U3 rotation
                X* x_gate = static_cast<X*>(gate);

                DFEGate.target_qbit = x_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = X_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(M_PI/2*(1<<25));
                DFEGate.Phi = (int32_t)(0);
                DFEGate.Lambda = (int32_t)(M_PI*(1<<25)); 
                DFEGate.metadata = 0;
	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == Y_OPERATION) {
                // get the inverse parameters of the U3 rotation
                Y* y_gate = static_cast<Y*>(gate);

                DFEGate.target_qbit = y_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = Y_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(M_PI/2*(1<<25));
                DFEGate.Phi = (int32_t)(M_PI/2*(1<<25));
                DFEGate.Lambda = (int32_t)(M_PI/2*(1<<25)); 
                DFEGate.metadata = 0;
	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == Z_OPERATION) {
                // get the inverse parameters of the U3 rotation
                Z* z_gate = static_cast<Z*>(gate);

                DFEGate.target_qbit = z_gate->get_target_qbit();
                DFEGate.control_qbit = -1;
                DFEGate.gate_type = Z_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(0);
                DFEGate.Phi = (int32_t)(0);
                DFEGate.Lambda = (int32_t)(M_PI*(1<<25)); 
                DFEGate.metadata = 0;
	    	
                gate_idx = gate_idx + 1;
            }
            else if (gate->get_type() == SX_OPERATION) {
                // get the inverse parameters of the U3 rotation
                SX* sx_gate = static_cast<SX*>(gate);
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: SX_gate not implemented");
                throw error;	    	
            }
            else if (gate->get_type() == BLOCK_OPERATION) {
                Gates_block* block_gate = static_cast<Gates_block*>(gate);
                const Matrix_real parameters_layer_mtx(parameters_mtx.get_data() + parameter_idx - gate->get_parameter_num(), 1, gate->get_parameter_num() );
                block_gate->convert_to_DFE_gates( parameters_layer_mtx, DFEgates, gate_idx );
                parameter_idx = parameter_idx - block_gate->get_parameter_num();
            }
            else if (gate->get_type() == UN_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: UN_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == ON_OPERATION) {

                // THE LAST GATE IS A GENERAL GATE APPENDED IN THE BLOCK-WISE OPTIMISATION ROUTINE OF DECOMPOSITION_BASE
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: ON_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == COMPOSITE_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: Composite_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == GENERAL_OPERATION) {
                std::string error("N_Qubit_Decomposition_Base::convert_to_DFE_gates: general_gate not implemented");
                throw error;
            }
            else if (gate->get_type() == ADAPTIVE_OPERATION) {
                // definig the rotation parameter
                double Phi;
                // get the inverse parameters of the U3 rotation
                Adaptive* ad_gate = static_cast<Adaptive*>(gate);
                double varthetaOver2 = std::fmod( activation_function(parameters_data[parameter_idx-1], ad_gate->get_limit()), 2*M_PI);
                parameter_idx = parameter_idx - 1;
                DFEGate.target_qbit = ad_gate->get_target_qbit();
                DFEGate.control_qbit = ad_gate->get_control_qbit();
                DFEGate.gate_type = ADAPTIVE_OPERATION;
                DFEGate.ThetaOver2 = (int32_t)(varthetaOver2*(1<<25)); 
                DFEGate.Phi = (int32_t)(0);  
                DFEGate.Lambda = (int32_t)(0); 
                DFEGate.metadata = 0;
	    		                    
                gate_idx = gate_idx + 1;
            }
            else {
                std::string err("Gates_block::convert_to_DFE_gates: unimplemented gate"); 
                throw err;
            }

        }


    return;

}
#endif



/**
@brief ?????????
@return Return with ?????????
*/
void 
export_gate_list_to_binary(Matrix_real& parameters, Gates_block* gates_block, const std::string& filename, int verbosity) {

    std::stringstream sstream;
    sstream << "Exporting circuit into binary format. Filename: " << filename << std::endl;
    logging log;
    log.verbose = verbosity;
    log.print(sstream, 3);	

    FILE* pFile;
    const char* c_filename = filename.c_str();
    
    pFile = fopen(c_filename, "wb");
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    export_gate_list_to_binary( parameters, gates_block, pFile, verbosity );

    fclose(pFile);
    return;

}



/**
@brief ?????????
@return Return with ?????????
*/
void 
export_gate_list_to_binary(Matrix_real& parameters, Gates_block* gates_block, FILE* pFile, int verbosity) {

    int qbit_num = gates_block->get_qbit_num();   
    fwrite(&qbit_num, sizeof(int), 1, pFile);

    int parameter_num = gates_block->get_parameter_num();
    fwrite(&parameter_num, sizeof(int), 1, pFile);


    int gates_num = gates_block->get_gate_num();
    fwrite(&gates_num, sizeof(int), 1, pFile);



    std::vector<Gate*> gates = gates_block->get_gates();
    double* parameters_data = parameters.get_data();

    for ( std::vector<Gate*>::iterator it=gates.begin(); it != gates.end(); ++it ) {
        Gate* op = *it;

        gate_type gt_type = op->get_type();

        fwrite(&gt_type, sizeof(gate_type), 1, pFile);

        int parameter_num = op->get_parameter_num();

        if (gt_type == CNOT_OPERATION || gt_type == CZ_OPERATION || gt_type == CH_OPERATION || gt_type == SYC_OPERATION) {
            int target_qbit = op->get_target_qbit();
            int control_qbit = op->get_control_qbit();
            fwrite(&target_qbit, sizeof(int), 1, pFile);
            fwrite(&control_qbit, sizeof(int), 1, pFile);
        }
        else if (gt_type == U3_OPERATION) {
            int target_qbit = op->get_target_qbit();
            fwrite(&target_qbit, sizeof(int), 1, pFile);

            U3* u3_op = static_cast<U3*>( op );

            int theta_bool  = u3_op->is_theta_parameter();
            int phi_bool    = u3_op->is_phi_parameter();
            int lambda_bool = u3_op->is_lambda_parameter();

            fwrite(&theta_bool, sizeof(int), 1, pFile);
            fwrite(&phi_bool, sizeof(int), 1, pFile);
            fwrite(&lambda_bool, sizeof(int), 1, pFile);


            fwrite(parameters_data, sizeof(double), parameter_num, pFile);

            
        }
        else if (gt_type == RX_OPERATION || gt_type == RY_OPERATION || gt_type == RZ_OPERATION ) {
            int target_qbit = op->get_target_qbit();
            fwrite(&target_qbit, sizeof(int), 1, pFile);

            fwrite(parameters_data, sizeof(double), parameter_num, pFile);
        }
        else if (gt_type == CRY_OPERATION) {
            int target_qbit = op->get_target_qbit();
            int control_qbit = op->get_control_qbit();
            fwrite(&target_qbit, sizeof(int), 1, pFile);
            fwrite(&control_qbit, sizeof(int), 1, pFile);

            fwrite(parameters_data, sizeof(double), parameter_num, pFile);
        }
        
        else if (gt_type == X_OPERATION || gt_type == Y_OPERATION || gt_type == Z_OPERATION || gt_type == SX_OPERATION) {
            int target_qbit = op->get_target_qbit();
            fwrite(&target_qbit, sizeof(int), 1, pFile);
        }
        else if (gt_type == BLOCK_OPERATION) {
            Gates_block* block_op = static_cast<Gates_block*>( op );
            Matrix_real parameters_loc(parameters_data, 1, parameter_num);
            export_gate_list_to_binary( parameters_loc, block_op, pFile );

        }
        else if (gt_type == ADAPTIVE_OPERATION) {
            int target_qbit = op->get_target_qbit();
            int control_qbit = op->get_control_qbit();
            fwrite(&target_qbit, sizeof(int), 1, pFile);
            fwrite(&control_qbit, sizeof(int), 1, pFile);

            fwrite(parameters_data, sizeof(double), parameter_num, pFile);
        }
        else {
            std::string err("export_gate_list_to_binary: unimplemented gate"); 
            throw err;
        }


        parameters_data = parameters_data + parameter_num;

    }

}


/**
@brief ?????????
@return Return with ?????????
*/
Gates_block* import_gate_list_from_binary(Matrix_real& parameters, const std::string& filename, int verbosity) {

    std::stringstream sstream;
    sstream << "Importing quantum circuit from binary file " << filename << std::endl;	
    logging log;
    log.verbose = verbosity;
    log.print(sstream, 2);	

    FILE* pFile;
    const char* c_filename = filename.c_str();
    
    pFile = fopen(c_filename, "rb");
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    Gates_block* ret = import_gate_list_from_binary(parameters, pFile, verbosity);

    fclose(pFile);
    return ret;
}

/**
@brief ?????????
@return Return with ?????????
*/
Gates_block* import_gate_list_from_binary(Matrix_real& parameters, FILE* pFile, int verbosity) {

    std::stringstream sstream;

    int qbit_num;
    size_t fread_status;

    fread_status = fread(&qbit_num, sizeof(int), 1, pFile);
    sstream << "qbit_num: " << qbit_num << std::endl;
    Gates_block* gate_block = new Gates_block(qbit_num);

    int parameter_num;
    fread_status = fread(&parameter_num, sizeof(int), 1, pFile);
    sstream << "parameter_num: " << parameter_num << std::endl;
    parameters = Matrix_real(1, parameter_num);
    double* parameters_data = parameters.get_data();

    int gates_num;
    fread_status = fread(&gates_num, sizeof(int), 1, pFile);
    sstream << "gates_num: " << gates_num << std::endl;

    std::vector<int> gate_block_level_gates_num;
    std::vector<Gates_block*> gate_block_levels;
    gate_block_level_gates_num.push_back( gates_num );
    gate_block_levels.push_back(gate_block);
    int current_level = 0;

    

    int iter_max = 1e5;
    int iter = 0;
    while ( gate_block_level_gates_num[0] > 0 && iter < iter_max) {

        gate_type gt_type;
        fread_status = fread(&gt_type, sizeof(gate_type), 1, pFile);

        //std::cout << "gate type: " << gt_type << std::endl;

        if (gt_type == CNOT_OPERATION) {
            sstream << "importing CNOT gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int control_qbit;
            fread_status = fread(&control_qbit, sizeof(int), 1, pFile);
            sstream << "control_qbit: " << control_qbit << std::endl;

            gate_block_levels[current_level]->add_cnot_to_end(target_qbit, control_qbit);
            gate_block_level_gates_num[current_level]--;
        }
        else if (gt_type == CZ_OPERATION) {
            sstream << "importing CZ gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int control_qbit;
            fread_status = fread(&control_qbit, sizeof(int), 1, pFile);
            sstream << "control_qbit: " << control_qbit << std::endl;

            gate_block_levels[current_level]->add_cz_to_end(target_qbit, control_qbit);
            gate_block_level_gates_num[current_level]--;
        }
        else if (gt_type == CH_OPERATION) {
            sstream << "importing CH gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int control_qbit;
            fread_status = fread(&control_qbit, sizeof(int), 1, pFile);
            sstream << "control_qbit: " << control_qbit << std::endl;

            gate_block_levels[current_level]->add_ch_to_end(target_qbit, control_qbit);
            gate_block_level_gates_num[current_level]--;
        }
        else if (gt_type == SYC_OPERATION) {
            sstream << "importing SYCAMORE gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int control_qbit;
            fread_status = fread(&control_qbit, sizeof(int), 1, pFile);
            sstream << "control_qbit: " << control_qbit << std::endl;

            gate_block_levels[current_level]->add_syc_to_end(target_qbit, control_qbit);
            gate_block_level_gates_num[current_level]--;
        }
        else if (gt_type == U3_OPERATION) {
            sstream << "importing U3 gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int Theta;
            int Phi;
            int Lambda;

            fread_status = fread(&Theta, sizeof(int), 1, pFile);
            fread_status = fread(&Phi, sizeof(int), 1, pFile);
            fread_status = fread(&Lambda, sizeof(int), 1, pFile);

            int parameter_num = Theta + Phi + Lambda;
            fread_status = fread(parameters_data, sizeof(double), parameter_num, pFile);
            parameters_data = parameters_data + parameter_num;

            gate_block_levels[current_level]->add_u3_to_end(target_qbit, Theta, Phi, Lambda);
            gate_block_level_gates_num[current_level]--;
            
        }
        else if (gt_type == RX_OPERATION) {

            sstream << "importing RX gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            fread_status = fread(parameters_data, sizeof(double), 1, pFile);
            parameters_data++;

            gate_block_levels[current_level]->add_rx_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == RY_OPERATION) {

            sstream << "importing RY gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            fread_status = fread(parameters_data, sizeof(double), 1, pFile);
            parameters_data++;

            gate_block_levels[current_level]->add_ry_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == CRY_OPERATION) {

            sstream << "importing CRY gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int control_qbit;
            fread_status = fread(&control_qbit, sizeof(int), 1, pFile);
            sstream << "control_qbit: " << control_qbit << std::endl;

            fread_status = fread(parameters_data, sizeof(double), 1, pFile);
            parameters_data++;

            gate_block_levels[current_level]->add_cry_to_end(target_qbit, control_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == RZ_OPERATION) {

            sstream << "importing RZ gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            fread_status = fread(parameters_data, sizeof(double), 1, pFile);
            parameters_data++;

            gate_block_levels[current_level]->add_rz_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == X_OPERATION) {

            sstream << "importing X gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            gate_block_levels[current_level]->add_x_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == Y_OPERATION) {

            sstream << "importing Y gate" << std::endl;

            int target_qbit;
            fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            gate_block_levels[current_level]->add_y_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == Z_OPERATION) {

            sstream << "importing Z gate" << std::endl;

            int target_qbit;
            fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            gate_block_levels[current_level]->add_z_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == SX_OPERATION) {

            sstream << "importing SX gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            gate_block_levels[current_level]->add_sx_to_end(target_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else if (gt_type == BLOCK_OPERATION) {


            sstream << "******* importing gates block ********" << std::endl;

            int qbit_num_loc;
            fread_status = fread(&qbit_num_loc, sizeof(int), 1, pFile);
            //std::cout << "qbit_num_loc: " << qbit_num_loc << std::endl;
            Gates_block* gate_block_inner = new Gates_block(qbit_num_loc);

            int parameter_num_loc;
            fread_status = fread(&parameter_num_loc, sizeof(int), 1, pFile);
            //std::cout << "parameter_num_loc: " << parameter_num_loc << std::endl;
        

            int gates_num_loc;
            fread_status = fread(&gates_num_loc, sizeof(int), 1, pFile);
            //std::cout << "gates_num_loc: " << gates_num_loc << std::endl;
            
            gate_block_levels[ current_level ]->add_gate_to_end( static_cast<Gate*>(gate_block_inner) );
            gate_block_levels.push_back( gate_block_inner );
            gate_block_level_gates_num.push_back(gates_num_loc);
            current_level++;
        }
        else if (gt_type == ADAPTIVE_OPERATION) {

            sstream << "importing adaptive gate" << std::endl;

            int target_qbit;
            fread_status = fread(&target_qbit, sizeof(int), 1, pFile);
            sstream << "target_qbit: " << target_qbit << std::endl;

            int control_qbit;
            fread_status = fread(&control_qbit, sizeof(int), 1, pFile);
            sstream << "control_qbit: " << control_qbit << std::endl;

            fread_status = fread(parameters_data, sizeof(double), 1, pFile);
            parameters_data++;

            gate_block_levels[current_level]->add_adaptive_to_end(target_qbit, control_qbit);
            gate_block_level_gates_num[current_level]--;

        }
        else {
            std::string err("import_gate_list_from_binary: unimplemented gate"); 
            throw err;
        }


        if ( gate_block_level_gates_num[current_level] == 0 ) {
            gate_block_levels.pop_back();
            gate_block_level_gates_num.pop_back();
            current_level--;
            gate_block_level_gates_num[current_level]--;
            sstream << "finishing gates block" << std::endl;
        }


        iter++;
    }

    logging log;
    log.verbose = verbosity;
    log.print(sstream, 4);	

  
    if ( iter == iter_max ) {
        std::string error("Corrupted input file, reached end of the file before contructing the whole gate structure");
        throw error;
    }

    return gate_block;

}


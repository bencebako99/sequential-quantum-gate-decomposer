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
/*! \file U3.h
    \brief Header file for a class representing a U3 gate.
*/

#ifndef U3_H
#define U3_H

#include "Gate.h"
#include "matrix.h"
#include "matrix_real.h"
#include <math.h>


/**
@brief A class representing a U3 gate.
*/
class U3: public Gate {

protected:

   /// logical value indicating whether the matrix creation takes an argument theta
   bool theta;
   /// logical value indicating whether the matrix creation takes an argument phi
   bool phi;
   /// logical value indicating whether the matrix creation takes an argument lambda
   bool lambda;
   /// value applied for theta if it is not varied during the gate decomposition
   double theta0;
   /// value applied for phi if it is not varied during the gate decomposition
   double phi0;
   /// value applied for lambda if it is not varied during the gate decomposition
   double lambda0;
   /// Parameters theta, phi, lambda of the U3 gate after the decomposition of the unitary is done
   Matrix_real parameters;



public:

/**
@brief Nullary constructor of the class.
*/
U3();


/**
@brief Constructor of the class.
@param qbit_num_in The number of qubits spanning the gate.
@param target_qbit_in The 0<=ID<qbit_num of the target qubit.
@param theta_in logical value indicating whether the matrix creation takes an argument theta.
@param phi_in logical value indicating whether the matrix creation takes an argument phi
@param lambda_in logical value indicating whether the matrix creation takes an argument lambda
*/
U3(int qbit_num_in, int target_qbit_in, bool theta_in, bool phi_in, bool lambda_in);

/**
@brief Destructor of the class
*/
~U3();

/**
@brief Call to retrieve the gate matrix
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@return Returns with a matrix of the gate
*/
Matrix get_matrix( Matrix_real& parameters );

/**
@brief Call to apply the gate on the input array/matrix by U3*input
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
void apply_to_list( Matrix_real& parameters, std::vector<Matrix>& input );


/**
@brief Call to apply the gate on the input array/matrix by U3*input
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
virtual void apply_to( Matrix_real& parameters, Matrix& input );


/**
@brief ???????????????
*/
virtual std::vector<Matrix> apply_derivate_to( Matrix_real& parameters, Matrix& input );



/**
@brief Call to apply the gate on the input array/matrix by input*U3
@param parameters An array of parameters to calculate the matrix of the U3 gate.
@param input The input array on which the gate is applied
*/
virtual void apply_from_right( Matrix_real& parameters, Matrix& input );



/**
@brief Call to set the number of qubits spanning the matrix of the gate
@param qbit_num_in The number of qubits
*/
void set_qbit_num(int qbit_num_in);



/**
@brief Call to reorder the qubits in the matrix of the gate
@param qbit_list The reordered list of qubits spanning the matrix
*/
void reorder_qubits( std::vector<int> qbit_list);

/**
@brief Call to check whether theta is a free parameter of the gate
@return Returns with true if theta is a free parameter of the gate, or false otherwise.
*/
bool is_theta_parameter();

/**
@brief Call to check whether Phi is a free parameter of the gate
@return Returns with true if Phi is a free parameter of the gate, or false otherwise.
*/
bool is_phi_parameter();

/**
@brief Call to check whether Lambda is a free parameter of the gate
@return Returns with true if Lambda is a free parameter of the gate, or false otherwise.
*/
bool is_lambda_parameter();



/**
@brief Calculate the matrix of a U3 gate gate corresponding to the given parameters acting on a single qbit space.
@param Theta Real parameter standing for the parameter theta.
@param Phi Real parameter standing for the parameter phi.
@param Lambda Real parameter standing for the parameter lambda.
@return Returns with the matrix of the one-qubit matrix.
*/
Matrix calc_one_qubit_u3(double Theta, double Phi, double Lambda );


/**
@brief Call to create a clone of the present class
@return Return with a pointer pointing to the cloned object
*/
U3* clone();


/**
@brief Call to set the final optimized parameters of the gate.
@param Theta Real parameter standing for the parameter theta.
@param Phi Real parameter standing for the parameter phi.
@param Lambda Real parameter standing for the parameter lambda.
*/
void set_optimized_parameters(double Theta, double Phi, double Lambda );

/**
@brief Call to get the final optimized parameters of the gate.
@param parameters_in Preallocated pointer to store the parameters Theta, Phi and Lambda of the U3 gate.
*/
Matrix_real get_optimized_parameters();


/**
@brief Call to set the parameter theta0
@param theta_in The value for the parameter theta0
*/
void set_theta(double theta_in );

/**
@brief Call to set the parameter phi0
@param theta_in The value for the parameter theta0
*/
void set_phi(double phi_in );


/**
@brief Call to set the parameter lambda0
@param theta_in The value for the parameter theta0
*/
void set_lambda(double lambda_in );

};


#endif //U3


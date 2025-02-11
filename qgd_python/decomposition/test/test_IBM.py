# -*- coding: utf-8 -*-
"""
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
"""
## \file test_decomposition.py
## \brief Functionality test cases for the qgd_N_Qubit_Decomposition class.



# cerate unitary q-bit matrix
from scipy.stats import unitary_group
import numpy as np
from qgd_python.utils import get_unitary_from_qiskit_circuit


try:
    from mpi4py import MPI
    MPI_imported = True
except ModuleNotFoundError:
    MPI_imported = False



class Test_Decomposition:
    """This is a test class of the python iterface to the decompsition classes of the QGD package"""

    

    def test_IBM_Chellenge(self):
        r"""
        This method is called by pytest. 
        Test to decompose a 4-qubit unitary of the IBM chellenge

        """

        from qgd_python.decomposition.qgd_N_Qubit_Decomposition import qgd_N_Qubit_Decomposition        
        from scipy.io import loadmat
    
        # load the unitary from file
        data = loadmat('Umtx.mat')  
        # The unitary to be decomposed  
        Umtx = data['Umtx']
        

        # creating a class to decompose the unitary
        cDecompose = qgd_N_Qubit_Decomposition( Umtx.conj().T, optimize_layer_num=True, initial_guess="CLOSE_TO_ZERO" )
        

        # set the number of successive identical blocks in the optimalization of disentanglement of the n-th qubits
        cDecompose.set_Identical_Blocks( {4: 2, 3: 1} )

        # set the maximal number of layers in the decomposition
        cDecompose.set_Max_Layer_Num( {4: 9, 3:4} )

        # set the number of iteration loops in the decomposition
        cDecompose.set_Iteration_Loops({4: 3, 3: 3, 2: 3})

        # setting the verbosity of the decomposition
        #cDecompose.set_Verbose( True )

        # set the number of block to be optimized in one shot
        cDecompose.set_Optimization_Blocks( 20 )

        # starting the decomposition
        cDecompose.Start_Decomposition()

        # list the decomposing operations
        cDecompose.List_Gates()

        # get the decomposing operations
        quantum_circuit = cDecompose.get_Quantum_Circuit()

        # print the quantum circuit
        print(quantum_circuit)


        import numpy.linalg as LA
    
        # test the decomposition of the matrix
        # the unitary matrix from the result object
        decomposed_matrix = get_unitary_from_qiskit_circuit( quantum_circuit ) 
        product_matrix = np.dot(Umtx,decomposed_matrix.conj().T)
        phase = np.angle(product_matrix[0,0])
        product_matrix = product_matrix*np.exp(-1j*phase)
    
        product_matrix = np.eye(16)*2 - product_matrix - product_matrix.conj().T
        # the error of the decomposition
        decomposition_error = (np.real(np.trace(product_matrix)))/2
       
        print('The error of the decomposition is ' + str(decomposition_error))

        assert( decomposition_error < 1e-3 )



    def test_IBM_Chellenge_adaptive(self):
        r"""
        This method is called by pytest. 
        Test to decompose a 4-qubit unitary of the IBM chellenge

        """

        from qgd_python.decomposition.qgd_N_Qubit_Decomposition_adaptive import qgd_N_Qubit_Decomposition_adaptive       
        from scipy.io import loadmat
    
        # load the unitary from file
        data = loadmat('Umtx.mat')  
        # The unitary to be decomposed  
        Umtx = data['Umtx']
        

        # creating a class to decompose the unitary
        cDecompose = qgd_N_Qubit_Decomposition_adaptive( Umtx.conj().T, level_limit_max=5, level_limit_min=0 )


        # setting the verbosity of the decomposition
        cDecompose.set_Verbose( 3 )

        # starting the decomposition
        cDecompose.Start_Decomposition()

        # list the decomposing operations
        cDecompose.List_Gates()

        # get the decomposing operations
        quantum_circuit = cDecompose.get_Quantum_Circuit()

        # print the quantum circuit
        print(quantum_circuit)

        import numpy.linalg as LA
    
        # the unitary matrix from the result object
        decomposed_matrix = get_unitary_from_qiskit_circuit( quantum_circuit )
        product_matrix = np.dot(Umtx,decomposed_matrix.conj().T)
        phase = np.angle(product_matrix[0,0])
        product_matrix = product_matrix*np.exp(-1j*phase)
    
        product_matrix = np.eye(16)*2 - product_matrix - product_matrix.conj().T
        # the error of the decomposition
        decomposition_error = (np.real(np.trace(product_matrix)))/2
       
        print('The error of the decomposition is ' + str(decomposition_error))

        assert( decomposition_error < 1e-3 )





    def test_IBM_Chellenge_adaptive_rectangular_input(self):
        r"""
        This method is called by pytest. 
        Test to decompose a 4-qubit unitary of the IBM chellenge

        """

        from qgd_python.decomposition.qgd_N_Qubit_Decomposition_adaptive import qgd_N_Qubit_Decomposition_adaptive       
        from scipy.io import loadmat
    
        # load the unitary from file
        data = loadmat('Umtx.mat')  
        # The unitary to be decomposed  
        Umtx = data['Umtx']
        Umtx = Umtx[0:14,:]
        

        # creating a class to decompose the unitary
        cDecompose = qgd_N_Qubit_Decomposition_adaptive( Umtx.conj().T, level_limit_max=5, level_limit_min=0 )


        # setting the verbosity of the decomposition
        cDecompose.set_Verbose( 3 )

        # starting the decomposition
        cDecompose.Start_Decomposition()

        # list the decomposing operations
        cDecompose.List_Gates()

        # get the decomposing operations
        quantum_circuit = cDecompose.get_Quantum_Circuit()

        # print the quantum circuit
        print(quantum_circuit)

        import numpy.linalg as LA
    
        # the unitary matrix from the result object
        decomposed_matrix = get_unitary_from_qiskit_circuit( quantum_circuit )
        decomposed_matrix = decomposed_matrix[0:14,:]
        product_matrix = np.dot(Umtx,decomposed_matrix.conj().T)
        phase = np.angle(product_matrix[0,0])
        product_matrix = product_matrix*np.exp(-1j*phase)

        print( product_matrix.shape )
    
        product_matrix = np.eye(product_matrix.shape[0])*2 - product_matrix - product_matrix.conj().T
        # the error of the decomposition
        decomposition_error = (np.real(np.trace(product_matrix)))/2
       
        print('The error of the decomposition is ' + str(decomposition_error))

        assert( decomposition_error < 1e-3 )
        



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

    def test_N_Qubit_Decomposition_Heavy_Hex(self):
        r"""
        This method is called by pytest. 
        Test to define custom gate structure in the decomposition

        """

        from qgd_python.decomposition.qgd_N_Qubit_Decomposition import qgd_N_Qubit_Decomposition

        # the number of qubits spanning the unitary
        qbit_num = 4

        # determine the soze of the unitary to be decomposed
        matrix_size = int(2**qbit_num)
   
        # creating a random unitary to be decomposed
        Umtx = unitary_group.rvs(matrix_size)
    
        # creating an instance of the C++ class
        decomp = qgd_N_Qubit_Decomposition( Umtx.conj().T)


        # create custom gate structure
        gate_structure = { 4: self.create_custom_gate_structure_heavy_hex_4(4), 3: self.create_custom_gate_structure_heavy_hex_3(3)}        


        # adding custom gate structure to the decomposition
        decomp.set_Gate_Structure( gate_structure )


        # set the maximal number of layers in the decomposition
        decomp.set_Max_Layer_Num( {4: 60, 3:16} )

        # set the number of block to be optimized in one shot
        decomp.set_Optimization_Blocks( 20 )

        # starting the decomposition
        decomp.Start_Decomposition()   


        # list the decomposing operations
        decomp.List_Gates()

        # get the decomposing operations
        quantum_circuit = decomp.get_Quantum_Circuit()

        from qiskit import execute
        import numpy.linalg as LA
    
    
        # the unitary matrix from the result object
        decomposed_matrix = np.asarray( get_unitary_from_qiskit_circuit( quantum_circuit ) )
        product_matrix = np.dot(Umtx,decomposed_matrix.conj().T)
        phase = np.angle(product_matrix[0,0])
        product_matrix = product_matrix*np.exp(-1j*phase)
    
        product_matrix = np.eye(matrix_size)*2 - product_matrix - product_matrix.conj().T
        # the error of the decomposition
        decomposition_error =  (np.real(np.trace(product_matrix)))/2
       
        print('The error of the decomposition is ' + str(decomposition_error))

        assert( decomposition_error < 1e-3 )




    def create_custom_gate_structure_heavy_hex_3(self, qbit_num):
        r"""
        This method is called to create custom gate structure for the decomposition on IBM QX2

        """

        from qgd_python.gates.qgd_Gates_Block import qgd_Gates_Block

        # creating an instance of the wrapper class qgd_Gates_Block
        Gates_Block_ret = qgd_Gates_Block( qbit_num )

        disentangle_qubit = qbit_num - 1 

        

        for qbit in range(0, disentangle_qubit ):

            # creating an instance of the wrapper class qgd_Gates_Block
            Layer = qgd_Gates_Block( qbit_num )


            if qbit == 0:

                # add U3 fate to the block
                Theta = True
                Phi = False
                Lambda = True      
                Layer.add_U3( 0, Theta, Phi, Lambda )                 
                Layer.add_U3( disentangle_qubit, Theta, Phi, Lambda ) 

                # add CNOT gate to the block
                Layer.add_CNOT( 0, disentangle_qubit)

            elif qbit == 1:

                # add U3 fate to the block
                Theta = True
                Phi = False
                Lambda = True      
                Layer.add_U3( 0, Theta, Phi, Lambda )                 
                Layer.add_U3( 1, Theta, Phi, Lambda ) 

                # add CNOT gate to the block
                Layer.add_CNOT( 0, 1)



            elif qbit == 2:

                # add U3 fate to the block
                Theta = True
                Phi = False
                Lambda = True      
                Layer.add_U3( 2, Theta, Phi, Lambda )                 
                Layer.add_U3( disentangle_qubit, Theta, Phi, Lambda ) 

                # add CNOT gate to the block
                Layer.add_CNOT( disentangle_qubit, 2 )

         

            Gates_Block_ret.add_Gates_Block( Layer )

        return Gates_Block_ret





    def create_custom_gate_structure_heavy_hex_4(self, qbit_num):
        r"""
        This method is called to create custom gate structure for the decomposition on IBM QX2

        """

        from qgd_python.gates.qgd_Gates_Block import qgd_Gates_Block

        # creating an instance of the wrapper class qgd_Gates_Block
        Gates_Block_ret = qgd_Gates_Block( qbit_num )

        disentangle_qubit = qbit_num - 1 

        

        for qbit in range(0, disentangle_qubit ):

            # creating an instance of the wrapper class qgd_Gates_Block
            Layer = qgd_Gates_Block( qbit_num )


            if qbit == 0:

                # add U3 fate to the block
                Theta = True
                Phi = False
                Lambda = True      
                Layer.add_U3( 0, Theta, Phi, Lambda )                 
                Layer.add_U3( disentangle_qubit, Theta, Phi, Lambda ) 

                # add CNOT gate to the block
                Layer.add_CNOT( 0, disentangle_qubit)

            elif qbit == 1:

                # add U3 fate to the block
                Theta = True
                Phi = False
                Lambda = True      
                Layer.add_U3( 0, Theta, Phi, Lambda )                 
                Layer.add_U3( 1, Theta, Phi, Lambda ) 

                # add CNOT gate to the block
                Layer.add_CNOT( 0, 1)



            elif qbit == 2:

                # add U3 fate to the block
                Theta = True
                Phi = False
                Lambda = True      
                Layer.add_U3( 2, Theta, Phi, Lambda )                 
                Layer.add_U3( 0, Theta, Phi, Lambda ) 

                # add CNOT gate to the block
                Layer.add_CNOT( 0, 2)

         

            Gates_Block_ret.add_Gates_Block( Layer )

        return Gates_Block_ret
    

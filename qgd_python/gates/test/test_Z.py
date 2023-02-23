import numpy as np
import random

from qiskit import QuantumCircuit
from qiskit.visualization import plot_histogram

from qgd_python.utils import get_unitary_from_qiskit_circuit
from qgd_python.gates.qgd_Z import qgd_Z
from scipy.stats import unitary_group

class Test_operations_squander:
    """This is a test class of the python iterface to compare the SQUANDER and the qiskit decomposition"""

    pi=np.pi


    def test_Z_get_matrix(self):
        r"""
        This method is called by pytest. 
        Test to create an instance of U3 gate and compare with qiskit.
        """
        global Z_qiskit
        Z_qiskit = [0]*6

        for qbit_num in range(1,7):

            # target qbit
            target_qbit = qbit_num-1

            # creating an instance of the C++ class
            Z = qgd_Z( qbit_num, target_qbit )

	    #SQUANDER#

            # get the matrix                       
            Z_squander = Z.get_Matrix( )

	    #print(Z_squander)
      
	    #QISKIT

            # Create a Quantum Circuit acting on the q register
            circuit = QuantumCircuit(qbit_num)

            # Add the CNOT gate on control qbit and target qbit
            circuit.z( target_qbit )

            # the unitary matrix from the result object
            Z_qiskit[qbit_num-1] = get_unitary_from_qiskit_circuit( circuit )
            Z_qiskit[qbit_num-1] = np.asarray(Z_qiskit[qbit_num-1])

            # Draw the circuit        
            #print(Z_qiskit)
        
            #the difference between the SQUANDER and the qiskit result        
            delta_matrix=Z_squander-Z_qiskit[qbit_num-1]

            # compute norm of matrix
            error=np.linalg.norm(delta_matrix)

            print("Get_matrix: The difference between the SQUANDER and the qiskit result is: " , np.around(error,2))
            assert( error < 1e-3 ) 


    def test_Z_apply_to(self):
        r"""
        This method is called by pytest. 
        Test to create an instance of U3 gate and compare with qiskit.
        """
        for qbit_num in range(1,7):

            # target qbit
            target_qbit = qbit_num-1

            # creating an instance of the C++ class
            Z = qgd_Z( qbit_num, target_qbit )
        
            #create text matrix 
            test_m = unitary_group.rvs(((2**qbit_num)))           
            test_matrix = np.dot(test_m, test_m.conj().T)

	    #QISKIT      

            # apply the gate on the input array/matrix 
            Z_qiskit_apply_gate=np.matmul(Z_qiskit[qbit_num-1], test_matrix)

	    #SQUANDER

            Z_squander = test_matrix

            # apply the gate on the input array/matrix                
            Z.apply_to(Z_squander)       

            #the difference between the SQUANDER and the qiskit result        
            delta_matrix=Z_squander-Z_qiskit_apply_gate

            # compute norm of matrix
            error=np.linalg.norm(delta_matrix)

            print("Apply_to: The difference between the SQUANDER and the qiskit result is: " , np.around(error,2))
            assert( error < 1e-3 ) 



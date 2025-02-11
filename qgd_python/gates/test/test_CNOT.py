import numpy as np
import random

from qiskit import QuantumCircuit
from qiskit.visualization import plot_histogram

from qgd_python.utils import get_unitary_from_qiskit_circuit
from qgd_python.gates.qgd_CNOT import qgd_CNOT
from scipy.stats import unitary_group

class Test_operations_squander:
    """This is a test class of the python iterface to the gates of the QGD package"""


    def test_CNOT_get_matrix(self):
        r"""
        This method is called by pytest. 
        Test to create an instance of CNOT gate.
        """
        pi=np.pi

        for qbit_num in range(2,7):

            # target qbit
            target_qbit = qbit_num-2

            # control qbit
            control_qbit = qbit_num-1

            # creating an instance of the C++ class
            CNOT = qgd_CNOT( qbit_num, target_qbit, control_qbit )

	    #SQUANDER

            # get the matrix              
            CNOT_squander = CNOT.get_Matrix(  )

	    #QISKIT

            # Create a Quantum Circuit acting on the q register
            circuit = QuantumCircuit(qbit_num)

            # Add the CNOT gate on qubit pi, pi,
            circuit.cx(control_qbit, target_qbit)

            # the unitary matrix from the result object
            CNOT_qiskit = get_unitary_from_qiskit_circuit( circuit )
            CNOT_qiskit = np.asarray(CNOT_qiskit)
        
            #the difference between the SQUANDER and the qiskit result        
            delta_matrix=CNOT_squander-CNOT_qiskit

            # compute norm of matrix
            error=np.linalg.norm(delta_matrix)

            #print("Get_matrix: The difference between the SQUANDER and the qiskit result is: " , np.around(error,2))
            assert( error < 1e-3 )        
 
    def test_CNOT_apply_to(self):
        r"""
        This method is called by pytest. 
        Test to create an instance of U3 gate and compare with qiskit.
        """
        pi=np.pi

        for qbit_num in range(2,7):

            # target qbit
            target_qbit = qbit_num-2

            # control qbit
            control_qbit = qbit_num-1

            # creating an instance of the C++ class
            CNOT = qgd_CNOT( qbit_num, target_qbit, control_qbit )

            #create text matrix 
            test_matrix= np.identity( 2**qbit_num, dtype=complex )              

	    #QISKIT
      
            # Create a Quantum Circuit acting on the q register
            circuit = QuantumCircuit(qbit_num)

            # Add the CNOT gate on qubit pi, pi,
            circuit.cx(control_qbit, target_qbit)

            # the unitary matrix from the result object
            CNOT_qiskit = get_unitary_from_qiskit_circuit( circuit )
            CNOT_qiskit = np.asarray(CNOT_qiskit)

            # apply the gate on the input array/matrix 
            #CNOT_qiskit_apply_gate=np.matmul(CNOT_qiskit, test_matrix)

	    #SQUANDER

            CNOT_squander=test_matrix

            # apply the gate on the input array/matrix                
            CNOT.apply_to(CNOT_squander )

            #the difference between the SQUANDER and the qiskit result        
            delta_matrix=CNOT_squander-CNOT_qiskit

            # compute norm of matrix
            error=np.linalg.norm(delta_matrix)

            #print("Apply_to: The difference between the SQUANDER and the qiskit result is: " , np.around(error,2))
            assert( error < 1e-3 ) 


 


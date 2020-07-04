#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jun 29 17:23:58 2020

@author: rakytap
"""

def two_qubit_decomposition():
 
    from decomposition.Two_Qubit_Decomposition import Two_Qubit_Decomposition
    
    print('****************************************')
    print('Test of two qubit decomposition')
    print(' ')
    
    # cerate unitary q-bit matrix
    from scipy.stats import unitary_group
    
    # the number of qubits
    qbit_num = 2
    
    Umtx = unitary_group.rvs(int(2**qbit_num))
    print('The test matrix to be decomposed is:')
    print(Umtx)
    print(' ')

    cDecomposition = Two_Qubit_Decomposition( Umtx )
    
    #start the decomposition
    cDecomposition.start_decomposition()
    
    print('')
    print('The matrix can be decomposed into operations:')
    print(' ')
    cDecomposition.list_operation_inverses()
    
    print(' ')
    print('Constructing quantum circuit:')
    print(' ')
    quantum_circuit = cDecomposition.get_quantum_circuit()
    
    print(quantum_circuit)
    
    #quantum_circuit.draw()
    
    
    
    
def three_qubit_decomposition():
 
    from decomposition.N_Qubit_Decomposition import N_Qubit_Decomposition
    
    print('****************************************')
    print('Test of three-qubit decomposition')
    print(' ')
    
    # cerate unitary q-bit matrix
    from scipy.stats import unitary_group
    
    # the number of qubits
    qbit_num = 3
    
    Umtx = unitary_group.rvs(int(2**qbit_num))
    print('The test matrix to be decomposed is:')
    print(Umtx)
    print(' ')
    #print(np.dot(Umtx, Umtx.conj().T))

    cDecomposition = N_Qubit_Decomposition( Umtx, optimize_layer_num=False )
    
    #start the decomposition
    cDecomposition.start_decomposition()
    
    #print(' ')
    #print('The matrix can be decomposed into operations:')
    #print(' ')
    #cDecomposition.list_operation_inverses()    
    
    
    
def four_qubit_decomposition():
 
    from decomposition.N_Qubit_Decomposition import N_Qubit_Decomposition
    
    print('****************************************')
    print('Test of four-qubit decomposition')
    print(' ')
    
    # cerate unitary q-bit matrix
    from scipy.stats import unitary_group
    
    # the number of qubits
    qbit_num = 4
    
    Umtx = unitary_group.rvs(int(2**qbit_num))
    print('The test matrix to be decomposed is:')
    print(Umtx)
    print(' ')

    cDecomposition = N_Qubit_Decomposition( Umtx, parallel = True )
    
    # Maximal number of iteartions in the optimalization process
    cDecomposition.set_max_iteration( int(1e6) )
    
    #start the decomposition
    cDecomposition.start_decomposition()
        
    #print(' ')
    #print('The matrix can be decomposed into operations:')
    #print(' ')
    #cDecomposition.list_operation_inverses() 




def IBM_challenge_decomposition():
 
    from decomposition.N_Qubit_Decomposition import N_Qubit_Decomposition
    
    print('****************************************')
    print('Test of four-qubit decomposition')
    print(' ')
    
    # cerate unitary q-bit matrix
    import numpy as np
    
    # The unitary given in the 4th problem of IBM chellenge 2020
    Umtx = [[complex(-0.21338835,0.33838835), complex(-0.14016504,-0.08838835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835)],
    [complex(-0.14016504,-0.08838835), complex(-0.21338835,0.33838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165)],
    [complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504)],
    [complex(0.03661165,0.08838835),  complex(0.21338835,-0.08838835), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165)],
    [complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165)],
    [complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835)],
    [complex(-0.08838835,0.14016504),  complex(0.33838835, 0.21338835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835)],
    [complex(0.33838835,0.21338835), complex(-0.08838835, 0.14016504), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835)],
    [complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835)],
    [complex(0.03661165,0.08838835),  complex(0.21338835,-0.08838835), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504)],
    [complex(0.39016504,0.08838835), complex(-0.03661165, 0.16161165),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835)],
    [complex(-0.03661165,0.16161165),  complex(0.39016504, 0.08838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165)],
    [complex(0.16161165,0.03661165),  complex(0.08838835,-0.39016504),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835)],
    [complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165), complex(-0.08838835-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835)],
    [complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.16161165, 0.03661165),  complex(0.08838835,-0.39016504),  complex(0.39016504, 0.08838835), complex(-0.03661165, 0.16161165),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835), complex(-0.08838835, 0.14016504),  complex(0.33838835, 0.21338835),  complex(0.08838835,-0.03661165), complex(-0.08838835,-0.21338835),  complex(0.21338835,-0.08838835),  complex(0.03661165, 0.08838835), complex(-0.21338835, 0.33838835), complex(-0.14016504,-0.08838835)],
    [complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.08838835,-0.39016504),  complex(0.16161165, 0.03661165), complex(-0.03661165, 0.16161165),  complex(0.39016504, 0.08838835),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835),  complex(0.33838835, 0.21338835), complex(-0.08838835, 0.14016504), complex(-0.08838835,-0.21338835),  complex(0.08838835,-0.03661165),  complex(0.03661165, 0.08838835),  complex(0.21338835,-0.08838835), complex(-0.14016504,-0.08838835), complex(-0.21338835, 0.33838835)]]
    
    print('The test matrix to be decomposed is:')
    print(Umtx)
    print(' ')
    print(np.dot(Umtx, Umtx.conj().T))

    cDecomposition = N_Qubit_Decomposition( Umtx )
    
    #start the decomposition
    cDecomposition.start_decomposition()
    
    # finalize the decomposition
    cDecomposition.finalize_decomposition()
    
    print(' ')
    print('The matrix can be decomposed into operations:')
    print(' ')
    cDecomposition.list_operation_inverses()        
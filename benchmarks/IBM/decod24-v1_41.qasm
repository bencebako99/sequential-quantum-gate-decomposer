OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
u(-pi,0.33084507,2.1518066) q[3];
u(pi/2,-pi/2,2.8080795) q[1];
cz q[3],q[1];
u(pi/2,-1.3545772,pi/2) q[1];
u(0.39268345,-3.1415762,-1.5708153) q[0];
cx q[1],q[0];
u(-7*pi/8,0,0) q[0];
cx q[1],q[0];
u(-2.1862759,-4.4976461,-0.083348027) q[3];
u(pi/2,pi/2,-11*pi/8) q[2];
cx q[3],q[2];
u(2*pi/3,0,0) q[2];
cx q[3],q[2];
u(-7.0398844e-08,3.1802665,1.9097747) q[2];
u(1.5708049,-1.7969342,-3*pi/2) q[0];
cz q[2],q[0];
u(4.0969095,-2.4133136,5.5448435) q[3];
u(pi,-3*pi/2,1.1817266) q[1];
cz q[3],q[1];
u(-7*pi/4,2.3858453,4.7695079) q[3];
u(0,0,0) q[0];
cz q[3],q[0];
u(-2.1006991,-3.9190577,-3.9566416) q[3];
u(7*pi/8,-pi,-1.9484491) q[2];
cx q[3],q[2];
u(-0.85888574,0,0) q[2];
cx q[3],q[2];
u(-pi,1.0191983,-0.63323225) q[2];
u(pi,-0.99522342,-8.1092102e-09) q[1];
cz q[2],q[1];
u(5.0778787,0.66645086,3.9190579) q[3];
u(3*pi/8,pi,-1.6524305) q[2];
cx q[3],q[2];
u(-5*pi/8,0,0) q[2];
cx q[3],q[2];
u(2.559678e-08,-4.8707184,2.3061418) q[2];
u(5.9604645e-08,1.0633997,0.90553087) q[0];
cz q[2],q[0];
u(pi,3.7839014,-6.0051295) q[3];
u(pi/2,pi/2,1.5607731) q[1];
cz q[3],q[1];
u(pi/2,-0.27269774,1*pi/2) q[1];
u(1.5708005,1*pi/2,3.9846182) q[0];
cx q[1],q[0];
u(9*pi/8,0,0) q[0];
cx q[1],q[0];
u(5*pi/8,-0.43005447,2.5645768) q[2];
u(0,0,-1.6316117) q[1];
cz q[2],q[1];
u(0,0,0.62164762) q[3];
u(2.126555,-3.1415883,1.7657747e-06) q[0];
cx q[3],q[0];
u(pi/8,0,0) q[0];
cx q[3],q[0];
u(pi,0,-1.5707981) q[0];
u(pi,-pi,pi/2) q[1];
u(pi/2,0,-4.2823345) q[2];
u(0,0,5.5118506) q[3];

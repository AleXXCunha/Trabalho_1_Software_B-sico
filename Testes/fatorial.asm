MUL_N: MACRO 
MULT N
STORE N
ENDMACRO
INPUT N
LOAD N
FAT: SUB ONE
JMPZ FIM
STORE AUX
MUL_N 
LOAD AUX
JMP FAT
FIM: OUTPUT N
STOP
AUX: SPACE
N: SPACE
ONE: CONST 1
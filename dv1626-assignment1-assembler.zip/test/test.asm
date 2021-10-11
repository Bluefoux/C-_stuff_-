MOV R1 #1
MOV R2 R0

arithmetic:
ADD R1 R1 R2
ADD R3 R1 #4
ADD R3 #-5 #4
ADD R0 #131 R2
SUB R1 R1 R2
SUB R3 R1 #4
SUB R3 #-5 #4
SUB R0 #131 R2

conditional:
CMP R0 R1
CMP R3 #6
JEQ conditional
JGT arithmetic
JLT conditional
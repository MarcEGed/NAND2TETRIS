// Initialize stack pointer to 0
@10      // Load constant 5
D=A     // D = 5
@SP     // Address of SP
M=D     // SP = 5
@8
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
D=M-D
@TRUE_0
D;JLT
D=0
@END_0
0;JMP
(TRUE_0)
D=-1
(END_0)
@SP
A=M
M=D
@SP
M=M+1

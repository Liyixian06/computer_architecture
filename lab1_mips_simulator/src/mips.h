// MIPS-32 instructions

//opcode
#define OP_J 0x02 //op=000010
#define OP_JAL 0x03
#define OP_BEQ 0x04
#define OP_BNE 0x05
#define OP_BLEZ 0x06
#define OP_BGTZ 0x07
#define OP_ADDI 0x08
#define OP_ADDIU 0x09
#define OP_SLTI 0x0a
#define OP_SLTIU 0x0b
#define OP_ANDI 0x0c
#define OP_ORI 0x0d
#define OP_XORI 0x0e
#define OP_LUI 0x0f
#define OP_LB 0x20
#define OP_LH 0x21
#define OP_LW 0x23
#define OP_LBU 0x24
#define OP_LHU 0x25
#define OP_SB 0x28
#define OP_SH 0x29
#define OP_SW 0x2b

//regimm rt(opcode=1)
#define OP_REGIMM 0x01
#define RT_BLTZ 0x00
#define RT_BGEZ 0x1 //rt=00001
#define RT_BLTZAL 0x10
#define RT_BGEZAL 0x11

//special funct (opcode=0)
#define OP_SPECIAL 0x00
#define FC_SLL 0x0
#define FC_SRL 0x2
#define FC_SRA 0x3
#define FC_SLLV 0x4
#define FC_SRLV 0x6
#define FC_SRAV 0x7
#define FC_JR 0x8
#define FC_JALR 0x9
#define FC_MFHI 0x10
#define FC_MTHI 0x11
#define FC_MFLO 0x12
#define FC_MTLO 0x13
#define FC_SYSCALL 0xc
#define FC_MULT 0x18
#define FC_MULTU 0x19
#define FC_DIV 0x1a
#define FC_DIVU 0x1b
#define FC_ADD 0x20 //funct=100000
#define FC_ADDU 0x21
#define FC_SUB 0x22
#define FC_SUBU 0x23
#define FC_AND 0x24
#define FC_OR 0x25
#define FC_XOR 0x26
#define FC_NOR 0x27
#define FC_SLT 0x2a
#define FC_SLTU 0x2b
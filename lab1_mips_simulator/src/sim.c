#include <stdio.h>
#include "shell.h"
#include "mips.h"

uint32_t inst;
uint32_t op, rs, rt, rd, shamt, funct, imm, target;
int sign_imm;
uint64_t prod;
uint32_t sign_extend_hex(uint16_t n){
    return (n & 0x8000)? (n|0xffff8000):n;
}
uint32_t sign_extend_b(uint8_t n){
    return (n & 0x80)? (n|0xffffff80):n;
}
int sign_extend_18b(){
    return (sign_imm<<2);
}
uint32_t zero_extend_hex(uint16_t n){
    return ((uint32_t)n);
}
uint32_t zero_extend_b(uint8_t n){
    return ((uint32_t)n);
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    
    //define variables
    inst = mem_read_32(CURRENT_STATE.PC);
    op = (inst >> 26) & 0x3F; //6-bit
    rs = (inst >> 21) & 0x1F; //5-bit
    rt = (inst >> 16) & 0x1F;
    rd = (inst >> 11) & 0x1F;
    shamt = (inst >> 6) & 0x1F;
    funct = (inst >> 0) & 0x3F;
    imm = (inst >> 0) & 0xFFFF; //16-bit
    sign_imm = sign_extend_hex(imm);
    target = (inst >> 0) & ((1UL << 26) - 1);

    switch(op){
        case OP_SPECIAL:
            switch(funct){
                //shift left logical
                case FC_SLL:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //shift right logical
                case FC_SRL:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //shift right arithmetic
                case FC_SRA:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //shift left logical variable
                case FC_SLLV:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //shift right logical variable
                case FC_SRLV:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //shift left arithmetic variable
                case FC_SRAV:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //jump register
                case FC_JR:
                    if(!(CURRENT_STATE.REGS[rs] & 0x03))
                        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    else{
                        printf("address is not word-aligned!\n");
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;
                
                //jump and link register
                case FC_JALR:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 8;
                    if(!(CURRENT_STATE.REGS[rs] & 0x03))
                        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    else{
                        printf("address is not word-aligned!\n");
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;

                //move from HI
                case FC_MFHI:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //move to HI
                case FC_MTHI:
                    if(rd!=0)
                        NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //move from LO
                case FC_MFLO:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //move to LO
                case FC_MTLO:
                    if(rd!=0)
                        NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //system call
                case FC_SYSCALL:
                    printf("SYSCALL\n");
                    RUN_BIT = 0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //multiply
                case FC_MULT:
                    prod = (int)CURRENT_STATE.REGS[rs] * (int)CURRENT_STATE.REGS[rt];
                    NEXT_STATE.HI = (prod >> 31) & 0xFFFFFFFF;
                    NEXT_STATE.LO = prod & 0xFFFFFFFF;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //multiply unsigned
                case FC_MULTU:
                    prod = CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt];
                    NEXT_STATE.HI = (prod >> 31) & 0xFFFFFFFF;
                    NEXT_STATE.LO = prod & 0xFFFFFFFF;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //divide
                case FC_DIV:
                    if(rt!=0){
                        NEXT_STATE.HI = (int)CURRENT_STATE.REGS[rs] % (int)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = (int)CURRENT_STATE.REGS[rs] / (int)CURRENT_STATE.REGS[rt];
                    }
                    else printf("divide by zero!\n");
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //divide unsigned
                case FC_DIVU:
                    if(rt!=0){
                        NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
                    }
                    else printf("divide by zero!\n");
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //add
                case FC_ADD:
                //add unsigned
                case FC_ADDU:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //substract
                case FC_SUB:
                //substract unsigned
                case FC_SUBU:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                break;

                //and
                case FC_AND:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //or
                case FC_OR:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //xor
                case FC_XOR:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                //nor
                case FC_NOR:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = ~CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //set on less than
                case FC_SLT:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = ((int)CURRENT_STATE.REGS[rs] < (int)CURRENT_STATE.REGS[rt])? 1:0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //set on less than unsigned
                case FC_SLTU:
                    if(rd!=0)
                        NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])? 1:0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                default:
                    printf("unknown special instruction!\n");
                    break;
            }
            break;
        
        //special branches
        case OP_REGIMM:
            switch(rt)
            {
                //branch on less than zero
                case RT_BLTZ:
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    if((int)CURRENT_STATE.REGS[rs] < 0)
                        NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
                    break;
                
                //branch on greater than or equal to zero
                case RT_BGEZ:
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    if((int)CURRENT_STATE.REGS[rs] >= 0)
                        NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
                    break;
                
                //branch on less than zero and link
                case RT_BLTZAL:
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    if((int)CURRENT_STATE.REGS[rs] < 0)
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + sign_extend_18b();
                    else
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                //branch on greater than or equal to zero and link
                case RT_BGEZAL:
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    if((int)CURRENT_STATE.REGS[rs] >= 0)
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + sign_extend_18b();
                    else
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                default:
                    printf("unknown regimm instruction!\n");
                    break;
            }
            break;

        //jump
        case OP_J:
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target<<2);
            break;
        
        //jump and link
        case OP_JAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target<<2);
            break;

        //branch if equal
        case OP_BEQ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if(CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
                NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
            break;
        
        //branch if not equal
        case OP_BNE:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if(CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
                NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
            break;

        //branch if less than or equal to zero
        case OP_BLEZ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if(CURRENT_STATE.REGS[rs] <= 0)
                NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
            break;

        //branch if greater than zero
        case OP_BGTZ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if(CURRENT_STATE.REGS[rs] > 0)
                NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
            break;

        //add immediate
        case OP_ADDI:
        //add immediate unsigned
        case OP_ADDIU:
            if(rt!=0)
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + sign_imm;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //set on less than immediate
        case OP_SLTI:
            NEXT_STATE.REGS[rt] = (((int)CURRENT_STATE.REGS[rs] < (int)sign_imm)? 1:0);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //set on less than immediate unsigned
        case OP_SLTIU:
            NEXT_STATE.REGS[rt] = ((CURRENT_STATE.REGS[rs] < sign_imm)? 1:0);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //and with immediate
        case OP_ANDI:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & zero_extend_hex(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        
        //or with immediate
        case OP_ORI:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | zero_extend_hex(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        
        //xor with immediate
        case OP_XORI:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ zero_extend_hex(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        
        //load upper immediate
        case OP_LUI:
            if(rs==0)
                NEXT_STATE.REGS[rt] = (imm << 16);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        
        //load byte
        case OP_LB:
            NEXT_STATE.REGS[rt] = sign_extend_b((mem_read_32(CURRENT_STATE.REGS[rs] + sign_imm)) & 0xFF);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //load halfword
        case OP_LH:
            NEXT_STATE.REGS[rt] = sign_extend_hex((mem_read_32(CURRENT_STATE.REGS[rs] + sign_imm)) & 0xFFFF);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //load word
        case OP_LW:
            NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + sign_imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //load byte unsigned
        case OP_LBU:
            NEXT_STATE.REGS[rt] = zero_extend_b((mem_read_32(CURRENT_STATE.REGS[rs] + sign_imm)) & 0xFF);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //load halfword unsigned
        case OP_LHU:
            NEXT_STATE.REGS[rt] = zero_extend_hex((mem_read_32(CURRENT_STATE.REGS[rs] + sign_imm)) & 0xFFFF);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //store byte
        case OP_SB:
            mem_write_32((CURRENT_STATE.REGS[rs] + sign_imm), (int)(CURRENT_STATE.REGS[rt] & 0xFF));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //store halfword
        case OP_SH:
            mem_write_32((CURRENT_STATE.REGS[rs] + sign_imm), (int)(CURRENT_STATE.REGS[rt] & 0xFFFF));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //store word
        case OP_SW:
            mem_write_32((CURRENT_STATE.REGS[rs] + sign_imm), (int)CURRENT_STATE.REGS[rt]);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        default:
            printf("unknown instruction!\n");
            break;
    }
}

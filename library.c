//
// Created by Alex Dasneves on 9/8/22.
//

// This will be the main file for the simulator

#pragma region Includes
#include <stdio.h>
#include <stdbool.h>
#pragma endregion

#pragma region Defines
// Define the maximum size of the memory
#define MAX_MEMORY_SIZE 0x80000000

// Define the maximum value of the program counter
#define MAX_PC_VAL 0xFFFF

// Define special locations in memory
#define IO_A 0xFFF0
#define KEYBOARD IO_A
#define IO_B 0xFFF1
#define LCD IO_B
#define IO_C 0xFFF2
#define IO_D 0xFFF3
#define IO_E 0xFFF4
#define IO_F 0xFFF5
#define IO_G 0xFFF6
#define IO_H 0xFFF7
#define VID_OUT 0xFFF8 // Video Output will now be handled by writing to this location, rather than indexing within memory.

#define VID_MODE 0xFFF9
#define MEMPAGEL 0xFFFA
#define MEMPAGEH 0xFFFB

#define REGA registers.A
#define REGB registers.B
#define REGX registers.X
#define REGY registers.Y
#define FLAGS registers.F
#define STCKPTR registers.SP
#define INSTR registers.IR
#define PROGCNTR registers.PC
#define STACKBASE 0x7F00

#pragma endregion

#pragma region Structs
// Define the registers
struct Registers{
    unsigned char* A;
    unsigned char* B;
    unsigned char* X;
    unsigned char* Y;
    unsigned char* F;
    unsigned char* SP;
    unsigned char* IR;
    unsigned short * PC;
};

struct Memory{
    unsigned char* RAM[0x8000];
    unsigned char* ROM[0xFF][0xFF][0x8000];
};
#pragma endregion

#pragma region Global Variables

unsigned char* memory[0xFF][0xFF][0xFFFF];
unsigned short PC = 0x0000;

struct Registers registers;
struct Memory mem;


unsigned char mpL = 0x00;
unsigned char mpH = 0x00;

bool debug = false;


#pragma endregion

#pragma region Function Prototypes
int main(int argc, char *argv[]);
void readFile(char *argv[]);
void doInstruction();
void readMemIncPC();
unsigned char ReadNext();
void push(unsigned char* val);
void pop(unsigned char* val);
void top(unsigned char* val);
unsigned char Read(unsigned short addr);
void write(unsigned char* low, unsigned char* high, unsigned char* val);
#pragma endregion

#pragma region Main Function
int main(int argc, char* argv[]) {
    // Check if the user has specified Debug Mode
    if(argc == 2){
        if(*argv[0] == 'd'){
            debug = true;
        }
    }


    // Check if the user has provided a file to run, if not, exit
    if(argc > 1){
        readFile(argv);
    }
    else{
        printf("No file specified!");
        return 0;
    }

    /*while(!halted && (reg->PC.get() <= 65535)) {
        doInstruction();
        if (debug) {
            printDebug();
        }
    }*/

}
#pragma endregion


void readFile(char *argv[]){
    FILE* f=fopen(argv[1],"r");

    int i=0;
    //Fill memory with 0x00 (NOP)
    for(i=0; i<0xFFFF; i++)
    {
        memory[mpH][mpL][i] = 0x00;
    }
    i=0x8000;

    unsigned char memLow, memHigh;

    memLow = memHigh = 0;

    //Read file into memory
    for(memHigh = 0; memHigh < 0xFF; memHigh++){
        for(memLow = 0; memLow < 0xFF; memLow++){
            for(i = 0; i < 0xFFFF; i++){
                int c;
                fscanf(f,"%x\n",&c);
                *memory[memHigh][memLow][i] = c;
            }
        }
    }

    fclose(f);
}

void doInstruction(){
    //Fetch
    readMemIncPC();

    //Decode/Execute
    unsigned char opcode = *INSTR;



    switch(opcode){
        case 0x00:
            //NOP
            break;
        case 0x01:
            //RST
            break;
        case 0x02:
            //HLT
            break;
        case 0x03:
            //BRK
            break;
        case 0x04:
            //CTN
            break;
        case 0x05:
            //CLC
            (*FLAGS) &= 0b11111011;
            break;
        case 0x06:
            //CLN
            (*FLAGS) &= 0b11111101;
            break;
        case 0x07:
            //CLZ
            (*FLAGS) &= 0b11111110;
            break;
        case 0x08:
            //CLV
            (*FLAGS) &= 0b11110111;
            break;
        case 0x10:
            //ROL A
            (*REGA) = (*REGA) >> 1 | (((*REGA) << 7) & 0x80);
            break;
        case 0x11:
            //ROR A
            (*REGA) = (*REGA) << 1 | (*REGA) >> 7;
            break;
        case 0x12:
            //ADD A IMM
            (*REGA) = (*REGA) + ReadNext();
            break;
        case 0x13:
            //SUB A IMM
            (*REGA) = (*REGA) - ReadNext();
            break;
        case 0x14:
            //ROL B
            (*REGB) = (*REGB) << 1 | (*REGB) >> 7;
            break;
        case 0x15:
            //ROR B
            (*REGB) = (*REGB) >> 1 | (*REGB) << 7;
            break;
        case 0x16:
            //ADD B IMM
            (*REGB) = (*REGB) + ReadNext();
            break;
        case 0x17:
            //SUB B IMM
            (*REGB) = (*REGB) - ReadNext();
            break;
        case 0x18:
            //ROL X
            (*REGX) = (*REGX) << 1 | (*REGX) >> 7;
            break;
        case 0x19:
            //ROR X
            (*REGX) = (*REGX) >> 1 | (*REGX) << 7;
            break;
        case 0x1A:
            //ADD X IMM
            (*REGX) = (*REGX) + ReadNext();
            break;
        case 0x1B:
            (*REGX) = (*REGX) - ReadNext();
            break;
        case 0x1C:
            (*REGY) = (*REGY) << 1 | (*REGY) >> 7;
            break;
        case 0x1D:
            (*REGY) = (*REGY) >> 1 | (*REGY) << 7;
            break;
        case 0x1E:
            (*REGY) = (*REGY) + ReadNext();
            break;
        case 0x1F:
            (*REGY) = (*REGY) - ReadNext();
            break;
        case 0x20:
            (*REGA)++;
            break;
        case 0x21:
            (*REGA)--;
            break;
        case 0x24:
            (*REGB)++;
            break;
        case 0x25:
            (*REGB)--;
            break;
        case 0x28:
            (*REGX)++;
            break;
        case 0x29:
            (*REGX)--;
            break;
        case 0x2C:
            (*REGY)++;
            break;
        case 0x2D:
            (*REGY)--;
            break;
        case 0x30:
            pop(REGA);
            break;
        case 0x31:
            top(REGA);
            break;
        case 0x34:
            pop(REGB);
            break;
        case 0x35:
            top(REGB);
            break;
        case 0x38:
            pop(REGX);
            break;
        case 0x39:
            top(REGX);
            break;
        case 0x3C:
            pop(REGY);
            break;
        case 0x3D:
            top(REGY);
            break;
        case 0x40:
            write(REGX, REGY, REGA);
            break;
        case 0x41:
            write(REGX, REGY, REGB);
            break;
        case 0x61:
            (*REGB) = (*REGA);
            break;
        case 0x62:
            (*REGX) = (*REGA);
            break;
        case 0x63:
            (*REGY) = (*REGA);
            break;
        case 0x64:
            (*REGA) = (*REGB);
            break;
        case 0x66:
            (*REGX) = (*REGB);
            break;
        case 0x67:
            (*REGY) = (*REGB);
            break;
        case 0x68:
            (*REGA) = (*REGX);
            break;
        case 0x69:
            (*REGB) = (*REGX);
            break;
        case 0x6B:
            (*REGY) = (*REGX);
            break;
        case 0x6C:
            (*REGA) = (*REGY);
            break;
        case 0x6D:
            (*REGB) = (*REGY);
            break;
        case 0x6E:
            (*REGX) = (*REGY);
            break;
        case 0x70:
            //JMP
            break;
        case 0x71:
            //JSR
            break;
        case 0x72:
            //RET
            break;
        case 0x73:
            //JSE
            break;
        case 0x74:
            //JSN
            break;
        case 0x75:
            //BNE
            break;
        case 0x76:
            //BEQ
            break;
        case 0x80:
            (*REGA) = (*REGA) + (*REGA);
            break;
        case 0x81:
            (*REGA) = (*REGA) + (*REGB);
            break;
        case 0x82:
            (*REGA) = (*REGA) + (*REGX);
            break;
        case 0x83:
            (*REGA) = (*REGA) + (*REGY);
            break;
        case 0x84:
            (*REGB) = (*REGB) + (*REGA);
            break;
        case 0x85:
            (*REGB) = (*REGB) + (*REGB);
            break;
        case 0x86:
            (*REGB) = (*REGB) + (*REGX);
            break;
        case 0x87:
            (*REGB) = (*REGB) + (*REGY);
            break;
        case 0x88:
            (*REGX) = (*REGX) + (*REGA);
            break;
        case 0x89:
            (*REGX) = (*REGX) + (*REGB);
            break;
        case 0x8A:
            (*REGX) = (*REGX) + (*REGX);
            break;
        case 0x8B:
            (*REGX) = (*REGX) + (*REGY);
            break;
        case 0x8C:
            (*REGY) = (*REGY) + (*REGA);
            break;
        case 0x8D:
            (*REGY) = (*REGY) + (*REGB);
            break;
        case 0x8E:
            (*REGY) = (*REGY) + (*REGX);
            break;
        case 0x8F:
            (*REGY) = (*REGY) + (*REGY);
            break;
        case 0x90:
            (*REGA) = (*REGA) - (*REGA);
            break;
        case 0x91:
            (*REGA) = (*REGA) - (*REGB);
            break;
        case 0x92:
            (*REGA) = (*REGA) - (*REGX);
            break;
        case 0x93:
            (*REGA) = (*REGA) - (*REGY);
            break;
        case 0x94:
            (*REGB) = (*REGB) - (*REGA);
            break;
        case 0x95:
            (*REGB) = (*REGB) - (*REGB);
            break;
        case 0x96:
            (*REGB) = (*REGB) - (*REGX);
            break;
        case 0x97:
            (*REGB) = (*REGB) - (*REGY);
            break;
        case 0x98:
            (*REGX) = (*REGX) - (*REGA);
            break;
        case 0x99:
            (*REGX) = (*REGX) - (*REGB);
            break;
        case 0x9A:
            (*REGX) = (*REGX) - (*REGX);
            break;
        case 0x9B:
            (*REGX) = (*REGX) - (*REGY);
            break;
        case 0x9C:
            (*REGY) = (*REGY) - (*REGA);
            break;
        case 0x9D:
            (*REGY) = (*REGY) - (*REGB);
            break;
        case 0x9E:
            (*REGY) = (*REGY) - (*REGX);
            break;
        case 0x9F:
            (*REGY) = (*REGY) - (*REGY);
            break;
        case 0xB0:
            push(REGA);
            break;
        case 0xB1:
            push(REGB);
            break;
        case 0xB2:
            push(REGX);
            break;
        case 0xB3:
            push(REGY);
            break;
        case 0xC0:
            write(REGA, ReadNext(), ReadNext());
            break;
        case 0xC1:
            write(REGB, ReadNext(), ReadNext());
            break;
        case 0xC2:
            write(REGX, ReadNext(), ReadNext());
            break;
        case 0xC3:
            write(REGY, ReadNext(), ReadNext());
            break;

        case 0xD0:
        case 0xD5:
        case 0xDA:
        case 0xDF:
            (*FLAGS) |= 0b00000001;
            break;



    }
#pragma region Flag Check
    if(opcode >= 0x80 && opcode <= 0x83 || opcode >= 0x90 && opcode <= 0x93){
        if((*REGA) == 0) {
            *FLAGS &= 0b11111101;
            *FLAGS |= 0b00000001;

        }else{
            *FLAGS &= 0b11111110;
            *FLAGS |= 0b00000010;
        }
    }
    if(opcode >= 0x84 && opcode <= 0x87 || opcode >= 0x94 && opcode <= 0x97){
        if((*REGB) == 0) {
            *FLAGS &= 0b11111101;
            *FLAGS |= 0b00000001;

        }else{
            *FLAGS &= 0b11111110;
            *FLAGS |= 0b00000010;
        }
    }
    if(opcode >= 0x88 && opcode <= 0x8B || opcode >= 0x98 && opcode <= 0x9B){
        if((*REGX) == 0) {
            *FLAGS &= 0b11111101;
            *FLAGS |= 0b00000001;

        }else{
            *FLAGS &= 0b11111110;
            *FLAGS |= 0b00000010;
        }
    }
    if(opcode >= 0x8C && opcode <= 0x8F || opcode >= 0x9C && opcode <= 0x9F){
        if((*REGY) == 0) {
            *FLAGS &= 0b11111101;
            *FLAGS |= 0b00000001;

        }else{
            *FLAGS &= 0b11111110;
            *FLAGS |= 0b00000010;
        }
    }
#pragma endregion
    
}

unsigned char ReadNext(){
    if(PC > 0x8000){
        return *mem.ROM[mpH][mpL][(PC - 0x8000)];
    }
    else{
        return *mem.RAM[PC];
    }
}

unsigned char Read(unsigned short addr){
    if(addr > 0x8000){
        return *mem.ROM[mpH][mpL][(addr - 0x8000)];
    }
    else{
        return *mem.RAM[PC];
    }
}

void readMemIncPC(){
    if(PC > 0x8000){
        registers.IR = mem.ROM[mpH][mpL][(PC - 0x8000)];
    }
    else{
        registers.IR = mem.RAM[PC];
    }

    registers.PC++;
}

void push(unsigned char* val){
    mem.RAM[*STCKPTR + STACKBASE] = val;
    (*STCKPTR)++;
}
void pop(unsigned char* val){
    (*STCKPTR)--;
    val = mem.RAM[*STCKPTR + STACKBASE];
}

void top(unsigned char* val){
    val = mem.RAM[*STCKPTR + STACKBASE];
}

void write(unsigned char* low, unsigned char* high, unsigned char* val){
    unsigned short addr = *low | (*high << 8);
}

void ROL(unsigned char* val){
    unsigned char temp = *val;
    (*val) = (*val) << 1 | (*val) >> 7;
}

void ROR(unsigned char* val){
    unsigned char temp = *val;
    (*val) = (*val) >> 1 | (*val) << 7;
}

void doReset(){
    unsigned char resetCode[] = {
            0x90,
            0x95,
            0x9A,
            0x9F,
            0x05,
            0x06,
            0x07,
            0x08
    }

    for(int i = 0; i < 8; i++){
        (*INSTR) = resetCode[i];
        doInstruction();
    }
}

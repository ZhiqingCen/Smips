// dcc smips.c -o smips
// ./smips examples/r0.hex
// 1521 smips examples/r0.hex
// 1521 autotest smips smips.c
// give cs1521 ass2_smips smips.c

// assumption:
// ->given a single file as a command line argument
// ->file contains only 32-bit hexadecimal numbers one per line
// ->file contains at most 1000 numbers.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#define MAX_CODE 1000

struct Instruction {
	char *pattern;      // eg. addi
    int t;              // $t
    int s;              // $s
    int d;              // $d
    int16_t immediate;  // I
};
typedef struct Instruction *ins;

void print_instruction (ins code, char *filename);
ins assembly (int code);

int main (int argc, char *argv[]) {
    // check if the given file exits
    if (!argv[1]) {
        perror(argv[1]);
        exit(1);
    }
    // open the given file for reading
    FILE *file = fopen(argv[1], "r");
    if (! file) {
        perror(argv[1]);
        exit(1);
    }

    // variable declaration
    int codes[MAX_CODE];
    int length = 0;
    int registers[32] = {MAX_CODE};
    int outputs[MAX_CODE] = {MAX_CODE};
    int type[MAX_CODE] = {0};
    int error = 0;
    int counter = 0;
    ins instruction =  malloc (sizeof *instruction);

    // loop through the hex file line by line to scan in data
    for (; (length < MAX_CODE )&& (fscanf(file, "%x", &codes[length]) == 1); length++) {
    }

    // loop through data in hex file for checking
    for (int i = 0; i < length; i++) {
        instruction = assembly(codes[i]);
        // print error message if an unknown instruction code appears
        // and terminate program
        if (! strcmp(instruction->pattern, "error")) {
            printf(
                "%s:%d: invalid instruction code: %08x\n",
                argv[1], i+1, codes[i]
                );
            exit(1);
        }
    }

    printf ("Program\n");
    // print all MIPS instructions line by line
    for (int i = 0; i < length; i++) {
        // call function assembly to break down hex
        instruction = assembly(codes[i]);
        printf ("%3d: ", i);
        // call function print_instruction to print program
        print_instruction(instruction, argv[1]);
    }

    // execute MIPS instructions
    for (int line = 0; line < length && line >= 0; line++) {
        // decode instruction into a strug
        instruction = assembly(codes[line]);
        // $0 is always 0
        registers[0] = 0;

        if (! strcmp(instruction->pattern, "add  ")) {
            // add $d, $s, $t, d = s + t
            registers[instruction->d] = registers[instruction->s] + registers[instruction->t];

        } else if (! strcmp(instruction->pattern, "sub  ")) {
            // sub $d, $s, $t, d = s - t
            registers[instruction->d] = registers[instruction->s] - registers[instruction->t];

        } else if (! strcmp(instruction->pattern, "and  ")) {
            // and $d, $s, $t, d = s & t
            registers[instruction->d] = registers[instruction->s] & registers[instruction->t];

        } else if (! strcmp(instruction->pattern, "or   ")) {
            // or $d, $s, $t, d = s | t
            registers[instruction->d] = registers[instruction->s] | registers[instruction->t];

        } else if (! strcmp(instruction->pattern, "slt  ")) {
            // slt $d, $s, $t, d = 1 if s < t else 0
            if (registers[instruction->s] < registers[instruction->t]) {
                registers[instruction->d] = 1;
            } else {
                registers[instruction->d] = 0;
            }

        } else if (! strcmp(instruction->pattern, "mul  ")) {
            // mul $d, $s, $t, d = s * t
            registers[instruction->d] = registers[instruction->s] * registers[instruction->t];

        } else if (! strcmp(instruction->pattern, "addi ")) {
            // addi $t, $s, I, t = s + I
            registers[instruction->t] = registers[instruction->s] + instruction->immediate;

        } else if (! strcmp(instruction->pattern, "slti ")) {
            // slti $t, $s, I, t = (s < I)
            if (registers[instruction->s] < instruction->immediate) {
                registers[instruction->t] = 1;
            } else {
                registers[instruction->t] = 0;
            }

        } else if (! strcmp(instruction->pattern, "andi ")) {
            // andi $t, $s, I, t = s & I
            registers[instruction->t] = registers[instruction->s] & instruction->immediate;

        } else if (! strcmp(instruction->pattern, "ori  ")) {
            // ori $t, $s, I, t = s | I
            registers[instruction->t] = registers[instruction->s] | instruction->immediate;

        } else if (! strcmp(instruction->pattern, "lui  ")) {
            // lui $t, I, t = I << 16
            registers[instruction->t] = instruction->immediate << 16;

        } else if (! strcmp(instruction->pattern, "beq  ")) {
            // beq $s, $t, I, if (s == t) PC += I
            // go back or forward by immediate steps of instruction
            if (registers[instruction->s] == registers[instruction->t]) {
                line += instruction->immediate - 1;
            }

        } else if (! strcmp(instruction->pattern, "bne  ")) {
            // bne $s, $t, I, if (s != t) PC += I
            // go back or forward by immediate steps of instruction
            if (registers[instruction->s] != registers[instruction->t]) {
                line += instruction->immediate - 1;
            }

        } else if (! strcmp(instruction->pattern, "syscall")) {
            // when syscall is called, output value in $a0, $a1, $a2, $a3

            // check if v0 != 1 || 10 || 11
            if (
                (registers[2] != MAX_CODE) && (registers[2] != 1) &&
                (registers[2] != 10) && (registers[2] != 11)
                ) {
                error = 1;
                break;
            }

            if (registers[4] != MAX_CODE) {
                // registers[4] = values stored in $4($a0)
                outputs[counter] = registers[4];
                type[counter] = registers[2];
                counter++;

            } else if (registers[5] != MAX_CODE) {
                // registers[5] = values stored in $5($a1)
                outputs[counter] = registers[5];
                type[counter] = registers[2];
                counter++;

            } else if (registers[6] != MAX_CODE) {
                // registers[6] = values stored in $6($a2)
                outputs[counter] = registers[6];
                type[counter] = registers[2];
                counter++;

            } else if (registers[7] != MAX_CODE) {
                // registers[7] = values stored in $7($a3)
                outputs[counter] = registers[7];
                type[counter] = registers[2];
                counter++;
            }
        }

        // if (values stored in $2($v0) == 10) stop execution
        if (registers[2] == 10) {
            break;
        }

        // $0 is always 0
        registers[0] = 0;
    }

    printf("Output\n");
    // if($v0 = 1) print int   if($v0 = 11) print char
    for (int i = 0; (outputs[i] != MAX_CODE) && (type[i] != 0); i++) {
        if (type[i] == 1) {
            printf("%d", outputs[i]);
        } else if (type[i] == 11) {
            printf("%c", outputs[i]);
        }
    }

    // if v0 != 1 || 10 || 11, print error message
    if (error == 1) {
        printf("Unknown system call: %d\n", registers[2]);
    }

    // print values stored in registers after execution
    printf("Registers After Execution\n");
    for (int i = 1; i < 32; i++) {
        if (registers[i] != 0) {
            printf("$%-2d = %d\n", i, registers[i]);
        }
    }

    // close the hex file
    fclose(file);
    return 0;
}

// a function to print program instruction line by line
void print_instruction (ins instruction, char *filename) {

    if (! strcmp(instruction->pattern, "syscall")) {
        printf("%s\n", instruction->pattern);

    } else if (
        (! strcmp(instruction->pattern, "addi ")) ||
        (! strcmp(instruction->pattern, "slti ")) ||
        (! strcmp(instruction->pattern, "andi ")) ||
        (! strcmp(instruction->pattern, "ori  "))
        ) {
        printf(
            "%s$%d, $%d, %d\n", instruction->pattern,
            instruction->t, instruction->s, instruction->immediate
            );

    } else if (
        (! strcmp(instruction->pattern, "beq  ")) ||
        (! strcmp(instruction->pattern, "bne  "))
        ){
        printf("%s$%d, $%d, %d\n", instruction->pattern,
            instruction->s, instruction->t, instruction->immediate
            );

    } else if (! strcmp(instruction->pattern, "lui  ")){
        printf(
            "%s$%d, %d\n", instruction->pattern,
            instruction->t, instruction->immediate
            );

    } else if (
        (! strcmp(instruction->pattern, "add  ")) ||
        (! strcmp(instruction->pattern, "sub  ")) ||
        (! strcmp(instruction->pattern, "and  ")) ||
        (! strcmp(instruction->pattern, "or   ")) ||
        (! strcmp(instruction->pattern, "slt  ")) ||
        (! strcmp(instruction->pattern, "mul  "))
        ){
        printf(
            "%s$%d, $%d, $%d\n", instruction->pattern,
            instruction->d, instruction->s, instruction->t
            );

    }
}

// a function to break down 8 digits hexadecimal number
ins assembly (int code) {
    ins instruction =  malloc (sizeof *instruction);
    if (! instruction) {
        exit(1);
    }

    if ((code & 0b111111) == 0b001100) {
        // 00000000000000000000000000001100
        instruction->pattern = "syscall";

    } else if (((code >> 26) & 0b111111) == 0b000100) {
        // 000100ssssstttttIIIIIIIIIIIIIIII
        instruction->pattern = "beq  ";
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;
        instruction->immediate = code & 0b1111111111111111;

    } else if (((code >> 26) & 0b111111) == 0b000101) {
        // 000101ssssstttttIIIIIIIIIIIIIIII
        instruction->pattern = "bne  ";
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;
        instruction->immediate = code & 0b11111111111111111;

    } else if (((code >> 26) & 0b111111) == 0b001000) {
        // 001000ssssstttttIIIIIIIIIIIIIIII
        instruction->pattern = "addi ";
        instruction->t = (code >> 16) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->immediate = code & 0b1111111111111111;

    } else if (((code >> 26) & 0b111111) == 0b001010) {
        // 001010ssssstttttIIIIIIIIIIIIIIII
        instruction->pattern = "slti ";
        instruction->t = (code >> 16) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->immediate = code & 0b1111111111111111;

    } else if (((code >> 26) & 0b111111) == 0b001100) {
        // 001100ssssstttttIIIIIIIIIIIIIIII
        instruction->pattern = "andi ";
        instruction->t = (code >> 16) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->immediate = code & 0b1111111111111111;

    } else if (((code >> 26) & 0b111111) == 0b001101) {
        // 001101ssssstttttIIIIIIIIIIIIIIII
        instruction->pattern = "ori  ";
        instruction->t = (code >> 16) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->immediate = code & 0b1111111111111111;

    } else if (((code >> 26) & 0b111111) == 0b001111) {
        // 00111100000tttttIIIIIIIIIIIIIIII
        instruction->pattern = "lui  ";
        instruction->t = (code >> 16) & 0b11111;
        instruction->immediate = code & 0b1111111111111111;

    } else if ((code & 0b111111) == 0b100000) {
        // 000000ssssstttttddddd00000100000
        instruction->pattern = "add  ";
        instruction->d = (code >> 11) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;

    } else if ((code & 0b111111) == 0b100010) {
        // 000000ssssstttttddddd00000100010
        instruction->pattern = "sub  ";
        instruction->d = (code >> 11) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;

    } else if ((code & 0b111111) == 0b100100) {
        // 000000ssssstttttddddd00000100100
        instruction->pattern = "and  ";
        instruction->d = (code >> 11) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;

    } else if ((code & 0b111111) == 0b100101) {
        // 000000ssssstttttddddd00000100101
        instruction->pattern = "or   ";
        instruction->d = (code >> 11) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;

    } else if ((code & 0b111111) == 0b101010) {
        // 000000ssssstttttddddd00000101010
        instruction->pattern = "slt  ";
        instruction->d = (code >> 11) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;

    } else if ((code & 0b111111) == 0b000010) {
        // 011100ssssstttttddddd00000000010
        instruction->pattern = "mul  ";
        instruction->d = (code >> 11) & 0b11111;
        instruction->s = (code >> 21) & 0b11111;
        instruction->t = (code >> 16) & 0b11111;

    } else {
        // invalid instruction code
        instruction->pattern = "error";
    }

    return instruction;
}

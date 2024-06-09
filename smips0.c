// Assignement 2 - smips, Simple smips

// dcc smips.c -o smips
// ./smips examples/4242424242.hex
// 1521 smips examples/4242424242.hex
// 1521 autotest smips smips.c
// give cs1521 ass2_smips smips.c

// how to convert a string of binary digits into an uint32_t

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

struct Assembler {
	char *pattern;          // eg. addi
    uint32_t t;             // $t
    uint32_t s;             // $s
    uint32_t d;             // $d
    uint32_t immediate;     // I
	// struct Assembler *next;
};
typedef struct Assembler *Ass;

// void program (char *filename);
char *convertHex(uint32_t value);
char *one (char *bin);
uint32_t s_pattern (char *bits);
uint32_t t_pattern (char *bits);
uint32_t d_pattern (char *bits);
uint32_t immediate_pattern (char *bits);
// void output ();
// void registers ();


int main (int argc, char *argv[]) {

    // program (argv[1]);

    // output ();
    // registers ();
//     return 0;
// }

// void program (char *filename) {
    // read in hex file
    FILE *file = fopen(argv[1], "r");
    FILE *count = fopen(argv[1], "r");
    // check error
    if (! file || !count) {
        perror(argv[1]);
        exit(1);
    }

    Ass instruction =  malloc (sizeof *instruction);
    assert (instruction != NULL);

    printf ("Program\n");

    char hex[9];
    uint32_t registers[32] = {0};
    // char bin[32];
    uint32_t data = 0;
    int error = 0;
    // char output[32] = {0};
    // int k = 0;
    // char printed = 0;
    // uint32_t num = 0;

    int lines = 0;
    // count number of lines in the file
    for (int c = getc(count); c != EOF; c = getc(count)) {
        // increment line if this character is newline
        if (c == '\n') {
            lines++;
        }
    }
    for (int i = 0; fgets(hex, sizeof(hex), file); i++) {
        if (i < lines) {
            printf ("%3d: ", i);
            fscanf(file, "%[^\n]", hex);
            if (! strcmp(hex, "c")) {
                printf("syscall\n");
                instruction->pattern = "syscall\0";
                instruction->t = 0;
                instruction->s = 0;
                instruction->d = 0;
                instruction->immediate = 0;
            } else {
                sscanf(hex, "%x", &data);    // data = 3404002a
                // printf("%x\n", data);
                char *bin = convertHex(data);   // bin = 00110100000001000000000000101010
                // printf("bin = %s\n", bin);

                // print data
                instruction->pattern = one(bin);
                printf("%s", instruction->pattern);

                // print first reg
                if (
                    (! strcmp(instruction->pattern, "beq  \0")) ||
                    (! strcmp(instruction->pattern, "bne  \0"))
                    ) {
                    instruction->s = s_pattern(bin);
                    printf("$%d, ", instruction->s);

                } else if (
                    (! strcmp(instruction->pattern, "addi \0")) ||
                    (! strcmp(instruction->pattern, "slti \0")) ||
                    (! strcmp(instruction->pattern, "andi \0")) ||
                    (! strcmp(instruction->pattern, "ori  \0")) ||
                    (! strcmp(instruction->pattern, "lui  \0"))
                    ) {
                    instruction->t = t_pattern(bin);
                    printf("$%d, ", instruction->t);
                } else {
                    instruction->d = d_pattern(bin);
                    instruction->immediate = 0;
                    printf("$%d, ", instruction->d);
                }

                // print second reg
                if (! strcmp(instruction->pattern, "lui  \0")) {
                    instruction->s = 0;
                } else if (
                    (! strcmp(instruction->pattern, "beq  \0")) ||
                    (! strcmp(instruction->pattern, "bne  \0"))
                    ) {
                    instruction->t = t_pattern(bin);
                    printf("$%d, ", instruction->t);
                } else {
                    instruction->s = s_pattern(bin);
                    printf("$%d, ", instruction->s);
                }

                // print last reg
                if (
                    (! strcmp(instruction->pattern, "beq  \0")) ||
                    (! strcmp(instruction->pattern, "bne  \0"))
                    ) {
                    instruction->immediate = immediate_pattern(bin);
                    instruction->d = 0;
                    printf("%d\n", instruction->immediate);
                    // registers[instruction->s] = instruction->immediate;
                } else if (
                    (! strcmp(instruction->pattern, "addi \0")) ||
                    (! strcmp(instruction->pattern, "slti \0")) ||
                    (! strcmp(instruction->pattern, "andi \0")) ||
                    (! strcmp(instruction->pattern, "ori  \0")) ||
                    (! strcmp(instruction->pattern, "lui  \0"))
                    ) {     // print immediate instead of $t
                    instruction->immediate = immediate_pattern(bin);
                    instruction->d = 0;
                    printf("%d\n", instruction->immediate);
                    registers[instruction->t] = instruction->immediate;
                    if (
                        (registers[2] != 0) && (registers[2] != 1) &&
                        (registers[2] != 10) && (registers[2] != 11)
                        ) {
                        // printf("Unknown system call: %d\n", registers[2]);
                        error = registers[2];
                    }
                } else {    // print $t
                    instruction->t = t_pattern(bin);
                    printf("$%d\n", instruction->t);
                    // registers[instruction->d] = instruction->t;
                }

                // if ((registers[2] == 1) && (registers[4] != 0) && (registers[4] != printed)) {
                //     // output[k] = registers[4];
                //     itoa(registers[4],output,10);
                //     printed = registers[4];
                //     k++;
                //     // break;
                // }
                // num = binary_to_uint(bin);          // convert binary to unsigned int
                // sscanf(bin, "%b", &num);
                // num = sixteen_in(bin);
                // printf("%d\n", num);
            }
        }
    }

    printf("Output\n");
    if (error) {
        printf("Unknown system call: %d\n", error);
    }
    // printf("%s\n", output);
    // if (
    //     (registers[2] != 0) && (registers[2] != 1) &&
    //     (registers[2] != 10) && (registers[2] != 11)
    //     ) {
    //     printf("Unknown system call: %d\n", registers[2]);
    // // } else if () {
    // //     printf("argv[1]:%d: invalid instruction code: %d\n");
    // }

    printf("Registers After Execution\n");
    for (int i = 0; i < 32; i++) {
        if ((registers[i] != 0) && (i < 10)) {
            printf("$%d  = %d\n", i, registers[i]);
        } else if ((registers[i] != 0)) {
            printf("$%d = %d\n", i, registers[i]);
        }
    }

    fclose(count);
    fclose(file);

    return 0;
}

char *convertHex (uint32_t value) {
    char *ans = malloc(8 * sizeof(value) + 1);
    uint32_t a = 1;
    for (int pos = 8 * sizeof(value) - 1; pos >= 0; pos--){
        if((a << (31 - pos)) & value){
            ans[pos] = '1';
        }
        else{
            ans[pos] = '0';
        }
    }
    ans[32] = '\0';
    return ans;
}

// get first 6 binary digits
char *one (char *bin) {
    char *ret = NULL;

    if (bin[1] == '1') {
        ret = "mul  \0";            // mul %d, %s, %t
    } else if (bin[2] == '1' && bin[3] == '1' && bin[4] == '1' && bin[5] == '1') {
        ret = "lui  \0";            // lui $t, I
    } else if (bin[2] == '1' && bin[3] == '1' && bin[5] == '1') {
        ret = "ori  \0";            // ori $t, $s, I
    } else if (bin[2] == '1' && bin[3] == '1') {
        ret = "andi \0";            // andi $t, $s, I
    } else if (bin[2] == '1' && bin[4] == '1') {
        ret = "slti \0";            // slti $t, $s, I
    } else if (bin[2] == '1') {
        ret = "addi \0";            // addi $t, $s, I
    } else if (bin[3] == '1' && bin[5] == '1') {
        ret = "bne  \0";            // bne $s, $t, I
    } else if (bin[3] == '1') {
        ret = "beq  \0";            // beq $s, $t, I
    } else if (bin[26] == '1' && bin[29] == '1' && bin[31] == '1') {
        ret = "or   \0";            // or $d, $s, $t
    } else if (bin[26] == '1' && bin[29] == '1') {
        ret = "and  \0";            // and $d, $s, $t
    } else if (bin[26] == '1' && bin[28] == '1' && bin[30] == '1') {
        ret = "slt  \0";            // slt $d, $s, $t
    } else if (bin[26] == '1' && bin[30] == '1') {
        ret = "sub  \0";            // sub $d, $s, $t
    } else {
        ret = "add  \0";            // add $d, $s, $t
    }

    return ret;
}

uint32_t s_pattern (char *bits) { // TODO
    uint32_t ans = 0;
    for (int i = 6; i <= 10; i++) {
        if (bits[i] == '1') {
            ans |= (1 << (10 - i));
        }
    }
    return ans;
}

uint32_t t_pattern (char *bits) { // TODO
    uint32_t ans = 0;
    for (int i = 11; i <= 15; i++) {
        if (bits[i] == '1') {
            ans |= (1 << (15 - i));
        }
    }
    return ans;
}

uint32_t d_pattern (char *bits) { // TODO
    uint32_t ans = 0;
    for (int i = 16; i <= 20; i++) {
        if (bits[i] == '1') {
            ans |= (1 << (20 - i));
        }
    }
    return ans;
}

uint32_t immediate_pattern (char *bits) { // TODO
    uint32_t ans = 0;
    // positive immediate
    if (bits[16] != '1') {
        for (int i = 16; i <= 31; i++) {
            if (bits[i] == '1') {
                ans |= (1 << (31 - i));
            }
        }
        // negative immediate
    } else {    // wrong
        uint32_t j = 1;
        if (bits[31] =='1') {
            ans = 0;
        } else {
            ans = 1;
        }
        for (int i = 30; i >= 16; i--) {
            if (bits[i] =='1') {
                j *= 2;
            } else {
                ans += pow(j, 1);
                j *= 2;
            }
        }
        ans = (ans * -2) - 1;
    }

    // } else {
    //     char imm[16] = {0};
    //     int j = 0;
    //     long int ret = 0;
    //     char* end;
    //     for (int i = 16; i <= 31; i++) {
    //         imm[j] = bits[i];
    //         j++;
    //     }
    //     ret = strtol(imm, &end, 10);
    //     printf("ret = %ld", ret);

    // }

        // printf ("\nans = %d\n", ans);
        // for (int i = 16; i <= 31; i++) {
        //     ans += (j * bits[31-i]);
        //     j *= 2;
        // }
        // for (int i = 16; i <= 31; i++) {
        //     if (i != 31) {
        //         ans |= bits[i] & 0xffff;
        //         ans <<= 16;
        //     } else {
        //         ans |= bits[i] & 0xffff;
        //     }
        // }
    // }
    return ans;
}

// void output () {
//     printf ("Output\n");
// }

// void registers () {
//     printf ("Registers After Execution\n");
//     // for (int i = 0; i < registers; i++) {
//     //     printf ("$%d = %d\n", registers, value);
//     // }
// }

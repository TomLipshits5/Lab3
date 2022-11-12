#include <stdio.h>
#include <stdlib.h>


void PrintHex(FILE* output, char* buffer, int length){
    for(int i = 0; i < length ; i++){
        fprintf(output, "%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("No file was provided:%d", argc);
        exit(1);
    }
    FILE* input;
    if((input = fopen(argv[1],  "r")) != NULL){
        char buffer[100];
        int numOfBytes = fread(buffer, sizeof(*buffer), 100, input);
        PrintHex(buffer, numOfBytes);
    }
    fclose(input);
}
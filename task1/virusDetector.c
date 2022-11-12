#include <stdio.h>
#include <stdlib.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

void PrintHex(FILE* output, char* buffer, int length){
    for(int i = 0; i < length ; i++){
        fprintf(output, "%02X ", buffer[i]);
    }
    printf("\n");
}

virus* readVirus(FILE* file){
    virus *v = (virus*) calloc(1, sizeof (virus));
    int nameBytes[2];
    fread(nameBytes, 1, 2, file);
    v->SigSize = (nameBytes[0]) | nameBytes[1]<<8;
    fread(v -> virusName, 1 ,16, file);
    v -> sig = (unsigned char*) calloc(v -> SigSize, sizeof (char));
    fread(v -> sig, 1 , v -> SigSize, file);
    return v;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output,"SigSize: %d\n", virus->SigSize);
    fprintf(output,"Name:\n");
    for(int i = 0 ; i<16 ; i++){
        if((int)virus->virusName[i]!=0){
            fprintf(output, "%d ", virus->virusName[i]);
        }
    }
    PrintHex(output, virus->sig, virus->SigSize);
}


int main(){
    FILE* f = fopen("../test", "r");
    virus* v = readVirus(f);
//    printVirus(v, stdout);
    free(v);
    fclose(f);
    return 0;
}
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
    virus *v = calloc(1, sizeof (virus));
    char nameBytes[2];
    fread(nameBytes, 1, 2, file);
    v->SigSize = (nameBytes[0]) | nameBytes[1]<<8;
    v -> sig = calloc(v -> SigSize, sizeof (unsigned char));
    fread(v -> virusName, 1 ,16, file);
    fread(v -> sig, 1 , v -> SigSize, file);
    return v;
}

void printVirus(virus* virus, FILE* output){
    if(virus == NULL){
        printf("VIRUS IS NULLLLLLL");
        return;
    }
   fprintf(output,"virus size: %d\n", virus->SigSize);
    printf("virus name:\n");
    for (int i = 0; i < 16; ++i) {
        if((int)virus->virusName[i] != 0){
            fprintf(output,"%c", (int)virus->virusName[i]);
        }
    }
    printf("\nvirus sig:\n");
    PrintHex(output, (char*)virus->sig, virus->SigSize);
}

void freeVirus(virus* v){
    free(v->sig);
    free(v);
}

struct link {
    struct link *nextVirus;
    virus *vir;
};

void list_print(struct link *virus_list, FILE* file){
    printVirus(virus_list->vir, file);
    if(virus_list->nextVirus != NULL){
        list_print(virus_list->nextVirus, file);
    }
}
struct link* list_append(struct link* virus_list, virus* data){
    struct link* newLink = calloc(1, sizeof(struct link));
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}

void list_free(struct link *virus_list){
    freeVirus(virus_list->vir);
    if(virus_list->nextVirus != NULL){
        list_free(virus_list->nextVirus);
    }
    free(virus_list);
}

struct link* load_sigs(FILE* file){
    struct link* virus_link = calloc(1, sizeof(struct link));
    virus_link->vir = NULL;
    virus_link->nextVirus = NULL;
    int bytesNum = 0;
    int first = 1;
    while(1){
        virus* newVirus = readVirus(file);
        if(ftell(file) == bytesNum){
            freeVirus(newVirus);
            break;
        }else{
            bytesNum = ftell(file);
            if(first){
                virus_link->vir = newVirus;
                first = 0;
            }else{
                virus_link = list_append(virus_link, newVirus);
            }
        }
    }
    return virus_link;
}

struct fun_desc {
    char *name;
    void (*fun)(struct link*, FILE*);
};



int main(){

    char* virus_funcs[] = { "Load signatures",
                               "Print signatures",
//                               {"Detect viruses", &encrypt},
//                               {"Fix file", &decrypt},
                               "Quit"
                               };

    struct link* virus_list;
    while(1){
        for(int i = 0; i < (*(&virus_funcs + 1) - virus_funcs); i++){
            printf("%d) %s\n", i+1,  virus_funcs[i]);
        }
        printf("Please enter your choice\n");
        char c[10];
        if(fgets(c, 10, stdin) != NULL){
            int index = atoi(c);
            if(index >= 0 && index <=(*(&virus_funcs + 1) - virus_funcs)-1){
                printf("With in bound\n");
                switch (index) {
                    case 1:
                        char buffer[100];
                        char fileName[100];
                        if(fgets(buffer, 100, stdin)!=NULL){
                            sscanf(buffer, "%s\n", &fileName);
                            printf("Insert file name\n");
                            FILE* file = fopen(fileName, "r");
                            if(file == NULL){
                                printf("Can't open file in case 1");
                                break;
                            }
                            printf("file was opened successfully");
                            virus_list = load_sigs(file);
                            fclose(file);
                        }
                        break;

                    case 2:
                        list_print(virus_list, stdout);
                        break;

                    default:
                        break;

                }
                printf("DONE\n");
            }else{
                if(index != 3){
                    printf("Out of bound\n");
                }
                break;
            }
        }
    }
    list_free(virus_list);
    return 0;
}
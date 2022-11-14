#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

void PrintHex(FILE* output, char* buffer, int length){
    for(int i = 0; i < length ; i++){
        fprintf(output, "%02X ", (unsigned int)(buffer[i] & 0xFF));
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

void print_virus_name(virus* virus, FILE* output){
    printf("virus name:\n");
    for (int i = 0; i < 16; ++i) {
        if((int)virus->virusName[i] != 0){
            fprintf(output,"%c", (int)virus->virusName[i]);
        }
    }
    printf("\n");
}

void printVirus(virus* virus, FILE* output){
    if(virus == NULL){
        printf("VIRUS IS NULLLLLLL");
        return;
    }
   fprintf(output,"virus size: %d\n", virus->SigSize);
    printf("virus name:\n");
    print_virus_name(virus, output);
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

FILE* get_file(){
    char buffer[100];
    char fileName[100];
    printf("Insert file name\n");
    fgets(buffer, 100, stdin);
    sscanf(buffer, "%s\n", &fileName);
    FILE *file = fopen(&fileName, "r");
    if (file == NULL) {
        printf("Can't open file in case 1");
    }else{
        printf("file was opened successfully");
    }
    return file;
}

void detect_virus(char *buffer, unsigned int size, struct link *virus_link){
    int counter = 0;
    virus* v = virus_link->vir;
    while(counter < size){
        if(memcmp(v->sig, buffer + counter, v->SigSize) == 0){
            printf("Virus Detected\nVirus Start Byte: %d\n", counter);
            print_virus_name(v, stdout);
            printf("Virus size: %d\n", v->SigSize);
        }
        counter++;;
    }
}


void detect_all_viruses(struct link* virus_list, char * fileName){
    FILE *suspected_file = fopen(fileName, "r");
    int min_size = 10000;
    char buffer[min_size];
    int bytes_read = fread(buffer, 1, min_size, suspected_file);
    if(bytes_read < min_size){
        min_size = bytes_read;
    }
    while(virus_list != NULL){
        detect_virus(buffer, min_size, virus_list);
        virus_list = virus_list->nextVirus;
    }
    fclose(suspected_file);
}

void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    FILE* infected_file = fopen(fileName,"r+b");
    if (fseek(infected_file, signitureOffset, SEEK_SET) == 0){
        char nop[signitureSize];
        for (int i = 0; i < signitureSize; ++i) {
            nop[i] = 0x90;
        }
        fwrite(nop, 1, signitureSize, infected_file);
    }
    fclose(infected_file);
}



int main(int argc, char** argv){
    char* virus_funcs[] = { "Load signatures",
                               "Print signatures",
                               "Detect viruses",
                               "Fix file",
                               "Quit"
                               };
    int array_len =(*(&virus_funcs + 1) - virus_funcs);
    struct link* virus_list;
    while(1){
        for(int i = 0; i < array_len; i++){
            printf("%d) %s\n", i+1,  virus_funcs[i]);
        }
        printf("Please enter your choice\n");
        char c[10];
        if(fgets(c, 10, stdin) != NULL){
            int index = atoi(c);
            if(index >= 0 && index <= array_len-1){
                printf("With in bound\n");
                switch (index) {
                    case 1:
                        FILE* file = get_file();
                        char ignore[4];
                        fread(ignore, 1, 4, file);
                        virus_list = load_sigs(file);
                        fclose(file);
                        break;

                    case 2:
                        list_print(virus_list, stdout);
                        break;

                    case 3:
                        detect_all_viruses(virus_list, argv[1]);
                        break;

                    case 4:
                        printf("Insert virus_offset virus_size\n");
                        char buffer[10];
                        fgets(buffer, 10, stdin);
                        int offset, virus_size;
                        sscanf(buffer, "%d %d", &offset, &virus_size);
                        kill_virus(argv[1], offset, virus_size);
                        break;

                    default:
                        break;

                }
                printf("DONE\n");
            }else{
                if(index != array_len){
                    printf("Out of bound\n");
                }
                break;
            }
        }
    }
    list_free(virus_list);
    return 0;
}
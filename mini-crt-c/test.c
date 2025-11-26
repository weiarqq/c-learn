#include "minicrt.h"

int main(int argc, char* argv[]){
    int i;
    FILE* fp;
    char** v1 = malloc(argc*sizeof(char*));
    for(i = 0; i < argc; ++i) {
        v1[i] = malloc(strlen(argv[i]) + 1);
        strcpy(v1[i], argv[i]);
    }

    fp = fopen("test.txt","w");
    for(i = 0; i < argc; ++i) {
        int len = strlen(v1[i]);
        fwrite(&len, 1, sizeof(int), fp);
        fwrite(v1[i],1, len, fp);
    }
    fclose(fp);

    fp = fopen("test.txt","r");
    for(i = 0; i < argc; ++i) {
        int len;
        char* buf;
        fread(&len, 1, sizeof(int), fp);
        buf = malloc(len + 1);
        fread(buf, 1, len, fp);
        buf[len] = '\0';
        printf("%d %s\n", len, buf);
        free(buf);
        free(v1[i]);
    }
    fclose(fp);
}
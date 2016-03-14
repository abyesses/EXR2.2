//
//  main.c
//
//
//  Created by Abraham Esses
//  Copyright © 2016 Abraham Esses. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>

#define TEMP 3
#define FICH 5

int grabar = 1;
FILE *fp;
DIR * dir;
char buf[100];
void createDir(); //Crea la carpeta datos
void handleAlrm(int);
void handleSignals(int);
void handleAlrm2(int);
struct stat st ={0};
struct stat st2;
struct dirent * file;
void createDir(){
    if (stat("datos", &st) == -1) {
        mkdir("datos", 0755);
    }
    else{
        char nom2[10];
        for (int i=0; i<FICH; i++) {
            sprintf(nom2, "datos/a%d",i);
            remove(nom2);
        }
        rmdir("datos");
        
        printf("datos borrado vuelve a ejecutarme!\n");
        exit(-1);
    }
}
void printLS(){
    dir = opendir("./datos");
    printf("Nombre de fichero   Tamaño\n");
    while ((file = readdir(dir)) != NULL) {
        stat(file->d_name,&st2);
        
        printf("    %s              ",file->d_name);
        printf("%lld bytes\n",st2.st_size);
        
    }
    closedir(dir);
}
void handleAlrm(int fd){
    printf("Dejando de escribir...\n");
    grabar = 0;
    fclose(fp);
}
void handleAlrm2(int fd){
    printf("Permitiendo escribir...\n");
    grabar = 1;
}
void writeToFile(int i,int sig){
    char nom[15];
    sprintf(nom,"./datos/a%d",i);
    fp = fopen(nom, "w");
    while (grabar) {
        fprintf(fp, "x\n");
        fprintf(fp,"He recibido la señal %d y estaba bloqueada\n",sig);
        sleep(1);
    }
}
int main(int argc, const char * argv[]) {
    sigset_t todas,pendientes;
    sigfillset(&todas);
    sigdelset(&todas, SIGALRM);
    sigprocmask(SIG_BLOCK, &todas, NULL);
    int sig;
    //sigpending(&pendientes);
    createDir();
    //Crear directorios
    for( int i = 0; i < FICH; i++){
        sigpending(&pendientes);
        for (int j=1; j< NSIG; j++) {
            if (sigismember(&pendientes, j)) {
                sig=j;
            }
        }
        signal(SIGALRM, handleAlrm);
        alarm(TEMP);
        writeToFile(i,sig);
        signal(SIGALRM, handleAlrm2);
        alarm(TEMP);
        sleep(TEMP);
    }
    printLS();
    return 0;
}

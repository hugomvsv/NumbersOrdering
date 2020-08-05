//
// Created by rubenjorge on 23/04/20.
//
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "lib_util.c"
char *socket_path = "/tmp/socket";

#define BUF_SIZE 4096			/* block transfer size */
char* codificador(int pid,int indexStart,int indexEnd,int *vec,int nNumeros);
int main(int argc, char **argv)
{
    int c, uds, bytes,num=0,aux[1000];
    char buf[BUF_SIZE];            /* buffer for incoming file */
    struct sockaddr_un channel;        /* Unix Domain socket */
    int *vec, *subsq;
    int index_start=0,index_end=0,tam=0,pid=0;


    if (argc != 5) {
        printf("Usage: client file-name\n");
        exit(1);
    }

    if ((uds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    memset(&channel, 0, sizeof(channel));
    channel.sun_family = AF_UNIX;
    strncpy(channel.sun_path, socket_path, sizeof(channel.sun_path) - 1);

    if (connect(uds, (struct sockaddr *) &channel, sizeof(channel)) == -1)
    {
        perror("connect error");
        exit(1);
    }

    printf("\nArquivo: %s, PID: %s, INDEX START: %s ,INDEX END: %s\n",argv[1],argv[2],argv[3],argv[4]);

    subsq=(int *) malloc(sizeof(int) * (tam));
    pid=atoi(argv[2]);
    index_start=atoi(argv[3]);
    index_end=atoi(argv[4]);
    tam=((index_end-index_start)+1);
    num=readInts(argv[1],aux,999);
    vec=(int*)malloc(sizeof(int)*num);
    for(int i=0;i<num;i++)
    {
        *(vec + i) = *(aux + i);
    }
    int auxstart_index=index_start;
    for(int i=0;i<tam;i++)
    {
        *(subsq+i)= *(vec+auxstart_index);
        auxstart_index++;
    }
    printf("\n----------------------------------------\n");
    printf("\n\nVETOR ORIGIN!\n");
    printArray(vec,num);
    printf("\nMINHA SUBSEQUENCIA!!\n");
    printArray(subsq,tam);
    mergesort_run(subsq,tam,0,(tam-1));
    printf("\nMINHA SUBSEQUENCIA ORDENADA!!\n");
    printArray(subsq,tam);
    char *protocol=codificador(pid,index_start,index_end,subsq,tam);
    printf("\n\nMENSAGEM:%s\n\n",protocol);
    writen(uds,protocol,strlen(protocol));
    printf("\n----------------------------------------\n");
    exit(1);

}

char* codificador(int pid,int indexStart,int indexEnd,int *vec,int nNumeros)
{
    //#pid*index_start;index_end*sequencia|
    char cardinal='#';
    char asterisco='*';
    char pontovirgula=';';
    char virgula=',';
    char barravertical='|';
    char *numeros =(char*)malloc(sizeof(char)*10);
    char *aux=(char*)malloc(sizeof(char)*1000);
    sprintf(aux,"#%d*%d;%d*",pid,indexStart,indexEnd);

    for(int i=0;i<nNumeros;i++)
    {
        sprintf(numeros,"%d,",vec[i]);

        numeros[strlen(numeros)]='\0';
        strcat(aux,numeros);
        memset(numeros,0,strlen(numeros+3));
    }

    aux[strlen(aux)-1]=barravertical;
    aux[strlen(aux)]='\0';
    char *origin=(char*)malloc(sizeof(char)*strlen(aux));
    strcpy(origin,aux);
    free(aux);
    return origin;
}
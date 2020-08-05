#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#include "lib_util.c"

char* codificador(int pid,int indexStart,int indexEnd,int *vec,int nNumeros);
void descodificador(int *vec, char* aux);

char* codificar(int pid, int indexstart, int indexend, int *vec, int nnumeros);
/**
 * Handler do sinal
 * @param signumber
 */
void handler(int signumber) {
    printf("Já ordenei a minha subsequência primaço\n");
    return;
}

int main(int argc, char *argv[]){

    char *arq = argv[1]; //path do ficheiro de numeros
    int num = 0;  //numero de int existentes no vetor
    int *vec;   //vetor de inteiros
    int aux[1000]; //vetor auxiliar
    int div = 2; // divisoes possiveis
    int *pids;

    //instalacao do sinal
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    //leitura do ficheiro
    if (arq == NULL)
    {
        printf("Problemas na CRIACAO do arquivo\n");
        return 0;
    }
    else
        {
        num = readInts(arq, aux, 999); //retorna o numero de inteiros e coloca-os no vetor auxiliar
        vec = (int *) malloc(sizeof(int) * num);  // alocamos memoria para os numeros no vetor principal
        for (int i = 0; i < num; ++i)
        {
            *(vec + i) = *(aux + i);  //copiamos do auxiliar para o principal
        }

        printArray(vec, num);       //print array
    }


    while (div != num)
    {
        for (int k = div; k <= num; ++k)
        {
            if (num % k == 0)
            {
                int nfilhos = num / k;       //numero de numeros existeste de ficheiros/ bloco de posições
                printf("Dividiu em %d partes e criou %d filhos\n", k, num / k);
                pids = newIntArray(nfilhos);
                /* Create a pipe.  File descriptors for the two ends of the pipe are placed in fds.  */

                int fd[2];
                /*
                * Create the pipe
                */
                if(pipe(fd)== -1)
                {
                    perror("pipe");
                    exit(1);
                }
                for (int i = 0; i < nfilhos; ++i)

                {   //criaçao dos filhos
                    if ((pids[i] = fork()) == -1)
                        perror("Algo correu mal com o fork(): ");
                    if (pids[i] == 0)
                    {   //---------------------------------------------------------------------------------------
                        //------------------------------------CODIGO-DO-FILHO------------------------------------
                        //---------------------------------------------------------------------------------------
                        printf("Vou dividir do %d ao %d\n", ((i * k)),
                               ((i + 1) * k) - 1); //posiçao inicial - posiçao final
                        int a[k];     //criaçao do subvetor
                        for (int j = 0; j < k; ++j)
                        {
                            a[j] = vec[(i * k) + j];      //passa para o subvetor a sequencia a ordenar
                        }

                       char filename[] = "aaaaaaaaaa.txt";
                         sprintf(filename, "%d.txt", getpid());
                         open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                         writeInts(filename, a, k);


                        mergesort_run(a, k, 0, k - 1); //ordena com o mergesor
                        printf("MINHA SEQUENCIA!");
                        printArray(a, k);
                        printf("\n");

                        char *s=codificador(getpid(),(i * k),(((i + 1) * k) - 1),a,k);
                        // printf("\n\n%s\n",s);
                        kill(getppid(), SIGUSR1); //envia o sinal

                        //comunicaçao pipe
                        char *token;
                        close(fd[0]);
                        writen(fd[1],s,strlen(s));
                        token = strtok(NULL, "|");
                        exit(0);
                    }

                }
                //-------------------------------------------------------------------------------------------------
                //-----------------------------------CODIGO-DO-PAI-------------------------------------------------
                //-------------------------------------------------------------------------------------------------
                close(fd[1]);
                ssize_t n;
                char buffer[4096];
                int nlinhas=0;
                char** arraystring=malloc(sizeof(*arraystring)*(nlinhas+1));
                int nletras;

                while ((n = readn(fd[0],     buffer, 4096)) > 0)
                {
                    char *message = strtok(buffer, "|"); // retira um protocolo de buff

                    while (message != NULL)
                    {
                        printf("\nmensagem: %s",message);
                        arraystring=realloc(arraystring,(nlinhas+1)* sizeof(message));
                        *(arraystring+nlinhas)=message;
                        nlinhas++;
                        message = strtok(NULL, "|");
                    }
                }
                for (int m = 0; m < nlinhas-1; ++m)
                {
                    descodificador(vec,(*(arraystring+m)));
                    printf("\n");
                    printArray(vec,num);
                }
                //---------------------------------------------------------------------------------------------------------
                //----------------------------------ENVIAR-PARA-O-TXT-O-VEC-ATUALIZADO-------------------------------------
                //---------------------------------------------------------------------------------------------------------
                for (int l = 0; l < nfilhos; ++l)
                {
                    wait(NULL); //espera os filhos

                }
               
                div = k;
            }
        }
        
    }
    
    return 0;
}

/**
 * esta funçao vai descodificar as mensagens/tokens que o filho envia para o pai !
 * @param vec vetor original de inteiros que vai ser modificado
 * @param aux mensagem a descodificar
 */
void descodificador(int *vec, char* aux)
{
    //#23244*54;34*1,2,3,4,5,6,7|
    int firstindex = 0, secondindex = 0, auxindex=0,i=0;
    char *symbols[] = {"*", ";", ","};
    char *token = strtok(aux, *(symbols + i));
    token = strtok(NULL, *(symbols + (i + 1)));
    printf("\nfirstpos: %s", token);
    firstindex = atoi(token);                                             //primeiro index
    token = strtok(NULL, *(symbols + i));
    printf("\nsecondpos: %s", token);
    secondindex = atoi(token);                                          //segundo index
    auxindex = firstindex;
    while (token != NULL)
    {
        token = strtok(NULL, *(symbols + (i + 2)));
        if(auxindex>secondindex)
        {
            break;
        }
        vec[auxindex] = atoi(token);
        auxindex++;
    }
}
//descodificar o indexstart e o indexend e a sequencia e colocala no array
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
int numerosexistentes(int x) {
    int contaDigitos = 0;
    if (x == 0) contaDigitos = 1;
    else
        while (x != 0) {
            contaDigitos = contaDigitos + 1;
            x = x / 10;
        }
    return contaDigitos;
}
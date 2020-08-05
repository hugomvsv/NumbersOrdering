#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <wait.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include "lib_util.c"

#define BUF_SIZE 2048                                                       // block transfer size


int prepare_socket(struct sockaddr_un channel_srv, char * socket_path,int n);
void descodificador(int *vec, char* aux);
char *socket_path = "/tmp/socket";
/**
 * Handler do sinal
 * @param signumber
 */

int main(int argc, char *argv[]) {

    int listenfd, connfd, bytes;
    char buf[BUF_SIZE];                                                     // buffer for outgoing file instalação do socket
    struct sockaddr_un channel_srv;
    char *arq = argv[1]; //path do ficheiro de numeros
    int num = 0;  //numero de int existentes no vetor
    int *vec;   //vetor de inteiros
    int aux[1000]; //vetor auxiliar
    int div = 2; // divisoes possiveis
    int *pids;
    char *filename = "/home/rubenjorge/Desktop/Projeto_SO_socket/Origin.txt";


    //leitura do ficheiro

    if (arq == NULL) {
        printf("Problemas na CRIACAO do arquivo\n");
        return 0;
    } else
        {
        num = readInts(arq, aux, 999); //retorna o numero de inteiros e coloca-os no vetor auxiliar
        vec = (int *) malloc(sizeof(int) * num);  // alocamos memoria para os numeros no vetor principal
        for (int i = 0; i < num; ++i) {
            *(vec + i) = *(aux + i);  //copiamos do auxiliar para o principal
        }
        printArray(vec, num);       //print array
    }

    //faz socket
    listenfd = prepare_socket(channel_srv, socket_path, 10);

    //código do server
    while (div != num) {

        for (int k = div; k <= num; ++k)
        {
            open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
            writeInts(filename, vec, num);
            if (num % k == 0)
            {
                int nfilhos = num / k;       //numero de numeros existeste de ficheiros/ bloco de posições
                printf("Dividiu em %d partes e criou %d filhos\n", k, num / k);
                pids = newIntArray(nfilhos);
                printf("\n\n----------------------VOU CRIAR FILHOS!-----------------------\n\n\n");
                for (int i = 0; i < nfilhos; i++){   //criaçao dos filhos
                    if ((pids[i] = fork()) == -1)
                        perror("Algo correu mal com o fork():");
                    if (pids[i] == 0) {
                        //---------------------------------------------------------------------------------------
                        //----------------------------INICIALIZAÇAO DO EXEC DO FILHO-----------------------------
                        //---------------------------------------------------------------------------------------
                        char *indexStart = (char *) malloc(sizeof(char) * 5);
                        char *indexEnd = (char *) malloc(sizeof(char) * 5);
                        char *filho = (char *) malloc(sizeof(char) * 7);
                        sprintf(filho, "%d", getpid());
                        sprintf(indexStart, "%d", (i * k));
                        sprintf(indexEnd, "%d", ((i + 1) * k) - 1);
                        printf("vou mandar para o FILHO !");
                        char *args[6] = {"/home/rubenjorge/Desktop/Projeto_SO_socket/client", filename, filho,
                                         indexStart, indexEnd, NULL};
                        execvp(args[0], args);
                        perror("execv:");
                        exit(1);
                    }
                }
                //-------------------------------------------------------------------------------------------------
                //-----------------------------------CODIGO-DO-PAI-------------------------------------------------
                //-------------------------------------------------------------------------------------------------
                for (int i = 0; i <nfilhos; i++)
                {
                    if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
                        perror("accepts error;");
                        continue;
                    }
                    else {

                        ssize_t n;
                        char buffer[4096];
                        char *token_message;
                        char *message = NULL;
                        while ((n = readn(connfd, buffer, 4096)) > 0)
                        {
                            token_message = strtok(buffer, "|"); // retira um protocolo
                            //mete para um array de strings os protocolos lidos do socket
                            while(token_message!=NULL)
                            {
                                message = (char *) malloc(sizeof(char) * (strlen(token_message)+1));
                                strcpy(message, token_message);
                                printf("\n\nTOKEN:%s", message);

                                descodificador(vec, message);
                                printf("\n\n-----VETOR NO SERVER-----");
                                printArray(vec,num);
                                printf("\n\nVETOR ORIGINAL NO SERVER !");
                                printArray(vec, num);
                                token_message=strtok(NULL, "|");
                            }
                        }
                    }
                }

            }
            div=k;
        }
    }
        free(pids);
        printf("\n\n\nVETOR FINAL ORDENADO:");
        printArray(vec, num);
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
    token = strtok(NULL, *(symbols +  1));
    printf("\nfirstpos: %s", token);
    firstindex = atoi(token);                                             //primeiro index
    token = strtok(NULL, *(symbols + 0));
    printf("\nsecondpos: %s", token);
    secondindex = atoi(token);                                          //segundo index
    auxindex = firstindex;
    while (token != NULL)
    {
        token = strtok(NULL, *(symbols +  2));
        if(auxindex>secondindex)
        {
            break;
        }
        vec[auxindex] = atoi(token);
        auxindex++;
    }
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

int prepare_socket(struct sockaddr_un channel_srv, char * socket_path,int n)
{

    int listenfd;

    if ( (listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {              // Creating the server socket
        perror("socket error");
        exit(-1);
    }

    unlink(socket_path);
    memset(&channel_srv, 0, sizeof(channel_srv));
    channel_srv.sun_family = AF_UNIX;
    strncpy(channel_srv.sun_path, socket_path, sizeof(channel_srv.sun_path)-1);

    if (bind(listenfd, (struct sockaddr*)&channel_srv, sizeof(channel_srv)) == -1) {      // Binding the server socket to its name
        perror("bind error");
        exit(-1);
    }
    if (listen(listenfd, n) == -1) {                                  // Configuring the listen queue
        perror("listen error");
        exit(-1);
    }
    //ate aqui a instalação
    return listenfd;
}

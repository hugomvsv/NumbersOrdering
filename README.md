# NumbersOrdering
Criar um programa que consiga ordenar um grande conjunto de dados com recurso a computação paralela.

Resumo:

Deve ser implementado um programa que receba por *argv[] um caminho para um ficheiro onde
com um grande volume de dados a ordenar. O programa deve implementar o algoritmo de ordenação
MergeSort.

O programa deve ler e armazenar em memória o input de valores lidos do ficheiro.

![alt text](https://i.imgur.com/MGWDzIT.png)

Na fase de ordenação, o programa deve seguir uma filosofia de divisão e conquista, para isso,
divide o conjunto de valores lidos em N blocos. Com recurso à chamada ao sistema fork(), o programa deve
criar N filhos, cada um responsável por ordenar um dos blocos, em intervalos previamente definidos pelo
pai. Na primeira fase, os processos filho devem devolver as subsequências ordenadas ao processo pai
recorrendo a um protocolo de mensagens indicado no exemplo seguinte:

                      #pid*index_start;index_end*sequência|

● pid: identificador do processo filho.

● index_start: limite inferior do intervalo a ordenar.

● index_end: limite superior do intervalo a ordenar.

● sequência: sequência numérica ordenados a retornar ao pai.

![alt text](https://i.imgur.com/HTPe1cx.png)

A fase de ordenação anteriormente descrita deve ser executada as vezes necessárias até que o
conjunto de valores de encontre ordenado.

![alt text](https://i.imgur.com/gRr0vpp.png)

Finalmente o pai cria o último filho responsável por ordenar os dois últimos intervalos e retornar
a sequência ordenada ao pai.

![alt text](https://i.imgur.com/RDKys2C.png)

Após a terminação do último filho o pai grava a sequência ordenada em ficheiro.

Requesitos:

1. Ler e armazenar o conjunto de valores a ordenar.

2. Criar N filhos, cada um deles deve ordenar a sua subsequência. Após a ordenação deve criar um
ficheiro onde vai escrever a subsequência ordenada.No final o processo filho deve enviar um sinal ao
pai SIGURS1 a notificar que a sua subsequência já se encontra ordenada. O pai deve esperar pela
finalização dos seus filhos.

3. Esta etapa implica que o programa permita suportar a comunicação entre processos com recurso
a pipes. Cada filho deve retornar a sua subsequência ordenada para o pai através do pipe. O programa
deve suportar um protocolo de comunicação que permita ao pai saber que filho é que ordenou
determinada subsequência e a que intervalo pertence. O pai recebe as subsequências ordenadas e
guarda as subsequências no array original. Nesta etapa o programa deve fazer recurso da função readn
e writen
               Protocolo: #pid*index_start;index_end*sequencia|

4.  Esta etapa implica que o programa permita suportar a comunicação entre processos com recurso
a Unix Domin Sokects

. Cada filho deve estabelecer conexão com o server (pai). O pai deve atender as
conexões e armazenar as subsequências ordenadas array original.

#include "stdlib.h"
#include "stdio.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "arpa/inet.h"
#include "error.h"
#include <pthread.h>

#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <error.h>

#define PORTNUMBER 1234
#define MAXCLIENTS 2000
//AWESOME MACRO
#define XOR_SWAP(a,b) a ^= b; b ^= a; a ^= b;


// John, Before read the code, the pt-br comments aren't mine k?
// TCP

int ClientsList[MAXCLIENTS];
int ClientNumber;

void *ChatConnection(void *socket);
void FixClientList(int socket);
void ServerCommands(char *buffer, int socket);

int main(){
   int ServerSocket;
   struct sockaddr_in Server_Addr;
   struct sockaddr_in Client_Addr;
   int ConnectionSocket;
   int SocketSize;
   int flag; // THIS SHIT IS NEVER USED, WHY THIS IS HERE???

   //Criacao do socket no servidor e inicialização do número de clientes conectados
   ClientNumber = 0;
   printf("Iniciando o servidor. \n");
   if ((ServerSocket = socket(AF_INET,SOCK_STREAM,0))<0)
      perror("Erro ao criar socket do servidor: \n");

   //Configuracao do endereco do servidor que sera ligado ao socket
   //zera toda a estrutura
   memset(&Server_Addr, 0, sizeof(Server_Addr));

   //Tipo de endereco
   //especificacao do endereco
   //Querendo especificar manualmente um endereco pode-se usar
   //Numero da porta
   Server_Addr.sin_family = AF_INET;
   Server_Addr.sin_addr.s_addr   = INADDR_ANY;
   Server_Addr.sin_addr.s_addr   = inet_addr("127.0.0.1");
   Server_Addr.sin_port   = htons(PORTNUMBER);
   //memset(&(Server_Addr.sin_zero), '\0', sizeof(Server_Addr.sin_zero));

   //Binding Server to the socket
   if (bind(ServerSocket, (struct sockaddr *)&Server_Addr, sizeof(struct sockaddr))<0){
      perror("Erro de ligacao de socket no servidor: \n");
   }

   //Indica que o socket deve "escutar" <----OOOHHHH RLY ¬_¬ i'm not THAT stupid man =.=
   if (listen(ServerSocket, 1)<0){
      perror("Erro ao inicializar a escuta do socket no servidor: \n");
   }

   printf("Servidor escutando na porta: %d .\n", PORTNUMBER);

   //Servidor fica em loop para receber conexoes
   while(1){
      SocketSize = sizeof(Client_Addr);

      //Servidor aguardando por conexao
      ConnectionSocket = accept(ServerSocket, (struct sockaddr *)&Client_Addr, &SocketSize);
      printf("Connection make!\n");
      ClientsList[ClientNumber] = ConnectionSocket;
      ClientNumber++;
      pthread_t tid;
      if ((pthread_create(&tid, NULL, ChatConnection, (void *)&ConnectionSocket )) < 0) 
      {
          // bugz
          abort();
      }
   }
   //Encerra o socket servidor
   close(ServerSocket);
   //Finaliza o servidor
   exit(0);
}


//Thread for the Connection with each client
void *ChatConnection(void *socket) {
    int ConnectionSocket;
    int tamr;
    char buffer[100];
    int Index;

    ConnectionSocket = *(int *)socket;
    if(ConnectionSocket < 0){
        return;
    } else{
        printf("Conexao estabelecida com cliente novo. \n");
        while ((tamr = recv(ConnectionSocket, buffer, 100, 0)) > 0) 
        {
            //Broadcast the message
            for (Index= 0; Index < ClientNumber; Index++)
            {
              if(ClientsList[Index] != ConnectionSocket){
                send(ClientsList[Index],buffer,strlen(buffer),0);
                printf("Mandando para Cliente: %d\n",ClientsList[Index] );
              }
            }
            // To keep the server clean, the messages will only be broadcasted
            //buffer[tamr] = '\0';
            //printf("Cliente enviou mensagem %s", buffer);
            ServerCommands(buffer,ConnectionSocket);
            
            
            if(tamr == strlen(buffer) && ClientNumber < 1){
            printf("Mensagem enviada Para Clientes. \n");

  }
        }
        //Encerra a conexao com o cliente <-Don't even know why this is here....
        close(ConnectionSocket);
    }
}

//When a Client Disconnects, Fill the position he leaves
void FixClientList(int socket)
{
  int Index;
  Index = 0;
  while(socket != ClientsList[Index])
    Index++;
  if (Index == ClientNumber)
  {
    //Last pos, whatever then
    return;
  }
  else
  {
    for (; Index < ClientNumber - 1; Index++)
    {
      XOR_SWAP(ClientsList[Index],ClientsList[Index+1]);
    }
  }
  return;
}

//ADITIONAL: A little commands to garantee somethings, the only who sees is the server k?
void ServerCommands(char *buffer, int socket)
{
  int Index;

  if(strstr(buffer, "desligar")){
    printf("Desligando cliente %d\n",socket);
    FixClientList(socket);
    ClientNumber--;
    close(socket);
  }
  if(strstr(buffer, "serveroff")){
    //Needs to me more graceful, close every socket, tell the clients then close the listening socket, and flies away
    printf("Desligando cliente %d\n",socket);
    FixClientList(socket);
    close(socket);
    abort();
  }
  if(strstr(buffer, "serverlist")){
    printf("Clientes:\n");
    for (Index= 0; Index < ClientNumber; Index++)
    {
      printf("%d\n",ClientsList[Index]);
    }
  }
  if(strstr(buffer, "servercon"))
    printf("Clientes Conectados:%d\n",ClientNumber);
}
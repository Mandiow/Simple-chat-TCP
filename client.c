#include <pthread.h>
#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <error.h>

#define MAXLINE 200



// TCP
void *SendMessage();
void *RecvMessage();

int ClientSock;

int main(){

   int Conn;
   struct sockaddr_in S;
   int portaServidor = 1234;


   //Criando socket cliente
   ClientSock = socket(AF_INET, SOCK_STREAM, 0);
   if(ClientSock < 0){
		perror("Erro na criacao do socket client: ");
    abort();
   }

   memset(&S, 0, sizeof(S));

   //Especificacao do endereco do servidor para efetuar conexao
   S.sin_family = AF_INET;
   S.sin_addr.s_addr = inet_addr("143.54.6.73");
   S.sin_port = htons(portaServidor);

   //Efetua conexao com o sevidor
   Conn = connect(ClientSock, (struct sockaddr *) &S, sizeof(S));
   if(Conn < 0){
      perror("Erro ao tentar se conectar com o servidor: ");
      abort();
   }

    while(1) 
    {
        //Create a Thread To SEND and RECV
        pthread_t SendThread, RecvThread;
        if (pthread_create(&SendThread, NULL, SendMessage, NULL ) < 0) {
            // bugz
            abort();
        }
        if (pthread_create(&RecvThread, NULL, RecvMessage, NULL ) < 0) {
            // bugz
            abort();
        }
        //Guess What!? To keep the main Thread from shitting useless threads we join'em ;3
        pthread_join(RecvThread, NULL);
        pthread_join(SendThread, NULL);


        
    }

   
   
}
//Thread
void *SendMessage()
{
    unsigned int BufferSize;
    char buffer[MAXLINE];
    while(1)
    {
      printf("Entre com a mensagem que gostaria de enviar: \n");
      fgets(buffer,MAXLINE,stdin);

      //Envia mensagem ao servidor
      BufferSize = write(ClientSock,buffer,strlen(buffer));
      if(BufferSize == strlen(buffer)){
        printf("Mensagem enviada. \n");

      }
      else
      {
        printf("Erro no servidor.\n");
      }
      if (strstr(buffer, "desligar")) 
      {
        close(ClientSock);
        printf("Cliente encerrado. \n");
        exit(0);
      }
    }
}

//Yes, is THAT simple, but is very, very, veeeeryyy buggy with the server ¬_¬
void *RecvMessage()
{
  char buffer[MAXLINE];
  int BufferSize;
  while (BufferSize=recv(ClientSock,buffer,MAXLINE,0) > 0) 
  {
    printf("Message:\n%s\n" , buffer);
  }
  
  return;
  

//AWWW YEAH
}

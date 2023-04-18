/* CODE CLIENT. Socket Unix : fichier local ./MySocket */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sclient; 			// File descripteur pour socket client
	char message[BUFFER_SIZE] = {0};
	//char reponse[BUFFER_SIZE] = {0};

	// Initialisation struct adress du serveur
	struct sockaddr_un saddr = {0}; 	// Struct adress du serveur
	saddr.sun_family = AF_UNIX;		// Address Family UNIX
	strcpy(saddr.sun_path, "./MySock");	// Chemin vers fichier socket

	// creation socket client
	sclient = socket(AF_UNIX,SOCK_STREAM,0);


	// tentative de connextion au serveur, connect renvoi -1 en cas d'erreur
	while (connect(sclient,(struct sockaddr *)&saddr,sizeof(saddr)) == -1) ;
  printf("Success to connect to server. To quit enter f.\n");
	// Communication
	while(message[0] != 'f') {
	  printf("User: Enter a message.\n");
		scanf("%s", message);
		write(sclient,message,strlen(message)+1);
	}

  /*
  memset(reponse, '\0', BUFFER_SIZE);
  read(sclient,reponse,BUFFER_SIZE);
  printf("Response received by server : %s \n",reponse);
  */

	// fermeture service, et fermeture socket/serveur
	shutdown(sclient,SHUT_RDWR);
	close(sclient);
	return 0;
}

/* CODE SERVEUR. Socket Unix : fichier local ./MySock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>

#define BUFFER_SIZE 1024
#define NB_MESSAGE_MAX 25
#define NB_USER_MAX 10

int cptClient = 0;

int fdservices[NB_USER_MAX];
pthread_t* thread_users[NB_USER_MAX];

typedef struct{
  char* matrice[NB_USER_MAX][NB_MESSAGE_MAX];
}messages_struct;

void* fonct_lecture(void* arg){
  messages_struct* mess = (messages_struct*) arg;

  mess->matrice[cptClient] = malloc(sizeof(char*)*NB_MESSAGE_MAX);

  //lecture
  int nbMessages = 0;
  char message[BUFFER_SIZE];
  while (message[0] != 'f') {
    memset(message,'\0',BUFFER_SIZE);
    read(fdservices[cptClient],message,BUFFER_SIZE);
    printf("Message recu : %s\nint fdservices[NB_USER_MAX];", message);
    mess->matrice[cptClient][nbMessages] = message;
    nbMessages++;

    //Envoie du message a tous les autres clients (peut etre creer un thread)
    for (int i = 0; i<cptClient; i++){
      if (fdservices[i] == -1) //le client s'est deconnecte
        write(fdservices[i],message,strlen(message)+1);
    }
  }
  fdservices = -1;
  shutdown(fdservices[cptClient],SHUT_RDWR);
  close(fdservices[cptClient]);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	unlink("MySock");

	int secoute;
  messages_struct mess;
	socklen_t caddrlen;

	// Initialisation struct addr serveur
	struct sockaddr_un saddr = {0};
	saddr.sun_family = AF_UNIX;
	strcpy(saddr.sun_path, "./MySock");

	// Initialisation struct addr client
	struct sockaddr_un caddr = {0};

	// Creation socket serveur
	secoute = socket(AF_UNIX, SOCK_STREAM, 0);
	if (secoute  == -1){
		perror("Error creating socket.\n");
		exit(EXIT_FAILURE);
	}

	// Connection de la socket a l'addresse serveur
	bind(secoute, (struct sockaddr *) &saddr, sizeof(saddr));

	// Socket en attente de connexion (a l'ecoute),
	listen(secoute,5);

	while (1){
		// accepter les connexions
		caddrlen = sizeof(caddr);
		fdservices[cptClient] = accept(secoute,(struct sockaddr *)&caddr,&caddrlen);

    //Creation du thread lecture pour la connexion
    cptClient ++;
    int status = pthread_create(thread_users[cptClient], NULL, fonct_lecture, (void*) &mess);
    if (status < 0){
      perror("Error creating thread.\n");
      exit(EXIT_FAILURE);
    }
	}

  //Liberation de la matrice message
  for(int i = 0; i<cptClient; i++)
    free(mess.matrice[i]);
  mess = NULL;
  free(mess);


	close(secoute);
	unlink("MySock");
	return 0;
}

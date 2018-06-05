// Envoi.c
// BENIT Romain

// Include généraux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <fcntl.h>

// Include particuliers
#include "udp_envoi.h"
#include "son.c"

int main (int argc, char **argv) {

    // sd = socket descriptor
    // rc = valeur de retour
    int sd, rc, i, j, compt, compt2;
    struct sockaddr_in CliAddr, ServAddr ;
    struct hostent * h;
    // Varibales concernant le fichier son
	FILE *entree;
    char buff[Taille_msg];
    int rd = 1, taille_fic[1];

    h = gethostbyname(argv[1]);
    if ( h == NULL ) {
       perror("Erreur dans le gethostbyname de l'argument 1");
       exit(2);
   }
   printf("Nom de machine entré : %s\n", h -> h_name );
	 printf("Adresse IP de la machine %s\n\n", inet_ntoa(*(struct in_addr *)h -> h_addr_list[0]) );

    ServAddr.sin_family = h -> h_addrtype;
    // faire un test sur memcpy
    memcpy((char *)&ServAddr.sin_addr, h -> h_addr_list[0], h -> h_length);

    // convertit  un  entier  court  (short) hostshort  depuis  l'ordre des octets de l'hôte vers celui du réseau.
    ServAddr.sin_port = htons(Ports);

    // Création du socket
    sd = socket(AF_INET, SOCK_DGRAM, 17);
    // Test du socket
    if ( sd > 0 ) {
        CliAddr.sin_family = AF_INET;
        CliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        CliAddr.sin_port = htons( 0 );
    }

    // Liaison avec le socket
    rc = bind(sd, (struct sockaddr *)&CliAddr, sizeof(CliAddr));
    // Test sur le bind
    if ( rc < 0 ) {
        perror("Erreur dans le bind");
        exit(3);
    }

    // Indication du debut de l'envoi du fichier avec l'envoi d'un flag
    rc = sendto(sd, DEB_FLAG, strlen(DEB_FLAG), 0, (struct sockaddr *)&ServAddr, sizeof(ServAddr));
    if ( rc < 0 ) {
        perror("Erreur dans le sendto");
        exit(7);
    }

    // Indication de la taille du fichier que l'on va envoyer
    taille_fic[0] = traiter_son();
	printf("Taille fichier pour envoi de la vidéo : %d octets\n",taille_fic[0]);
    rc = sendto(sd, taille_fic, sizeof(taille_fic), 0, (struct sockaddr *)&ServAddr, sizeof(ServAddr));
    if ( rc < 0 ) {
        perror("Erreur dans le sendto");
        exit(7);
    }

    // Gestion des erreurs et ouverture du fichier son
    if (!(entree=fopen("Danzarumena1.wav","rb"))) {
        perror("Erreur d'ouverture du fichier source son\n");
        exit(4);
    }

    // Lecture du fichier son
    do {
      rd = fread(buff, 1, Taille_msg, entree);
      if ( rd < 0 ) {
          perror("Erreur dans la lecture du fichier son");
          exit(5);
      }

			compt = compt + rd;

      //   Envois du buffer
      rc = sendto(sd, buff, rd, 0, (struct sockaddr *)&ServAddr, sizeof(ServAddr));
      if ( rc < 0 ) {
          perror("Erreur dans le sendto");
          exit(6);
      }
					compt2 = compt2 + rc;
	  			for (j=0; j<120000;j++){ ; }
    } while ( rd > 0 );

					printf("Nombre de octets lu dans le fichier : %i\n", compt);
+					printf("Nombre de octets envoyés : %i\n", compt2);

    // Fermeture des fichiers
    close(entree);

    // Indication de la fin de l'envoi du fichier avec l'envoi d'un flag
    rc = sendto(sd, FIN_FLAG, strlen(FIN_FLAG), 0, (struct sockaddr *) & ServAddr, sizeof(ServAddr));
    if ( rc < 0 ) {
        perror("Erreur dans le sendto");
        exit(7);
    }

    // Fermeture du socket UDP
    close(sd);
}

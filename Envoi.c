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

int compteur(int header[], int n, int m) {
	int i,j=0,a=0,compteur=0, copieheader;

	for (i=n; i<=m; i++){
		// Il faut faire une copie de la valeur puisque nous allons ensuite la modifier dans la boucle
		copieheader = header[i];
        for (j=0;j<8;j++){
			// Ici nous faisons un modulo base 2 de la valeur décimale du fichier source, ce qui nous donne 0 ou 1
			// Puis nous la multiplions par 2 puissance a, qui croit de 1 à chaque passage de la boucle
			// Ainsi nous obtenons les comptes des valeurs sur plusieurs octets
			compteur = compteur+(int)fmod(copieheader,2)*(int)pow(2,a);
			copieheader = copieheader/2;
			a++;
        }
	}
	return compteur;
}

int traiter_son() {

	FILE *entree;
	int c, d=0, i;
	int tailleheader = 0;
	struct header_son header_son;
	// Ici la taille du header peut varier, donc on met une taille de 100 par défaut
	int header[100] = {0};

    // Gestion des erreurs et ouverture des fichiers
	if (!(entree=fopen("Danzarumena1.wav","rb"))) {
		perror("Erreur d'ouverture du fichier source son\nMéthode : ./FichierSon >'fichier source son'< 'fichier destination son'");
        exit(1);
	} else {

	// Début du travail avec les fichiers
		// Lecture du header tant que l'on ne trouve pas le mot "data"
		while ((c = fgetc(entree)) || d != 4 ) {
			if (header[tailleheader-(1+d)] != 97 || header[tailleheader-(4+d)] != 100 || header[tailleheader-(3+d)] != 97 || header[tailleheader-(2+d)] != 116) {
				header[tailleheader] = c;
				tailleheader++;
			} else {
				// On ajoute l'espace pour les champs de taille du fichier (4 puisque ce champs est sur 4 octets)
				header[tailleheader] = c;
				tailleheader++;
				d++;
			}
        }

		// Calcul du nombre de voies
		header_son.nbrvoix = compteur(header,22,23);
		// Calcul de la taille du fichiers (partie data)
		header_son.tailledata = compteur(header,4,7) - tailleheader;
		// Calcul de la fréquence d'échantillonage
		header_son.samplerate = compteur(header,24,27);
		// Calcul du byterate
		header_son.byterate = compteur(header,28,31);
		// Calcul du BlockAlign
		header_son.blockalign = compteur(header,32,33);
		// Calcul des bits par échantillons
		header_son.bitspersample = compteur(header,34,35);

		printf("Ici le fichier à une taille pour l'entête de: %i\n", tailleheader);
		printf("Ici le fichier à une taille pour les données de: %i\n", header_son.tailledata);
		printf("Ici le fichier à une taille totale: %i\n", header_son.tailledata + tailleheader);
		printf("Ici le fichier à un nombre de voix de: %i\n", header_son.nbrvoix);
		printf("Ici le fichier à un samplerate de: %i\n", header_son.samplerate);
		printf("Ici le fichier à un byterate de: %i\n", header_son.byterate);
		printf("Ici le fichier à un blockalign de: %i\n", header_son.blockalign);
		printf("Ici le fichier à un bitspersample de: %i\n\n", header_son.bitspersample);

		// Fermeture des fichiers
	    fclose(entree);

    }
	return header_son.tailledata + tailleheader;
}

int main (int argc, char **argv) {

    // sd = socket descriptor
    // rc = valeur de retour
    int sd, rc, i,j;
    struct sockaddr_in CliAddr, ServAddr ;
    struct hostent * h;
    // Varibales concernant le fichier son
	FILE *entree;
    char buff[Taille_msg];
    int rd = 1, taille_fic[1], compt = 0, compt2 = 0;

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
	printf("Taille fichier pour envoi : %d octets\n",taille_fic[0]);
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
	printf("Nombre de octets envoyés : %i\n", compt2);

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

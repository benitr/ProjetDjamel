// Reception.c
// BENIT Romain

// Include généraux
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// Include particuliers
#include "udp_reception.h"

int main (int argc, char *argv[]) {

    printf("Réception de la musique en attente\n");

    // sd = socket descriptor
    // rc = valeur de retour
    int sd, rc, optval, i, clilen;
    ssize_t rcvf;
    struct sockaddr_in CliAddr, ServAddr ;
    // Varibales concernant le fichier son
    FILE *sortie;
    int wr, k = 0;
    char *buff, *buffer;
    int taille_fic[1];

    // Création d'un socket sur le réseau (PF_INET) en mode UDP (SOCK_DGRAM).
    sd = socket(AF_INET, SOCK_DGRAM, 17);
    // Test du socket
    if ( sd < 0 ) {
        perror("Erreur dans le socket");
        exit(1);
    }

    /* setsockopt: Handy debugging trick that lets
    * us rerun the server immediately after we kill it;
    * otherwise we have to wait about 20 secs.
    * Eliminates "ERROR on binding: Address already in use" error.
    */
    optval = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    // Défini le type d'adresse, le port et le flux entrant
    ServAddr.sin_family = AF_INET; // IPV4
    ServAddr.sin_port = htons (Ports); // Port
    ServAddr.sin_addr.s_addr = htonl (INADDR_ANY); // Flux entrant venant de ANY

    // Liaison avec le socket
    rc = bind(sd,(struct sockaddr *)&ServAddr, sizeof(ServAddr));
    // Test sur le bind
    if ( rc < 0 ) {
        perror("Erreur dans le bind");
        exit(2);
    }

    // Création en dynamique de notre buffer de stockage à la taille du fichier à recevoir
    buff = (char *)calloc(Taille_msg, sizeof(char));
    // Test du bufffer
    if(!buff) {
        perror("Erreur dans le calloc");
        exit(3);
    }

    clilen = sizeof(CliAddr);

    // Boucle infinie pour l'attente de service
    while(1) {
        // Test de la lecture du socket
        rcvf = recvfrom(sd, buff, Taille_msg, 0, (struct sockaddr *)&CliAddr, &clilen);
        if ( rcvf < 0 ) {
            perror("Erreur dans le recvfrom");
            exit(5);
        }

        if ( *buff == *DEB_FLAG ) {
        // Test du signal de debut de transmission
            printf("Demande de transmission de fichier\n");

            // Lecture du message suivant contenant la taille du fichier
            rcvf = recvfrom(sd, taille_fic, sizeof(int), 0, (struct sockaddr *)&CliAddr, &clilen);
            if ( rcvf < 0 ) {
                perror("Erreur dans le recvfrom");
                exit(5);
            }
            printf("Taille du fichier envoyée par le serveur : %d octets\n", taille_fic[0]);

            // Création du fichier son de destination
            if (!(sortie=fopen("Son.wav","wb"))) {
                perror("Erreur d'écriture du fichier destination");
                exit(4);
            }

            // Création en dynamique de notre buffer de stockage à la taille du fichier à recevoir
            buffer = (char *)calloc(taille_fic[0], sizeof(char));
            // Test du bufffer
            if(!buffer) {
                perror("Erreur dans le calloc");
                exit(3);
            }

        } else if ( *buff == *FIN_FLAG ) {
        // Test du signal de fin de transmission
            printf("Fin de fichier recu\n");
            free(buff);
            break;
        } else {
            // Remplissage du buffer de stockage
    		for(i=0 ; i < rcvf; i++ & k++){
    			buffer[k] = buff[i];
    		}
        }

    }

    // Ecriture dans le fichier de destination
    wr = fwrite(buffer, 1, taille_fic[0], sortie);
    if ( wr < 0 ) {
        perror("Erreur dans la lecture du fichier son");
        exit(6);
    }

    // Fermeture du fichier de destination
    close(sortie);

}

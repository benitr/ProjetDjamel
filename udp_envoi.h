// udp_envoi.h
// BENIT Romain

# ifndef _UDP_ENVOI_H
# define _UDP_ENVOI_H

# define Taille_msg 65000
# define Ports 1501
# define FIN_FLAG "FIN_DE_TRANSFERT"
# define DEB_FLAG "DEBUT_DE_TRANSFERT"

typedef struct header_son {
    int nbrvoix;
    int tailledata;
    int samplerate;
    int byterate;
    int blockalign;
    int bitspersample;
} header_son;

# endif

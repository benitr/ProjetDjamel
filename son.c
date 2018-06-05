// son.c
// MALLET Maxime

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

# ProjetDjamel

Pour compiler l'envoi : gcc Envoi.c -o Envoi -lm

Pour compiler la réception : gcc Reception.c -o Reception

Pour éxécuter :
	* ./Reception
	* ./Envoi <@IP ou nom de machine>
	
Pour le RPC :
	rpcgen -a es.x
	make -f Makefile.c
	

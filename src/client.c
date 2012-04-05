#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>

#include <wordexp.h>

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*
 * Autores:
 *
 * Barata Gonzalez, Jorge
 * Galan Galiano, Cristian
 *
 * */


int debug = 0;
int serverConnected;
int servicio;

void usage(char *program_name) {
	printf("Usage: %s [-d] -s <server> -p <port>\n", program_name);
}


void f_ping(){
	if (debug)
		printf("PING\n");

	// Write code here
	servicio = 1;

	struct timespec ini, end;

	clock_gettime(CLOCK_REALTIME, &ini);

	if(write(serverConnected, &servicio, sizeof(int)) < 0){
		perror("No se puede enviar el servicio de ping");
	}
	char ack;
	if(read(serverConnected, &ack, sizeof(char)) < 0){
		perror("Error recibiendo el servicio");
	}
	clock_gettime( CLOCK_REALTIME, &end);

    fprintf(stderr, "%f s\n", (float)(1.0*(1.0*end.tv_nsec - ini.tv_nsec*1.0)*1e-9 + 1.0*end.tv_sec - 1.0*ini.tv_sec));
}

void f_swap(char *src, char *dst){
	if (debug)
		printf("SWAP <SRC=%s> <DST=%s>\n", src, dst);

	// Write code here
	servicio = 2;

	//Le envia el tipo de servicio que es
	if(write(serverConnected, &servicio, sizeof(int)) <0 ){
		perror("No se puede enviar el servicio de swap");
	}

	//Obtiene los datos del fichero
    FILE *archivo;
    char caracteres[10];

    archivo = fopen(src,"r");
    if(archivo == NULL)
		exit(1);

    int total = 0;
    printf("\nEl contenido del archivo de prueba es \n\n");
    char* copia = calloc(total, sizeof(char));
    char* resultado;
    while (fgets(caracteres,10,archivo) != NULL)
    {
    	total = total + 10;
		resultado = calloc(total, sizeof(char));

		printf("%s \n",caracteres);

		strcpy(resultado, copia);
		strcat(resultado, caracteres);
		copia = calloc(total, sizeof(char));
		strcpy(copia, resultado);
    }
	printf("%s \n",copia);
	free(resultado);
    fclose(archivo);

	//Le envia la longitud del texto
	int longitud = strlen(copia);

	if(write(serverConnected, &longitud, sizeof(int)) < 0){
		perror("No se puede enviar el servicio de swap");
	}

	//Le envia un cadena
	if(write(serverConnected, copia, longitud) < 0){
		perror("No se puede enviar el servicio de swap");
	}

	//Recibe la cantidad de letras cambiadas
	int letrasCambiadas;
	if(read(serverConnected, &letrasCambiadas, sizeof(int)) < 0){
		perror("No se puede recibir el servicio de swap");
	}
	printf("%i \n",letrasCambiadas);

	//Recibe la nueva cadena
	if(read(serverConnected, copia, longitud) < 0){
		perror("No se puede recibir el servicio de swap");
	}
	printf("%s \n",copia);
	free(copia);
}

void f_hash(char *src){
	if (debug)
		printf("HASH <SRC=%s>\n", src);
	
	// Write code here
	servicio = 3;

	if(write(serverConnected, &servicio, sizeof(int)) <0 ){
		perror("No se puede enviar el servicio de hash");
	}

	//Obtiene los datos del fichero
    FILE *archivo;
    char caracteres[10];

    archivo = fopen(src,"r");
    if(archivo == NULL)
		exit(1);

    int total = 0;
    printf("\nEl contenido del archivo de prueba es \n\n");
    char* copia = calloc(total, sizeof(char));
    char* resultado;
    while (fgets(caracteres,10,archivo) != NULL)
    {
    	total = total + 10;
		resultado = calloc(total, sizeof(char));

		printf("%s \n",caracteres);

		strcpy(resultado, copia);
		strcat(resultado, caracteres);
		copia = calloc(total, sizeof(char));
		strcpy(copia, resultado);
    }
	printf("%s \n",copia);
	free(resultado);
    fclose(archivo);

	//Le envia la longitud del texto
	int longitud = strlen(copia);

	if(write(serverConnected, &longitud, sizeof(int)) < 0){
		perror("No se puede enviar el servicio de hash");
	}

	//Le envia una cadena
	if(write(serverConnected, copia, longitud) < 0){
		perror("No se puede enviar el servicio de hash");
	}

	//Recibe el valor de hash
	unsigned int hash;
	if(read(serverConnected, &hash, sizeof(unsigned int)) < 0){
		perror("No se puede recibir el servicio de hash");
	}
	printf("%u \n", hash);

	free(copia);
}

void f_check(char *src, int hash){
	if (debug)
		printf("CHECK <SRC=%s> <HASH=%d>\n", src, hash);
	
	// Write code here
	servicio = 4;

	if(write(serverConnected, &servicio, sizeof(int)) <0 ){
		perror("No se puede enviar el servicio de check");
	}

	//Obtiene los datos del fichero
    FILE *archivo;
    char caracteres[10];

    archivo = fopen(src,"r");
    if(archivo == NULL)
		exit(1);

    int total = 0;
    printf("\nEl contenido del archivo de prueba es \n\n");
    char* copia = calloc(total, sizeof(char));
    char* resultado;
    while (fgets(caracteres,10,archivo) != NULL)
    {
    	total = total + 10;
		resultado = calloc(total, sizeof(char));

		printf("%s \n",caracteres);

		strcpy(resultado, copia);
		strcat(resultado, caracteres);
		copia = calloc(total, sizeof(char));
		strcpy(copia, resultado);
    }
	printf("%s \n",copia);
	free(resultado);
    fclose(archivo);

	//Le envia la longitud del texto
	int longitud = strlen(copia);

	if(write(serverConnected, &longitud, sizeof(int)) < 0){
		perror("No se puede enviar el servicio de check");
	}

	//Le envia una cadena
	if(write(serverConnected, copia, longitud) < 0){
		perror("No se puede enviar el servicio de check");
	}

	//Le envia el valor hash
	unsigned int uhash = hash;
	if(write(serverConnected, &uhash, sizeof(unsigned int)) < 0){
		perror("No se puede enviar el servicio de check");
	}

	//Recibe si es correcto o falso la funcion resumen
	int correcto;
	if(read(serverConnected, &correcto, sizeof(int)) < 0){
		perror("No se puede recibir el servicio de check");
	}

	if(correcto == 0)
		printf("FAIL \n");
	else
		printf("OK \n");


}

void f_stat(){
	if (debug)
		printf("STAT\n");
	
	// Write code here
	servicio = 5;

	if(write(serverConnected, &servicio, sizeof(int)) <0 ){
		perror("No se puede enviar el servicio de stat");
	}

	//Recibe el valor de ping
	unsigned int ping;
	if(read(serverConnected, &ping, sizeof(unsigned int)) < 0){
		perror("No se puede recibir el servicio de stat");
	}
	printf("ping %u \n", ping);

	//Recibe el valor de swap
	unsigned int swap;
	if(read(serverConnected, &swap, sizeof(unsigned int)) < 0){
		perror("No se puede recibir el servicio de stat");
	}
	printf("swap %u \n", swap);

	//Recibe el valor de hash
	unsigned int hash;
	if(read(serverConnected, &hash, sizeof(unsigned int)) < 0){
		perror("No se puede recibir el servicio de stat");
	}
	printf("hash %u \n", hash);

	//Recibe el valor de check
	unsigned int check;
	if(read(serverConnected, &check, sizeof(unsigned int)) < 0){
		perror("No se puede recibir el servicio de stat");
	}
	printf("check %u \n", check);

	//Recibe el valor de stat
	unsigned int stat;
	if(read(serverConnected, &stat, sizeof(unsigned int)) < 0){
		perror("No se puede recibir el servicio de stat");
	}
	printf("stat %u \n", stat);

}

void quit(){
	if (debug)
		printf("QUIT\n");

	// Write code here
	servicio = 6;

	if(write(serverConnected, &servicio, sizeof(int)) <0 ){
		perror("No se puede enviar el servicio de quit");
	}
}

void shell() {
	char line[1024];
	char *pch;
	int exit = 0;
	
	wordexp_t p;
	char **w;
	int ret;
	
	memset(&p, 0, sizeof(wordexp));
	
	do {
		fprintf(stdout, "c> ");
		memset(line, 0, 1024);
		pch = fgets(line, 1024, stdin);
		
		if ( (strlen(line)>1) && ((line[strlen(line)-1]=='\n') || (line[strlen(line)-1]=='\r')) )
			line[strlen(line)-1]='\0';
		
		ret=wordexp((const char *)line, &p, 0);
		if (ret == 0) {
			w = p.we_wordv;
		
			if ( (w != NULL) && (p.we_wordc > 0) ) {
				if (strcmp(w[0],"ping")==0) {
					if (p.we_wordc == 1)
						f_ping();
					else
						printf("Syntax error. Use: ping\n");
				} else if (strcmp(w[0],"swap")==0) {
					if (p.we_wordc == 3)
						f_swap(w[1],w[2]);
					else
						printf("Syntax error. Use: swap <source_file> <destination_file>\n");
				} else if (strcmp(w[0],"hash")==0) {
					if (p.we_wordc == 2)
						f_hash(w[1]);
					else
						printf("Syntax error. Use: hash <source_file>\n");
				} else if (strcmp(w[0],"check")==0) {
					if (p.we_wordc == 3)
						f_check(w[1], atoi(w[2]));
					else
						printf("Syntax error. Use: check <source_file> <hash>\n");
				} else if (strcmp(w[0],"stat")==0) {
					if (p.we_wordc == 1)
						f_stat();
					else
						printf("Syntax error. Use: stat\n");
				} else if (strcmp(w[0],"quit")==0) {
					quit();
					exit = 1;
				} else {
					fprintf(stderr, "Error: command '%s' not valid.\n", w[0]);
				}
			}
			
			wordfree(&p);
		}
	} while ((pch != NULL) && (!exit));
}

int main(int argc, char *argv[]){
	char *program_name = argv[0];
	int opt, port=0;
	char *server, *port_s;
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "ds:p:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 's':
				server = optarg;
				break;
			case 'p':
				port_s = optarg;
				port = strtol(optarg, NULL, 10);
				break;
			case '?':
				if ((optopt == 's') || (optopt == 'p'))
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				usage(program_name);
				exit(EX_USAGE);
		}
	}
	
	if ((port < 1024) || (port > 65535)) {
		fprintf (stderr, "Error: Port must be in the range 1024 <= port <= 65535\n");
		usage(program_name);
		exit(EX_USAGE);
	}
	
	if (debug)
		printf("SERVER: %s PORT: %d\n",server, port);
	
	/*
	 *  EMPIEZA EL CODIGO PROPIO
	 *  NO SE SI HAY QUE USAR STDERR O STDOUT. Realmente no piden salida, se puede usar printf.
	 **/

	struct sockaddr_in serverIn;
	serverConnected = socket(AF_INET, SOCK_STREAM, 0);

	if(serverConnected < 0){
		perror("Error creando socket");
	}

	serverIn.sin_family = AF_INET;
	serverIn.sin_port = htons(port);
	serverIn.sin_addr.s_addr = INADDR_ANY;

	if(connect(serverConnected, (struct sockaddr *) &serverIn, sizeof(serverIn)) <0){
		perror("Error al conectar");
		exit(-1);
	}else
		shell();

	close(serverConnected);

	exit(EXIT_SUCCESS);
}


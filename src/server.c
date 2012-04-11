#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>

/*
 * Autores:
 *
 * Barata Gonzalez, Jorge
 * Galan Galiano, Cristian
 *
 * */

typedef struct{
	char* ip;
	int port;
	int descriptor;
} conexion;

/**
 * Funciones
 * */

void obtenerIpLocal(char* ip);
int crearConexion(int port);
int crearPeticion(conexion *sock);
void f_ping();
void f_swap();
void f_hash();
void f_check();
void f_stat();
void recibir(int clientConnected, char* copia, unsigned int longitud);


/**
 * Atributos Globales:
 * */

int debug = 0;

//Obtiene la ip local
void obtenerIpLocal(char* ip) {
	struct sockaddr_in host;
	char hostname[255];

	gethostname(hostname, 255);
	host.sin_addr = * (struct in_addr*) gethostbyname(hostname)->h_addr;
	ip = inet_ntoa(host.sin_addr);

}


//Crea la conexion
int crearConexion(int port){
	struct sockaddr_in server;

	//Se crea el socket
	int descriptorSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(descriptorSocket < 0){
		perror("Error creando socket");
	}

	//Se da los atributos del servidor
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(descriptorSocket, (struct sockaddr *) &server, sizeof (server)) < 0){
		perror("Error haciendo bind");
	}

	if(listen(descriptorSocket, 10) < 0){
		perror("Error haciendo listen");
	}

	return descriptorSocket;
}

int crearPeticion(conexion *hijo){

	int clientConnected = hijo->descriptor, port = hijo->port, servicio;
	char* ip = hijo->ip;
	unsigned int ping = 0, swap = 0, hash = 0, check = 0, stat = 0;

	while(1){
		//Recibe el numero de servicio
		if(read(clientConnected, &servicio, sizeof(int)) < 1){
			perror("Error recibiendo el servicio");
		}

		//Eleccion de servicio
		if(servicio == 1){
			f_ping(clientConnected);
			ping++;
			//Se imprime por pantalla
			fprintf(stderr, "s> %s:%i ping\n", ip, port);
		}else if(servicio == 2){
			f_swap(clientConnected, ip, port);
			swap++;
		}else if(servicio == 3){
			f_hash(clientConnected, ip, port);
			hash++;
		}else if(servicio == 4){
			f_check(clientConnected, ip, port);
			check++;
		}else if(servicio == 5){
			fprintf(stderr, "s> %s:%i init stat\n", ip, port);
			f_stat(clientConnected, ping, swap, hash, check, stat);
			fprintf(stderr, "s> %s:%i stat = %u %u %u %u %u\n", ip, port, ping, swap, hash, check, stat);
			stat++;
		}else if(servicio == 6){
			return 1;
		}
	}
}

void f_ping(int clientConnected){
	char ack;
	if(write(clientConnected, &ack, sizeof(char)) <0 ){
		perror("No se puede enviar el servicio de ping");
	}
	fprintf(stderr, "\n");
}

void f_swap(int clientConnected, char* ip, int port){
	//Recibe la longitud del texto
	unsigned int longitud;
	if(read(clientConnected, &longitud, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede recibir el servicio de swap: Longitud.");
		fprintf(stderr, "Error: %u", longitud);
		exit(-1);
	}

	//Se imprime por pantalla
	fprintf(stderr, "s>%s:%i init swap %u\n", ip, port, longitud);

	//Recibe la cadena
	char* copia = calloc(longitud, sizeof(char));
	recibir(clientConnected,copia, longitud);

	//Intercambia los valores de la cadena
	int i=0;
	unsigned int letrasCambiadas = 0;

	for(i=0; i< longitud; i++){
		if(copia[i] >= 'A' && copia[i] <= 'Z') {
			copia[i] = copia[i] + 32;    /* resta a c el valor ascii de A */
			letrasCambiadas++;
		}else if(copia[i] >= 'a' && copia[i] <= 'z') {
			copia[i] = copia[i] - 32;    /* resta a c el valor ascii de a */
			letrasCambiadas++;
		}
	}

	//Envia la cantidad de letras cambiadas
	if(write(clientConnected, &letrasCambiadas, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de swap: LetrasCambiadas");
		fprintf(stderr, "Error: %u", letrasCambiadas);
		exit(-1);
	}

	//Envia la nueva copia de la cadena
	if(write(clientConnected, copia, longitud) != longitud){
		perror("No se puede enviar el servicio de swap: Cadena");
		fprintf(stderr, "Error: %s", copia);
		exit(-1);
	}

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i swap = %u\n", ip, port, letrasCambiadas);
	fprintf(stderr, "\n");
	free(copia);
}

void f_hash(int clientConnected, char* ip, int port){
	//Recibe la longitud del texto
	unsigned int longitud;
	if(read(clientConnected, &longitud, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede recibir el servicio de hash: Longitud.");
	}

	//Se imprime por pantalla
	fprintf(stderr, "%s> s:%i init hash %u\n", ip, port, longitud);

	//Recibe la cadena
	char* copia = calloc(longitud, sizeof(char));
	recibir(clientConnected,copia, longitud);

	//Obtiene la funcion hash
	int i=0;
	unsigned int hash = 0;
	for(i=0; i< longitud; i++){
		hash = (hash + copia[i]) % 1000000000;
	}

	//Envia el hash
	if(write(clientConnected, &hash, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de hash: Hash.");
	}

	//Se imprime por pantalla
	fprintf(stderr, "s> %s:%i hash = %u\n", ip, port, hash);
	fprintf(stderr, "\n");

	free(copia);
}

void f_check(int clientConnected, char* ip, int port){
	//Recibe la longitud del texto
	unsigned int longitud;
	if(read(clientConnected, &longitud, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede recibir el servicio de check: Longitud.");
	}

	//Recibe la cadena
	char* copia = calloc(longitud, sizeof(char));
	recibir(clientConnected,copia, longitud);

	//Recibe el valor hash
	unsigned int hash = 0;
	if(read(clientConnected, &hash, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede recibir el servicio de check: Hash.");
	}

	//Se imprime por pantalla
	fprintf(stderr, "%s> s:%i init check %i %u\n", ip, port, longitud, hash);

	//Obtiene la funcion hash
	int i=0;
	unsigned int uhash = 0;
	for(i=0; i< longitud; i++){
		uhash = (uhash + copia[i]) % 1000000000;
	}

	//Comprueba si es correcta
	char correcto = 0;
	if(hash == uhash)
		correcto = 1;

	if(write(clientConnected, &correcto, sizeof(char)) != sizeof(char)){
		perror("No se puede enviar el servicio de check: Correcto.");
	}

	//Se imprime por pantalla
	if(correcto == 1)
		fprintf(stderr, "s> %s:%i check = OK\n", ip, port);
	else
		fprintf(stderr, "s> %s:%i check = FAIL\n", ip, port);

	//Se imprime por pantalla
	fprintf(stderr, "\n");

	free(copia);
}

void f_stat(int clientConnected, unsigned int ping, unsigned int swap, unsigned int hash,
		unsigned int check, unsigned int stat){
	//Envia ping
	if(write(clientConnected, &ping, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de stat: Ping.");
		exit(-1);
	}

	//Envia swap
	if(write(clientConnected, &swap, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de stat: Swap.");
		exit(-1);
	}

	//Envia hash
	if(write(clientConnected, &hash, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de stat: Hash.");
		exit(-1);
	}

	//Envia check
	if(write(clientConnected, &check, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de stat: Check.");
		exit(-1);
	}

	//Envia stat
	if(write(clientConnected, &stat, sizeof(unsigned int)) != sizeof(unsigned int)){
		perror("No se puede enviar el servicio de stat: Stat.");
		exit(-1);
	}
	fprintf(stderr, "\n");
}

void recibir(int clientConnected, char* copia, unsigned int longitud){
	//Envia la nueva copia de la cadena
	int datosRestantes = longitud;
	int enviado = 0;

	while (datosRestantes != 0){
		char* cadena = calloc(datosRestantes, sizeof(char));
		enviado = read(clientConnected, cadena, datosRestantes);
		datosRestantes = datosRestantes - enviado;
		strcat(copia, cadena);
		free(cadena);
	}
}



void usage(char *program_name) {
	printf("Usage: %s [-d] -p <port>\n", program_name);
}

int main(int argc, char *argv[]) {
	char *program_name = argv[0];
	int opt, port=0;
	char *port_s;
	
	// Parse command-line arguments
	while ((opt = getopt(argc, argv, "dp:")) != -1) {
		switch (opt) {
			case 'd':
				debug = 1;
				break;
			case 'p':
				port_s = optarg;
				port = strtol(optarg, NULL, 10);
				break;
			case '?':
				if (optopt == 'p')
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
		fprintf(stderr, "DEBUG ON\n");
	
	/*
	 *  EMPIEZA EL CODIGO PROPIO
	 **/

	//Crear configuracion inicial
	char* ip = calloc(16, sizeof(char));
	obtenerIpLocal(ip);
	int size, socket = crearConexion(atoi(argv[2]));
	fprintf(stderr, "s> init server %s:%s \n", ip, argv[2]);

	//Crear concurrencia
	struct sockaddr_in client;
	int clientConnected;
	char* ipClient;
	pthread_t child;

	while (1) {
		fprintf(stderr, "s> waiting\n");
		size = sizeof(client);

		//Ha llegado un cliente
		clientConnected = accept(socket, (struct sockaddr *) &client, (socklen_t *) &size);
		if(clientConnected < 0){
			perror("Error al aceptar las peticiones");
			exit(-1);
		}

		ipClient = inet_ntoa(client.sin_addr);
		port = ntohs(client.sin_port);
		fprintf(stderr, "s> accept %s:%i\n", ipClient, port);

		conexion hijo;
		hijo.ip = ipClient;
		hijo.port = port;
		hijo.descriptor = clientConnected;

		if(pthread_create(&child, NULL, (void *)crearPeticion, &hijo) < 0){
			perror("Error al crear el hilo");
			exit(-1);
		}
	}
	//Cierra la conexion
	close(clientConnected);

	//Cierra el socket
	close(socket);

	//Termina el main
	exit(1);

}

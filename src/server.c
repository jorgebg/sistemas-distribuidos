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

char* obtenerIpLocal();
int crearConexion(int port);
int crearPeticion(int *sock);
void f_ping();
void f_swap();
void f_hash();
void f_check();
void f_stat();

/**
 * Atributos Globales:
 * */

int debug = 0;


char* obtenerIpLocal() {
	struct sockaddr_in host;
	char hostname[255];

	gethostname(hostname, 255);
	host.sin_addr = * (struct in_addr*) gethostbyname(hostname)->h_addr;
	return inet_ntoa(host.sin_addr);

}

int crearConexion(int port){
	struct sockaddr_in server;
	int descriptorSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(descriptorSocket < 0){
		perror("Error creando socket");
	}

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

int crearPeticion(int *client){
	int clientConnected = *client, servicio;

	if(read(clientConnected, &servicio, sizeof(int)) < 1){
		perror("Error recibiendo el servicio");
	}

	//Eleccion de servicio
	if(servicio == 1){
		f_ping();
	}else if(servicio == 2){
		f_swap();
	}else if(servicio == 3){
		f_hash();
	}else if(servicio == 4){
		f_check();
	}else if(servicio == 5){
		f_stat();
	}
	return 1;
}

void f_ping(){
	// Write code here
	fprintf(stderr, "s> ping\n");
}

void f_swap(){
	// Write code here
	fprintf(stderr, "s> swap\n");
}

void f_hash(){
	// Write code here
	fprintf(stderr, "s> hash\n");
}

void f_check(){
	// Write code here
	fprintf(stderr, "s> check\n");
}

void f_stat(){
	// Write code here
	fprintf(stderr, "s> stat\n");
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
	 *  NO SE SI HAY QUE USAR STDERR O STDOUT. Realmente no piden salida, se puede usar printf.
	 **/

	//Crear configuracion inicial
	char* ip = obtenerIpLocal();
	int size, socket = crearConexion(atoi(argv[2]));
	fprintf(stderr, "s> init server %s %s \n", ip, argv[2]);

	//Crear concurrencia
	struct sockaddr_in client;
	int clientConnected;
	char* ipClient;

	while (1) {
		pthread_t child;
		fprintf(stderr, "s> waiting\n");
		size = sizeof(client);

		//Ha llegado un cliente
		clientConnected = accept(socket, (struct sockaddr *) &client, &size);
		if(clientConnected < 0){
			perror("Error al aceptar las peticiones");
		}

		ipClient = inet_ntoa(client.sin_addr);
		fprintf(stderr, "s> accept %s:\n", ipClient);

		if(pthread_create(&child, NULL, (void *)crearPeticion, &clientConnected) < 0){
			perror("Error al crear el hilo");
		}

	}

	//Cierra el socket
	close(socket);

	//Termina el main
	exit(1);

}

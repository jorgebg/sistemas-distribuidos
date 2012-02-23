#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <ctype.h>



int debug = 0;


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
	
	// Write code here
	
	while (1) {
		// Write code here
	}
}

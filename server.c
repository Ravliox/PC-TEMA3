#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>


#define MAXLEN 50000
#define MAX_CLIENTS 5
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(1);
}

char* gethost(char *name)
{
	struct hostent* result = malloc (sizeof(struct hostent*));
	printf ("%d", strlen(name));
    result = gethostbyname(name);
    //printf("%s\n", result->h_name);
    //char **t = result->h_aliases;

    struct in_addr **a = malloc(sizeof(struct in_addr **));
    a = result->h_addr_list;

    struct in_addr *l = malloc(sizeof(struct in_addr *));
    char *line = malloc(200);
    line = inet_ntoa(**a);
    printf("ADDRES: %s\n", line);
	return line;
}

ssize_t Readline(int sockd, void * vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, * buffer;

    buffer = vptr;

    for (n = 1; n < maxlen; n++) {
        if ((rc = read(sockd, & c, 1)) == 1) { * buffer++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return 0;
            else
                break;
        } else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }

    // * b
	printf("%s", buffer);
    return n;
}

/**
 * Trimite o comanda HTTP si asteapta raspuns de la server.
 * Comanda trebuie sa fie in buffer-ul sendbuf.
 * Sirul expected contine inceputul raspunsului pe care
 * trebuie sa-l trimita serverul in caz de succes (de ex. codul
 * 200). Daca raspunsul semnaleaza o eroare se iese din program.
 */
void send_command(int sockfd, char sendbuf[], char * expected) {
    char recvbuf[MAXLEN];
    char header[MAXLEN];
    int nbytes;
    // sprintf (header, "Content-type: text/html\n\n");
    // send(sockfd, header, strlen(header), 0);

    //sprintf (sendbuf, "GET / HTTP/1.0\n\n");
    printf ("%s", sendbuf);

    send(sockfd, sendbuf, strlen(sendbuf), 0);
    
    recv (sockfd, expected, sizeof(expected), 0);
    recv (sockfd, recvbuf, sizeof(recvbuf), 0);
    printf ("%s", expected);
    printf ("%s", recvbuf);

    //TODO: trimitere comanda
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i, j;

    fd_set read_fds; //multimea de citire folosita in select()
    fd_set tmp_fds;  //multime folosita temporar
    int fdmax;       //valoare maxima file descriptor din multimea read_fds

    if (argc < 2)
    {
	fprintf(stderr, "Usage : %s port\n", argv[0]);
	exit(1);
    }

    //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	error("ERROR opening socket");

    portno = atoi(argv[1]);

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // foloseste adresa IP a masinii
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
	error("ERROR on binding");

    listen(sockfd, MAX_CLIENTS);

    //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;
	//gethost("ocw.cs.pub.ro");
    // main loop
    while (1)
    {
	tmp_fds = read_fds;
	if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1)
	    error("ERROR in select");

	for (i = 0; i <= fdmax; i++)
	{
	    if (FD_ISSET(i, &tmp_fds))
	    {

		if (i == sockfd)
		{
		    clilen = sizeof(cli_addr);
		    if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
		    {
			error("ERROR in accept");
		    }
		    else
		    {
			//adaug noul socket intors de accept() la multimea descriptorilor de citire
			FD_SET(newsockfd, &read_fds);
			if (newsockfd > fdmax)
			{
			    fdmax = newsockfd;
			}
		    }
		    //printf("Noua conexiune de la %s, port %d, socket_client %d\n ", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
		}

		else
		{
		    // am primit date pe unul din socketii cu care vorbesc cu clientii
		    //actiunea serverului: recv()
		    memset(buffer, 0, BUFLEN);
		    if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0)
		    {
			if (n == 0)
			{
			    //conexiunea s-a inchis
			    printf("selectserver: socket %d hung up\n", i);
			}
			else
			{
			    error("ERROR in recv");
			}
			close(i);
			FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care
		    }

		    else
		    { //recv intoarce >0
			//printf("Am primit de la clientul de pe socketul %d, mesajul:%d %s", i, strlen(buffer), buffer);
			// int cached = 1;
			//printf ("%s", buffer);
			char* line = strtok (buffer, "\n");
			line = strtok (NULL, "\n");
			//printf ("%s\n\n", line);
			char* host = strtok (line, " ");
			host = strtok (NULL, " \n");
			//printf ("%d %s\n\n", strlen(host), host);
			char* s = malloc (sizeof(char) * (strlen(host) - 1));
			int k = 0;
			while (k < strlen(host) - 1){
				s[k] = host[k];
				k++;
			}
			//printf("%s\n\n");
			printf ("%s", buffer);
			if (strcmp (s, "ocw.cs.pub.ro") == 0){
				printf ("FUCK C\n\n");
				gethost("ocw.cs.pub.ro");

			}
			else
				printf ("I don't even know anymore");

			// 	k++;
			// }
			// char* s = malloc (sizeof(char) * strlen (buffer));
			// int k = 0;
			// while (k < strlen(buffer) - 2){
			// 	s[k] = buffer[k];
			// 	k++;
			// }
			// printf ("WHAT DA FUCK IS HAPPENING");
			// printf ("HELLO%s", s);
			// struct sockaddr_in addr;
			// memset((char *)&serv_addr, 0, sizeof(serv_addr));
    		// addr.sin_family = AF_INET;
    		// addr.sin_port = htons(80);
			// inet_aton(gethost(s), & addr.sin_addr);
			// int socketn;
			// socketn = socket(AF_INET, SOCK_STREAM, 0);

			// if (connect(socketn, (struct sockaddr * ) & addr, sizeof(addr)) < 0) {
       		// 	printf("Eroare la conectare\n");
        	// 	exit(-1);
    		// }

			// char* sendbuf = malloc (sizeof(char) * 250);
			// sprintf (sendbuf, "GET / HTTP/1.0\n\n");
			// memset(buffer, 0, sizeof(buffer));
			// char buffer2[MAXLEN];
			// send_command(socketn, sendbuf, buffer2);
 


		}
	    }
	}
    }
	}
    close(sockfd);

    return 0;
}

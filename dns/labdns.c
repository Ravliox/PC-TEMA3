#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// struct hostent{
//     char *h_name;
//     char **h_aliases;
//     int h_addrtype;
//     int h_length;
//     int
// }

int main (int argc, char* argv[]){
    char* type = strdup (argv[1]);
    char* argument = strdup (argv[2]);
    struct hostent* result = malloc (sizeof(struct hostent*));

    if (strcmp (type, "-n") == 0){
        result = gethostbyname(argument);
        printf ("%s\n", result->h_name);
        char**t = result->h_aliases;
        char* c;
        for (c = *t; c; c=*++t){
            printf ("ALIAS: %s\n", c);
        }
        struct in_addr** a = malloc (sizeof (struct in_addr**));
        a = result->h_addr_list;
        
        struct in_addr* l = malloc (sizeof (struct in_addr*));
        for (l = *a; l; l=*++a){
            char* line = malloc (200);
            line = inet_ntoa (**a);
            printf ("ADDRESS %s\n", line);
        }
    }

    if (strcmp (type, "-a") == 0){
        struct in_addr x;
        inet_aton (argument, &x);
        result = gethostbyaddr (&x, sizeof(x), PF_INET);
        printf ("%s\n", result->h_name);
        char**t = result->h_aliases;
        char* c;
        for (c = *t; c; ++t){
            printf ("ALIAS: %s\n", c);
        }
        struct in_addr** a = result->h_addr_list;
        struct in_addr* l;
        for (l = *a; l; l=*++a){
            char* line = malloc (200);
            line = inet_ntoa (**a);
            printf ("ADDRESS %s\n", line);
        }
        
    }

}
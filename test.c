#include <string.h>

int main (){
    char* string = malloc (sizeof(char) *1000);

    sprintf (string, "GET google.ro HTTP/1.0");

    char* token = strtok (string, " ");
    char* command = strtok (NULL, " ");
    printf ("%s, %s", token, command);
}
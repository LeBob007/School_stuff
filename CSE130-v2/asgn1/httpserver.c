#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <unistd.h> // write
#include <string.h> // memset
#include <stdlib.h> // atoi
#include <stdbool.h> // true, false
#include <errno.h>

#define BUFFER_SIZE 4096

struct httpObject {
    /*
        Create some object 'struct' to keep track of all
        the components related to a HTTP message
        NOTE: There may be more member variables you would want to add
    */
    char method[5];         // PUT, HEAD, GET
    char filename[28];      // what is the file we are worried about
    char httpversion[9];    // HTTP/1.1
    ssize_t content_length; // example: 13
    int status_code;
    uint8_t buffer[BUFFER_SIZE + 1];
};

/*
    \brief 1. Want to read in the HTTP message/ data coming in from socket
    \param client_sockd - socket file descriptor
    \param message - object we want to 'fill in' as we read in the HTTP message
*/
void read_http_response(ssize_t client_sockd, struct httpObject* message) {
    
    if(message->status_code == 500){
        return;
    }
    
    message->status_code = 0;
    message->content_length = 0;
    ssize_t bytes = recv(client_sockd, message->buffer, BUFFER_SIZE, 0);
    message->buffer[bytes] = 0; // null terminate
    char str1[BUFFER_SIZE];
    int n;
    
    sscanf(message->buffer, "%s %s %s\r\n", message->method, str1, message->httpversion);
    if(str1[0] == '/'){
        memcpy(message->filename, str1+1, sizeof(message->filename)-1);
    }
    else{
        message->status_code = 400;
        return;
    }
    
    sscanf(str1, "%*[-_/a-zA-Z0-9]%n", &n);
    if(str1[n] != 0 || n > 28){
        message->status_code = 400;
        return;
    }
    
    char ** res = NULL;
    char * ch = strtok (message->buffer, " \r\n");
    int spaces = 0;
    while (ch){
    res = (char**)realloc(res, sizeof(char*) * ++spaces);
        if (res == NULL)
            exit(-1);
        res[spaces-1] = ch;
        ch = strtok(NULL, " \r\n");
    }
    res = (char**)realloc(res, sizeof(char*) * (spaces+1));
    res[spaces] = NULL;
    
    for(int k = 0; k < spaces-1; k++){
        if(strcmp(res[k], "Content-Length:") == 0){
            sscanf(res[k+1], "%ld", &message->content_length);
            break;
        }
    }
    
    return;
}

/*
    \brief 2. Want to process the message we just recieved
*/
void process_request(ssize_t client_sockd, struct httpObject* message) {
    
    if(message->status_code == 400 || message->status_code == 500){
        return;
    }
    
    int fd;
    struct stat st;
    
    if(strcmp(message->method, "PUT") == 0){
        fd = open(message->filename, O_CREAT|O_WRONLY|O_TRUNC, 0666);
        if(fd == -1){
           if(errno == EACCES) {
                message->status_code = 403;
            }
            else if(errno == ENOENT) {
                message->status_code = 404;
            }
            close(fd);
            return;
        }
        else{
            message->status_code = 201;
        }
        
        ssize_t bytes;
        uint8_t response_body[BUFFER_SIZE + 1];
        
        while(message->content_length > BUFFER_SIZE){
            bytes = read(client_sockd, response_body, BUFFER_SIZE);
            if(bytes == -1){
                message->status_code = 500;
            }
            response_body[bytes] = 0; // null terminate
            bytes = write(fd, response_body, BUFFER_SIZE);
            if(bytes == -1){
                message->status_code = 500;
            }
            message->content_length -= BUFFER_SIZE;
        }
        bytes = read(client_sockd, response_body, message->content_length);
        if(bytes == -1){
            message->status_code = 500;
        }
        bytes = write(fd, response_body, message->content_length);
        if(bytes == -1){
            message->status_code = 500;
        }
        close(fd);
        message->content_length = 0;
        
    }
    
    else if(strcmp(message->method, "GET") == 0){
        fd = open(message->filename, O_RDONLY);
        if(fd == -1){
           if(errno == EACCES) {
                message->status_code = 403;
            }
            else if(errno == ENOENT) {
                message->status_code = 404;
            }
            close(fd);
            return;
        }
        else{
            message->status_code = 200;
        }
        fstat(fd, &st);
        message->content_length = st.st_size;
        
    }
    
    else{
        fd = open(message->filename, O_RDONLY);
        if(fd == -1){
           if(errno == EACCES) {
                message->status_code = 403;
            }
            else if(errno == ENOENT) {
                message->status_code = 404;
            }
            close(fd);
            return;
        }
        else{
            message->status_code = 200;
        }
        fstat(fd, &st);
        message->content_length = st.st_size;
    }
    
    close(fd);
    return;
}

/*
    \brief 3. Construct some response based on the HTTP request you recieved
*/
void construct_http_response(ssize_t client_sockd, struct httpObject* message) {
    
    ssize_t bytes;
    int fd, k;
    
    uint8_t response_body[BUFFER_SIZE + 1];
    uint8_t status_string[25] = {0};
    switch(message->status_code){
        case 200:
            snprintf(status_string, 25, "OK");
            break;
        case 201:
            snprintf(status_string, 25, "Created");
            break;
        case 400:
            snprintf(status_string, 25, "Bad Request");
            break;
        case 403:
            snprintf(status_string, 25, "Forbidden");
            break;
        case 404:
            snprintf(status_string, 25, "Not Found");
            break;
        case 500:
            snprintf(status_string, 25, "Internal Server Error");
            break;
    }
    uint8_t response_string[100];
    
    k = snprintf(response_string, 100, "%s %d %s\r\nContent-Length: %ld\r\n\r\n", message->httpversion, message->status_code, status_string, message->content_length);
    send(client_sockd, response_string, k, 0);
    
    if(message->status_code == 400 || message->status_code == 403 || message->status_code == 404 || message->status_code == 500){
        return;
    }
    else{
        memset(message->buffer, '\0', BUFFER_SIZE + 1);
        if(strcmp(message->method, "GET") == 0){
            fd = open(message->filename, O_RDONLY);
            while(message->content_length > BUFFER_SIZE){
                bytes = read(fd, response_body, BUFFER_SIZE);
                if(bytes == -1){
                    message->status_code = 500;
                }
                response_body[bytes] = 0; // null terminate
                bytes = write(client_sockd, response_body, BUFFER_SIZE);
                if(bytes == -1){
                    message->status_code = 500;
                }
                message->content_length -= BUFFER_SIZE;
            }
            bytes = read(fd, response_body, message->content_length);
            if(bytes == -1){
                message->status_code = 500;
            }
            bytes = write(client_sockd, response_body, message->content_length);
            if(bytes == -1){
                message->status_code = 500;
            }
            close(fd);
        }
    }
    
    return;
}


int main(int argc, char** argv) {
    
    if(argc != 2) {
        printf("Usage: ./httpserver port \n");
        exit(EXIT_FAILURE);
    }

    /*
        Create sockaddr_in with server information
    */
    char* port = argv[1];
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t addrlen = sizeof(server_addr);

    /*
        Create server socket
    */
    int server_sockd = socket(AF_INET, SOCK_STREAM, 0);

    // Need to check if server_sockd < 0, meaning an error
    if (server_sockd < 0) {
        perror("socket");
    }

    /*
        Configure server socket
    */
    int enable = 1;

    /*
        This allows you to avoid: 'Bind: Address Already in Use' error
    */
    int ret = setsockopt(server_sockd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    /*
        Bind server address to socket that is open
    */
    ret = bind(server_sockd, (struct sockaddr *) &server_addr, addrlen);

    /*
        Listen for incoming connections
    */
    ret = listen(server_sockd, 5); // 5 should be enough, if not use SOMAXCONN

    if (ret < 0) {
        return EXIT_FAILURE;
    }

    /*
        Connecting with a client
    */
    struct sockaddr client_addr;
    socklen_t client_addrlen;

    struct httpObject message;

    while (true) {
        int client_sockd = accept(server_sockd, &client_addr, &client_addrlen);
        if(client_sockd == -1){
            message.status_code = 500;
        }

        read_http_response(client_sockd, &message);

        process_request(client_sockd, &message);

        construct_http_response(client_sockd, &message);

        close(client_sockd);
        
    }

    return EXIT_SUCCESS;
}

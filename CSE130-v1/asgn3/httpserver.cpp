#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <inttypes.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

#define BUFFER_SIZE 32768

uint8_t buffer[BUFFER_SIZE];
char clientMessage[BUFFER_SIZE];
char clientBody[BUFFER_SIZE];
char response[BUFFER_SIZE];

struct Entry {
    uint8_t *fileName;
    long fileSize;
    char *requestType;
    int sock;
    int status;
} requests[100], available;

struct args {
    int sock;
};

char ** splitString(char *);


int main(int argc, char **argv)
{
    
    if(argc < 2) {
        printf("Usage: ./httpserver host_name [port_number] [-l log_file] [-N thread_num] \n");
        exit(EXIT_FAILURE);
    }

    struct hostent *hent = gethostbyname(argv[1]);
    struct sockaddr_in addr;
    memcpy(&addr.sin_addr.s_addr, hent->h_addr, hent->h_length);
    if(isdigit(*argv[2])) {
        char *p = argv[2];
        int port = atoi(p);
        addr.sin_port = htons(port);
    } else {
        addr.sin_port = htons(80);
    }
    addr.sin_family = AF_INET;
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        exit(1);
    }

    int enable = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if(listen(sock, 128) == -1) {
        perror("listen");
        exit(1);
    }
    
    fprintf(stdout, "Listening in on %s, port %s\n", argv[1], argv [2]);
    fprintf(stdout, "Waiting on connection....\n");
    int cl;
    while((cl = accept(sock, NULL, NULL)) > 0) {
        fprintf(stdout, "Connected....\n");
        int length = recv(cl, clientMessage, BUFFER_SIZE, 0);
        clientMessage[length] = '\0';
        char temp[BUFFER_SIZE];
        
        strcpy(temp, clientMessage);
        temp[strlen(temp)-1] = '\0';\
        char **strArray = splitString(clientMessage);
        char *fileName = strArray[1];
        int status, fileSize = 0, fd;
        char statusString[100];
        struct stat st;\
        if(strcmp(strArray[0], "GET") == 0){
            fd = open(fileName, O_RDONLY);
            status = 200;
            sprintf(statusString, "OK");
        }
        else/* if(strcmp(strArray[0], "PUT") == 0)*/{
            if(access(fileName, F_OK) == 0){
                fd = open(fileName, O_TRUNC | O_RDWR);
            }
            else {
                fd = open(fileName, O_CREAT | O_RDWR, 0666);
            }
            status = 201;
            sprintf(statusString, "Created");
        }
        if(fd == -1 || strlen(strArray[1]) != 27){
            fileSize = 0;
            if(errno == EACCES) {
                status = 403;
                sprintf(statusString, "Forbidden");
            }
            else if(errno == ENOENT) {
                status = 404;
                sprintf(statusString, "Not Found");
            }
            else {
                status = 400;
                sprintf(statusString, "Bad Request");
            }
            sprintf(response, "HTTP/1.1 %d %s\r\nContent-Length: %d\r\n\r\n", status, statusString, fileSize);
            send(cl, response, strlen(response), 0);
        }
        else{
            fstat(fd, &st);
            fileSize = st.st_size;
            if(strcmp(strArray[0], "GET") == 0){
                sprintf(response, "HTTP/1.1 %d %s\r\nContent-Length: %d\r\n\r\n", status, statusString, fileSize);
                send(cl, response, strlen(response), 0);
                while(read(fd, clientBody, 1)>0){
                    write(cl, clientBody, 1);
                }
            }
            else {
                sprintf(response, "HTTP/1.1 %d %s\r\nContent-Length: %d\r\n\r\n", status, statusString, fileSize);
                send(cl, response, strlen(response), 0);
                while((length = recv(cl, clientBody, BUFFER_SIZE, 0)) > 0)
                    write(fd, clientBody, length);
            }
            close(fd);
        }
        close(cl);
    }

    close(sock);
    return 0;
}

char **splitString(char *mstring){
    char ** res = NULL;
    char * ch = strtok (mstring, " \r\n");
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
    res[spaces-1][strlen(res[spaces-1])-1] = '\0';
    
    return res;
}


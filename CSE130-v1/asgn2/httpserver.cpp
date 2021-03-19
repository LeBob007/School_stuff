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

int getFileSize(const char *);
char ** splitString(char *);
int sendFile(int, const char *, const uint8_t *);
bool sendData(int, const char *);


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
    
    fprintf(stderr, "Listening in on %s, port %s\n", argv[1], argv [2]);

    int cl;
    while((cl = accept(sock, NULL, NULL)) > 0) {
        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            printf("%s", strerror(errno));
            exit(1);
        }
        int length = recv(cl, clientMessage, BUFFER_SIZE, 0);
        write(cl, clientMessage, 100);
        clientMessage[length] = '\0';
        char temp[BUFFER_SIZE];
        
        strcpy(temp, clientMessage);
        temp[strlen(temp)-1] = '\0';
        
        char **strArray = splitString(clientMessage);
        fprintf(stderr, "%s %s %s", strArray[0], strArray[3], strArray[2]);
        char *fileName = strArray[1];
        int fd = open(fileName, O_RDWR);
        int status, fileSize;
        struct stat st;
        fstat(fd, &st);
        fileSize = st.st_size;
        if(fd == -1){
            status = 404;
        }
        status = 
        
        
        
        printf("%s", strerror(errno));
        close(sock);
        exit(1);
    }

    close(sock);
    return 0;
}

char **splitString(char *mstring){
    char ** res = NULL;
    char * ch = strtok (mstring, " ");
    int spaces = 0;
    while (ch){
        res = (char**)realloc(res, sizeof(char*) * ++spaces);
        if (res == NULL)
            exit(-1);
        
        res[spaces-1] = ch;
        ch = strtok(NULL, " ");
    }
    res = (char**)realloc(res, sizeof(char*) * (spaces+1));
    res[spaces] = NULL;
    res[spaces-1][strlen(res[spaces-1])-1] = '\0';
    
    return res;
}


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

#define BUFFER_SIZE 32768

int tokenCnt(char *message)
{
    char *pch;
    int count = 0;
    pch = strtok(message, " ");
    while(pch != NULL) {
        count++;
        pch = strtok(NULL, " ");
    }
    return count;
}

char *parseStr(char* message)
{
    char *pch;
    int count = tokenCnt(message);
    char *tokens[count];
    memset(tokens, '\0', sizeof(char) * count);
    int i = 0;
    pch = strtok(message, " ");
    while(pch != NULL) {
        tokens[i++] = pch;
        pch = strtok(NULL, " ");
    }
    return *tokens;
}

int main(int argc, char **argv)
{
    uint8_t buffer[BUFFER_SIZE];
    char *message = new char[BUFFER_SIZE];
    char *tokens = new char[BUFFER_SIZE];

    if(argc < 2) {
        printf("Usage: %s host_name [port_number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct hostent *hent = gethostbyname(argv[1]);
    struct sockaddr_in addr;
    memcpy(&addr.sin_addr.s_addr, hent->h_addr, hent->h_length);
    if(argc == 3) {
        char *p = argv[2];
        int port = atoi(p);
        printf("%d", port);
        addr.sin_port = htons(port);
    } else {
        addr.sin_port = htons(80);
    }
    addr.sin_family = AF_INET;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        printf("%s", strerror(errno));
        exit(1);
    }

    int enable = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("%s", strerror(errno));
        close(sock);
        exit(1);
    }

    if(listen(sock, 0) == -1) {
        printf("%s", strerror(errno));
        close(sock);
        exit(1);
    }

    int cl;
    if((cl = accept(sock, NULL, NULL)) == -1) {
        printf("%s", strerror(errno));
        close(sock);
        exit(1);
    }
    printf("%%");
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("%s", strerror(errno));
        close(sock);
        exit(1);
    }
    printf("%%");
    while(1) {
        printf("%%");
        read(cl, message, 1);
        tokens = parseStr(message);
        for(int i = 2; i < BUFFER_SIZE; i++) {
            if(strcmp(&tokens[i], "-T") == 0) {
                int fd1 = open(&tokens[i+1], O_RDONLY);
                int fd2 = open(&tokens[i+3], O_WRONLY | O_CREAT | O_TRUNC);
            }
        }
    }

    close(sock);
    return 0;
}

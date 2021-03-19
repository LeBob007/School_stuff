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
#include <pthread.h>
#include <limits.h>
#include <err.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUFFER_SIZE 32768


int hcRequest;
int parallelOp;
int requestCounter;
const char checkReq[] = "GET /healthcheck HTTP/1.1\r\nHost: localhost:1234\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\n";
const char failResponse[] = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
int numServers;
int bestServerport;
pthread_mutex_t Mainlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Maincondition_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

struct server{
    int port;
    bool alive;
    int total_requests;
    int fail_requests;
    pthread_mutex_t lock;
    int serverfd;
} servers[1000];

struct healthChecker{
    pthread_t hcProbe_id;
    pthread_cond_t condition_var;
    pthread_mutex_t* lock;
    uint8_t buffer[BUFFER_SIZE];
};

struct parallel{
    int client_sockd;
    int id;
    pthread_t parallel_id;
    pthread_cond_t condition_var;
    pthread_mutex_t* lock;
    
};

/*
 * client_connect takes a port number and establishes a connection as a client TO THE HTTPSERVER
 * connectport: port number of server to connect to
 * returns: valid socket if successful, -1 otherwise
 */
int client_connect(uint16_t connectport) {
    int connfd;
    struct sockaddr_in servaddr;

    connfd=socket(AF_INET,SOCK_STREAM,0);
    if (connfd < 0)
        return -1;
    memset(&servaddr, 0, sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(connectport);

    /* For this assignment the IP address can be fixed */
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    if(connect(connfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        return -1;
    return connfd;
}

/*
 * server_listen takes a port number and creates a socket to listen on that port FOR CLIENT CONNECTIONS
 * port: the port number to receive connections
 * returns: valid socket if successful, -1 otherwise
 */
int server_listen(int port) {
    int listenfd;
    int enable = 1;
    struct sockaddr_in servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
        return -1;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
        return -1;
    if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof servaddr) < 0)
        return -1;
    if (listen(listenfd, 500) < 0)
        return -1;
    return listenfd;
}

/*
 * bridge_connections send up to 100 bytes from fromfd to tofd
 * fromfd, tofd: valid sockets
 * returns: number of bytes sent, 0 if connection closed, -1 on error
 */
int bridge_connections(int fromfd, int tofd) {
    char recvline[BUFFER_SIZE];
    int n = recv(fromfd, recvline, BUFFER_SIZE, 0);
    if (n < 0) {
        printf("connection error receiving\n");
        return -1;
    } else if (n == 0) {
        printf("receiving connection ended\n");
        return 0;
    }
    recvline[n] = '\0';
    n = send(tofd, recvline, n, 0);
    if (n < 0) {
        printf("connection error sending\n");
        return -1;
    } else if (n == 0) {
        printf("sending connection ended\n");
        return 0;
    }
    return n;
}

/*
 * bridge_loop forwards all messages between both sockets until the connection
 * is interrupted. It also prints a message if both channels are idle.
 * sockfd1, sockfd2: valid sockets
 */
void bridge_loop(int sockfd1, int sockfd2) {
    fd_set set;
    struct timeval timeout;

    int fromfd, tofd;
    while(1) {
        // set for select usage must be initialized before each select call
        // set manages which file descriptors are being watched
        FD_ZERO (&set);
        FD_SET (sockfd1, &set);
        FD_SET (sockfd2, &set);

        // same for timeout
        // max time waiting, 3 seconds, 0 microseconds
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        // select return the number of file descriptors ready for reading in set
        switch (select(FD_SETSIZE, &set, NULL, NULL, &timeout)) {
            case -1:
                printf("error during select, exiting\n");
                return;
            case 0:
                send(sockfd1, failResponse, sizeof(failResponse), 0);
                return;
            default:
                if (FD_ISSET(sockfd1, &set)) {
                    fromfd = sockfd1;
                    tofd = sockfd2;
                } else if (FD_ISSET(sockfd2, &set)) {
                    fromfd = sockfd2;
                    tofd = sockfd1;
                } else {
                    return;
                }
        }
        if (bridge_connections(fromfd, tofd) <= 0){
            return;
        }
    }
}

int determineBest(){
    int bestPort = -1;
    int total = INT_MAX;
    int fail = INT_MAX;
    for(int i = 0; i < numServers; i++){
        pthread_mutex_lock(&servers[i].lock);
        if(servers[i].alive == false){
            pthread_mutex_unlock(&servers[i].lock);
            continue;
        }
        if(servers[i].total_requests < total){
            bestPort = servers[i].port;
            total = servers[i].total_requests;
            fail = servers[i].fail_requests;
        }
        else if (servers[i].total_requests == total){
            if (servers[i].fail_requests < fail){
                bestPort = servers[i].port;
                total = servers[i].total_requests;
                fail = servers[i].fail_requests;
            }
        }
        pthread_mutex_unlock(&servers[i].lock);
    }
    return bestPort;
}

void* probeHealthcheck(void* thread){
    
    struct healthChecker* probeThread = (struct healthChecker*)thread;
    struct timeval now;
    struct timespec ts;
    
    while(true){
        
        for(int i = 0; i <  numServers; i++){
            pthread_mutex_lock(&servers[i].lock);
            int connect = client_connect(servers[i].port);
            if(connect == -1){
                servers[i].alive = false;
                pthread_mutex_unlock(&servers[i].lock);
                continue;
            }
            int n = send(connect, checkReq, sizeof(checkReq), 0);
            int t = 0;
            memset(probeThread->buffer, 0, sizeof(probeThread->buffer));
            while(1){
                n = recv(connect, probeThread->buffer+t, BUFFER_SIZE-1-t, 0);
                if(n < 0){
                    return;
                }
                if(n == 0){
                    break;
                }
                t += n;
            }
            char ** res = NULL;
            char * a;
            char * ch = strtok_r(probeThread->buffer, " \r\n", &a);
            int spaces = 0;
            while (ch){
                res = (char**)realloc(res, sizeof(char*) * ++spaces);
                if (res == NULL)
                    exit(-1);
                res[spaces-1] = ch;
                ch = strtok_r(NULL, " \r\n", &a);
            }
            res = (char**)realloc(res, sizeof(char*) * (spaces+1));
            res[spaces] = NULL;
            if(strcmp(res[1], "200") != 0){
                servers[i].alive = false;
                pthread_mutex_unlock(&servers[i].lock);
                continue;
            }
            servers[i].total_requests = atoi(res[6]);
            servers[i].fail_requests = atoi(res[5]);
            pthread_mutex_unlock(&servers[i].lock);
        }
        
        bestServerport = determineBest();
        gettimeofday(&now, NULL);
        ts.tv_sec = now.tv_sec + 3;
        pthread_cond_timedwait(&probeThread->condition_var, &probeThread->lock, &ts);
        requestCounter = 0;
        
    }
    
}

void* handle_connection(void* thread){
    
    struct parallel* paraThread = (struct parallel*)thread;
    
    while(true){
        while(paraThread->client_sockd < 0){
            pthread_cond_wait(&paraThread->condition_var, &paraThread->lock);
        }
        int c = client_connect(bestServerport);   
        if(c == -1){
            send(paraThread->client_sockd, failResponse, sizeof(failResponse), 0);
            close(paraThread->client_sockd);
        }
        else{
            bridge_loop(paraThread->client_sockd, c);
            close(paraThread->client_sockd);
            close(c);
        }
        
        paraThread->client_sockd = INT_MIN;
    }
    
}

int main(int argc, char** argv) {
    
    hcRequest = 5;
    parallelOp = 4;
    requestCounter = 0;
    
    if (argc < 3) {
        fprintf(1, "missing arguments: usage %s port_to_listen port_to_connect [-N value] [-R value]", argv[0]);
        return -1;
    }
    
    int opt;  
    while((opt = getopt(argc, argv,"N:R:")) != -1)  
    {
        switch(opt)  
        {  
            case 'N':
                parallelOp = atoi(optarg); 
                break;  
            case 'R':
                hcRequest = atoi(optarg); 
                break;
        }  
    }
    
    if(optind == argc){
        fprintf(1, "Error: Please enter port numbers\n");
        return -1;
    }
    
    int listenPort = atoi(argv[optind]);
    if(listenPort <= 0){
        fprintf(1, "Error: Not a valid listening port\n");
        return -1;
    }
    int listenfd;
    if ((listenfd = server_listen(listenPort)) < 0){
        err(1, "failed listening");
    }
    optind++;
    
    int startPort = optind;
    numServers = argc - startPort;
    if(numServers <= 0){
        fprintf(1, "Error: Please specify httpservers\n");
        return -1;
    }
    
    for(int i = startPort; i < argc; i++){
        int si = i - startPort;
        servers[si].port = atoi(argv[i]);
        servers[si].alive = false; 
    }
    
    for(int i = 0; i < numServers; i++){
        int serverfd = client_connect(servers[i].port);
        if(serverfd == -1){
            servers[i].alive = false;
            continue;
        }
        servers[i].alive = true;
        servers[i].serverfd = serverfd;
        servers[i].fail_requests = 0;
        servers[i].total_requests = 0;
        
    }
    
    struct parallel parallels[parallelOp];
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    for(int i = 0; i < parallelOp; i++){
        parallels[i].client_sockd = INT_MIN;
        parallels[i].id = i;
        parallels[i].condition_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
        parallels[i].lock = &lock;

        int create_error = pthread_create(&parallels[i].parallel_id, NULL, handle_connection, (void *)&parallels[i]);
        if(create_error){
            perror("could not create parallel thread");
            return EXIT_FAILURE;
        }
    }
    
    struct healthChecker hcProbe;
    hcProbe.condition_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    hcProbe.lock = &lock;
    if(pthread_create(&hcProbe.hcProbe_id, NULL, probeHealthcheck, (void *)&hcProbe)){
        perror("could not create probe thread");
        return EXIT_FAILURE;
    }
    int count = 0;
    int targetThread = 0;
    while (true) {
        int client_sockd = accept(listenfd, NULL, NULL);
        requestCounter++;
        if(requestCounter == hcRequest){
            requestCounter = 0;
            pthread_cond_signal(&hcProbe.condition_var);
        }
        bestServerport = determineBest();
        if(bestServerport == -1){
            send(client_sockd, failResponse, sizeof(failResponse), 0);
            close(client_sockd);
            continue;
        }
        
        targetThread = count % parallelOp;
        for(int i = 0; i < parallelOp; i++){
            if(pthread_cond_signal(&parallels[i].condition_var) == 0){
                parallels[i].client_sockd = client_sockd;
                break;
            }
            if(i == parallelOp-1){
                i = 0;
            }
                    
        }
        
        count++;
        
    }
    
}

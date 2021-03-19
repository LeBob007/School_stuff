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

#define BUFFER_SIZE 32768

struct httpObject {
    /*
        Create some object 'struct' to keep track of all
        the components related to a HTTP message
        NOTE: There may be more member variables you would want to add
    */
    char method[5];         // PUT, HEAD, GET
    char filename[28];      // what is the file we are worried about
    char bigname[300];
    char httpversion[9];    // HTTP/1.1
    ssize_t content_length; // example: 13
    ssize_t original_length;
    int status_code;
    uint8_t buffer[BUFFER_SIZE + 1];
    uint8_t log_string[BUFFER_SIZE + 1];
};

struct worker {
    int id;
    pthread_t worker_id;
    struct httpObject message;
    int client_sockd;
    pthread_cond_t condition_var;
    pthread_mutex_t* lock;
    int available;
    ssize_t* poffset;
};

int threadNum;
int lf; // indicator of log file
int hcf; // indicator of healthcheck request
int totalCount; // Counter for request total
int failureCount; // Counter for request failure total
char* logFile = NULL;
int logfd;
int hcfd;
char* port = NULL;
int availWorker;
pthread_mutex_t Mainlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Maincondition_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

/*
    \brief 1. Want to read in the HTTP message/ data coming in from socket
    \param client_sockd - socket file descriptor
    \param message - object we want to 'fill in' as we read in the HTTP message
*/
void read_http_response(ssize_t client_sockd, struct httpObject* message) {
    
    if(client_sockd == -1){
        message->status_code = 500;
    }
    
    if(message->status_code == 500){
        return;
    }
    
    message->status_code = 0;
    message->content_length = 0;
    ssize_t bytes = recv(client_sockd, message->buffer, BUFFER_SIZE, 0);
    message->buffer[bytes] = 0; // null terminate
    int n;
    
    sscanf(message->buffer, "%s %s %s\r\n", message->method, message->bigname, message->httpversion);
    
    sscanf(message->bigname, "%*[-_/a-zA-Z0-9]%n", &n);
    if(message->bigname[n] != 0 || n > 28){
        message->status_code = 400;
        return;
    }
    
    if(message->bigname[0] == '/'){
        memcpy(message->filename, (message->bigname)+1, sizeof(message->filename)-1);
    }
    else{
        message->status_code = 400;
        return;
    }
    
    if(strcmp(message->method, "GET") != 0 && strcmp(message->method, "PUT") != 0 && strcmp(message->method, "HEAD") != 0){
        message->status_code = 400;
        return;
    }
    if(strcmp(message->httpversion, "HTTP/1.1") != 0){
        message->status_code = 400;
        return;
    }
    
    if(strcmp(message->filename, "healthcheck") == 0){
        if(strcmp(message->method, "GET") != 0){
            message->status_code = 403;
            return;
        }
        if(lf != 1){
            message->status_code = 404;
            return;
        }
        else{
            hcf = 1;
            hcfd = open("healthcheck", O_CREAT|O_RDWR|O_TRUNC, 0666);
            dprintf(hcfd, "%d\n%d", failureCount, totalCount);
            close(hcfd);
        }
    }
    
    char ** res = NULL;
    char * a;
    char * ch = strtok_r(message->buffer, " \r\n", &a);
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
    
    for(int k = 0; k < spaces-1; k++){
        if(strcmp(res[k], "Content-Length:") == 0){
            sscanf(res[k+1], "%ld", &message->content_length);
            sscanf(res[k+1], "%ld", &message->original_length);
            break;
        }
    }
    
    return;
}

/*
    \brief 2. Want to process the message we just recieved
*/
void process_request(ssize_t client_sockd, struct httpObject* message) {
    
    if(message->status_code == 400 || message->status_code == 500 || message->status_code == 404 || message->status_code == 403){
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
        
        while(message->content_length > BUFFER_SIZE){
            bytes = read(client_sockd, message->buffer, BUFFER_SIZE);
            if(bytes == -1){
                message->status_code = 500;
            }
            message->buffer[bytes] = 0; // null terminate
            bytes = write(fd, message->buffer, BUFFER_SIZE);
            if(bytes == -1){
                message->status_code = 500;
            }
            message->content_length -= BUFFER_SIZE;
        }
        bytes = read(client_sockd, message->buffer, message->content_length);
        if(bytes == -1){
            message->status_code = 500;
        }
        bytes = write(fd, message->buffer, message->content_length);
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
        message->original_length = st.st_size;
        
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
        message->original_length = st.st_size;
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
    struct stat st;
    
    uint8_t status_string[25] = {0};
    switch(message->status_code){
        case 200:
            snprintf(status_string, 25, "OK");
            totalCount++;
            break;
        case 201:
            snprintf(status_string, 25, "Created");
            totalCount++;
            break;
        case 400:
            snprintf(status_string, 25, "Bad Request");
            totalCount++;
            failureCount++;
            break;
        case 403:
            snprintf(status_string, 25, "Forbidden");
            totalCount++;
            failureCount++;
            break;
        case 404:
            snprintf(status_string, 25, "Not Found");
            totalCount++;
            failureCount++;
            
            break;
        case 500:
            snprintf(status_string, 25, "Internal Server Error");
            totalCount++;
            failureCount++;
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
                bytes = read(fd, message->buffer, BUFFER_SIZE);
                if(bytes == -1){
                    message->status_code = 500;
                }
                message->buffer[bytes] = 0; // null terminate
                bytes = write(client_sockd, message->buffer, BUFFER_SIZE);
                if(bytes == -1){
                    message->status_code = 500;
                }
                message->content_length -= BUFFER_SIZE;
            }
            bytes = read(fd, message->buffer, message->content_length);
            if(bytes == -1){
                message->status_code = 500;
            }
            bytes = write(client_sockd, message->buffer, message->content_length);
            if(bytes == -1){
                message->status_code = 500;
            }
            
            close(fd);
        }
    }
    
    return;
}

void* handle_task(void* thread){
    struct stat st;
    
    struct worker* w_thread = (struct worker*)thread;
    while(true){
        while(w_thread->client_sockd < 0){
            pthread_cond_wait(&w_thread->condition_var, &w_thread->lock);
        }
        
        read_http_response(w_thread->client_sockd, &w_thread->message);
        pthread_mutex_lock(w_thread->lock);
        process_request(w_thread->client_sockd, &w_thread->message);

        construct_http_response(w_thread->client_sockd, &w_thread->message);
        pthread_mutex_unlock(w_thread->lock);
        if(lf == 1){
            ssize_t lcount = 0;
            ssize_t totallen = 0;
            ssize_t offset = 0;
            if(w_thread->message.status_code == 200 || w_thread->message.status_code == 201){
                lcount += snprintf(w_thread->message.log_string, BUFFER_SIZE, "%s %s length %ld\n",
                                        w_thread->message.method, w_thread->message.bigname, w_thread->message.original_length);
                totallen = lcount + (((w_thread->message.original_length)/20)*69)+(((w_thread->message.original_length)%20)*3)+18;
                pthread_mutex_lock(w_thread->lock);
                offset = *(w_thread->poffset);
                *(w_thread->poffset) += totallen;
                pthread_mutex_unlock(w_thread->lock);
                if(strcmp(w_thread->message.method, "GET") == 0 || strcmp(w_thread->message.method, "PUT") == 0){
                    if(strcmp(w_thread->message.method, "PUT") == 0){
                        int fd = open(w_thread->message.filename, O_RDONLY);
                        int bytes = read(fd, w_thread->message.buffer, BUFFER_SIZE);
                    }
                    const unsigned char * pc = (const unsigned char *)w_thread->message.buffer;
                    for (int i = 0; i < w_thread->message.original_length; i++) {
                        if ((i % 20) == 0) {
                            if (i != 0)
                                lcount += snprintf (w_thread->message.log_string + lcount, BUFFER_SIZE,"\n");
                            
                            lcount += snprintf(w_thread->message.log_string + lcount, BUFFER_SIZE, "%08d", i);
                        }
                        lcount += snprintf(w_thread->message.log_string + lcount, BUFFER_SIZE," %02x", pc[i]);
                    }
                    lcount += snprintf (w_thread->message.log_string + lcount, BUFFER_SIZE,"\n");
                }
                lcount += snprintf (w_thread->message.log_string + lcount, BUFFER_SIZE,"========\n");
            }
            else {
                if(lf == 1){
                    lcount += snprintf(w_thread->message.log_string, BUFFER_SIZE, "FAIL: %s %s %s --- response %d\n========\n", w_thread->message.method, w_thread->message.bigname, w_thread->message.httpversion, w_thread->message.status_code);
                    pthread_mutex_lock(w_thread->lock);
                    offset = *(w_thread->poffset);
                    *(w_thread->poffset) += lcount;
                    pthread_mutex_unlock(w_thread->lock);
                }
            }
            //fstat(logfd, &st);
            pwrite(logfd, w_thread->message.log_string, lcount, offset);
        }
        
        
        
        close(w_thread->client_sockd);
        w_thread->available = 1;
        w_thread->client_sockd = INT_MIN;
        pthread_cond_signal(&Maincondition_var);
        
    }
    
}

int main(int argc, char** argv) {
    
    threadNum = 4;
    lf = 0;
    hcf = 0;
    totalCount = 0;
    failureCount = 0;
    logfd = 0;
    hcfd = 0;
    
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-N") == 0){
            threadNum = atoi(argv[i+1]);
            availWorker = threadNum;
            i++;
        }
        else if(strcmp(argv[i], "-l") == 0){
            logFile = argv[i+1];
            logfd = open(logFile, O_CREAT|O_WRONLY|O_TRUNC, 0666);
            lf = 1;
            i++;
        }
        else{
            port = argv[1];
        }
    }
    
    
    if(argc < 2) {
        printf("Usage: ./httpserver port [-N threadNum] [-l logFile]\n");
        exit(EXIT_FAILURE);
    }

    /*
        Create sockaddr_in with server information
    */
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
    
    struct worker workers[threadNum];
    int create_error = 0;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    ssize_t offset = 0;
    
    for(int i = 0; i < threadNum; i++){
        workers[i].client_sockd = INT_MIN;
        workers[i].id = i;
        workers[i].condition_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
        workers[i].lock = &lock;
        workers[i].available = 1;
        workers[i].poffset = &offset;
        
        create_error = pthread_create(&workers[i].worker_id, NULL, handle_task, (void *)&workers[i]);
        if(create_error){
            perror("could not create worker thread");
            return EXIT_FAILURE;
        }
    }
    
    int count = 0;
    int targetThread = 0;
    while (true) {
        int client_sockd = accept(server_sockd, &client_addr, &client_addrlen);
        targetThread = count % threadNum;
        for(int i = targetThread; i < threadNum; i++){
            if(workers[i].available == 1){
                workers[i].client_sockd = client_sockd;
                workers[i].available = 0;
                pthread_cond_signal(&workers[i].condition_var);
                break;
            }
            if(i == threadNum-1){
                pthread_cond_wait(&Maincondition_var, &Mainlock);
                i = 0;
            }
                    
        }
        
        count++;
        
    }

    return EXIT_SUCCESS;
}

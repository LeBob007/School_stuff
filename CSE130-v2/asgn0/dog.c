#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 32768

int main(int argc, char **argv)
{
    uint8_t buffer[BUFFER_SIZE];

    // Code for reading from standard input and
    // writing to standard output
    if(argc == 1) {
        while(read(0, buffer, 1) > 0) {
            int w = write(1, buffer, 1);
            if(w == -1) {
                continue;
            }
        }
    }

    // Code to handle more than one argument
    else if(argc >= 2) {
        for(int i = argc-1; i > 0; i--) {
            // Code to handle '-' input
            if(strcmp(argv[i], "-") == 0) {
                while(read(0, buffer, 1) > 0) {
                    int w = write(1, buffer, 1);
                    if(w == -1) {
                        warn("%s", argv[i]);
                        continue;
                    }
                }
                continue;
            }
            // Open file and check if valid
            int file = open(argv[i], O_RDONLY);
            if(file == -1) {
                warn("%s", argv[i]);
                continue;
            }
            // Read file, check if file is readable and
            // write to standard output
            while(read(file, buffer, 1) > 0) {
                int w = write(1, buffer, 1);
                if(w == -1) {
                    warn("%s", argv[i]);
                    continue;
                }
            }
            if(read(file, buffer, 1) == -1) {
                warn("%s", argv[i]);
                continue;
            }
            close(file);
        }
    }
    return 0;
}
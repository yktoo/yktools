/*********************************************************************
 Rewrites the contents of the file given on the command line. For details, see
   https://yktoo.com/en/blog/post/2015/07/20-fixing-samsung-ssd-840-evo-performance-issues-in-linux/

 WARNING:
   This program comes with ABSOLUTELY NO WARRANTY whatsoever, use
   it entirely at your own risk! The author shall never be held
   responsible for loss of your data, ruining your hard drive, PC,
   house, city or planet, including but not limited to wars,
   hurricanes, famines, global warming or alien invasions, arising
   from using it.

 Author:
    Dmitry Kann, https://yktoo.com/

 License:
    LGPL 3.0

 Compile with:
    gcc rewrite-inplace.c -o rewrite-inplace

*********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig){
    interrupted = 1;
}

int main(int argc, char **argv) {
    int    file_desc;
    char   buf[1000000];
    int    chunk_num = 0;
    size_t read_size;
    size_t write_size;
    size_t buf_size = sizeof(buf);
    off_t  file_pos;
    off_t  file_size;
    char* file_name;

    // Check command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(1);
    }

    // Try to open the file
    file_name = argv[1];
    if ((file_desc = open(file_name, O_RDWR)) == -1) {
        perror("Could not open the file");
        exit(2);
    }

    // Get file length
    if ((file_size = lseek(file_desc, 0, SEEK_END)) == (off_t)-1) {
        perror("Could not get file position");
        exit(3);
    }

    printf("File %s is open, size %zu bytes\n", file_name, file_size);

    // Register the signal
    signal(SIGINT, handle_sigint);

    // Iterate through the file contents
    file_pos = 0;
    while (file_pos<file_size && !interrupted) {

        // Progress display
        if (chunk_num++ % 10 == 0) {
            printf("%zu/%zu bytes written\r", file_pos, file_size);
        }

        // Position the file pointer for reading
        if (lseek(file_desc, file_pos, SEEK_SET) == -1) {
            perror("Reading: lseek() failed");
            exit(4);
        }

        // Fill in the buffer with bytes
        if ((read_size = read(file_desc, buf, buf_size)) < 0) {
            perror("Reading: read() failed");
            exit(5);
        }

        if (read_size == 0)
            break;

        // Position the file pointer for writing
        if (lseek(file_desc, file_pos, SEEK_SET) == -1) {
            perror("Writing: lseek() failed");
            exit(6);
        }

        // Write the bytes
        if ((write_size = write(file_desc, buf, read_size)) == -1) {
            perror("Writing: write() failed");
            exit(7);
        }
        if (write_size < read_size) {
            perror("Writing: write() wrote less bytes than required");
            exit(7);
        }

        // Advance the pointer
        file_pos += read_size;
    }

    // Close the file
    close(file_desc);

    if (interrupted) {
        fprintf(stderr, "File writing has been interrupted; %zu/%zu bytes written\n", file_pos, file_size);
        exit(15);
    } else {
        printf("File has been rewritten successfully; %zu bytes written\n", file_size);
        exit(0);
    }
}


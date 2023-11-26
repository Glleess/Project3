#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

void mycp(char *src, char *dst, int bufsize) {
    int in_fd, out_fd, n;
    char *buf;

    if (bufsize <= 0) {
        perror("Buffer size should be positive.");
        exit(1);
    }

    if ((in_fd = open(src, O_RDONLY)) == -1) {
        perror("Error opening input file.");
        exit(1);
    }

    if ((out_fd = open(dst, O_WRONLY | O_CREAT, 0644)) == -1) {
        perror("Error opening output file.");
        exit(1);
    }

    if ((buf = (char *)malloc(bufsize)) == NULL) {
        perror("Error allocating memory for buffer.");
        exit(1);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while ((n = read(in_fd, buf, bufsize)) > 0) {
        if (write(out_fd, buf, n) != n) {
            perror("Error writing to output file.");
            exit(1);
        }
    }

    if (n == -1) {
        perror("Error reading from input file.");
        exit(1);
    }

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long micros = end.tv_usec - start.tv_usec;
    double elapsed = seconds + micros / 1000000.0;

    printf("Copied %s to %s in %.6f seconds with buffer size %d bytes.\n", src, dst, elapsed, bufsize);

    close(in_fd);
    close(out_fd);
    free(buf);
}

int main(int argc, char *argv[]) {
    int opt, bufsize = 0;
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
            case 'b':
                bufsize = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-b number] file1 file2\n", argv[0]);
                exit(1);
        }
    }

    if (argc - optind != 2) {
        fprintf(stderr, "Error: exactly two non-option arguments are required.\n");
        exit(1);
    }

    mycp(argv[optind], argv[optind + 1], bufsize);
    return 0;
}
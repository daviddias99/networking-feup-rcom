#include "linklayer/linklayer.h"

int main(int argc, char** argv)
{
    int fd = llopen(0, TRANSMITTER);
    if (fd == -1) {
      printf("fd = %d\n", fd);
      return fd;
    }


    char buf[255];

    while (true) {
    	printf("input: ");
    	fgets(buf, 255, stdin);
    	llwrite(fd, buf, strlen(buf) - 1);
    }

    llclose(fd);
    return 0;
}

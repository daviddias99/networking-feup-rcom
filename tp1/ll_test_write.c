#include "linklayer/linklayer.h"

int main(int argc, char** argv)
{
    int fd = llopen(0, TRANSMITTER);
    if (fd == -1) {
      printf("fd = %d\n", fd);
      return fd;
    }


    uint8_t buf[255];

    while (true) {
    	printf("input: ");
    	fgets((char*)buf, 255, stdin);
    	llwrite(fd, buf, strlen(buf) - 1);
      if (buf[0] == '0')
        break;
    }

    llclose(fd);
    return 0;
}

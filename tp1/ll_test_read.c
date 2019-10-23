#include "linklayer/linklayer.h"

int main(int argc, char** argv)
{
    int fd = llopen(0, RECEIVER);
    if (fd == -1) {
      printf("fd = %d\n", fd);
      return fd;
    }

    
    char buf[255];

    while (true) {
        
    	int result = llread(fd, buf);
        

        printf("-- %d bytes read(%s)\n",result,buf);

    }

    llclose(fd);
    return 0;
}

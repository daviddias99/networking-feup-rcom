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
        

      buf[result] = '\0';
      printf("-- %d bytes read(%s)\n",result,buf);

    }

    llclose(fd);
    return 0;
}

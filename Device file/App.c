#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 
int main()
{
        int fd;
        int n;
        printf("\nOpening Driver\n");
        fd = open("/dev/device_file", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
        else
        {
        	printf("\nFile is openned");
        }
        printf("\nEnter 1 to 3 to control led 1 to led 3, enter 4 to turn off all led");
        printf("\nEnter the number of leds[1:3]: ");
        scanf("%d", &n);
        if(n==1)
        {
        	write(fd, "1", 8);
        }
        else if(n==2)
        {
        	write(fd, "2", 8);
        }
        else if(n==3)
        {
        	write(fd, "3", 8);
        }
        else if(n==4)
        {
        	write(fd, "4", 8);
        }
        close(fd);
}

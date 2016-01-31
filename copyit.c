// Victoria Johnston ~ copyit copies the contents of one file over to another
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

void display_message(int s){
	printf("copyit: still copying...\n");
	alarm(1);
}

int main (int argc, char * argv[])
{
	// declare varibles 
	int fsource, fdest; // file descriptors
	mode_t mode = S_IRUSR; // user read permissions
	char line[2000]; // string to be copied over from one file to another
	size_t n, bytes; // n=number of bytes in each line. bytes=number of bytes in total

	// declare signal alarm
	signal(SIGALRM, display_message);

	// check if program is invoked incorrectly
	// check for 0 or 1 arguments
	if (argc<3){
		printf("copyit: Too few arguments!\n");
		printf("usage: copyit <sourcefile> <targetfile>\n");
		exit(1);
	// check for over 2 arguments
	} else if (argc>3){
		printf("copyit: Too many arguments!\n");
                printf("usage: copyit <sourcefile> <targetfile>\n");
                exit(1);
	}

	// open the source file
	if ((fsource=open(argv[1],O_RDONLY,0))<0){
		printf("copyit: Couldn't open file %s: %s\n",argv[1],strerror(errno));
		exit(1);
	}

	// create the target file
	if ((fdest=creat(argv[2],mode))<0){
		printf("copyit: Couldn't create file %s: %s\n",argv[2],strerror(errno));
		exit(1);
	}

	// start alarm
	alarm(1);

	// transfer over data
	while (1) {
		//alarm(1);
		// clear line
		memset(line,'\0',sizeof(line));

		// read data from source file
		while ((n=read(fsource,line,sizeof(line)))<0){
			// if an interrupt, try to read it line again
			if (errno==EINTR){
				continue;
			}
			// otherwise print error and exit
			printf("copyit: Couldn't read from file %s: %s\n",argv[1],strerror(errno));
			exit(1);
			
		}

		// increment bytes
		bytes += n;

		// if no data is left exit the loop
		if (n==0) break;
		
		// write data to target file
		while (write(fdest,line,n)<0){
			// if interrupt keep trying to write
			if (errno==EINTR){
				continue;
			}

			// otherwise exit with an error
			printf("copyit: Couldn't write to file %s: %s\n",argv[2],strerror(errno));
			exit(1);
		}
	}

	// close the files
	close(fsource);
	close(fdest);
	
	// If there have been no errors, print success message
	printf("copyit: Copied %zu bytes from file %s to %s.\n",bytes,argv[1],argv[2]);

	exit(0);
}

// Victoria Johnston ~ copyit copies the contents of one file over to another
#define _GNU_SOURCE
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
#include <dirent.h>

void display_message(int s){
	printf("copyit: still copying...\n");
	alarm(1);
}

int copyfile(char* source, char* dest){
	int fsource, fdest; // file descriptors
        mode_t mode = S_IRUSR; // user read permissions
	char line[2000]; // string to be copied over from one file to another
	int bytes = 0;
	int n;

	// open the source file
	if ((fsource=open(source,O_RDONLY,0))<0){
		printf("copyit: Couldn't open file %s: %s\n",source,strerror(errno));
		exit(1);
	}

	// create the target file
	if ((fdest=creat(dest,mode))<0){
		printf("copyit: Couldn't create file %s: %s\n",dest,strerror(errno));
		exit(1);
	}

	// transfer over data
	while (1) {
                // clear line
                memset(line,'\0',sizeof(line));

                // read data from source file
                while ((n=read(fsource,line,sizeof(line)))<0){
                        // if an interrupt, try to read it line again
			if (errno==EINTR){
				continue;
			}
                       	// otherwise print error and exit
			printf("copyit: Couldn't read from file %s: %s\n",source,strerror(errno));
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
			printf("copyit: Couldn't write to file %s: %s\n",dest,strerror(errno));
			exit(1);
		}
	}
	
	// close files
	close(fsource);
	close(fdest);

	return bytes;
}

int isdir(char* file){
	struct stat s;
	if (stat(file,&s)==0){
		if (s.st_mode & S_IFDIR){
			return 1;
		}
	}
	return 0;	
}

int copydir(char* dirname,char* target){
	DIR *dp;
	struct dirent *ep;
	int bytes = 0;

	mkdir(target,S_IRWXU);	

	dp = opendir(dirname);
	if (dp!=NULL){
		while ((ep = readdir (dp))){
			// get source file
			char *source_file = NULL;
			asprintf(&source_file,"%s/%s",dirname,ep->d_name);

			// get target file
			char *target_file = NULL;
			asprintf(&target_file,"%s/%s",target,ep->d_name);
			
			// match all files which do not begin with a '.'
			if (ep->d_name[0]!='.'){
				// check if the file is a directory
				if (isdir(source_file)){
					// recurse call copydir for the new directory
					bytes += copydir(source_file,target_file);
				// if the file is not a directory
				} else {
					bytes += copyfile(source_file,target_file);
				}
			}

		}
		(void) closedir(dp);
	}
	return bytes;
}

int main (int argc, char * argv[])
{
	// declare varibles 
	int bytes; // n=number of bytes in each line. bytes=number of bytes in total

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

	// start alarm
	alarm(1);

	// check if source if directory
	if (isdir(argv[1])){
		bytes = copydir(argv[1],argv[2]);	
	} else {
		bytes = copyfile(argv[1],argv[2]);
	}
	
	// If there have been no errors, print success message
	printf("copyit: Copied %i bytes from file %s to %s.\n",bytes,argv[1],argv[2]);

	exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <openssl/md5.h>

void display_message(int s){
	printf("copyit: still copying...\n");
	alarm(1);
	signal(SIGALRM, display_message);
}

int main (int argc, char * argv[])
{
	// declare variables
	char sourcefile[100];
	char targetfile[100];
	FILE * infile;
	FILE * outfile;

	// check if program is invoked incorrectly
	// no arguments
	if (argc<3){
		printf("copyit: Too few arguments!\n");
		printf("usage: copyit <sourcefile> <targetfile>\n");
		exit(1);
	} else if (argc>3){
		printf("copyit: Too many arguments!\n");
                printf("usage: copyit <sourcefile> <targetfile>\n");
                exit(1);
	}

	// set the source and target files
	memset(sourcefile,'\0',sizeof(sourcefile));
	memset(targetfile,'\0',sizeof(targetfile));
	memcpy(sourcefile,argv[1],sizeof(argv[1]));
	memcpy(targetfile,argv[2],sizeof(argv[2]));

	// open the source file
	infile = fopen(sourcefile,"r");
	if (infile==NULL){
		printf("copyit: Unable to open %s: %s\n",sourcefile,strerror(errno));
		exit(1);
	}

	// create the target file
	// check if file already exists
	if (fopen(targetfile,"rb+")==NULL){
		outfile = fopen(targetfile,"w");
		if (outfile==NULL){
			printf("copyit: Unable to create %s: %s\n",targetfile,strerror(errno));
			exit(1);
		}
	} else {
		printf("copyit: %s already exists!\n",targetfile);
		exit(1);
	}

	// initialize string to be copied from one file to another
	char line[200];
	int n;
	int bytes;

	// transfer over data
	while (1) {
		// clear line
		memset(line,'\0',sizeof(line));
		// read data from source file
		if ((n=fread(line,sizeof(char),sizeof(line),infile))<0){
			// if an interrupt, try to read it line again
			if (errno==EINTR){
				continue;
			}
			// otherwise print error and exit
			printf("copyit: Error reading from %s: %s\n",sourcefile,strerror(errno));
			exit(1);
		}
		// increment bytes
		bytes += n;
		// if no data is left exit the loop
		if (n==0) break;
		
		// write data to target file
		while (fwrite(line,sizeof(char),sizeof(line),outfile)<0){
			if (errno!=EINTR){
				printf("copyit: Error writing to %s: %s\n",targetfile,strerror(errno));
				exit(1);
			}
		}

		// signal
		signal(SIGALRM, display_message);
		alarm(1);
	}

	// close files
	fclose(infile);
	fclose(outfile);
	
	// calculate md5
	char sourcemd5[50];
	char targetmd5[50];
	memset(sourcemd5,'\0',sizeof(sourcemd5));
	memset(targetmd5,'\0',sizeof(targetmd5));
	infile = fopen(sourcefile,"rb");
	int bt;
	unsigned char mdbuf[200000];
	MD5_CTX mdContext;

	// sourcefile md5
	MD5_Init(&mdContext);
	while((bt = fread(mdbuf,sizeof(char),200000,infile))!= 0){
		MD5_Update(&mdContext,mdbuf,bt);
	}
	MD5_Final(sourcemd5,&mdContext);

	// targetfile md5
	outfile = fopen(targetfile,"rb");
	MD5_Init(&mdContext);
        while((bt = fread(mdbuf,sizeof(char),200000,outfile))!= 0){
                MD5_Update(&mdContext,mdbuf,bt);
        }
        MD5_Final(targetmd5,&mdContext);

	int i;
	// print
	for(i=0; i<MD5_DIGEST_LENGTH; i++) {
		printf("%02x",sourcemd5[i]);
	}
	printf("\n");
	for(i=0; i<MD5_DIGEST_LENGTH; i++) {
                printf("%02x",targetmd5[i]);
        }       
	printf("%s\n",sourcemd5);
	printf("%s\n",targetmd5);
	
	fclose(infile);
	fclose(outfile);
	// check if file copied properly
	if (!strcmp(sourcemd5,targetmd5)){
		printf("copyit: Copied %i bytes from file %s to %s.\n",bytes,sourcefile,targetfile);
	} else {
		printf("copyit: File %s did not copy over to %s successfully.\n",sourcefile,targetfile);
		exit(1);
	}
}

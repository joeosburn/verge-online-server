#include <stdio.h>
#include <stdlib.h>
#include "zlib.h"

#define ZLIB_TAG 0x4B52485A

int main(int argc, char *argv[])
{
	FILE *in, *out;
	int len, myErr;
	unsigned char *inbuf, *outbuf;
	uLong comprLen;
	int i;

	if (argc != 2)
	{
		printf("You must supply a filename.\n");
		exit(-1);
	}

	in = fopen(argv[1], "rb");
	if (!in)
	{
		printf("Could not open input file.\n");
		exit(-1);
	}

	fread(&i, 1, 4, in);
	if (i == ZLIB_TAG)
	{
		printf("%s is already compressed! skipping\n", argv[1]);
		exit(-1);
	}

	out = fopen("TEMP.$$$", "wb");
	if (!out)
	{
		printf("Disk is read only.\n");
		exit(-1);
	}

	fseek(in, 0, 2);
	len = ftell(in);
	fseek(in, 0, 0);
	
	comprLen = len+(len*20/100);
	inbuf = (unsigned char *) malloc(len);
	outbuf = (unsigned char *) malloc(comprLen);
	
	fread(inbuf, 1, len, in);
	fclose(in);
	
	myErr = compress(outbuf, &comprLen, inbuf, len);
	if (myErr != Z_OK)
	{
		if (myErr == Z_MEM_ERROR) printf("Z_MEM_ERROR \n");
		if (myErr == Z_BUF_ERROR) printf("Z_BUF_ERROR \n");
		if (myErr == Z_STREAM_ERROR) printf("Z_STREAM_ERROR \n");
		printf("Error during compression. :( \n");
		fclose(out);
		remove("TEMP.$$$");
		exit(-1);
	}
	fwrite("ZHRK", 1, 4, out);
	fwrite(&len, 1, 4, out);
	fwrite(&comprLen, 1, 4, out);
	fwrite(outbuf, 1, comprLen, out);
	fclose(out);

	remove(argv[1]);
	rename("TEMP.$$$", argv[1]);
    printf("%s(%d bytes): compressed %d [%d%c]\n",argv[1], len, (int) comprLen, 100-(100*comprLen/len), '%');
	return 0;
}

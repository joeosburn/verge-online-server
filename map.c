/*
 *
 * map.c
 *         by benjamin eirich
 *
 */

#include "conf.h"
#include "vo.h"

char		*obstruct_map;
struct layer_r	layer[6];

void decode(unsigned char *dest, int len, unsigned char *buf)
{
	int j, n;
	unsigned char run, w;

	n = 0;
	do
	{
		w = *buf;
		buf++;
		if (w == 0xFF)
		{
			run = *buf;
			buf++;
			w = *buf;
			buf++;
			for (j = 0; j < run; j++)
				dest[n + j] = w;
			n += run;
		}
		else
		{
			dest[n] = w;
			n++;
		}
	} while (n < len);
}

int load_map(char *fn)
{
	FILE *f;
	char strbuf[128], *cb;
	int i;

	char numlayers;
	int bufsize;

	f = fopen(fn, "rb");
	if (!f)
	{
		vo_die("load_map(), could not open %s", fn);
		return 0;
	}

	fread(strbuf, 1, 10, f);
	fread(strbuf, 1, 60, f);
	fread(strbuf, 1, 60, f);
	fread(strbuf, 1, 20, f);
	fread(strbuf, 1, 55, f);

	fread(&numlayers, 1, 1, f);
	for (i=0; i<numlayers; i++)
		fread(&layer[i], 1, 12, f);


	for (i=0; i<numlayers; i++)
	{ 
		fread(&bufsize, 1, 4, f);
		fseek(f, bufsize, SEEK_CUR);
	}	


	fread(&bufsize, 1, 4, f);
	cb = (char *) malloc(bufsize);
	fread(cb, 1, bufsize, f);
	obstruct_map = (char *) malloc(layer[0].sizex * layer[0].sizey);
	decode((unsigned char *) obstruct_map, (layer[0].sizex * layer[0].sizey), cb);
	free(cb);

	vo_log(log_info, "Succesfully initialized %s", fn);
	fclose(f);
	return 1;
}

char get_obstruct_at(int x, int y)
{
	return 0;
	x %= layer[0].sizex;
	y %= layer[0].sizey;
	return obstruct_map[(y*layer[0].sizex)+x];
}

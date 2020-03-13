/*
 *
 * map.h
 *      by benjamin eirich
 *
 */

#ifndef _MAP_H_

struct layer_r
{
        char pmultx,pdivx;                 // parallax multiplier/divisor for X
        char pmulty,pdivy;                 // parallax multiplier/divisor for Y
        unsigned short sizex, sizey;       // layer dimensions.
        unsigned char trans, hline;        // transparency flag | hline (raster fx)
        unsigned char pad1, pad2;
};

int load_map(char *fn);
char get_obstruct_at(int x, int y);

#define _MAP_H_
#endif

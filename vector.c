#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"


void vadd(float *v1, float *v2, float *sum)
/* Add two vectors of dimension dim, place in sum */
{
	sum[0] = v1[0] + v2[0];
	sum[1] = v1[1] + v2[1];
}

void vsubtract(float *v1, float *v2, float *sum)
/* subtract v1 from v2, place in sum */
{
	sum[0] = v2[0] - v1[0];
	sum[1] = v2[1] - v1[1];
}

float vlength(float *v)
/* return length of vector v */
{
	return sqrt(vlength2(v));
}

float vlength2(float *v)
/* return length squared of vector v */
{
	return v[0]*v[0] + v[1]*v[1];
}

void vscale(float *v, float sf, float *result) 
/* Place a vector in result scaled by scale factor sf */
{
	result[0] = sf * v[0];
	result[1] = sf * v[1];
}

void vnormalise(float *v, float *result)
/* Return a vector of length 1 with same direction as v */
{
	vscale(v, 1.0 / vlength(v), result);
}

float vdot(float *v1, float *v2)
/* Return dot product of v1 and v2 */
{
	return v1[0]*v2[0] + v1[1]*v2[1];
}

float vcomp(float *v1, float *v2)
/* Return component of v1 along the direction of v2 */
{
	float *normed = vector();
	vnormalise(v2, normed);

	float ret = vdot(v1, normed);
	
	free(normed);

	return ret;
}
/*
float vanglerad(float *v1, float *v2, int dim)
/x* Return angle between v1 and v2 in radians in range [0:pi] *x/
{
	return acos(vdot(vnormalise(v1, dim), vnormalise(v2, dim), dim));
}

float vangledegrees(float *v1, float *v2, int dim)
/x* Return angle between v1 and v2 in degrees in range [0:180] *x/
{
	return rad2deg(vanglerad(v1, v2, dim));
}
*/
float *vector() 
/* allocate a vector dimension dim */
{
	return (float *)malloc(2 * sizeof(float));
}

void free_vector(float *v)
/* Free a vector allocated by *vector */
{
	free(v);
}

char *vprint(float *v, int dim) 
/* Return a string representation of the vector (components to 5sf) */
{
	/* 
		Arbitrary looking formula for max string length.
		Just allocate buffer for string and fill it with
		null characters.
	*/
	char *buff = malloc((12*dim+2) * sizeof(char));
	memset(buff, '\0', (12*dim+2) * sizeof(char));
	int i; // index variable
	*buff = '('; // Opening bracket
	for (i = 0; i < dim; i++) {
		// Build string from vector components
		sprintf(buff, "%s%.5g", buff, v[i]);
		// Add a comma and a space unless we're on last component
		if (i != dim-1) {
			sprintf(buff, "%s, ", buff);
		}
	}
	// Closing bracket
	sprintf(buff, "%s)", buff);
	return buff;
}

void vequal(float *v1, float *v2)
/* Copy v1 to v2 */
{
	v2[0] = v1[0];
	v2[1] = v1[1];
}

float rad2deg(float rad)
/* Converts angle rad to degrees */
{
	return rad * 180.0 / M_PI;
}

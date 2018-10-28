#include "header.h"
#include <math.h>
#include <stdlib.h>

/*
	Global variables
*/
float *new_pos_b;
float *new_pos_c;
float *c_to_c;
float *rel_vel;
float *impulse;
float *dvelb;
float *dvelc;

float separation2;
float min_sep;
float vel_along_normal;
float j;
/**********************************************************/

int collide_walls(struct ball *b)
{
	int ret = 0;
	if (b->pos[0] + b->dpos[0] > 1.0*SIZE/2.0 - b->r ||
		b->pos[0] + b->dpos[0] < -1.0*SIZE/2.0 + b->r) {
		
		b->vel[0] *= -1.0;
		ret = 1;
	}

	if (b->pos[1] + b->dpos[1] > 1.0*SIZE/2.0 - b->r ||
		b->pos[1] + b->dpos[1] < -1.0*SIZE/2.0 + b->r) {
			
		b->vel[1] *= -1.0;
		ret = 1;
	}
	
	return ret;
}

int collide_balls(struct ball *b, struct ball *c)
{
	vadd(b->pos, b->dpos, new_pos_b);
	vadd(c->pos, c->dpos, new_pos_c);
	vsubtract(new_pos_b, new_pos_c, c_to_c);
	separation2 = vlength2(c_to_c);

	min_sep = b->r + c->r;
	min_sep *= min_sep;

	if (separation2 > min_sep)
		return 0; 

	// Get relative velocity along the centre-to-centre line
	vsubtract(b->vel, c->vel, rel_vel);
	vscale(c_to_c, 1.0 / sqrt(separation2), c_to_c);
	vel_along_normal = vdot(rel_vel, c_to_c);

	// If balls are already separating then don't resolve collision
	if (vel_along_normal > 0)
		return 0;
	
	// Relative velocity along normal multiplied by reduced mass
	j = vel_along_normal / (b->m_inv + c->m_inv);

	// Divvy up j between the masses
	vscale(c_to_c, j, impulse);
	vscale(impulse,  2.0 * b->m_inv, dvelb);
	vscale(impulse, -2.0 * c->m_inv, dvelc);
	vadd(b->vel, dvelb, b->vel);
	vadd(c->vel, dvelc, c->vel);

	return 1;
}

void col_init()
{
	new_pos_b = vector();
	new_pos_c = vector();
	c_to_c = vector();
	impulse = vector();
	dvelb = vector();
	dvelc = vector();
	rel_vel = vector();
}

void col_free_vectors()
{
	free_vector(new_pos_b);
	free_vector(new_pos_c);
	free_vector(c_to_c);
	free_vector(impulse);
	free_vector(dvelb);
	free_vector(dvelc);
	free_vector(rel_vel);
}

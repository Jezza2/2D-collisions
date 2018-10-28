/*
	quadtree.c

	JEREMY STANGER - 18/02/2018

	Requires a header file with definitions of any public functions, and the
	two structures used:

	struct ball {
		float *pos;
		float *vel;
		float *dpos;
		float r;
		float m_inv;
		int marked;
	}
	
	struct quadtree {
		int level;
		float bounds[4];
		struct ball **members;
		struct quadtree *nodes[4];
		int has_nodes;
		int num_members;
		int max_members;
		int overfowed;
	}

	Usage:
	-	Declare and initialise 2 arrays of pointers to balls,
		reserve the memory for the first only.
	-	Declare a pointer to a quadtree.  Run qt_init.
	-	Each iteration, run qt_clear then qt_populate with one
		array of balls.
	-	Then run qt_retrieve for each ball in the first array 
		with the second array of balls.  This second array now contains 
		a list of balls which the ball could collide with.
	-	Run qt_free when done with quadtree


	---------- WARNING -------------
	The memory usage of a quadtree obviously goes as sum_0^max_level {4^n}.
	For context, for max_level==11, memory usage is around 1GB.
	For max_level==12, memory usage is around 3.6GB.
*/



#include "header.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int max_members;
// There's no point in this being any bigger than n, where 10/2^n is
// at least 5 times bigger than the radius of your smallest balls
int max_level;

void qt_insert(struct ball *b, struct quadtree *parent);
int qt_getIndex(struct ball *b, struct quadtree *parent);
void qt_build(struct quadtree *qt, float *bounds, int level);


void qt_init(struct quadtree **qt, float size, int max_mems, int max_lev)
{
	max_members = max_mems;
	max_level = max_lev;
	*qt = malloc(sizeof(struct quadtree));

	float bounds[4];
	bounds[0] = -1.0 * size / 2.0;
	bounds[1] = size / 2.0;
	bounds[2] = -1.0 * size / 2.0;
	bounds[3] = size / 2.0;
	qt_build(*qt, bounds, 0);
}

void qt_build(struct quadtree *qt, float *bounds, int level)
{
	qt->level = level; 
	qt->max_members = max_members;
	qt->num_members = 0;
	qt->members = malloc(max_members * sizeof(struct ball *));
	for (int i = 0; i < 4; i++)
		qt->bounds[i] = bounds[i];

	if (level != max_level) {
		qt->nodes = malloc(4 * sizeof(struct quadtree *));
		for (int i = 0; i < 4; i++)
			qt->nodes[i] = malloc(sizeof(struct quadtree));

		float node_bounds[4];
		float size_x = fabs(bounds[1] - bounds[0]) / 2.0;
		float size_y = fabs(bounds[3] - bounds[2]) / 2.0;

		node_bounds[0] = bounds[0] + size_x;
		node_bounds[1] = bounds[1];
		node_bounds[2] = bounds[2] + size_y;
		node_bounds[3] = bounds[3];
		qt_build(qt->nodes[0], node_bounds, level + 1);

		node_bounds[0] = bounds[0];
		node_bounds[1] = bounds[1] - size_x;
		node_bounds[2] = bounds[2] + size_y;
		node_bounds[3] = bounds[3];
		qt_build(qt->nodes[1], node_bounds, level + 1);
		
		node_bounds[0] = bounds[0];
		node_bounds[1] = bounds[1] - size_x;
		node_bounds[2] = bounds[2];
		node_bounds[3] = bounds[3] - size_y;
		qt_build(qt->nodes[2], node_bounds, level + 1);

		node_bounds[0] = bounds[0] + size_x;
		node_bounds[1] = bounds[1];
		node_bounds[2] = bounds[2];
		node_bounds[3] = bounds[3] - size_y;
		qt_build(qt->nodes[3], node_bounds, level + 1);
	}
}

void qt_clear(struct quadtree *parent)
/*
	Empties the quadtree
*/
{
	parent->num_members = 0;
	parent->overflowed = 0;

	if (parent->level == max_level)
		return;

	for (int i = 0; i < 4; i++)
		qt_clear(parent->nodes[i]);
}

void qt_free(struct quadtree *parent)
/*
	Frees all the memory allocated to the quadtree
*/
{
	free(parent->members);

	if (parent->level == max_level)
		return;

	for (int i = 0; i < 4; i++) {
		qt_free(parent->nodes[i]);
	}
}

int qt_getIndex(struct ball *b, struct quadtree *parent)
/*	
	Return node the ball fits into.  If it overlaps any node boundaries,
	returns -1
*/
{
	if (parent->level == max_level)
		return -1;

	int in_top = 0;
	int in_bottom = 0;

	float new_pos_x = b->pos[0] + b->dpos[0];
	float new_pos_y = b->pos[1] + b->dpos[1];

	if (new_pos_y - b->r > parent->nodes[0]->bounds[2])
		in_top = 1;
	if (new_pos_y + b->r < parent->nodes[3]->bounds[3]) 
		in_bottom = 1;
	
	// If on right
	if (new_pos_x - b->r > parent->nodes[0]->bounds[0]) {
		if (in_top) return 0;	
		if (in_bottom) return 3;
	}
	// If on left
	if (new_pos_x + b->r < parent->nodes[1]->bounds[1]) {
		if (in_top) return 1;	
		if (in_bottom) return 2;
	}

	// If none of these apply, return -1
	return -1;
}

void qt_insert(struct ball *b, struct quadtree *parent)
/*
	Insert a ball into the quadtree.  If the parent node needs to split,
	it will populate the child nodes with its current members and then
	add the ball to the correct node.
*/
{
	int index = -1; // Index of subnode

	/*
		If the node reaches the maximum number of members, we need to
		go to a new level
	*/
	if (parent->num_members >= max_members) {

		int num_members_init = parent->num_members;

	// We can only go to a new level if we're not already at the max level
		if (parent->level != max_level) {
			parent->overflowed = 1;
		
			int i, j; // Index variables

	/*
		Since we've overflowed, we need to divvy up the current members
		of this node between the sub nodes
	*/
			for (i = 0; i < parent->num_members; i++) {
	/*
		If the ball fits into a subnode, we can insert it and pop it from
		the members stack.
	*/
				index = qt_getIndex(parent->members[i], parent);	

				if (index != -1) {
					parent->num_members--;
					qt_insert(parent->members[i], parent->nodes[index]);
	// Now we need to rearrange the array
					j = i - 1;
					while (++j < parent->num_members) 
						parent->members[j] = parent->members[j+1];
					i--;
				} 
			}
		}

	/*
		If the number of members of the node is too large, and we haven't
		been able to redistribute the balls (either because they overlap
		nodes or because we're at MAX_LEVEL, we need to enlarge the members
		array.
	*/
		if (parent->num_members >= parent->max_members) {

			parent->max_members += max_members;
			parent->members = realloc(parent->members, 
				(parent->max_members) * sizeof(struct ball *));
		}
	}

	// Get the node the ball fits into
	index = qt_getIndex(b, parent);
	
	/*
		If it doesn't fit in any node, or we don't need to go to 
		a new level, just add the ball to this node's members.
		Otherwise, insert the ball into the correct node.
	*/
	if (index == -1 || !parent->overflowed) {
		parent->members[parent->num_members++] = b;
	} else {
		qt_insert(b, parent->nodes[index]);
	}
}

void qt_populate(struct ball **balls, struct quadtree *qt)
/*
	Inserts all the balls into the quadtree
*/
{
	for (int i = 0; i < NUM_BALLS; i++) {
		qt_insert(balls[i], qt);
	}
}

void qt_print(struct quadtree *parent)
/*
	Debugging function; prints the quadtree in a (relatively) readable form.
*/
{
	char buff[100];	
	memset(buff, '\0', 100);
	for (int i = 0; i < parent->level; i++) {
		sprintf(buff, "%s-", buff);
	}
	printf("%s%d\n", buff, parent->num_members);
	if (parent->overflowed) {
		for (int i = 0; i < 4; i++) {
			qt_print(parent->nodes[i]);
		}
	}
}

int qt_retrieve(struct quadtree *parent, struct ball *b, 
	int num, struct ball **colls)
/*
	Place a list of balls that might collide with b into colls.
	Return the number of said balls
*/
{
	// Add all the members of node, but not b itself
	for (int i = 0; i < parent->num_members; i++)
		if (parent->members[i] != b)
			colls[num++] = parent->members[i];
	
	int index = qt_getIndex(b, parent);

	// If the node has populated children, retreive recursively
	if (parent->overflowed) { 
	/*
		If the ball fits into no single sub node, we need to return all the
		the members of all the sub nodes and their sub nodes etc.
	*/
		if (index == -1) {
			for (int i = 0; i < 4; i++) {
				num = qt_retrieve(parent->nodes[i], b, num, colls);
			}
		} else {
			num = qt_retrieve(parent->nodes[index], b, num, colls);
		}
	}

	return num;	
}

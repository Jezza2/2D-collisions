/*
	Public definitions
*/
#define NUM_BALLS 10
#define SIZE 10.0

int index_k;

struct ball {
	float *pos;
	float *vel;
	float *dpos; // velocity reduced by timestep
	float r;
	float m_inv;
	int marked;
};

struct quadtree {
	int level;

	// min_x, max_x, min_y, max_y
	float bounds[4];

	struct ball **members;

	//quadrants 1, 2, 3, and 4 starting from positive and going anticlockwise
	struct quadtree **nodes;

	// keep track of the number of members of the node
	int num_members;
	// This may be more than MAX_MEMBERS, we need to remember this to
	// avoid unnecessary calls to realloc
	int max_members;
	// If the node has overflowed we know to go to the next level by default
	int overflowed;
};

/*
	Functions in vector.c
*/
void vadd(float *v1, float *v2, float *sum);
void vsubtract(float *v1, float *v2, float *sum);
float vlength(float *v);
float vlength2(float *v);
void vscale(float *v, float sf, float *scaled);
void vnormalise(float *v, float *result);
float vdot(float *v1, float *v2);
float vcomp(float *v1, float *v2);
float vanglerad(float *v1, float *v2);
float vangledegrees(float *v1, float *v2);
float *vector();
void free_vector(float *v);
char *vprint(float *v, int dim);
void vequal(float *v, float *result);
float rad2deg(float rad);
/****************************************************/

/*
	Functions in drawing.c
*/
void initGlut(int *argc, char **argv, int size, float sf, struct ball** items);
int getCurrentTime(void);
void setupTimer(void);
void rerender(void);
/******************************************************/


/*
	Functions in balls.c
*/
void loop(int value);
/******************************************************/

/*
	Functions in collisions.c
*/
void col_init(void);
int collide_walls(struct ball *b);
int collide_balls(struct ball *b, struct ball *c);
void col_free_vectors(void);
/*****************************************************/

/*
	Functions in quadtree.c
*/
void qt_init(struct quadtree **qt, float size, int max_mems, int max_lev);
void qt_free(struct quadtree *parent);
void qt_clear(struct quadtree *parent);
void qt_populate(struct ball **balls, struct quadtree *qt);
void qt_print(struct quadtree *parent);
int qt_retrieve(struct quadtree *parent, struct ball *b, 
	int num, struct ball **colls); 

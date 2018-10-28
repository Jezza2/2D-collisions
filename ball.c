#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void init_objects(void);
void quit(void);
void setup_balls(void);
void generate_balls_diffusion(void);
void generate_balls_brownian(void);
void generate_balls(void);

int prevTime = 0;
int currTime = 0;
int timeSincePrevFrame = 0;

struct ball **balls;
struct ball **colls;
struct quadtree *qt;

int main(int argc, char **argv)
{
	atexit(quit);

	init_objects();

	generate_balls();

	setup_balls();

	initGlut(&argc, argv, 1000, SIZE, balls);
}

void init_objects()
{
	balls = malloc(NUM_BALLS * sizeof(struct ball *));
	colls = malloc(NUM_BALLS * sizeof(struct ball *));

	for (int i = 0; i < NUM_BALLS; i++) {
		balls[i] = malloc(sizeof(struct ball));
		balls[i]->pos = vector();
		balls[i]->dpos = vector();
		balls[i]->vel = vector();
	}
	
	qt_init(&qt, SIZE, 10, 1);
	col_init();
}	

void setup_balls()
{
	FILE *fp = fopen("ball_properties.csv", "r");
	int i = 0;

	float posx = 0.0;
	float posy = 0.0;
	float velx = 0.0;
	float vely = 0.0;
	float m = 0.0;
	float r = 0.0;
	int marked = 0;
	
	while(
		fscanf(fp, "%f,%f,%f,%f,%f,%f,%d", &posx, &posy, 
			&velx, &vely, &m, &r, &marked) && i < NUM_BALLS)
	{
		balls[i]->pos[0] = posx; 
		balls[i]->pos[1] = posy;
		balls[i]->vel[0] = velx;
		balls[i]->vel[1] = vely;
		balls[i]->m_inv = m;
		balls[i]->r = r;
		balls[i]->marked = marked;

		i++;
	}

	fclose(fp);
}

void generate_balls_diffusion()
{
	float ball_spacing = SIZE / (sqrt(1.0 * NUM_BALLS) + 2.0);
	float max_vel = 0.5;

	float starting_pos = -1.0 * SIZE / 2.0 + ball_spacing;

	float b_posx = starting_pos;
	float b_posy = starting_pos;
	float b_velx;
	float b_vely;
	float b_r = 0.001;
	float b_m_inv = 1.0;
	int b_marked = 0;

	srand(time(NULL));

	FILE *fp = fopen("ball_properties.csv", "w+");

	for (int i = 0; i < NUM_BALLS; i++) {
		if((b_posx += ball_spacing) > 1.0 * SIZE / 2.0 - ball_spacing) {
			b_posx = starting_pos;
			if ((b_posy += ball_spacing) > 1.0 * SIZE / 2.0 - ball_spacing)
				break;
		}

		b_marked = 0;
		b_r = 0.001;
		b_m_inv = 1.0;
		if (b_posx <= -2.5 && b_posy >= 2.5) {
			b_marked = 1;
			b_r = 0.01;
			b_m_inv = 0.25;
		}

		b_velx = 2.0 * max_vel * rand() / (1.0 * RAND_MAX) - max_vel;
		b_vely = 2.0 * max_vel * rand() / (1.0 * RAND_MAX) - max_vel;

		fprintf(fp, "%f,%f,%f,%f,%f,%f,%d\n",
			b_posx, b_posy, b_velx,
			b_vely, b_m_inv, b_r, b_marked);
	}

	fclose(fp);
}

void generate_balls_brownian()
{
	float ball_spacing = SIZE / (sqrt(1.0 * NUM_BALLS) + 5.0);
	float max_vel = 0.0;

	float starting_pos = -1.0 * SIZE / 2.0 + ball_spacing;

	float b_posx = 0.0;
	float b_posy = 0.0;
	float b_velx = 0.0;
	float b_vely = 3.0;
	float b_r = 0.7;
	float b_m_inv = 0.01;
	int b_marked = 0;

	srand(time(NULL));

	FILE *fp = fopen("ball_properties.csv", "w+");

	fprintf(fp, "%f,%f,%f,%f,%f,%f,%d\n",
		b_posx, b_posy, b_velx,
		b_vely, b_m_inv, b_r, b_marked);

	b_posx = starting_pos;
	b_posy = starting_pos;
	b_r = 0.001;
	b_m_inv = 1.0;

	for (int i = 1; i < NUM_BALLS; i++) {
		if((b_posx += ball_spacing) > 1.0 * SIZE / 2.0 - ball_spacing) {
			b_posx = starting_pos;
			if ((b_posy += ball_spacing) > 1.0 * SIZE / 2.0 - ball_spacing)
				break;
		}
		
		if (b_posx > -0.7 && b_posx < 0.7 && b_posy < 0.7 && b_posy > -0.7) {
			i--;
			continue;
		}

		b_velx = 2.0 * max_vel * rand() / (1.0 * RAND_MAX) - max_vel;
		b_vely = 2.0 * max_vel * rand() / (1.0 * RAND_MAX) - max_vel;

		fprintf(fp, "%f,%f,%f,%f,%f,%f,%d\n",
			b_posx, b_posy, b_velx,
			b_vely, b_m_inv, b_r, b_marked);
	}

	fclose(fp);
}

void generate_balls()
{
	float ball_spacing = SIZE / (sqrt(1.0 * NUM_BALLS) + 2.0);
	float max_vel = 2.5;

	float starting_pos = -1.0 * SIZE / 2.0 + ball_spacing;

	float b_posx = starting_pos;
	float b_posy = starting_pos;
	float b_velx;
	float b_vely;
	float b_r = 0.7;
	float b_m_inv = 1.0;
	int b_marked = 0;

	srand(time(NULL));

	FILE *fp = fopen("ball_properties.csv", "w+");

	for (int i = 0; i < NUM_BALLS; i++) {
		if((b_posx += ball_spacing) > 1.0 * SIZE / 2.0 - ball_spacing) {
			b_posx = starting_pos;
			if ((b_posy += ball_spacing) > 1.0 * SIZE / 2.0 - ball_spacing)
				break;
		}

		b_velx = 2.0 * max_vel * rand() / (1.0 * RAND_MAX) - max_vel;
		b_vely = 2.0 * max_vel * rand() / (1.0 * RAND_MAX) - max_vel;

		fprintf(fp, "%f,%f,%f,%f,%f,%f,%d\n",
			b_posx, b_posy, b_velx,
			b_vely, b_m_inv, b_r, b_marked);
	}

	fclose(fp);
}

void quit()
{
	qt_free(qt);
	free(qt);

	col_free_vectors();

	for (int i = 0; i < NUM_BALLS; i++) {
		free_vector(balls[i]->pos);
		free_vector(balls[i]->vel);
		free(balls[i]);
	}

	free(balls);
	free(colls);

	exit(0);
}

void loop(int value)
{
	// This snippet makes OpenGL call this function at a constant (frame) rate,
	// while also allowing for code taking too long to run.
	setupTimer();
	currTime = getCurrentTime();
	timeSincePrevFrame = currTime - prevTime;
	prevTime = currTime;
	
	// Force screen to rerender when this function is run
	rerender();

	// Work out where the ball will move next, barring collisions
	for (int i = 0; i < NUM_BALLS; i++) {
		vscale(balls[i]->vel, timeSincePrevFrame * 0.001, balls[i]->dpos);
	}

	// clear and repopulate quadtree 
	qt_clear(qt);	
	qt_populate(balls, qt);

	int num_colls = 0;

	// Detect and resolve any ball-ball collisions
	for (int i = 0; i < NUM_BALLS; i++) {		
		num_colls = qt_retrieve(qt, balls[i], 0, colls);

		for (int j = 0; j < num_colls; j++) {

			if (collide_balls(balls[i], colls[j])) {
				vscale(balls[i]->vel, timeSincePrevFrame * 0.001, 
					balls[i]->dpos);
				vscale(colls[j]->vel, timeSincePrevFrame * 0.001, 
					colls[j]->dpos);

				//printf("%d %d\n", timeSincePrevFrame, num_colls);
			}
		}
	}
	

	// Resolve collisions with walls last
	for (int i = 0; i < NUM_BALLS; i++) {
		if (collide_walls(balls[i]))
			vscale(balls[i]->vel, timeSincePrevFrame * 0.001, balls[i]->dpos);
	}


	// Update the dpos vector and reset the state of the ball if it's collided
	// Then update the postions of the balls
	for (int i = 0; i < NUM_BALLS; i++) {
		vscale(balls[i]->vel, timeSincePrevFrame * 0.001, balls[i]->dpos);
		vadd(balls[i]->pos, balls[i]->dpos, balls[i]->pos);
	}

	
/*
	if (k++ == 100) {
		k = 0;
		//FILE *fp = fopen("output.csv", "w+");
		
		float ke = 0;

		for (int i = 0; i < NUM_BALLS; i++) {
			//fprintf(fp, "%f\n", vlength(balls[i]->vel));
			ke += vlength2(balls[i]->vel);
		}
		printf("%f\n", ke);

		//fclose(fp);
	}
*/
}

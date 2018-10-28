#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include "header.h"

#define NUM_TRIANGLES 30 // Resolution of circles
#define M_2PI 2.0f * M_PI
#define TIMERMSECS 33 // Max FPS

void render(void);
void drawCircle(float x, float y, float radius);
float scale(float len);

float scale_factor;
// List of objects to be drawn each frame
struct ball **objs;

void initGlut(int *argc, char **argv, int size, float sf, struct ball** items)
/*
	Initialise glut, open a window
*/
{
	// For scaling window to physics
	scale_factor = 2.0 / sf;
	objs = items;

	glutInit(argc, argv); 
	glutInitWindowPosition(-1,-1); // Window manager chooses position
	glutInitWindowSize(size, size);
	glutCreateWindow("N body simulator"); 
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA);

	// Calls loop function for display callback
	glutDisplayFunc(render);

	setupTimer();
	
	// Set background color
	glClearColor(0,0,0,0);

	// Enter main execution loop - runs until window closed
	glutMainLoop();
	
}

void render()
/*
	Render all the balls on the screen
*/
{
	// Clear back buffer ready for redrawing
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw in all the balls
	for (int i = 0; i < NUM_BALLS; i++) {
		if (objs[i]->marked) {
			glColor4f(0,1,1,0);
		} else {
			glColor4f(1,1,1,0);
		}
		drawCircle(
			objs[i]->pos[0],
			objs[i]->pos[1],
			objs[i]->r
		);
	}
 
 	// Swap front and back buffers
	glutSwapBuffers();
}

void drawCircle(float x, float y, float radius)
/*
	Draw a circle of radius radius, centre (x,y)
*/
{	
	x = scale(x);
	y = scale(y);
	if (radius <= 0.01) {
		glBegin(GL_POINTS);
   			glVertex2f(x, y);
		glEnd();
		return;
	}
	int numTriangles = (int)(NUM_TRIANGLES * sqrt(radius));

	radius = scale(radius);

	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y);
		for (int i = 0; i <= numTriangles; i++) {
			glVertex2f(
				x + (radius * cos(i* M_2PI / numTriangles)),
				y + (radius * sin(i * M_2PI / numTriangles))
			);
		}
	glEnd();
	return;
}

float scale(float len)
/*
	Scales physical lengths to GL lengths
*/
{
	return scale_factor * len;
}

int getCurrentTime()
/* 
	Used for constant rate animation 
*/
{
	return glutGet(GLUT_ELAPSED_TIME);
}

void rerender()
/*
	Forces a rerender of the screen
*/
{
	glutPostRedisplay();
}

void setupTimer()
{
	glutTimerFunc(TIMERMSECS, loop, 0);
}

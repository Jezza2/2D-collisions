CC=gcc
CFLAGS=-lm -lGL -lGLU -lglut
DEPS = header.h
OBJ = ball.c vector.c drawing.c collisions.c quadtree.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ball: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ 
	
clean:
	rm ./ball

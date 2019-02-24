#ifndef WATERFALL_H_
#define WATERFALL_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <GLFW/glfw3.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

struct Point
{
  float x, y, z;
}; // struct Point

struct Colour
{
  int r, g, b;
}; // struct Colour

struct Particle
{
  struct Point position;
  float speed;
  struct Point direction; // a unit vector
  float acceleration;
  int age;
  int lifespan;
  struct Colour colour;
  float opacity;
  float diameter;
}; // struct Particle

struct ListItem
{
  struct Particle * particle;
  struct ListItem * next;
}; // struct ListItem

struct Particle * create_particle(struct Point line_start,
                                  struct Point line_end);
void draw_particle(struct Particle * particle);
void draw_particles(struct ListItem * particles);
void draw_particles2(struct ListItem * particles);
struct Point initial_particle_position(struct Point line_start,
                                       struct Point line_end);
struct Point initial_particle_direction(void);
void draw_ejection_line(struct Point line_start,
                        struct Point line_end);
struct Point add_directions(struct Point dir_a, struct Point dir_b);
struct Point scalar_multiply(struct Point * point, float scalar);
void print_controls(void);
struct Point * set_viewpoints(void);
void average_ms_per_frame(void);

// Utility functions for handling linked lists
struct ListItem * add_particle(struct ListItem * list,
                               struct Particle * particle);
struct ListItem * create_item(void);
void push_item(struct ListItem ** list, struct ListItem * item);
struct ListItem * remove_item(struct ListItem ** list, struct ListItem * item);
ListItem * pop_front(ListItem * head);

// General utility functions
double random_probability(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void reshape(int width, int height);
void create_axes(void);
void initialise_system(int argc, char * argv[]);

#endif

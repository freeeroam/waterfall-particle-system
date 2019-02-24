#include "waterfall.h"

struct Point initial_direction = {0.0, -0.2, -1.0};
struct Point direction_variance = {0.2, 0.3, 0.1};
float diameter_mean = 0.5;
float diameter_variance = 0.05;
int new_particles_mean = 50.0;
float new_particles_variance = 5.0;
float speed_mean = 0.4;
float speed_variance = 0.05;
struct Colour water_colour = {0, 153, 204};
float opacity_mean = 0.7;
float opacity_variance = 0.1;
int max_age = 70;
float acceleration_gravity = 0.0098;
struct Point gravity_direction = {0.0, -0.1, 0.0};
float wind_scale = 1000.0;
struct Point wind_direction = {-1.0, 0.0, 0.0};
struct Point line_start = {0.0, 50.0, 0.0};
struct Point line_end = {100.0, 50.0, 0.0};
struct Point * viewpoints = NULL;
int current_view = 0;

int current_time = 0;
struct ListItem * particles = NULL;
int particles_removed = 0; // for debugging

// Global variables for testing performance
double last_time = 0;
double current_time_fps = 0;
int num_frames = 0;
double ms_per_frame = 0;
double total_ms_per_frame = 0;
int num_checks = 0;

GLuint axisList; // Display list for coordinate axis
int AXIS_SIZE = 200;
int axis_enabled = 1;

int main(int argc, char *argv[])
{
  if (!glfwInit())
  {
    return -1;
  } // if

  last_time = glfwGetTime();

  particles = create_item();

  print_controls();
  viewpoints = set_viewpoints();
  srand(time(NULL));
  initialise_system(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();

  glfwTerminate();
  return 0;
} // function main

//////////////////////////////////////////////////////////////////////////////

//Return random double within range [0,1]
double random_probability(void)
{
  return (rand() / (double)RAND_MAX);
} // function random_probability

void display(void)
{
  /** The following FPS counter was taken and modified from www.opengl-tutorial.org **/
  current_time_fps = glfwGetTime();
  num_frames++;
  if (current_time_fps - last_time >= 1.0)
  {
    ms_per_frame = 1000.0 / (double) num_frames;
    total_ms_per_frame += ms_per_frame;
    num_checks++;
    num_frames = 0;
    last_time += 1;
  } // if
  //////////////////////////////////////////////////////////////////////////////////////


  glLoadIdentity();
  gluLookAt(viewpoints[current_view].x, viewpoints[current_view].y,
            viewpoints[current_view].z,
            25.0, 30.0, 0.0,
            0.0, 1.0, 0.0);
  // Clear the screen
  glClearColor(0.0, 0.0, 0.0, 2.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // If enabled, draw coordinate axis
  if (axis_enabled)
  {
    glCallList(axisList);
  } // if

  // Draw particles
  draw_ejection_line(line_start, line_end);
  draw_particles(particles);

  glutSwapBuffers();
  glutPostRedisplay();
} // function display

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: //ESC
      average_ms_per_frame();
      getchar();
      exit(0);
      break;
    case 'v':
      if (current_view == 2)
      {
        current_view = 0;
      } else
      {
        current_view++;
      } // else
      break;
    case 'z': // Enable/disable axes
      axis_enabled = !axis_enabled;
      break;
    case 'q': // Double initial speed
      speed_mean *= 2;
      break;
    case 'a': // Half the initial speed
      speed_mean *= 0.5;
      break;
    case 'w': // Double the acceleration of gravity
      acceleration_gravity *= 2;
      break;
    case 's': // Half the acceleration of gravity
      acceleration_gravity *= 0.5;
      break;
    case 'e': // Increase number of particles by 20%
      new_particles_mean *= 1.2;
      break;
    case 'd': // Decrease number of particles by 20%
      if (new_particles_mean * 1.2 > 20)
      {
        new_particles_mean *= 0.2;
      } // if
      break;
    case 'r': // Increase particle size by 50%;
      diameter_mean *= 1.5;
      break;
    case 'f': // Decrease particle size by 50%;
      diameter_mean *= 0.5;
      break;
    case 't': // Increase particle opacity by 0.1
      if (opacity_mean < 1)
      {
        opacity_mean += 0.1;
      } // if
      break;
    case 'g': // Decrease particle opacity by 0.1
      if (opacity_mean > 0)
      {
        opacity_mean -= 0.1;
      } // if
      break;
    default:
      printf("This key does nothing.\n");
  } // switch
  glutPostRedisplay();
} // function keyboard

void reshape(int width, int height)
{
  glClearColor(0.9, 0.9, 0.9, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
} // function reshape

void create_axes(void)
{
  // Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
    glLineWidth(2.0);
    glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
} // function create_axes

void initialise_system(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("Waterfall");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  create_axes();
} // function initialise_system

//////////////////////////////////////////////////////////////////////////////

struct Particle * create_particle(struct Point line_start,
                                  struct Point line_end)
{
  struct Particle * particle =
    (struct Particle *) malloc((int) sizeof(struct Particle));
  particle->position = initial_particle_position(line_start, line_end);
  particle->speed = speed_mean + (float) random_probability() * speed_variance;
  particle->direction = add_directions(initial_particle_direction(),
                                       scalar_multiply(&wind_direction,
                                                       wind_scale));
  particle->lifespan = max_age;
  particle->age = 0;
  particle->colour = water_colour;
  particle->opacity = opacity_mean + (float) random_probability() * opacity_variance;
  particle->diameter = diameter_mean + (float) random_probability() * diameter_variance;
  particle->acceleration = acceleration_gravity;
  return particle;
} // function create_particle

void draw_particle(struct Particle * particle)
{
  // Calculate particle attributes at current time step
  float old_speed = particle->speed;
  struct Point old_direction = particle->direction;
  particle->direction.x += gravity_direction.x;
  particle->direction.y += gravity_direction.y;
  particle->direction.z += gravity_direction.z;
  particle->speed += particle->acceleration;
  particle->position.x += (old_speed * old_direction.x
    + particle->speed * particle->direction.x) / 2;
  particle->position.y += (old_speed * old_direction.y
    + particle->speed * particle->direction.y) / 2;
  particle->position.z += (old_speed * old_direction.z
    + particle->speed * particle->direction.z) / 2;
  particle->age++;

  glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f((float) particle->colour.r, (float) particle->colour.g,
              (float) particle->colour.b, particle->opacity);
    glTranslatef(particle->position.x, particle->position.y,
                 particle->position.z);
    glutSolidSphere(particle->diameter / 2, 10, 10);
  glPopMatrix();

  /**
  glBegin(GL_POINTS);
    glPointSize(1.0);
    glColor3f((float) particle->colour.r, (float) particle->colour.g,
              (float) particle->colour.b);
    glVertex3f(particle->position.x, particle->position.y,
               particle->position.z);
  glEnd();
  **/
} // function draw_particle

void draw_particles(struct ListItem * particles)
{
  struct ListItem * current_item = particles;

  // Remove dead particles
  while (particles != NULL
         && particles->particle->age > particles->particle->lifespan)
  {
    particles = pop_front(particles);
  } // while

  // Create and initialise new particles
  int num_new_particles = new_particles_mean + (float) random_probability()
    * new_particles_variance;
  int count = 0;
  for (count = 0; count < num_new_particles; count++)
  {
    push_item(&particles, create_item());
  } // for

  // Update attributes for each particle
  current_item = particles;
  while (current_item != NULL)
  {
    draw_particle(current_item->particle);
    current_item = current_item->next;
  } // while
  current_time++;
} // function draw_particles

void draw_particles2(struct ListItem * particles)
{
  struct ListItem * current_item = particles;
  int num_new_particles = new_particles_mean + (float) random_probability()
    * new_particles_variance;

  // Create and initialise new particles
  int count = 0;
  for (count = 0; count < num_new_particles; count++)
  {
    push_item(&particles, create_item());
  } // for

  // Remove particles if dead or update their position
  while (current_item != NULL)
  {
    // Remove particle if dead
    if (current_item->particle->age + 1 > current_item->particle->lifespan)
    {
      particles_removed++;
      current_item = remove_item(&particles, current_item);
      continue;
    } // if

    // Update particle position
    draw_particle(current_item->particle);
    current_item = current_item->next;
  } // while
  current_time++;
} // function draw_particles2

struct Point initial_particle_position(struct Point line_start,
                                       struct Point line_end)
{
  float vector_x = line_end.x - line_start.x;
  float vector_y = line_end.y - line_start.y;
  float vector_z = line_end.z - line_start.z;
  double random_val = random_probability();
  struct Point initial_position = {
    line_start.x + vector_x * (float) random_val,
    line_start.y + vector_y * (float) random_val,
    line_start.z + vector_z * (float) random_val
  };
  return initial_position;
} // function initial_particle_position

void draw_ejection_line(struct Point line_start,
                        struct Point line_end)
{
  glBegin(GL_LINES);
   glLineWidth(1.0);
   glVertex3f(line_start.x, line_start.y, line_start.z);
   glVertex3f(line_end.x, line_end.y, line_end.z);
  glEnd();
} // function draw_ejection_line

// Utility functions for handling linked lists
struct ListItem * add_particle(struct ListItem * list,
                               struct Particle * particle)
{
  // Create a new ListItem for the particle
  struct ListItem * particle_item = create_item();
  particle_item->particle = particle;
  particle_item->next = NULL;

  // Add item to end of the list
  struct ListItem * current_item = list;
  while (current_item->next != NULL)
  {
    current_item = current_item->next;
  } // while
  current_item->next = particle_item;
  return particle_item;
} // function add_particle

struct ListItem * create_item(void)
{
  struct ListItem * item =
    (struct ListItem *) malloc((int) sizeof(struct ListItem));
  item->particle = create_particle(line_start, line_end);
  item->next = NULL;
  return item;
} // function create_item

void push_item(struct ListItem ** list, struct ListItem * item)
{
  if (list != NULL)
  {
    if (*list == NULL)
    {
      *list = item;
    } else
    {
      struct ListItem * current_item = *list;
      while (current_item->next != NULL)
      {
        current_item = current_item->next;
      } // while
      current_item->next = item;
    } // else
  } // if
} // function push_item

struct ListItem * remove_item(struct ListItem ** list, struct ListItem * item)
{
  if (list == NULL || *list == NULL)
  {
    return NULL;
  } // if

  struct ListItem * current = *list;
  struct ListItem * previous = NULL;

  while (current != item && current->next != NULL)
  {
    previous = current;
    current = current->next;
  } // while

  if (current == item)
  {
    if (previous != NULL)
    {
      previous->next = current->next;
    } else
    {
      *list = current->next;
    } // else
    current = current->next;
    free(item);
    return current;
  } // if

  return NULL;
} // function remove_item

// Removes the item at the front of the list and returns the new front of the list.
ListItem * pop_front(ListItem * head)
{
  if (head != NULL && head->next != NULL)
  {
    return head->next;
  } // if
  return NULL;
} // function pop_front

struct Point add_directions(struct Point dir_a, struct Point dir_b)
{
  struct Point resultant;
  resultant.x = dir_a.x + dir_a.x;
  resultant.y = dir_a.y + dir_a.y;
  resultant.z = dir_a.z + dir_a.z;
  return resultant;
} // function add_directions

struct Point scalar_multiply(struct Point * point, float scalar)
{
  struct Point result;
  result.x = point->x * scalar;
  result.y = point->y * scalar;
  result.z = point->z * scalar;
  return result;
} // function scalar_multiply

struct Point initial_particle_direction(void)
{
  struct Point direction = initial_direction;
  direction.x += random_probability() * direction_variance.x;
  direction.y += random_probability() * direction_variance.y;
  direction.z += random_probability() * direction_variance.z;
  return direction;
} // function initial_particle_direction

void print_controls(void)
{
  printf("Controls:\n");
  printf("Increase/decrease initial velocity: q/a\n");
  printf("Increase/decrease acceleration of gravity: w/s\n");
  printf("Increase/decrease number of particles generated: e/d\n");
  printf("Increase/decrease particle size: r/f\n");
  printf("Increase/decrease particle opacity: t/g\n");
  printf("Enable/disable axes: z\n");
  printf("Next view: v\n");
} // function print_controls

struct Point * set_viewpoints(void)
{
  struct Point * views  = (struct Point *) malloc(sizeof(struct Point) * 3);
  views[0].x = -40.0; views[0].y = 50.0; views[0].z = -100.0; // view 0
  views[1].x = -40.0; views[1].y = 50.0; views[1].z = 0.0; // view 1
  views[2].x = 40.0; views[2].y = 50.0; views[2].z = -100.0; // view 2
  return views;
} // function set_viewpoints

void average_ms_per_frame(void)
{
  printf("Average ms/frame: %.2f", total_ms_per_frame / (double) num_checks);
} // function average_ms_per_frame

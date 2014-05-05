#ifndef RUN_SIMULATION
#define RUN_SIMULATION
#include<windows.h>
#include<stdio.h>
#include<math.h>

/* Constant defines */

/*Number of Faces */
#define MAX_FACES  4
#define MAX_POINTS 4
#define AREA 1
#define SPRING_CONSTANT 10
#define PRESSURE_VALUE 30
#define FORCE_FACTOR 0.01
#define TIME_STEPS 300
#define POINTS_PER_FACE 3
#define NORMAL_LOG_NAME "tetra_model_normals.csv"
#define FORCE_LOG_NAME "tetra_model_forces.csv"
#define POINT_LOG_NAME "tetra_model_points.csv"

typedef struct point_struct {
    double pos[3];
    double delta_pos[3];
    double net_force[3];
} Point;

typedef struct face_struct {
    Point * points[POINTS_PER_FACE];
    /* Spring_constants[0] -> segment opposite vertex 0
       This makes the spring constant between points a and b 
        equal (a + b) % 2
    */
    double spring_constants[3];
    double normal[3];
} Face;


Face faces[MAX_FACES];
Point points[MAX_POINTS];

void change_state();
void reset_net_force();
void add_force(Point * point, double * force);
void calculate_forces(Face * face, int point_index);
void update_pos(Point * point);
void update_normal(Face * face);
double get_norm(double x, double y, double z);

void clear_file(const char *);
void log_normals();
void log_forces();
void log_points();
void print_all_faces();
void print_face(Face * face);
void print_all_points();
void print_point(Point * point);

/* for each face:
       for each point in face:
           calc pressure force from face, add to net_force
           calc spring force between point and other points and add
               to net force
*/
#endif
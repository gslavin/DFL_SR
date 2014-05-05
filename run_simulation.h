#ifndef RUN_SIMULATION
#define RUN_SIMULATION

#include "parse_stl.h"

/*Number of Faces */
#define AREA 1
#define SPRING_CONSTANT 10
#define PRESSURE_VALUE 3
#define FORCE_FACTOR 0.01
#define TIME_STEPS 300
#define NORMAL_LOG_NAME "tetra_model_normals.csv"
#define FORCE_LOG_NAME "tetra_model_forces.csv"
#define POINT_LOG_NAME "tetra_model_points.csv"

void change_state(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
void reset_net_force( Point * points, uint32_t total_points);
void add_force(Point * point, double * force);
void calculate_forces(Face * face, int point_index);
void update_pos(Point * point);
void update_normal(Face * face);
double get_norm(double x, double y, double z);

void clear_file(const char *);
void log_normals(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
void log_forces(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
void log_points(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
void print_all_faces(Face * faces, uint32_t max_faces);
void print_face(Face * face);
void print_all_points(Point * points, uint32_t total_points);
void print_point(Point * point);

void free_faces(Face * faces);
void free_points(Point * points);

/* for each face:
       for each point in face:
           calc pressure force from face, add to net_force
           calc spring force between point and other points and add
               to net force
*/
#endif
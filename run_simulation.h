#ifndef RUN_SIMULATION
#define RUN_SIMULATION

#include "parse_stl.h"

/*STL NAMES */
#define OUTPUT_STL "new_cylinder.stl"

/*Number of Faces */
#define BOUND_LIMIT 50000
#define AREA 1
#define SPRING_CONSTANT 1
#define PRESSURE_VALUE 1
#define FORCE_FACTOR 0.01 
#define TIME_STEPS 300
#define NORMAL_LOG_NAME "tetra_model_normals.csv"
#define FORCE_LOG_NAME "tetra_model_forces.csv"
#define POINT_LOG_NAME "tetra_model_points.csv"

/* error codes */
typedef enum sim_error_codes {
    SIM_E_NONE = 0,
    SIM_E_LOG_FILE = -1
} sim_error_t;

sim_error_t clear_logs();
sim_error_t clear_file(const char *);
sim_error_t log_normals(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
sim_error_t log_forces(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
sim_error_t log_points(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);

void get_center_coordinates(Point * points, uint32_t max_points, float * center_coordinates);

sim_error_t run_simulation(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
void change_state(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points);
void reset_net_force( Point * points, uint32_t total_points);
void add_force(Point * point, double * force);
void calculate_forces(Face * face, int point_index);
int point_in_bounds(Point * point);
void update_pos(Point * point);
void update_normal(Face * face);
double get_norm(double x, double y, double z);

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

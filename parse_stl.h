#ifndef PARSE_STL
#define PARSE_STL

/* standard imports */
#include<windows.h>
#include<stdio.h>
#include<stdint.h> 
#include<math.h>

/* error codes */
typedef enum _error_codes {
    E_NONE = 0,
    E_NORMAL = -1,    
    E_VERTEX = -2,
    E_FACE = -3,
    E_HEADER = -4,
    E_NUM_FACES = -5,
    E_ATTRIBUTE = -6
} error_t;



/* MAX_FACES should be determined by the # of triangles
   in the STL */
#define POINTS_PER_FACE 3
#define HEADER_LENGTH 80

uint32_t max_faces = 0;
uint32_t max_points = 0;
uint32_t total_points = 0;

typedef struct point_struct {
    float pos[3];
    float delta_pos[3];
    float net_force[3];
} Point;

typedef struct face_struct {
    Point * points[POINTS_PER_FACE];
    /* Spring_constants[0] -> segment opposite vertex 0
       This makes the spring constant between points a and b 
       equal (a + b) % 2
     */
    float spring_constants[3];
    float normal[3];
} Face;


Face * faces = NULL;
Point * points = NULL;

/* parses the stl and returns the filled Face and Point arrays */
error_t parse_stl(const char * stl_name);

/* parses the next facet in the stl file */
error_t parse_facet(int face_index, FILE * stl_file);

/* finds the vertex in the point array*/
Point * find_vertex(float * vertex_pos);
#endif
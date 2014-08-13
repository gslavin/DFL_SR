#ifndef PARSE_STL
#define PARSE_STL

/* standard imports */
#if defined(_WIN32) || defined(WIN32)
	#include <windows.h>

#elif __unix__
	#include <stdlib.h>
	#include <string.h>

#endif

#include <stdio.h>
#include <stdint.h> 
#include <math.h>

/* MAX_FACES should be determined by the # of triangles
   in the STL */
#define HEADER_LENGTH 80
#define POINTS_PER_FACE 3
/* error codes */
typedef enum stl_error_codes {
   STL_E_NONE = 0,
   STL_E_NORMAL = -1,    
   STL_E_VERTEX = -2,
   STL_E_FACE = -3,
   STL_E_HEADER = -4,
   STL_E_NUM_FACES = -5,
   STL_E_ATTRIBUTE = -6
} stl_error_t;

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


/* parses the stl and returns the filled Face and Point arrays */
stl_error_t parse_stl(const char * stl_name, Face ** faces, Point ** points,
    uint32_t * max_faces, uint32_t * max_points, uint32_t * total_points);

/* parses the next facet in the stl file */
stl_error_t parse_facet(int face_index, FILE * stl_file,  Face * faces, Point * points,
    uint32_t * total_points);

/* finds the vertex in the point array*/
Point * find_vertex(float * vertex_pos,  Face * faces, Point * points,
    uint32_t * total_points);
    
/* write the face data back into an stl */
stl_error_t write_stl(const char * stl_name, Face * faces, uint32_t max_faces);

stl_error_t write_facet(int face_index, FILE * stl_file, Face * faces);
#endif

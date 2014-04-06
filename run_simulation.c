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
#define MAX_NEIGHBORS 3
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

int main() {
    int i,j,k;

    /////////////////////////////////////Dumby data fill
    //dumby points
    points[0].pos[0] = 0; points[0].pos[1] = 0; points[0].pos[2] = 0;
    points[1].pos[0] = 1; points[1].pos[1] = 0; points[1].pos[2] = 0;
    points[2].pos[0] = 0; points[2].pos[1] = 1; points[2].pos[2] = 0;
    points[3].pos[0] = 0, points[3].pos[1] = 0, points[3].pos[2] = 1;

    //dumby faces
    //set points in face
    faces[0].points[0] = points+0;
    faces[0].points[1] = points+1;
    faces[0].points[2] = points+2;
    faces[0].normal[0] = 0;
    faces[0].normal[1] = 0;
    faces[0].normal[2] = -1;

    faces[1].points[0] = points+3;
    faces[1].points[1] = points+1;
    faces[1].points[2] = points+2;
    faces[1].normal[0] = 0.5774;
    faces[1].normal[1] = 0.5774;
    faces[1].normal[2] = 0.5774;

    faces[2].points[0] = points+3;
    faces[2].points[1] = points+0;
    faces[2].points[2] = points+2;
    faces[2].normal[0] = -1;
    faces[2].normal[1] = 0;
    faces[2].normal[2] = 0;

    faces[3].points[0] = points+3;
    faces[3].points[1] = points+1;
    faces[3].points[2] = points+0;
    faces[3].normal[0] = 0;
    faces[3].normal[1] = -1;
    faces[3].normal[2] = 0;

    //set spring constants
    for(i = 0; i < MAX_FACES; i++) {
        for(j = 0; j < POINTS_PER_FACE; j++) {
            faces[i].spring_constants[j] = SPRING_CONSTANT;
        }
    }
    ////////////////////////////////////////Dumby data fill

    //clear csv log files
    clear_file(NORMAL_LOG_NAME);
    clear_file(FORCE_LOG_NAME);
    clear_file(POINT_LOG_NAME);

    /* calculate new normal of faces */
    for(i = 0; i < MAX_FACES; i++) {
        update_normal(&faces[i]);
    }
    print_all_faces();
    for(i = 0; i < TIME_STEPS; i++) {
        reset_net_force();
        change_state();
        /* print_all_points(); */
        log_normals();
        log_forces();
        log_points();
    }
    return 0;
}

void
clear_file(const char * name)
{
    FILE * fp = NULL;

    fp = fopen(name, "w");
    if (fp == NULL) {
        printf("Error Opening %s\n", name);
        return;
    }
    fclose(fp);
}

/* logs point pos each iteration to the line of a csv */
void
log_normals()
{
    FILE * fp = NULL;
    int i, j;

    fp = fopen(NORMAL_LOG_NAME, "a");
    if (fp == NULL) {
        printf("Error Opening normal log file\n");
        return;
    }
    /* each line is a point */
    for(i = 0; i < MAX_FACES; i++) {
        for(j = 0; j < 3; j++) {
            fprintf(fp, "%f ", faces[i].normal[j]);
        }
        fprintf(fp, "\n");
    }
    /* blank lines delimit iterations */
    fprintf(fp, "\n");
    fclose(fp);
}

/* logs point pos each iteration to the line of a csv */
void
log_forces()
{
    FILE * fp = NULL;
    int i, j;

    fp = fopen(FORCE_LOG_NAME, "a");
    if (fp == NULL) {
        printf("Error Opening force log file\n");
        return;
    }
    /* each line is a point */
    for(i = 0; i < MAX_POINTS; i++) {
        for(j = 0; j < 3; j++) {
            fprintf(fp, "%f ", points[i].net_force[j]);
        }
        fprintf(fp, "\n");
    }
    /* blank lines delimit iterations */
    fprintf(fp, "\n");
    fclose(fp);
}

/* logs point pos each iteration to the line of a csv */
void
log_points()
{
    FILE * fp = NULL;
    int i, j;

    fp = fopen(POINT_LOG_NAME, "a");
    if (fp == NULL) {
        printf("Error Opening point log file\n");
        return;
    }
    /* each line is a point */
    for(i = 0; i < MAX_POINTS; i++) {
        for(j = 0; j < 3; j++) {
            fprintf(fp, "%f ", points[i].pos[j]);
        }
        fprintf(fp, "\n");
    }
    /* blank lines delimit iterations */
    fprintf(fp, "\n");
    fclose(fp);
}


/* resets the net force.  Used after each iteration
   so the forces don't keep piling up */
void
reset_net_force()
{
    int i, j;

    for(i = 0; i < MAX_POINTS; i++) {
        for(j = 0; j < 3; j++) {
            points[i].net_force[j] = 0;
        }
    }
}

/* step through 1 time step */
void
change_state()
{
    int i, j, k;
    
    /* calculate new normal of faces */
    for(i = 0; i < MAX_FACES; i++) {
        update_normal(&faces[i]);
    }

    /* calculate forces */
    for(i = 0; i < MAX_FACES; i++) {
        for(j = 0; j < POINTS_PER_FACE; j++) {
            /* TODO: change to faces+i? */
            calculate_forces(&faces[i],j);
        }
    }

    /* calculate new position of points */
    for(i = 0; i < MAX_POINTS; i++) {
        update_pos(&points[i]);
    }
}

void 
print_all_faces()
{
    int i;

    for(i = 0; i < MAX_FACES; i++) {
        printf("FACE: %d\n", i);
        print_face(faces+i);
        printf("\n\n");
    }
}

void 
print_all_points()
{
    int i;

    for(i = 0; i < MAX_POINTS; i++) {
        printf("Point: %d\n", i);
        print_point(points+i);
        printf("\n\n");
    }
}

static void
_get_vectors(Face * face, double * v1, double * v2)
{
    int i;

    for(i = 0; i < 3; i++) {
        v1[i] = face->points[1]->pos[i] - face->points[0]->pos[i];
        v2[i] = face->points[2]->pos[i] - face->points[0]->pos[i];
    }
}

static double
_get_norm(double x, double y, double z)
{
    return sqrt(x*x + y*y + z*z);
}

static int
_get_sign_for_norm(double * new_normal, double * normal)
{
    double sum = 0;
    int i;

    for(i = 0; i < 3; i++) {
        sum += new_normal[i]*normal[i];
    }
    /*extract and return sign of dot product (clever/no branching)*/
    return (sum > 0) - (sum < 0);
}

/*TODO: unsure if normal is facing the right way
take dot product of normal and new_normal
if negitive flip sign of new_normal*/
void
update_normal(Face * face)
{
    double new_normal[3];
    double v1[3], v2[3];
    double norm;
    int i, sign = 1;
    /* use the three points to get two vectors */

    _get_vectors(face, v1, v2);
    /*calculate cross_product */
    new_normal[0] = v1[1]*v2[2] - v1[2]*v2[1];
    new_normal[1] = v2[0]*v1[2] - v1[0]*v2[2];
    new_normal[2] = v1[0]*v2[1] - v2[0]*v1[1]; 

    norm = _get_norm(new_normal[0], new_normal[1], new_normal[2]);
    sign = _get_sign_for_norm(new_normal, face->normal);
    /* normalize and set Face normal to new normal */
    for(i = 0; i < 3; i++) {
        new_normal[i] /= norm;
        face->normal[i] = sign*new_normal[i];
    }
}

void 
add_force(Point * point, double * force)
{
    int i;

    for(i = 0; i < 3; i++) {
        point->net_force[i] +=  force[i];
    }
}

void 
calculate_forces(Face * face, int point_index)
{
    double pressure_force[3];
    double spring_force[3];
    double delta_pos[3];
    int i, j;

    /* calc pressure force from face */
    for(i = 0; i < 3; i++) {
        pressure_force[i] = AREA*(face->normal[i]*PRESSURE_VALUE);
    }
    add_force(face->points[point_index], pressure_force);

    /* calc spring forces from other vertices */
    /* x, y, z components */
    for(i = 0; i < 3; i++) {
        /* other points on face*/
        for(j = 0; j < 3; j++) {
            /* don't calc spring force from opposite segment
               (segment not compared to itself)*/ 
            if (j == point_index) {
                continue;
            }
            /* TODO: every segment belongs to two faces,
               so the spring force will be calculated twice.
               should then divide by 2? */
            delta_pos[i] = face->points[j]->delta_pos[i] - 
                face->points[point_index]->delta_pos[i];
            spring_force[i] = (face->
                /* 2*(j+point_index)%3 maps vertex pairs to
                   their spring constant */
                spring_constants[2*(j+point_index)%3]*delta_pos[i])/2;
        }
    }
    #if 0
    printf("SPRING_FORCE: %f %f %f\n", spring_force[0],
        spring_force[1], spring_force[2]);
    #endif
    add_force(face->points[point_index], spring_force);
}

void 
update_pos(Point * point)
{
    int i;
    double delta_pos[3];

    for(i = 0; i < 3; i++) {
        delta_pos[i] = point->net_force[i]*FORCE_FACTOR;
        point->pos[i] += delta_pos[i];
        point->delta_pos[i] += delta_pos[i];
    }
}

void
print_face(Face * face)
{
    int i;
    
    printf("SPRING_CONSTANTS: ");
    for(i = 0; i < 3; i++) {
        printf("%f ", face->spring_constants[i]);
    }
    printf("\n");

    printf("NORMAL: ");
    for(i = 0; i < 3; i++) {
        printf("%f ", face->normal[i]);
    }
    printf("\n");

    for(i = 0; i < POINTS_PER_FACE; i++) {
        print_point(face->points[i]);
    }
}

void
print_point(Point * point)
{
    int i;
    printf("POS      : ");
    for(i = 0; i < 3; i++) {
        printf("%f ", point->pos[i]);
    }
    printf("\n");

    printf("DELTA_POS: ");
    for(i = 0; i < 3; i++) {
        printf("%f ", point->delta_pos[i]);
    }
    printf("\n");

    printf("NET_FORCE: ");
    for(i = 0; i < 3; i++) {
        printf("%f ", point->net_force[i]);
    }
    printf("\n");
    printf("\n");
}

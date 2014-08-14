#include "run_simulation.h"

void show_usage(void);


int main(int argc, char *argv[]) {

	/* parse args */
	int opt_write_stl = 0;

	if (argc == 3){
		opt_write_stl = 1;
	}
	else if (argc != 2){
		show_usage();
		return 1;
	}

    Face * faces = NULL;
    Point * points = NULL;
    uint32_t max_faces = 0;
    /* max points = 3*max_faces */
    uint32_t max_points = 0;
    /* total points = total number of independent points */
    uint32_t total_points = 0;
    int i,j, ret;
    float center_coordinates[3] = {0,0,0};
    int tally_lower = 0, tally_higher = 0;
    
    /* parse the stl file */
    ret = parse_stl(argv[1], &faces, &points, &max_faces,
        &max_points, &total_points);
    if(ret < 0) {
        printf("STL Error %d\n", ret);
		return 1;
    }
    printf("There are %d faces\n", max_faces);
    printf("There are %d points\n", total_points);

    get_center_coordinates(points, total_points, center_coordinates);
    printf("CENTER COORDINATES: %f %f %f\n", center_coordinates[0], center_coordinates[1], center_coordinates[2]);
    /* Define center point and split stl in half
    One half has a high Spring Constant, the another has a low Spring Constant
    */
    //set spring constants (Points on the lowr half of the stl will have larger spring constant)
    for(i = 0; i < max_faces; i++) {
        for(j = 0; j < POINTS_PER_FACE; j++) {
            if (faces[i].points[j]->pos[1] > center_coordinates[1]) {
                faces[i].spring_constants[j] = SPRING_CONSTANT;
                tally_higher++;
            }
            else {
                faces[i].spring_constants[j] = 20*SPRING_CONSTANT;
                tally_lower++;
            }
        }
    }
    printf("TALLY_H: %d\n", tally_higher);
    printf("TALLY_L: %d\n", tally_lower);
    
    /* clear all log files */
    clear_logs();
    
    /* run sim for TIME_STEPS time steps */
    printf("Sim running for %d iteration\n", TIME_STEPS);
    run_simulation(faces, max_faces, points, total_points);
    
    /* write to a new stl */
	if (opt_write_stl){
		printf("Writing final state to stl %s\n", argv[2]);
		ret = write_stl(argv[2], faces, max_faces);
		if(ret < 0) {
			printf("STL Error %d", ret);
		}
	}
    /* free all malloc objects */
    free_faces(faces);
    free_points(points);
    
    return 0;
}

void get_center_coordinates(Point * points, uint32_t total_points, float * center_coordinates)
{
    int i, j;
    
    for(i = 0; i < total_points; i++) {
        for(j = 0; j < 3; j++) {
            center_coordinates[j] += points[i].pos[j];
        }
    }
    for(j = 0; j < 3; j++) {
        center_coordinates[j] /= total_points;
    }
}

sim_error_t
clear_logs()
{
    clear_file(NORMAL_LOG_NAME);
    clear_file(FORCE_LOG_NAME);
    clear_file(POINT_LOG_NAME);

    return SIM_E_NONE;
}

sim_error_t
run_simulation(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points)
{
    int i;
    /* calculate new normal of faces */
    for(i = 0; i < max_faces; i++) {
        update_normal(&faces[i]);
    }
    //print_all_faces(faces, max_faces);
    for(i = 0; i < TIME_STEPS; i++) {
        reset_net_force(points, total_points);
        change_state(faces, max_faces, points, total_points);
        /* print_all_points(); */
        log_normals(faces, max_faces, points, total_points);
        log_forces(faces, max_faces, points, total_points);
        log_points(faces, max_faces, points, total_points);
    }
    
    return SIM_E_NONE;
}

sim_error_t
clear_file(const char * name)
{
    FILE * fp = NULL;
    int ret_err = SIM_E_NONE;

    fp = fopen(name, "w");
    if (fp == NULL) {
        printf("Error Opening %s\n", name);
        ret_err = SIM_E_LOG_FILE;
    }
    fclose(fp);
    
    return ret_err;
}

/* logs point pos each iteration to the line of a csv */
sim_error_t
log_normals(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points)
{
    FILE * fp = NULL;
    int i, j;
    int ret_err = SIM_E_NONE;

    fp = fopen(NORMAL_LOG_NAME, "a");
    if (fp == NULL) {
        printf("Error Opening normal log file\n");
        ret_err = SIM_E_LOG_FILE;
    }
    /* each line is a point */
    for(i = 0; i < max_faces; i++) {
        for(j = 0; j < 3; j++) {
            fprintf(fp, "%f ", faces[i].normal[j]);
        }
        fprintf(fp, "\n");
    }
    /* blank lines delimit iterations */
    fprintf(fp, "\n");
    fclose(fp);
    
    return ret_err;
}

/* logs point pos each iteration to the line of a csv */
sim_error_t
log_forces(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points)
{
    FILE * fp = NULL;
    int i, j;
    int ret_err = SIM_E_NONE;

    fp = fopen(FORCE_LOG_NAME, "a");
    if (fp == NULL) {
        printf("Error Opening force log file\n");
        ret_err = SIM_E_LOG_FILE;
    }
    /* each line is a point */
    for(i = 0; i < total_points; i++) {
        for(j = 0; j < 3; j++) {
            fprintf(fp, "%f ", points[i].net_force[j]);
        }
        fprintf(fp, "\n");
    }
    /* blank lines delimit iterations */
    fprintf(fp, "\n");
    fclose(fp);
    
    return ret_err;
}

/* logs point pos each iteration to the line of a csv */
sim_error_t
log_points(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points)
{
    FILE * fp = NULL;
    int i, j;
    int ret_err = SIM_E_NONE;

    fp = fopen(POINT_LOG_NAME, "a");
    if (fp == NULL) {
        printf("Error Opening point log file\n");
        ret_err = SIM_E_LOG_FILE;
    }
    /* each line is a point */
    for(i = 0; i < total_points; i++) {
        for(j = 0; j < 3; j++) {
            fprintf(fp, "%f ", points[i].pos[j]);
        }
        fprintf(fp, "\n");
    }
    /* blank lines delimit iterations */
    fprintf(fp, "\n");
    fclose(fp);
    
    return ret_err;
}


/* resets the net force.  Used after each iteration
   so the forces don't keep piling up */
void
reset_net_force(Point * points, uint32_t total_points)
{
    int i, j;

    for(i = 0; i < total_points; i++) {
        for(j = 0; j < 3; j++) {
            points[i].net_force[j] = 0;
        }
    }
}

/* step through 1 time step */
void
change_state(Face * faces, uint32_t max_faces, Point * points,
    uint32_t total_points)
{
    int i, j;
    
    /* calculate new normal of faces */
    for(i = 0; i < max_faces; i++) {
        update_normal(&faces[i]);
    }

    /* calculate forces */
    for(i = 0; i < max_faces; i++) {
        for(j = 0; j < POINTS_PER_FACE; j++) {
            /* TODO: change to faces+i? */
            calculate_forces(&faces[i],j);
        }
    }

    /* calculate new position of points */
    for(i = 0; i < total_points; i++) {
        update_pos(&points[i]);
    }
}

void 
print_all_faces(Face * faces, uint32_t max_faces)
{
    int i;

    for(i = 0; i < max_faces; i++) {
        printf("FACE: %d\n", i);
        print_face(faces+i);
        printf("\n\n");
    }
}

void 
print_all_points( Point * points, uint32_t total_points)
{
    int i;

    for(i = 0; i < total_points; i++) {
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
_get_sign_for_norm(double * new_normal, float * normal)
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

int
point_in_bounds(Point * point)
{
    int i, in_bounds = 1;
    
    for(i = 0; i < 3; i++) {
            in_bounds = (in_bounds && (abs(point->pos[i]) < BOUND_LIMIT));
    }
    return in_bounds;
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
    
    /* undo position update if point out of bounds */
    if(!point_in_bounds(point)) {
        for(i = 0; i < 3; i++) {
            delta_pos[i] = point->net_force[i]*FORCE_FACTOR;
            point->pos[i] -= delta_pos[i];
            point->delta_pos[i] -= delta_pos[i];
        }
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

void 
free_faces(Face * faces)
{
    /* TODO: check if malloc is successful */
    free(faces);
}

void 
free_points(Point * points)
{
    /* TODO: check if malloc is successful */
    free(points);
}

void 
show_usage(void)
{
    printf("Usage: ./simulation <input-stl> [output-stl]\n"
	       "  input-stl                Filename of an binary formatted stl file to be simulated\n"
	       "  output-stl     OPTIONAL: Filename of the output stl that the simulator will write the\n" 
		   "                           final of the simulated object \n"
		   );
}

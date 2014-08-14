/* my imports */
#include "parse_stl.h"

stl_error_t
parse_stl(const char * stl_name, Face ** faces, Point ** points,
    uint32_t * max_faces, uint32_t * max_points, uint32_t * total_points)
{
    FILE * stl_file = NULL;
    char buffer[HEADER_LENGTH];
    int i;
    stl_error_t e_flag;

    stl_file = fopen(stl_name, "rb");
    if (stl_file == NULL) {
        printf("Error Opening %s for reading\n", stl_name);
        return STL_E_FILE;
    }
    /* Read out header */
    if (fread(buffer, sizeof(char), HEADER_LENGTH, stl_file)
        < HEADER_LENGTH) {
        printf("ERROR READING STL HEADER\n");
        return STL_E_HEADER;
    }
    /* Read out number of facets */
    if (fread(max_faces, sizeof(uint32_t), 1,
         stl_file) < 1) {
        printf("ERROR READING NUMBER OF FACETS\n");
        return STL_E_NUM_FACES;
    }
    *max_points = 3*(*max_faces);
    *faces = calloc(*max_faces, sizeof(Face));
    *points = calloc(*max_points, sizeof(Point));

    /* Parse all the facets */
    for(i = 0; i < *max_faces; i++) {
        if ((e_flag = parse_facet(i, stl_file, *faces, *points, total_points)) < 0) {
            return e_flag;
        }
    }
    
    fclose(stl_file);
    
    return STL_E_NONE;
}

stl_error_t
parse_facet(int face_index, FILE * stl_file, Face * faces, Point * points,
    uint32_t * total_points)
{
    uint16_t attribute_data;
    float vertex_pos[3];
    int i, j;
    size_t buff;

    /*normal vector */
    for(i = 0; i < 3; i++) {
        if ((buff = fread(&(faces[face_index].normal[i]), sizeof(char), 4,
            stl_file)) < 4) {
            printf("%lu\n", buff);
            printf("ERROR READING NORMAL OF FACET %d COMPONENT %d\n",
                face_index, i);
            return STL_E_NORMAL;
        }
    }
    #if 0
    printf("FACET %d\n", face_index);
    printf("%f %f %f\n", faces[face_index].normal[0],
        faces[face_index].normal[1],
        faces[face_index].normal[2]);
    #endif
    
    /* read each componet of each point in the face*/
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            if (fread(vertex_pos+j, sizeof(uint32_t), 1,
                        stl_file) < 1) {
                printf("ERROR READING VERTEX %d COMPONET %d OF FACET %d\n",
                    i, j, face_index);
                return STL_E_VERTEX;
            }
        }
        /* place reference to vertex in face */
        faces[face_index].points[i] =
            find_vertex(vertex_pos, faces, points, total_points);
        #if 0
        printf("FACET %d\n", face_index);
        printf("VERTEX %d\n", i);
        printf("%x %x %x\n", vertex_pos[0],
            vertex_pos[1], vertex_pos[2]);
        #endif
    }

    /* attribute data (uint16) */
    if (fread(&attribute_data, sizeof(attribute_data), 1,
                stl_file) < 1) {
        printf("ERROR READING ATTRIBUTE DATA of OF FACET: %d\n",
            face_index);
        return STL_E_ATTRIBUTE;
    }
    
    return STL_E_NONE;
}


Point *
find_vertex(float * vert_pos, Face * faces, Point * points, uint32_t * total_points)
{
    int i;
    /* search for the vertex in the points array */
    for(i = 0; i < *total_points; i++) {
        if (memcmp(vert_pos, points+i, 3*sizeof(float)) == 0) {
            return points+i;
        }
    }
    /* if not found, place at end of array */
    memcpy(points[*total_points].pos, vert_pos, 3*sizeof(float));
    /* return a refernce to the point, not the actual point */
    return &points[(*total_points)++]; 
}

stl_error_t
write_stl(const char * stl_name, Face * faces, uint32_t max_faces)
{
    FILE * stl_file = NULL;
    char buffer[HEADER_LENGTH];
    int i;
    stl_error_t e_flag;
    
    for(i = 0; i < HEADER_LENGTH; i++) {
        buffer[i] = 0;
    }
    
    stl_file = fopen(stl_name, "wb");
    if (stl_file == NULL) {
        printf("Error Opening %s for writing\n", stl_name);
        return STL_E_FILE;
    }
    /* Write header */
    if (fwrite(buffer, sizeof(char), HEADER_LENGTH, stl_file)
        < HEADER_LENGTH) {
        printf("ERROR WRITING STL HEADER\n");
        return STL_E_HEADER;
    }
    /* Write out number of facets */
    if (fwrite(&max_faces, sizeof(uint32_t), 1, stl_file) < 1) {
        printf("ERROR WRITING NUMBER OF FACETS\n");
        return STL_E_NUM_FACES;
    }

    /* Write all the facets */
    for(i = 0; i < max_faces; i++) {
        if ((e_flag = write_facet(i, stl_file, faces)) < 0) {
            return e_flag;
        }
    }
    
    fclose(stl_file);

    return STL_E_NONE;
}

stl_error_t 
write_facet(int face_index, FILE * stl_file, Face * faces)
{
    uint16_t attribute_data = 0;
    int i, j;
    size_t buff;

    /*normal vector */
    for(i = 0; i < 3; i++) {
        if ((buff = fwrite(&(faces[face_index].normal[i]), sizeof(char), 4,
            stl_file)) < 4) {
            printf("%lu\n", buff);
            printf("ERROR READING NORMAL OF FACET %d COMPONET %d\n",
                face_index, i);
            return STL_E_NORMAL;
        }
    }
    
    /* write each componet of each point in the face*/
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            if (fwrite(&(faces[face_index].points[i]->pos[j]), sizeof(uint32_t), 1,
                        stl_file) < 1) {
                printf("ERROR READING VERTEX %d COMPONET %d OF FACET %d\n",
                    i, j, face_index);
                return STL_E_VERTEX;
            }
        }

    }

    /* attribute data (uint16) */
    if (fwrite(&attribute_data, sizeof(attribute_data), 1,
                stl_file) < 1) {
        printf("ERROR READING ATTRIBUTE DATA of OF FACET: %d\n",
            face_index);
        return STL_E_ATTRIBUTE;
    }
    

    return STL_E_NONE;
}

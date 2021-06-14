#include "kmean.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

#define max(a, b)    (((a) > (b)) ? (a) : (b))

static int initialized = 0;

// int rand_int(int n) {
//   if ((n - 1) == RAND_MAX) {
//     return rand();
//   } else {
//     int end = RAND_MAX / n; // truncate skew
//     end *= n;
//     int r;
//     while ((r = rand()) >= end);
//     return r % n;
//   }
// }

int rand_int(int n)
{
    struct timeval current_time;
    if (!initialized) {
        gettimeofday(&current_time, NULL);
        srand(current_time.tv_usec);
        initialized = 1;
    }
    int random_value = rand() % (n+1);
    return random_value;
}


int nearest_color(COLOR c, vector *color_vector)
{
    float dist_min = FLT_MAX;
    float dist = 0;
    int idx;
    COLOR current_color;
    unsigned char c1[3], c2[3];
    for (int i = 0; i < vector_size(*color_vector); i++) {
        vector_get_at(&current_color, *color_vector, i);
        c1[0] = c.r;
        c1[1] = c.g;
        c1[2] = c.b;
        c2[0] = current_color.r;
        c2[1] = current_color.g;
        c2[2] = current_color.b;
        dist = color_delta_f(c1, c2);
        if (dist < dist_min) {
            dist_min = dist;
            idx = i;
        }
    }
    return idx;
}


void colors_mean(vector *color_vector, COLOR *mean)
{
   int sz = vector_size(*color_vector);
   COLOR current_color;
   unsigned int r = 0, g = 0, b = 0;

   for (int i = 0; i < sz; i++) {
        vector_get_at(&current_color, *color_vector, i);
        r += current_color.r;
        g += current_color.g;
        b += current_color.b;
    }

    mean->r = r / sz;
    mean->g = g / sz;
    mean->b = b / sz;
}


float color_variance(vector *color_vector)
{
    int sz = vector_size(*color_vector);

    COLOR mean;
    COLOR current_color;
    unsigned char c1[3], c2[3];
    colors_mean(color_vector, &mean);
    float dist = 0, dist_sum = 0;
    for (int i = 0; i < sz; i++) {
        vector_get_at(&current_color, *color_vector, i);
        c1[0] = mean.r;
        c1[1] = mean.g;
        c1[2] = mean.b;
        c2[0] = current_color.r;
        c2[1] = current_color.g;
        c2[2] = current_color.b;
        dist = color_delta_f(c1, c2);
        dist_sum += (dist*dist);
    }
    return dist_sum / (float) sz;
}


void new_cluster_list(int k, vector *cluster_vector)
{
    for (int i = 0; i < k; i++) {
        CLUSTER_LIST cl;
        cl.cluster_vector = vector_init(sizeof(COLOR));
        vector_add_last(*cluster_vector, &cl);
    }
}

void free_cluster_list(vector *cluster_vector)
{
    CLUSTER_LIST current_cluster_list;
    for (int i = 0; i < vector_size(*cluster_vector); i++) {
        vector_get_at(&current_cluster_list, *cluster_vector, i);
        vector_destroy(current_cluster_list.cluster_vector);
    }
}



void get_cluster_centroid(IMAGE *image, vector *cluster_vector, vector *centroid_vector)
{
    CLUSTER_LIST current_cluster;

    int cl_size = vector_size(*cluster_vector);

    

    vector_clear(*centroid_vector);
    for (int i = 0; i < cl_size; i++) {
        COLOR no_color = {-1, -1, -1};
        vector_add_last(*centroid_vector, &no_color);
    }
    

    for (int i = 0; i < cl_size; i++) {        
        vector_get_at(&current_cluster, *cluster_vector, i);
        if (vector_size(current_cluster.cluster_vector) > 0) {
            COLOR c_mean;
            colors_mean(&current_cluster.cluster_vector, &c_mean);
            vector_set_at(*centroid_vector, i, &c_mean);
        } else {
            int r = rand_int(image->height * image->width);
            PIXEL p = image->pixels[r];
            COLOR c;
            c.r = p.r;
            c.g = p.g;
            c.b = p.b;
            vector_set_at(*centroid_vector, i, &c);
        }
    }
}



void guess_palette_kmean(IMAGE *image, PALETTE *palette, int reduc_size)
{
    vector clusters_vector = vector_init(sizeof(CLUSTER_LIST));
    vector centroid_vector = vector_init(sizeof(COLOR));
    int image_size =  image->height * image->width;

    new_cluster_list(reduc_size, &clusters_vector);

    int iter = 0, max_iter = 100;
    float *previous_variance = (float *) malloc(sizeof(float) * reduc_size);
    for (int i = 0; i < reduc_size; i++) previous_variance[i] = 1.0;
    float variance = 0.0, delta = 0.0, delta_max = 0.0, threshold = 0.00005;
    CLUSTER_LIST current_cluster;

    while(1) {
        // Génération des clusters
        get_cluster_centroid(image, &clusters_vector, &centroid_vector);
        free_cluster_list(&clusters_vector);
        vector_clear(clusters_vector);
        new_cluster_list(reduc_size, &clusters_vector);

        

        for (int i = 0; i < image->height * image->width; i++) {
            PIXEL p = image->pixels[i];
            COLOR c;
            c.r = p.r;
            c.g = p.g;
            c.b = p.b;
            int nc_idx = nearest_color(c, &centroid_vector);
            vector_get_at(&current_cluster, clusters_vector, nc_idx);
            vector_add_last(current_cluster.cluster_vector, &c);
        }

        // Test de la convergence
        delta_max = 0;
        for (int i = 0; i < vector_size(clusters_vector); i++) {
            vector_get_at(&current_cluster, clusters_vector, i);
            variance = color_variance(&(current_cluster.cluster_vector));
            delta = fabs(previous_variance[i] - variance);
            delta_max = max(delta, delta_max);
            previous_variance[i] = variance;
        }

        printf("Kmean iteration %d - variance %f\n", iter, delta_max);
        if (delta_max < threshold || iter++ > max_iter) {
            break;
        }
    }

    COLOR current_color;
    palette->size = vector_size(centroid_vector);
    for (int i = 0; i < vector_size(centroid_vector); i++) {
        vector_get_at(&current_color, centroid_vector, i);
        // printf("Color %d %d %d %d\n", i, current_color.r, current_color.g, current_color.b);
        palette->colors[i][0] = current_color.r;
        palette->colors[i][1] = current_color.g;
        palette->colors[i][2] = current_color.b;
    }
    
    free(previous_variance);

    free_cluster_list(&clusters_vector);

    vector_destroy(clusters_vector);
    vector_destroy(centroid_vector);
}

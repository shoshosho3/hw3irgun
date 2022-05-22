/********************************************************
 * Kernels to be optimized
 ********************************************************/

#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
        "בן הלמן ורננה שח""ק--------",              /* Team name */

        "Ben Hellmann",     /* First member full name */
        "ben.hellmann@campus.technion.ac.il",  /* First member email address */

        "Renana Shachak",                   /* Second member full name (leave blank if none) */
        "renanas@campus.technion.ac.il"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

char rotate3_descr[] = "rotate3";

void rotate3(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim - dim % 3; j += 3) {
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
            dst[RIDX(dim - 2 - j, i, dim)] = src[RIDX(i, j + 1, dim)];
            dst[RIDX(dim - 3 - j, i, dim)] = src[RIDX(i, j + 2, dim)];
        }
        for (j = dim - dim % 3; j < dim; j++) {
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
        }
    }
}

char rotate7_descr[] = "rotate7";

void rotate7(int dim, pixel *src, pixel *dst) {
    int i, j;

    register int dim_reg = dim;

    for (i = 0; i < dim_reg; i++) {
        for (j = 0; j < dim_reg - dim_reg % 7; j += 7) {
            dst[RIDX(dim_reg - 1 - j, i, dim_reg)] = src[RIDX(i, j, dim_reg)];
            dst[RIDX(dim_reg - 2 - j, i, dim_reg)] = src[RIDX(i, j + 1, dim_reg)];
            dst[RIDX(dim_reg - 3 - j, i, dim_reg)] = src[RIDX(i, j + 2, dim_reg)];
            dst[RIDX(dim_reg - 4 - j, i, dim_reg)] = src[RIDX(i, j + 3, dim_reg)];
            dst[RIDX(dim_reg - 5 - j, i, dim_reg)] = src[RIDX(i, j + 4, dim_reg)];
            dst[RIDX(dim_reg - 6 - j, i, dim_reg)] = src[RIDX(i, j + 5, dim_reg)];
            dst[RIDX(dim_reg - 7 - j, i, dim_reg)] = src[RIDX(i, j + 6, dim_reg)];
        }
        for (j = dim_reg - dim_reg % 7; j < dim_reg; j++) {
            dst[RIDX(dim_reg - 1 - j, i, dim_reg)] = src[RIDX(i, j, dim_reg)];
        }
    }
}

char rotate_7_no_RIDX_descr[] = "rotate_7_no_RIDX";
void rotate_7_no_RIDX(int dim, pixel *src, pixel *dst) {
    int i, j;

    register int dim_reg = dim;

    for (i = 0; i < dim_reg; i++) {
        for (j = 0; j < dim_reg - dim_reg % 7; j += 7) {
            dst[(dim_reg - 1 - j) * dim_reg + i] = src[i * dim_reg + j];
            dst[(dim_reg - 2 - j) * dim_reg + i] = src[i * dim_reg + j + 1];
            dst[(dim_reg - 3 - j) * dim_reg + i] = src[i * dim_reg + j + 2];
            dst[(dim_reg - 4 - j) * dim_reg + i] = src[i * dim_reg + j + 3];
            dst[(dim_reg - 5 - j) * dim_reg + i] = src[i * dim_reg + j + 4];
            dst[(dim_reg - 6 - j) * dim_reg + i] = src[i * dim_reg + j + 5];
            dst[(dim_reg - 7 - j) * dim_reg + i] = src[i * dim_reg + j + 6];
        }
        for (j = dim_reg - dim_reg % 7; j < dim_reg; j++) {
            dst[(dim_reg - 1 - j) * dim_reg + i] = src[i * dim_reg + j];
        }
    }
}

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";

void naive_rotate(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";

void rotate(int dim, pixel *src, pixel *dst) {
    rotate3(dim, src, dst);
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() {
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate3, rotate3_descr);
    add_rotate_function(&rotate7, rotate7_descr);
    add_rotate_function(&rotate_7_no_RIDX, rotate_7_no_RIDX_descr);
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }

static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) {
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) {
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) {
    current_pixel->red = (unsigned short) (sum.red / sum.num);
    current_pixel->green = (unsigned short) (sum.green / sum.num);
    current_pixel->blue = (unsigned short) (sum.blue / sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) {
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";

void naive_smooth(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}



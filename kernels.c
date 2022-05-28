/********************************************************
 * Kernels to be optimized
 ********************************************************/

#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
        "Ben Hellmann and Renana Shachak",              /* Team name */

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
char rotate_descr[] = "16 moves in each iteration, arrays in pointer registers, iteration indexes in registers"
                      ", powers of 2 done with shifts, matrix sizes in registers";

void rotate(int dim, pixel *src, pixel *dst) {
    register int i, j;

    register int dim_reg = dim;
    register int size = dim_reg * dim_reg;
    register pixel *dst_place;
    register pixel *src_place;

    for (i = 0; i < dim_reg; i++) {
        pixel *j_dst_place = dst + i + size; //instead of writing this each time
        pixel *j_src_place = src + i * dim_reg; //instead of writing this each time, fewer multiplications
        for (j = 0; j < dim_reg; j += 16) {
            dst_place = j_dst_place - j * dim_reg;
            src_place = j_src_place + j;
            *(dst_place - dim_reg) = *(src_place);
            *(dst_place - (dim_reg << 1)) = *(src_place + 1);
            *(dst_place - 3 * dim_reg) = *(src_place + 2);
            *(dst_place - (dim_reg << 2)) = *(src_place + 3);
            *(dst_place - 5 * dim_reg) = *(src_place + 4);
            *(dst_place - 6 * dim_reg) = *(src_place + 5);
            *(dst_place - 7 * dim_reg) = *(src_place + 6);
            *(dst_place - (dim_reg << 3)) = *(src_place + 7);
            *(dst_place - 9 * dim_reg) = *(src_place + 8);
            *(dst_place - 10 * dim_reg) = *(src_place + 9);
            *(dst_place - 11 * dim_reg) = *(src_place + 10);
            *(dst_place - 12 * dim_reg) = *(src_place + 11);
            *(dst_place - 13 * dim_reg) = *(src_place + 12);
            *(dst_place - 14 * dim_reg) = *(src_place + 13);
            *(dst_place - 15 * dim_reg) = *(src_place + 14);
            *(dst_place - (dim_reg << 4)) = *(src_place + 15);
        }
    }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() {
    add_rotate_function(rotate,rotate_descr);

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



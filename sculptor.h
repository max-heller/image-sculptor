#include <stdint.h>

typedef struct rgb_pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_pixel_t;

typedef struct row {
    rgb_pixel_t *pixels;
} row_t;

typedef struct im {
    unsigned int width;
    unsigned int height;
    row_t *rows;
} im_t;

typedef struct energy_row {
    double *energies;
} energy_row_t;

typedef struct seam {
    double energy;
    unsigned int *positions;
} seam_t;

typedef struct seams {
    seam_t *prev_row;
    seam_t *current_row;
} seams_t;

im_t *image_load(char *filename);
void image_save(im_t *image, char *filename);
void image_destroy(im_t *image, unsigned int original_height);

energy_row_t *energies_init(unsigned int width, unsigned int height);
void calculate_energies(energy_row_t *energies, im_t *image);
void energies_destroy(energy_row_t *energies, unsigned int original_height);

seams_t *seams_init(unsigned int width, unsigned int height);
void fill_seams(seams_t *seams, energy_row_t *energies, im_t *image);
void remove_seam(im_t *image, seam_t *seam);
void seams_destroy(seams_t *seams, unsigned int original_width);

static inline int brightness(rgb_pixel_t pix);
static inline int weighted_sum(int *l1, int *l2, int len);
double energy(im_t *im, int x, int y);
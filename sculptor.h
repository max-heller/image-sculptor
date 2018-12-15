#include <stdint.h>

typedef struct rgb_pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_pixel_t;

typedef struct im {
    uint16_t width;
    uint16_t height;
    rgb_pixel_t **pixels;
} im_t;

typedef struct seam {
    double energy;
    uint16_t *positions;
} seam_t;

typedef struct seams {
    seam_t *prev_row;
    seam_t *current_row;
} seams_t;

im_t *image_load(char *filename);
void image_save(im_t *image, char *filename);
void image_destroy(im_t *image, uint16_t original_height);

double **energies_init(uint16_t width, uint16_t height);
void calculate_energies(double **energies, im_t *image);
void energies_destroy(double **energies, uint16_t original_height);

seams_t *seams_init(uint16_t width, uint16_t height);
void fill_seams(seams_t *seams, double **energies, im_t *image);
void remove_seam(im_t *image, seam_t *seam);
void seams_destroy(seams_t *seams, uint16_t original_width);

static inline uint16_t brightness(rgb_pixel_t pix);
static inline int weighted_sum(uint16_t *l, const int8_t *weights, uint16_t len);
double energy(im_t *im, int x, int y);
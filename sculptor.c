#include "sculptor.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lodepng.h"

int main(int argc, char **argv) {
    // Verify and extract arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: ./sculptor input output seams\n");
        exit(1);
    }
    char *input_name = argv[1];
    char *output_name = argv[2];
    int to_carve = atoi(argv[3]);
    assert(to_carve >= 0);

    int start = clock();

    // Load input image
    im_t *image = image_load(input_name);

    int original_width = image->width;
    int original_height = image->height;
    assert((original_width > 1) && (original_height > 1));

    if (to_carve >= original_width) {
        fprintf(stderr, "Number of seams to carve too large\n");
        exit(1);
    }

    energy_row_t *energies = energies_init(original_width, original_height);
    seams_t *seams = seams_init(original_width, original_height);

    for (int iteration = 0; iteration < to_carve; iteration++) {
        int energy_start = clock();
        calculate_energies(energies, image);
        int energy_end = clock();
        printf("Energy time consumed: %ld ms\n", (energy_end - energy_start) * 1000 / CLOCKS_PER_SEC);

        int seams_start = clock();
        fill_seams(seams, energies, image);
        int seams_end = clock();
        printf("Seams time consumed: %ld ms\n", (seams_end - seams_start) * 1000 / CLOCKS_PER_SEC);

        seam_t *lowest_energy_seam = &seams->current_row[0];
        for (unsigned int col = 1; col < image->width; col++) {
            seam_t *seam = &seams->current_row[col];
            if (seam->energy < lowest_energy_seam->energy) lowest_energy_seam = seam;
        }

        remove_seam(image, lowest_energy_seam);
    }

    // Save output image
    image_save(image, output_name);

    // Cleanup
    energies_destroy(energies, original_height);
    seams_destroy(seams, original_width);
    image_destroy(image, original_height);

    int end = clock();
    printf("Time consumed: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    return 0;
}

im_t *image_load(char *filename) {
    unsigned char *input;
    unsigned int width, height;

    if (lodepng_decode24_file(&input, &width, &height, filename) != 0) {
        fprintf(stderr, "Error loading input image\n");
        free(input);
        exit(1);
    }

    // Initialize custom image struct
    row_t *rows = malloc(height * sizeof(row_t));
    for (unsigned int y = 0; y < height; y++) {
        rows[y].pixels = malloc(width * sizeof(rgb_pixel_t));
        memcpy(rows[y].pixels, &input[y * width * sizeof(rgb_pixel_t)],
               width * sizeof(rgb_pixel_t));
    }

    free(input);

    im_t *image = malloc(sizeof(im_t));
    image->width = width;
    image->height = height;
    image->rows = rows;

    return image;
}

void image_save(im_t *image, char *filename) {
    unsigned int width = image->width, height = image->height;

    unsigned char *raw = malloc(width * height * sizeof(rgb_pixel_t));
    for (unsigned int row = 0; row < height; row++) {
        memcpy(&raw[row * width * sizeof(rgb_pixel_t)], image->rows[row].pixels,
               width * sizeof(rgb_pixel_t));
    }

    lodepng_encode24_file(filename, raw, width, height);
    free(raw);
}

static inline int brightness(rgb_pixel_t pix) { return pix.red + pix.green + pix.blue; }

static inline int weighted_sum(int *l1, int *l2, int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += l1[i] * l2[i];
    }
    return sum;
}

double energy(im_t *im, int x, int y) {
    const int x_weights[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
    const int y_weights[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    int width = im->width, height = im->height;
    int brightnesses[9];
    memset(&brightnesses, 0, 9 * sizeof(int));

    for (int row_offset = -1; row_offset <= 1; row_offset++) {
        int row = y + row_offset;
        if ((row < 0) || (row >= height))
            continue;
        rgb_pixel_t *row_pixels = im->rows[row].pixels;
        for (int col_offset = -1; col_offset <= 1; col_offset++) {
            int col = x + col_offset;
            if ((col < 0) || (col >= width))
                continue;
            int idx = (row_offset + 1) * 3 + (col_offset + 1);
            brightnesses[idx] = brightness(row_pixels[col]);
        }
    }

    int x_energy = weighted_sum(brightnesses, (int *)x_weights, 9);
    int y_energy = weighted_sum(brightnesses, (int *)y_weights, 9);

    return sqrt(x_energy * x_energy + y_energy * y_energy);
}

energy_row_t *energies_init(unsigned int width, unsigned int height) {
    energy_row_t *energies = malloc(height * sizeof(energy_row_t));
    for (unsigned int row = 0; row < height; row++) {
        energies[row].energies = malloc(width * sizeof(double));
    }
    return energies;
}

void calculate_energies(energy_row_t *energies, im_t *image) {
    for (unsigned int row = 0; row < image->height; row++) {
        for (unsigned int col = 0; col < image->width; col++) {
            energies[row].energies[col] = energy(image, col, row);
        }
    }
}

void energies_destroy(energy_row_t *energies, unsigned int original_height) {
    for (unsigned int row = 0; row < original_height; row++) {
        free(energies[row].energies);
    }
    free(energies);
}

seams_t *seams_init(unsigned int width, unsigned int height) {
    seams_t *seams = malloc(sizeof(seams_t));
    seams->current_row = malloc(width * sizeof(seam_t));
    seams->prev_row = malloc(width * sizeof(seam_t));

    for (unsigned int col = 0; col < width; col++) {
        seams->prev_row[col].positions = malloc(height * sizeof(int));
        seams->current_row[col].positions = malloc(height * sizeof(int));
    }
    return seams;
}

void fill_seams(seams_t *seams, energy_row_t *energies, im_t *image) {
    unsigned int width = image->width, height = image->height;

    for (unsigned int col = 0; col < width; col++) {
        seams->prev_row[col].energy = energies[0].energies[col];
        seams->prev_row[col].positions[0] = col;
    }

    for (unsigned int row = 1; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            seam_t *min_seam = &seams->prev_row[col];

            seam_t *right = (col + 1 < width) ? &seams->prev_row[col + 1] : NULL;
            seam_t *left = (col >= 1) ? &seams->prev_row[col - 1] : NULL;

            if ((right) && right->energy < min_seam->energy) min_seam = right;
            if ((left) && left->energy < min_seam->energy) min_seam = left;

            seams->current_row[col].energy = min_seam->energy + energies[row].energies[col];
            memcpy(seams->current_row[col].positions, min_seam->positions,
                   row * sizeof(unsigned int));
            seams->current_row[col].positions[row] = col;
        }
        for (unsigned int i = 0; i < width; i++) {
            seams->prev_row[i].energy = seams->current_row[i].energy;
            memcpy(seams->prev_row[i].positions, seams->current_row[i].positions,
                   (row + 1) * sizeof(unsigned int));
        }
    }
}

void remove_seam(im_t *image, seam_t *seam) {
    for (unsigned int row = 0; row < image->height; row++) {
        unsigned int to_remove = seam->positions[row];
        memmove(&image->rows[row].pixels[to_remove],
                &image->rows[row].pixels[to_remove + 1],
                (image->width - to_remove - 1) * sizeof(rgb_pixel_t));
    }
    image->width--;
}

void seams_destroy(seams_t *seams, unsigned int original_width) {
    for (unsigned int col = 0; col < original_width; col++) {
        free(seams->prev_row[col].positions);
        free(seams->current_row[col].positions);
    }
    free(seams->prev_row);
    free(seams->current_row);
    free(seams);
}

void image_destroy(im_t *image, unsigned int original_height) {
    for (unsigned int row = 0; row < original_height; row++) {
        free(image->rows[row].pixels);
    }
    free(image->rows);
    free(image);
}
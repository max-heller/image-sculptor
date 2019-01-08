#include "computations.hpp"
#include <iostream>
using namespace std;

CImg<unsigned char> get_brightness(CImg<unsigned char> &img) {
    CImg<unsigned char> brightness(img.width(), img.height(), 1, 1);
    cimg_forXY(img, x, y) {
        brightness(x, y) =
            (img(x, y, 0, 0) + img(x, y, 0, 1) + img(x, y, 0, 2)) / 3;
    }
    return brightness;
}

CImg<float> calculate_energy(CImg<unsigned char> &brightness) {
    CImg<float> energy(brightness.width(), brightness.height(), 1, 1);
    CImg_3x3(I, unsigned char);
    cimg_for3x3(brightness, x, y, 0, 0, I, unsigned char) {
        float x_energy = Ipp - Inp + 2 * (Ipc - Inc) + Ipn - Inn;
        float y_energy = Ipp - Ipn + 2 * (Icp - Icn) + Inp - Inn;
        energy(x, y) = sqrt(x_energy * x_energy + y_energy * y_energy);
    }
    return energy;
}

Seam find_lowest_energy_seam(CImg<float> &energy) {
    const int width = energy.width(), height = energy.height();
    vector<Seam> prev_row(width, Seam(height)), current_row(width, Seam(height));

    for (int col = 0; col < width; col++) {
        prev_row[col].update(col, energy(col, 0));
    }

    for (int row = 1; row < height; row++) {
        for (int col = 0; col < width; col++) {
            auto start = max(col - 1, 0);
            auto end = min(col + 2, width);
            auto min_prev =
                min_element(begin(prev_row) + start, begin(prev_row) + end);
            current_row[col] = *min_prev;
            current_row[col].update(col, energy(col, row));
        }
        if (row != height - 1)
            swap(current_row, prev_row);
    }
    return *min_element(begin(current_row), end(current_row));
}
#include "computations.hpp"
using namespace std;

Seam find_lowest_energy_seam(CImg<float> &energies) {
    const int width = energies.width(), height = energies.height();
    vector<Seam> prev_row(width, Seam(height)), current_row(width, Seam(height));

    for (int col = 0; col < width; col++) {
        prev_row[col].update(0, col, energies(col, 0));
    }

    for (int row = 1; row < height; row++) {
        for (int col = 0; col < width; col++) {
            auto start = max(col - 1, 0);
            auto end = min(col + 2, width);
            auto min_prev =
                min_element(begin(prev_row) + start, begin(prev_row) + end);
            current_row[col] = *min_prev;
            current_row[col].update(row, col, energies(col, row));
        }
        if (row != height - 1)
            swap(current_row, prev_row);
    }
    return *min_element(begin(current_row), end(current_row));
}

void calculate_energies(CImg<unsigned char> &img, CImg<float> &energies) {
    CImg<unsigned char> brightness(img.width(), img.height());
    cimg_forXY(img, x, y) {
        brightness(x, y) =
            (img(x, y, 0, 0) + img(x, y, 0, 1) + img(x, y, 0, 2)) / 3;
    }
    CImg_3x3(I, unsigned char);
    cimg_for3x3(img, x, y, 0, 0, I, unsigned char) {
        float x_energy = Ipp - Inp + 2 * (Ipc - Inc) + Ipn - Inn;
        float y_energy = Ipp - Ipn + 2 * (Icp - Icn) + Inp - Inn;
        energies(x, y) = sqrt(x_energy * x_energy + y_energy * y_energy);
    }
}
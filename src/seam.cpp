#include "seam.hpp"

Seam::Seam(int height) {
    total_energy = 0;
    positions.reserve(height);
}

bool operator<(const Seam &s1, const Seam &s2) {
    return s1.total_energy < s2.total_energy;
}

void Seam::update(unsigned short int col, float energy) {
    total_energy += energy;
    positions.push_back(col);
}

CImg<unsigned char> Seam::removeFrom(CImg<unsigned char> &img) {
    int width = img.width(), height = img.height();
    int new_width = width - 1;
    int channels = img.spectrum();
    CImg<unsigned char> new_img(new_width, height, 1, channels);
    auto old_data = img.data();
    auto new_data = new_img.data();
    for (int channel = 0; channel < channels; channel++) {
        for (int row = 0; row < height; row++) {
            int skip_offset = positions.at(row);
            copy(old_data, old_data + skip_offset + 1, new_data);
            copy(old_data + skip_offset + 1, old_data + width, new_data + skip_offset);
            old_data += width;
            new_data += new_width;
        }
    }
    return new_img;
}
#include "seam.hpp"

Seam::Seam(int height) {
    total_energy = 0;
    positions = vector<int>(height);
}

bool operator<(const Seam &s1, const Seam &s2) {
    return s1.total_energy < s2.total_energy;
}

void Seam::update(int row, int col, float energy) {
    total_energy += energy;
    positions[row] = col;
}
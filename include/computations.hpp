#include "CImg.hpp"
#include "seam.hpp"
using namespace cimg_library;

Seam find_lowest_energy_seam(CImg<float> &energies);
void calculate_energies(CImg<unsigned char> &img, CImg<float> &energies);
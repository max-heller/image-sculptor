#include "CImg.hpp"
#include "seam.hpp"
using namespace cimg_library;

CImg<unsigned char> get_brightness(CImg<unsigned char> &img);
CImg<float> calculate_energy(CImg<unsigned char> &brightness);
Seam find_lowest_energy_seam(CImg<float> &energy);
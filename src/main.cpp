#include "CImg.hpp"
#include "computations.hpp"
#include <cassert>
#include <iostream>
using namespace cimg_library;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: ./sculptor in_file out_file num_seams" << endl;
        return -1;
    }

    char *in_file = argv[1], *out_file = argv[2];
    int num_seams = atoi(argv[3]);
    assert(num_seams >= 0);

    CImg<unsigned char> img(in_file);
    for (int i = 0; i < num_seams; i++) {
        CImg<unsigned char> brightness = get_brightness(img);
        CImg<float> energy = calculate_energy(brightness);
        Seam min_seam = find_lowest_energy_seam(energy);
        min_seam.removeFrom(img).move_to(img);
    }

    img.save(out_file);
    return 0;
}
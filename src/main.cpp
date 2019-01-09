#include "CImg.hpp"
#include "computations.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
using namespace cimg_library;
using namespace std;
using namespace std::chrono;

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
        auto start = high_resolution_clock::now();
        CImg<unsigned char> brightness = get_brightness(img);
        CImg<float> energy = calculate_energy(brightness);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by energy: "
             << duration.count() << " microseconds" << endl;

        start = high_resolution_clock::now();
        Seam min_seam = find_lowest_energy_seam(energy);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by seam finding: "
             << duration.count() << " microseconds" << endl;

        start = high_resolution_clock::now();
        min_seam.removeFrom(img).move_to(img);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by removal: "
             << duration.count() << " microseconds" << endl;
    }

    img.save(out_file);
    return 0;
}
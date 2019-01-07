#include "CImg.hpp"
#include "computations.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
using namespace cimg_library;
using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: ./sculptor in_file num_seams" << endl;
        return -1;
    }

    char *in_file = argv[1];
    int num_seams = atoi(argv[2]);
    assert(num_seams >= 0);

    CImg<unsigned char> img(in_file);
    for (int i = 0; i < num_seams; i++) {
        auto start = high_resolution_clock::now();
        CImg<float> energies(img.width(), img.height());
        calculate_energies(img, energies);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by energy: "
             << duration.count() << " microseconds" << endl;

        auto min_seam = find_lowest_energy_seam(energies);

        for (auto i : min_seam.getPositions()) {
            cout << i << endl;
        }
        cout << "Energy: " << min_seam.getEnergy() << endl;
    }
    return 0;
}
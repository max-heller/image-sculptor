#include "computations.hpp"
#include <chrono>
#include <iostream>
using namespace std;
using namespace std::chrono;

#define REPS 5

int main(int argc, char *argv[]) {
    CImg<unsigned char> img("broadway.jpg"), brightness(1920, 1080, 1, 1);
    CImg<float> energy(1920, 1080, 1, 1);
    Seam s(1080);
    for (int i = 0; i < 1080; i++)
        s.update(i, 0);

    microseconds duration(0);
    auto start = high_resolution_clock::now(), stop = start;

    duration -= duration;
    for (int i = 0; i < REPS; i++) {
        auto start = high_resolution_clock::now();
        brightness = get_brightness(img);
        auto stop = high_resolution_clock::now();
        duration += duration_cast<microseconds>(stop - start);
    }
    cout << "Time taken by brightness: "
        << duration.count() / REPS << " microseconds" << endl;

    duration -= duration;
    for (int i = 0; i < REPS; i++) {
        auto start = high_resolution_clock::now();
        energy = calculate_energy(brightness);
        auto stop = high_resolution_clock::now();
        duration += duration_cast<microseconds>(stop - start);
    }
    cout << "Time taken by energy: "
        << duration.count() / REPS << " microseconds" << endl;

    duration -= duration;
    for (int i = 0; i < REPS; i++) {
        start = high_resolution_clock::now();
        Seam min_seam = find_lowest_energy_seam(energy);
        stop = high_resolution_clock::now();
        duration += duration_cast<microseconds>(stop - start);
    }
    cout << "Time taken by seam finding: "
        << duration.count() / REPS << " microseconds" << endl;

    duration -= duration;
    for (int i = 0; i < REPS; i++) {
        start = high_resolution_clock::now();
        s.removeFrom(img);//.move_to(img);
        stop = high_resolution_clock::now();
        duration += duration_cast<microseconds>(stop - start);
    }
    cout << "Time taken by removal: "
        << duration.count() / REPS << " microseconds" << endl;
}
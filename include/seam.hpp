#include "CImg.hpp"
#include <vector>
using namespace std;
using namespace cimg_library;

class Seam {
    float total_energy;
    vector<int> positions;

  public:
    Seam(int height);
    friend bool operator<(const Seam &s1, const Seam &s2);
    float getEnergy() { return total_energy; };
    vector<int> &getPositions() { return positions; };
    void update(int col, float energy);
    CImg<unsigned char> removeFrom(CImg<unsigned char> &img);
};
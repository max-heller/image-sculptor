#include <vector>
using namespace std;

class Seam {
    float total_energy;
    vector<int> positions;

  public:
    Seam(int height);
    friend bool operator<(const Seam &s1, const Seam &s2);
    float getEnergy() { return total_energy; };
    vector<int> &getPositions() { return positions; };
    void update(int col, float energy);
};
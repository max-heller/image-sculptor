#include <vector>
using namespace std;

class Seam {
  public:
    float total_energy;
    vector<int> positions;
    Seam(int height);
    friend bool operator<(const Seam &s1, const Seam &s2);
    void update(int row, int col, float energy);
};
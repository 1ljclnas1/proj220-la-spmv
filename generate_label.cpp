#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Usage: ./program filename length" << endl;
    return -1;
  }

  string filename = argv[1];
  int length = stoi(argv[2]);

  ofstream ofs(filename);
  if (!ofs) {
    cout << "Error: Failed to open file." << endl;
    return -1;
  }

  ofs << length << " 1" << endl;

  for (int i = 0; i < length; ++i) {
    ofs << fixed << setprecision(6) << (double)rand() / RAND_MAX << endl;
  }

  ofs.close();

  cout << "Successfully generated " << length << " x 1 mtx format vector to file: " << filename << endl;

  return 0;
}

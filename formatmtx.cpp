#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./program dirname" << endl;
        return 1;
    }

    string dirname = argv[1];
    for (const auto& entry : fs::recursive_directory_iterator(dirname)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().string();
            ifstream ifs(filename);
            if (!ifs) {
                cerr << "Error: Failed to open file " << filename << endl;
                continue;
            }

            string line;
            string new_contents;
            while (getline(ifs, line)) {
                if (line.size() > 0 && line[0] == '%') {
                    continue;
                } else {
                    new_contents.append(line + "\n");
                }
            }
            ifs.close();

            ofstream ofs(filename);
            if (!ofs) {
                cerr << "Error: Failed to open file " << filename << endl;
                continue;
            }
            ofs << new_contents;
            ofs.close();

            cout << "Successfully processed file " << filename << endl;
        }
    }

    return 0;
}


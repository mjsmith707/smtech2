#include "MeshLoader.h"
using namespace smtech1;

MeshLoader::MeshLoader() {
}

std::vector<SMMesh> MeshLoader::loadMeshes(std::string filename) {
    std::vector<SMMesh> unimplemented;
    return unimplemented;
}

// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start
inline std::string& MeshLoader::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string& MeshLoader::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string& MeshLoader::trim(std::string &s) {
    return ltrim(rtrim(s));
}

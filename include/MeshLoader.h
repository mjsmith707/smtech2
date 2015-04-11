#ifndef __smtech1__MeshLoader__
#define __smtech1__MeshLoader__

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "SMMesh.h"
namespace smtech1 {
    class MeshLoader {
        private:
            void readFile(std::fstream& inputFile, unsigned int* charCounter);
            inline std::string& ltrim(std::string &s);
            inline std::string& rtrim(std::string &s);
            inline std::string& trim(std::string &s);
        public:
            MeshLoader();
            std::vector<SMMesh> loadMeshes(std::string filename);
    };
}


#endif /* defined(__smtech1__MeshLoader__) */

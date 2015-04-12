//
//  SMMesh.h
//  smtech1
//

#ifndef __smtech1__SMMesh__
#define __smtech1__SMMesh__

#include <vector>
#include "SMLine.h"

namespace smtech1 {
    class SMMesh {
        public:
            std::vector<SMLine> lines;
            SMMesh(std::vector<SMLine>& mesh);
    };
}

#endif /* defined(__smtech1__SMMesh__) */

#ifndef PATCH_H
#define PATCH_H

#include <QVector3D>
#include <array>

using std::array;

// Represents a quad patch with neighbours.
// our patch is located at:
//
//  (1,2) - - - (2, 2)
//    |           |
//    |           |
//    |           |
//    |           |
//  (1,1) - - - (2,1)
//  (1,1) => 5
//  (2,1) => 6
//  (1,2) => 9
//  (1,1) => 10

class QuadPatchWithNeighbourhood {
public:
    // These are indices into the half edge vertices structure!!
    array<unsigned int, 16> vertIndices;
};


// Represents a single quad patch.
class QuadPatch {
public:
    // These are indices into the half edge vertices structure!!
    array<unsigned int, 4> vertIndices;
};

#endif // PATCH_H

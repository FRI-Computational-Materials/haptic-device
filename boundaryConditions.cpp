#include "boundaryConditions.h"
#include "chai3d.h"

extern int just_unanchored;

using namespace chai3d;

// David's method -- atoms are not slowed down in any way, but atoms launched to walls are stopped theere
void applyDavidBoundaryConditions(cVector3d &A, cVector3d &B) {
    // freezing atoms at wall when they fly off 
    if (just_unanchored != 5) {
        // threshold for cube contining atoms
        double threshold = .5;
        // correct to keep atom inside of cube
        if (B.x() > threshold) {
        B.x(threshold);
        } else if (B.x() < threshold * -1) {
        B.x(threshold * -1);
        }
        if (B.y() > threshold) {
        B.y(threshold);
        } else if (B.y() < threshold * -1) {
        B.y(threshold * -1);
        }
        if (B.z() > threshold) {
        B.z(threshold);
        } else if (B.z() < threshold * -1) {
        B.z(threshold * -1);
        }
    }
}

// Sean's method
// this checkbounds is a temporary duplicate. All boundary information should be moved into this file eventually
bool checkBounds(cVector3d location, const double boundaryLimit) {
  if (location.y() > boundaryLimit || location.y() < -boundaryLimit ||
      location.x() > boundaryLimit || location.x() < -boundaryLimit ||
      location.z() > boundaryLimit || location.z() < -boundaryLimit) {
    return false;
  }
  return true;
}

void applySeanBoundaryConditions(chai3d::cVector3d &A,
                                chai3d::cVector3d &B, 
                                chai3d::cVector3d &spherePos, 
                                const chai3d::cVector3d &northPlanePos,
                                const chai3d::cVector3d &northPlaneNorm,
                                const chai3d::cVector3d &southPlanePos,
                                const chai3d::cVector3d &southPlaneNorm,
                                const chai3d::cVector3d &eastPlanePos,
                                const chai3d::cVector3d &eastPlaneNorm,
                                const chai3d::cVector3d &westPlanePos,
                                const chai3d::cVector3d &westPlaneNorm,
                                const chai3d::cVector3d &forwardPlanePos,
                                const chai3d::cVector3d &forwardPlaneNorm,
                                const chai3d::cVector3d &backPlanePos,
                                const chai3d::cVector3d &backPlaneNorm,
                                const double boundaryLimit
                                ) {
    // holds intersect point/norm if an intersect is made
    cVector3d intersectPoint;
    cVector3d intersectNorm;
    cVector3d tempPos;
    cVector3d tempA;
    tempA.copyfrom(A);
    tempPos.copyfrom(spherePos);

    // north plane
    if (cIntersectionSegmentPlane(A, B, northPlanePos, northPlaneNorm, intersectPoint, intersectNorm) == 1) {
        spherePos.zero();
        spherePos.copyfrom(intersectPoint);
        spherePos.y(spherePos.y() - (boundaryLimit * 2 - .01));
        if (!checkBounds(spherePos, boundaryLimit)) {
        spherePos.copyfrom(tempPos);
        }
    }
    // south plane
    if (cIntersectionSegmentPlane(A, B, southPlanePos, southPlaneNorm, intersectPoint, intersectNorm) == 1) {
        spherePos.zero();
        spherePos.copyfrom(intersectPoint);
        spherePos.y(spherePos.y() + (boundaryLimit * 2 - .01));
        if (!checkBounds(spherePos, boundaryLimit)) {
        spherePos.copyfrom(tempPos);
        }
    }
    // east plane
    if (cIntersectionSegmentPlane(A, B, eastPlanePos, eastPlaneNorm, intersectPoint, intersectNorm) == 1) {
        spherePos.zero();
        spherePos.copyfrom(intersectPoint);
        spherePos.x(spherePos.x() - (boundaryLimit * 2 - .01));
        if (!checkBounds(spherePos, boundaryLimit)) {
        spherePos.copyfrom(tempPos);
        }
    }
    // west plane
    if (cIntersectionSegmentPlane(A, B, westPlanePos, westPlaneNorm, intersectPoint, intersectNorm) == 1) {
        spherePos.zero();
        spherePos.copyfrom(intersectPoint);
        spherePos.x(spherePos.x() + (boundaryLimit * 2 - .01));
        if (!checkBounds(spherePos, boundaryLimit)) {
        spherePos.copyfrom(tempPos);
        }
    }
    // forward plane
    if (cIntersectionSegmentPlane(A, B, forwardPlanePos, forwardPlaneNorm, intersectPoint, intersectNorm) == 1) {
        spherePos.zero();
        spherePos.copyfrom(intersectPoint);
        spherePos.z(spherePos.z() - (boundaryLimit * 2 - .01));
        if (!checkBounds(spherePos, boundaryLimit)) {
        spherePos.copyfrom(tempPos);
        }
    }
    // back plane
    if (cIntersectionSegmentPlane(A, B, backPlanePos, backPlaneNorm, intersectPoint, intersectNorm) == 1) {
        spherePos.zero();
        spherePos.copyfrom(intersectPoint);
        spherePos.z(spherePos.z() + (boundaryLimit * 2 - .01));
        if (!checkBounds(spherePos, boundaryLimit)) {
        spherePos = tempPos;
        }
    }
}
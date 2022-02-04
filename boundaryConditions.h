#ifndef BOUNDARY_CONDITIONS_H
#define BOUNDARY_CONDITIONS_H

#include "chai3d.h"

using namespace chai3d;

void applyDavidBoundaryConditions(chai3d::cVector3d &A, chai3d::cVector3d &B);
bool checkBounds(chai3d::cVector3d location, const double boundaryLimit);
void applySeanBoundaryConditions(cVector3d& A,
                                cVector3d& B, 
                                cVector3d& spherePos, 
                                const cVector3d& northPlanePos,
                                const cVector3d& northPlaneNorm,
                                const cVector3d& southPlanePos,
                                const cVector3d& southPlaneNorm,
                                const cVector3d& eastPlanePos,
                                const cVector3d& eastPlaneNorm,
                                const cVector3d& westPlanePos,
                                const cVector3d& westPlaneNorm,
                                const cVector3d& forwardPlanePos,
                                const cVector3d& forwardPlaneNorm,
                                const cVector3d& backPlanePos,
                                const cVector3d& backPlaneNorm,
                                const double boundaryLimit
                                );

#endif
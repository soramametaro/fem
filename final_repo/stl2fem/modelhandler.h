#pragma once
#include "stlhandler.h"
#include "mesh.h"
#include "tetgen.h"

bool convertSTL2FEM(StlMesh *_stl, VolumeMesh *_fem, double _val = 1.2);
bool convertSTL2TetGen(StlMesh *_stl, tetgenio *_in);
bool convertTetGen2FEM(tetgenio *_out, VolumeMesh *_fem);

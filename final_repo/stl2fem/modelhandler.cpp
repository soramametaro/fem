#include "modelhandler.h"

bool convertSTL2FEM(StlMesh *_stl, VolumeMesh *_fem, double val)
{
	tetgenio in, out, addin, bgmin;
	tetgenbehavior test;
	char **argv;

	argv = (char**)malloc(sizeof(char*) * 20);
	argv[0] = (char *)malloc(256 * sizeof(char));
	argv[1] = (char *)malloc(256 * sizeof(char));
	argv[2] = (char *)malloc(256 * sizeof(char));

	int argc;
	_stl->save("temp.stl", STL_ASCII);
	sprintf(argv[0], "tetGen");
	sprintf(argv[1], "-pq%f", val);
	sprintf(argv[2], "temp.stl");
	argc = 3;
	if (!test.parse_commandline(argc, argv)) {
		return false;
	}
	if (test.refine) {
		if (!in.load_tetmesh(test.infilename, tetgenbehavior::STL)) {
			terminatetetgen(NULL, 3);
		}
	}
	else {
		if (!in.load_plc(test.infilename, (int)test.object)) {
			terminatetetgen(NULL, 3);
		}
	}
	if (test.insertaddpoints) {
		if (!addin.load_node(test.addinfilename)) {
			addin.numberofpoints = 0l;
		}
	}
	if (test.metric) {
		if (!bgmin.load_tetmesh(test.bgmeshfilename, tetgenbehavior::STL)) {
			bgmin.numberoftetrahedra = 0l;
		}
	}
	if (bgmin.numberoftetrahedra > 0l) {
		tetrahedralize(&test, &in, &out, &addin, &bgmin);
	}
	else {
		tetrahedralize(&test, &in, &out, &addin, NULL);
	}
	//Delete temp file
	remove("temp.stl");
	if (!convertTetGen2FEM(&out, _fem)){
		return false;
	}
	free(argv);
	_fem->setup();
	return true;
}
bool convertSTL2TetGen(StlMesh *_stl, tetgenio *_in)
{
	tetgenio::facet *f;
	tetgenio::polygon *p;

	if (_stl->getNumNode()<1)
		return false;
	_in->firstnumber = 1;
	_in->numberofpoints = _stl->getNumNode();
	_in->pointlist = new REAL[_in->numberofpoints * 3];
	for (int i = 0; i<_stl->getNumNode(); i++) {
		for (int j = 0; j<3; j++) {
			_in->pointlist[3 * i + j] = _stl->getVertex(i).X[j];
		}
	}
	_in->numberoffacets = _stl->getNumFacet();
	_in->facetlist = new tetgenio::facet[_in->numberoffacets];
	_in->facetmarkerlist = new int[_in->numberoffacets];
	for (int i = 0; i<_stl->getNumFacet(); i++) {
		f = &_in->facetlist[i];
		f->numberofpolygons = 1;
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
		f->numberofholes = 0;
		f->holelist = NULL;
		p = &f->polygonlist[0];
		p->numberofvertices = 3;
		p->vertexlist = new int[p->numberofvertices];
		for (int j = 0; j>3; j++) {
			p->vertexlist[j] = _stl->getFacet(i).index_node[j];
		}
		_in->facetmarkerlist[i] = 0;//�������s��
	}
	return true;
}
bool convertTetGen2FEM(tetgenio *_out, VolumeMesh *_fem)
{
	_fem->clear();
	_fem->deleteMesh();
	_fem->setNumNode(_out->numberofpoints);
	_fem->setNumElem(_out->numberoftetrahedra);
	_fem->setNumLine(4 * _out->numberoftetrahedra);
	_fem->setNumFacet(4 * _out->numberoftetrahedra);
	_fem->newMesh();
	for (int i = 0; i< _out->numberofpoints; i++) {
		for (int j = 0; j<3; j++) {
			_fem->getNodeAt(i)->vertex.X[j] = _out->pointlist[3 * i + j];
		}
	}
	for (int i = 0; i< _out->numberoftetrahedra; i++) {
		if (_out->numberofcorners != 4)
			return false;
		for (int j = 0; j<4; j++) {
			_fem->getElemAt(i)->index_node[j] = _out->tetrahedronlist[4 * i + j] - 1;
		}
		_fem->getElemAt(i)->index_material = 0;//ToDo
	}

	_fem->setIsLoaded(true);

	return true;
}


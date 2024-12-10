#include <iostream>
#include <iomanip>
#include "vectormatrix.h"
#include "modelhandler.h"

int main(int _argc, char *_argv[])
{
	char filename[256];
	char filename_out[256];
	double val = 2;

	StlMesh *stl = new StlMesh;
	VolumeMesh *fem = new VolumeMesh;

	std::cout<<"HELLO:)"<<std::endl;

	//入出力ファイル名の設定
	if(_argc>1)//コマンドライン入力の場合
		sprintf( filename, "%s", _argv[1] );
	else//デフォルトパス
		sprintf( filename, "../data/mode.stl" );
	sprintf(filename_out, "%s.fem", filename);

	//変換パラメータの取得
	if(_argc>2){
		val = atof(_argv[2]);
	}

	//ファイル読み込み，変換，ファイル出力
	if(stl->load(filename)){
		stl->centerize();
		stl->getInfo(std::cout);
		if (convertSTL2FEM(stl, fem, val)) {
			fem->save(filename_out);
		}
	}

	std::cout<<"GOOD BYE:("<<std::endl;

	delete stl;
	delete fem;
}


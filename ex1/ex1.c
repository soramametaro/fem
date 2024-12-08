#include "MathTool.h"

int main ( void )
{
	Vec3d v1, v2, v3;
	VecNd  v4, v5, v6;
	Matd T1, T2, T3, T4, T5;

	//3次元ベクトルのテスト
	v1.x = 1;	v1.y = 2;	v1.z = 3;
	v2.x = 4;	v2.y = 5;   v2.z = 6;
	printf("*************Vec3d*************\n");
	printf("v1 = ");
	printVec3( &v1 );
	printf("v2 = ");
	printVec3( &v2 );
	sumVec3andVec3( &v1, &v2, &v3 );
	printf("v1 + v2 = ");
	printVec3( &v3 );
	subVec3andVec3( &v1, &v2, &v3 );
	printf("v1 - v2 = ");
	printVec3( &v3 );
	scalingVec3( 0.5, &v1, &v3 );
	printf("0.5 * v1 = ");
	printVec3( &v3 );
	printf("v1 . v2 = ");
	printf( "%f\n", dotVec3andVec3( &v1, &v2 ) );
	crossVec3andVec3( &v1, &v2, &v3 );
	printf("v1 x v2 = ");
	printVec3( &v3 );
	printf("| v1 | = ");
	printf( "%f\n", absVec3( &v1 ) );

	//N次元ベクトルのテスト
	initVecN( &v4 );
	initVecN( &v5 );
	initVecN( &v6 );
	setVecNDim( &v4, 5 );
	setVecNDim( &v5, 5 );
	setVecNDim( &v6, 5 );
	v4.X[ 0 ] = 1;	v4.X[ 1 ] = 2;	v4.X[ 2 ] = 3;	v4.X[ 3 ] = 4;	v4.X[ 4 ] = 5;
	v5.X[ 0 ] = 6;	v5.X[ 1 ] = 7;	v5.X[ 2 ] = 8;	v5.X[ 3 ] = 9;	v5.X[ 4 ] = 10;
	printf("*************VecNd*************\n");
	printf("v4 = ");
	printVecN( &v4 );
	printf("v5 = ");
	printVecN( &v5 );
	sumVecNandVecN( &v4, &v5, &v6 );
	printf("v4 + v5= ");
	printVecN( &v6 );
	subVecNandVecN( &v4, &v5, &v6 );
	printf("v4 - v5 = ");
	printVecN( &v6 );
	scalingVecN( 0.5, &v4, &v6 );	
	printf("0.5 * v4 = ");
	printVecN( &v6 );
	printf("v4 . v5 = ");
	printf( "%f\n", dotVecNandVecN( &v4, &v5 ) );
	printf("| v4 | = ");
	printf( "%f\n", absVecN( &v4 ) );
	releaseVecN( &v5 );
    releaseVecN( &v6 );

	//行列のテスト
	initMat( &T1 );
	initMat( &T2 );
	initMat( &T3 );
    initMat( &T4 );
    initMat( &T5 );
    initVecN( &v5 );
    initVecN( &v6 );
    setMatDim( &T1, 4, 4);
    setMatDim( &T2, 4, 4);
    setMatDim( &T3, 3, 4);
    setMatDim( &T4, 5, 3);
    setVecNDim( &v5, 4 );
    setVecNDim( &v6, 3 );
    T1.X[ 0]=2;     T1.X[ 1]=5;     T1.X[ 2]=5;     T1.X[ 3]=3;
    T1.X[ 4]=3;     T1.X[ 5]=2;     T1.X[ 6]=5;     T1.X[ 7]=4;
    T1.X[ 8]=2;     T1.X[ 9]=1;     T1.X[10]=2;     T1.X[11]=4;
    T1.X[12]=3;     T1.X[13]=1;     T1.X[14]=2;     T1.X[15]=2;
    T2.X[ 0]=3;     T2.X[ 1]=2;     T2.X[ 2]=4;     T2.X[ 3]=1;
    T2.X[ 4]=2;     T2.X[ 5]=5;     T2.X[ 6]=3;     T2.X[ 7]=2;
    T2.X[ 8]=5;     T2.X[ 9]=2;     T2.X[10]=1;     T2.X[11]=3;
    T2.X[12]=1;     T2.X[13]=2;     T2.X[14]=3;     T2.X[15]=4;
	T3.X[ 0]=5;		T3.X[ 1]=3;		T3.X[ 2]=3;
    T3.X[ 3]=1;     T3.X[ 4]=2;		T3.X[ 5]=5;
    T3.X[ 6]=4;		T3.X[ 7]=3;     T3.X[ 8]=1;
    T3.X[ 9]=3;		T3.X[10]=2;		T3.X[11]=1;
	T4.X[ 0]=3;		T4.X[ 1]=2;		T4.X[ 2]=5;		T4.X[ 3]=4;     T4.X[ 4]=4;
    T4.X[ 5]=4;		T4.X[ 6]=3;		T4.X[ 7]=2;     T4.X[ 8]=5;     T4.X[ 9]=2;
    T4.X[10]=4;     T4.X[11]=2;     T4.X[12]=3;		T4.X[13]=4;		T4.X[14]=2;
	printf("*************Matd*************\n");
	printf("T1 = \n");
	printMat( &T1 );
	printf("T2 = \n");
	printMat( &T2 );
    printf("T3 = \n");
    printMat( &T3 );
    printf("T4 = \n");
    printMat( &T4 );
	sumMatandMat( &T1, &T2, &T5 );
	printf("T1 + T2 = \n");
	printMat( &T5 );
	subMatandMat( &T1, &T2, &T5 );
	printf("T1 - T2 = \n");
	printMat( &T5 );
	multiMatandMat( &T3, &T4, &T5 );
	printf("T3 * T4 = \n");
	printMat( &T5 );
    multiMatandVec3( &T3, &v1, &v5 );
    printf("T3 * v1 = \n");
    printVecN( &v5 );
    multiMatandVecN( &T4, &v4, &v6 );
    printf("T4 * v4 = \n");
    printVecN( &v6 );
	scalingMat( 0.5, &T1, &T5 );
	printf("0.5 * T1 = \n");
	printMat( &T5 );
	printf("det( T1 ) = ");
	printf( "%f\n", detMat( &T1 ) );
	trMat( &T1, &T5 );
	printf("tr( T1 ) = \n");
	printMat( &T5 );
	invMat( &T1, &T5);
	printf("T1^( -1 ) = \n");
	printMat( &T5 );
	releaseMat( &T1 );
	releaseMat( &T2 );
	releaseMat( &T3 );
    releaseMat( &T4 );
    releaseMat( &T5 );
    releaseVecN( &v4 );
    releaseVecN( &v5 );
    releaseVecN( &v6 );

	return 0;
}
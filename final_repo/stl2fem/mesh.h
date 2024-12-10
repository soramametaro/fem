#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "vectormatrix.h"

#define NUM_VERTEX_MAX 6


enum SELECT_MESH{
	SELECT_NONE,
	SELECT_NODE,
	SELECT_LINE,
	SELECT_FACET
};
enum NORMAL_TYPE{
	NORMAL_NONE,
	NORMAL_Facet,
	NORMAL_POINT
};

class Node{
public:
	Node(){this->clear();}
	~Node(){}
	Node &operator = (const Node &_node){
		memcpy(&vertex, &_node.vertex, sizeof(Vector3d));
		memcpy(&normal, &_node.normal, sizeof(Vector3d));
		index = _node.index;
		state = _node.state;
		is_surface_node = _node.is_surface_node;
		return (*this);
	}
	//ノードの番号
	int index;
	//ノードの状態
	int state;
	//ノードのラベル
	int label;
	//座標
	Vector3d vertex;
	//法線
	Vector3d normal;
	
	//表面電圧を保存する際既に一度保存したかどうか(異なる面が同じ接点を共有していることがある)
	bool is_shared;

	//表面のノードかどうか
	bool is_surface_node;

	//ノードが選択されているか
	bool is_selected;

	void clear(){
		this->index = -1;
		this->state = 0;
		this->label = -1;
		this->vertex = Vector3d(0,0,0);
		this->normal = Vector3d(0,0,0);
		this->is_selected = false;
		this->is_shared = false;
		this->is_surface_node = false;
	}

};

class Line
{
public:
	Line(){is_selected=false;}
	~Line(){;}
	bool is_selected;
	Vector3d vertex[2];
	int index_node[2];
	Line &operator = (const Line &_line){
		memcpy(vertex,_line.vertex,sizeof(Vector3d)*2);
		memcpy(index_node,_line.index_node,sizeof(int)*2);
		is_selected=_line.is_selected;
		return (*this);
	}
	double getLength(){return (vertex[0]-vertex[1]).abs();}
};

class Facet
{
public:

	//TODO：どこかに問題あり？
	Line line[NUM_VERTEX_MAX];
	int index_node[NUM_VERTEX_MAX];
	int index_normal[NUM_VERTEX_MAX];
	Vector3d vertex[NUM_VERTEX_MAX];
	Vector2d vertex_local[NUM_VERTEX_MAX];
	Vector3d normal[NUM_VERTEX_MAX];

	bool is_selected;
	bool isEnabled;
	int index_facet;
	int index_material;
	int num_node;
	int num_normal;
	int num_line;
	int desIndex;
	double desAngle;
	int index_elem;

	Vector3d min;
	Vector3d max;
	Vector3d center;
	//面積
	double area;

	bool is_shared;

	int normalType;
	Facet &operator = (const Facet &_facet){
		this->is_selected=_facet.is_selected;
		this->isEnabled=_facet.isEnabled;
		index_facet=_facet.index_facet;
		index_material=_facet.index_material;
		num_node=_facet.num_node;
		num_line=_facet.num_line;
		num_normal=_facet.num_normal;
		normalType = _facet.normalType;
		desIndex=_facet.desIndex;
		desAngle=_facet.desAngle;
		index_elem = _facet.index_elem;
		area = _facet.area;
		min = _facet.min;
		max = _facet.max;
		center = _facet.center;
		is_shared = _facet.is_shared;
		memcpy(index_node,_facet.index_node,sizeof(int)*num_node);
		memcpy(vertex,_facet.vertex,sizeof(Vector3d)*num_node);
		memcpy(vertex_local, _facet.vertex_local, sizeof(Vector3d) * num_node);
		memcpy(index_normal,_facet.index_normal,sizeof(int)*num_normal);
		memcpy(normal,_facet.normal,sizeof(Vector3d)*num_normal);
		memcpy(line,_facet.line,sizeof(Line)*num_line);
		return (*this);
	}
	Facet(int _num_node=0,	int _normalType=NORMAL_POINT) 
		: is_shared(false), num_node(_num_node), num_normal(_num_node), normalType(_normalType),is_selected(false)
		,isEnabled(true),index_material(0),num_line(0),desIndex(0),desAngle(0){
			this->setFacetTypeAsTriangle();
			center = Vector3d(0, 0, 0);
	}
	~Facet(){
	}
	void setFacetTypeAsTriangle(){
		num_node=3;
		num_line=3;
		num_normal=3;
		index_material=0;
	}
	void setFacetTypeAsPolygon(int _num=4){
		if(_num<NUM_VERTEX_MAX){
			num_node=_num;
			num_line=_num;
			num_normal=_num;
			index_material=0;
		}
	}
	Vector3d getMinVertex(){
		min=Vector3d(INT_MAX,INT_MAX,INT_MAX);
		for(int i=0;i<num_node;i++){
			if(min.x>vertex[i].x)min.x=vertex[i].x;
			if(min.y>vertex[i].y)min.y=vertex[i].y;
			if(min.z>vertex[i].z)min.z=vertex[i].z;
		}
		return min;
	}
	Vector3d getMaxVertex(){
		max=Vector3d(-INT_MAX,-INT_MAX,-INT_MAX);
		for(int i=0;i<num_node;i++){
			if(max.x<vertex[i].x)max.x=vertex[i].x;
			if(max.y<vertex[i].y)max.y=vertex[i].y;
			if(max.z<vertex[i].z)max.z=vertex[i].z;
		}
		return max;
	}
	bool getis_selected(){return this->is_selected;}
	void setis_selected(bool _is_selected){is_selected=_is_selected;}
	void calVertexLocal(){
		Vector3d e1 = (this->vertex[1] - this->vertex[0])/(this->vertex[1] - this->vertex[0]).abs();
		Vector3d e2 = e1 % this->normal[0];
		e2 /=e2.abs();
		this->vertex_local[0] = Vector2d(0, 0);
		this->vertex_local[1] = Vector2d(e1*(this->vertex[1] - this->vertex[0]), e2*(this->vertex[1] - this->vertex[0]));
		this->vertex_local[2] = Vector2d(e1*(this->vertex[2] - this->vertex[0]), e2*(this->vertex[2] - this->vertex[0]));
	}
	void calCenter() {
		this->center = Vector3d(0, 0, 0);
		for (int i = 0; i < this->num_node; i++)
			this->center += this->vertex[i];
		this->center /= this->num_node;
	}
	Matrixd calArea(){
		this->calVertexLocal();
		Matrixd result(3, 3);
		Matrixd temp(3, 3);
		for(int i = 0;i < 3;i++){
			for(int j = 0;j < 3;j++){
				if(j == 0)temp.X[3 * i + j] = 1;
				else temp.X[3 * i + j] = this->vertex_local[i].X[ j - 1];
			}
		}
		this->area = fabs(inverseLU(result.X, temp.X, 3)) / 2.0;
		return result;
	}
};

class Tetrahedra{
public:
	Tetrahedra(){this->position.malloc(12); this->clear();}
	~Tetrahedra(){this->position.free();}
	//ノードの座標
	VectorNd position;
	//材料番号
	int index_material;
	//ノード番号
	int index_node[4];
	//要素内のノード順序
	int order_node[16];
	//GUIで選択されたフラグ
	bool is_selecteded;

	//中心座標
	Vector3d center;
	//中心座標の計算
	//デフォルトではこの関数が実行されていない
	//要素の中心を使って何かするときは要注意
	void calCenter(){
		this->center = Vector3d(0,0,0);
		for(int i=0;i<4;i++)
			for(int j=0;j<3;j++)
				this->center.X[j] += this->position.X[3 * i + j] / 4;
	}
	//ノードの法線
	Vector3d normal[4];
	//法線の計算
	void calNormal(){
		Matrixd temp_coord(3,4);
		for(int i=0;i<4;i++){
			for(int j=0;j<3;j++)
				for(int k=0;k<4;k++)
					//temp_coord.X[3*k+j] = position.X[3 * (4*i+k)+j];
					temp_coord.X[3*k+j] = position.X[3 * order_node[4*i+k]+j];
			for(int j=0; j<3; j++){
				normal[i].X[j] = temp_coord.X[3*1+(j+1)%3]*temp_coord.X[3*2+(j+2)%3]-temp_coord.X[3*1+(j+1)%3]*temp_coord.X[3*0+(j+2)%3]
								-temp_coord.X[3*0+(j+1)%3]*temp_coord.X[3*2+(j+2)%3]-temp_coord.X[3*1+(j+2)%3]*temp_coord.X[3*2+(j+1)%3]
								+temp_coord.X[3*1+(j+2)%3]*temp_coord.X[3*0+(j+1)%3]+temp_coord.X[3*0+(j+2)%3]*temp_coord.X[3*2+(j+1)%3];
			}
			if(normal[i].abs()!=0)
				normal[i]/=normal[i].abs();	
			if(normal[i].X[0]*(temp_coord.X[3*3+0]-temp_coord.X[3*0+0])
				+normal[i].X[1]*(temp_coord.X[3*3+1]-temp_coord.X[3*0+1])
				+normal[i].X[2]*(temp_coord.X[3*3+2]-temp_coord.X[3*0+2])>0){
				normal[i]*=-1;
			}
		}
	}
	//体積
	double volume;
	//体積の計算
	Matrixd calVolume(){
		Matrixd result(4, 4);
		Matrixd temp(4, 4);
		for(int i = 0;i < 4;i++){
			for(int j = 0;j < 4;j++){
				if(j == 0)temp.X[4 * i + j] = 1;
				else temp.X[4 * i + j] = position.X[ 3 * i + j - 1];
			}
		}
		this->volume = fabs(inverseLU(result.X, temp.X, 4)) / 6.0;
		return result;
	}

	void clear(){
		is_selecteded=false;
		this->index_material = 0;
		this->center = Vector3d(0,0,0);
		memset(this->index_node, -1, sizeof(int) * 4);
		this->volume = 0;
		order_node[0]=2;order_node[1]=1;order_node[2]=0;order_node[3]=3;
		order_node[4]=2;order_node[5]=3;order_node[6]=1;order_node[7]=0;
		order_node[8]=0;order_node[9]=3;order_node[10]=2;order_node[11]=1;
		order_node[12]=0;order_node[13]=1;order_node[14]=3;order_node[15]=2;
		memset(this->position.X, 0, sizeof(double) * this->position.n);
		for(int i=0;i<4;i++){
			this->normal[i]=Vector3d(0,0,0);
		}
	}
};

class SurfMesh
{
protected:
	int num_node;
	int num_normal;
	int num_facet;
	int num_line;
	int num_material;
	Facet *facet;
	Line *line;
	Node *node;
	Vector3d *normal;
	Vector3d *vertex;
	Vector3f *color;
	int *labelIndex;
	bool *is_selected;
	Vector3d center;
	Vector3d size;
	Vector3d selectedVertex;
	int selectedIndex;
	double scale;

	bool isUseDisplayList;
	bool is_listed;
	bool is_loaded;
	bool is_view;
	bool is_view_line;
	bool is_view_node;
	bool is_view_facet;
	bool is_auto_scale;
	bool is_tri;
	bool is_identity;
	bool is_view_label;
	bool is_alpha_blend;
	bool is_view_line_extracted;
	bool is_vertex_color_enabled;

	void calFacetNormal();
	void calFacetVertex();
	void calLine();
	void calVertex();
	void calCenter();
	void calScale();

	void addVertex(Vector3d &_vertex);
	void addNormal(Vector3d &_normal);
	void addFacet(Facet &_facet);
public:
	SurfMesh(void);
	~SurfMesh(void);

	bool load(const char *filename);
	bool save(const char *filename);

	void newMesh();
	void deleteMesh();
	void clearMesh();
	void clearView();

	bool getIsTri(){return this->is_tri;}
	bool getIsLoaded(){return this->is_loaded;}
	bool getIsView(){return this->is_view;}
	bool getIsAutoScale(){return this->is_auto_scale;}
	bool getIsIdentity(){return this->is_identity;}
	bool getIsViewNode(){return this->is_view_node;}
	bool getIsViewLine(){return this->is_view_line;}
	bool getIsViewFacet(){return this->is_view_facet;}
	bool getIsUseDisplayList(){return this->isUseDisplayList;}
	bool getIsVertex_color_enabled(){return this->is_vertex_color_enabled;}
	bool getIsViewLabel(){return this->is_view_label;}

	int getNumNode() { return num_node; }
	int getNumNormal() { return num_normal; }
	int getNumFacet() { return num_facet; }
	int getNumMaterial() { return num_material; }
	int getNumLine() {return num_line;}

	Vector3d getSize(){return size;}
	double getScale(){return scale;}
	Vector3d getCenter(){return center;}
	Vector3d getVertex(int _index){return vertex[_index];}
	Vector3d* getVertexPointer(){return vertex;}
	Node getNode(int _index) { return node[_index]; }
	Vector3d getNormal(int _index){return normal[_index];}
	Line getLine(int _index){return this->line[_index];}
	Vector3f getColor(int _index){return color[_index];}
	void setVertex(int _index, Vector3d _vertex){vertex[_index]=_vertex;}
	Facet getFacet(int _index){return facet[_index];}
	Facet* getFacetPointer(int _index){return &facet[_index];}
	void setFacet(int _index, Facet _facet){facet[_index]=_facet;}
	void setNormal(int _index, Vector3d _normal){normal[_index]=_normal;}
	void setLine(int _index, Line _line){line[_index]=_line;}
	int getLabelIndex(int _index){return this->labelIndex[_index];}

	std::ostream& getInfo(std::ostream &stream);

	void setColorAt(int _index, Vector3f _color){this->color[_index]=_color;}
	void setLabelIndexAt(int _index, int _label){this->labelIndex[_index] = _label;}
	void setNumNode(int _num_node){this->num_node=_num_node;}
	void setNumNormal(int _num_normal){this->num_normal=_num_normal;}
	void setNumFacet(int _num_facet){this->num_facet=_num_facet;}
	void setNumMaterial(int _num_material){this->num_material=_num_material;}
	void setNumLine(int _num_line){this->num_line=_num_line;}
	void setIsView(bool _is_view){this->is_view=_is_view;}
	void setIsViewLabel(bool _is_view_label){this->is_view_label=_is_view_label;}
	void setIsViewLine(bool _is_view_line){this->is_view_line=_is_view_line;}
	void setIsViewFacet(bool _is_view_facet){this->is_view_facet=_is_view_facet;}
	void setIsViewNode(bool _is_view_node){this->is_view_node=_is_view_node;}
	void setIsAutoScale(bool _is_auto_scale){this->is_auto_scale=_is_auto_scale;}
	void setIsTri(bool _is_tri){this->is_tri=_is_tri;}
	void setIsIdentity(bool _is_identity){this->is_identity=_is_identity;}

	void centerize();
};

class VolumeMesh{

protected:
	//読み込みのフラグ
	bool is_loaded;
	//ノード
	Node *node;
	//ノード数
	int num_node;
	//要素
	Tetrahedra *elem;
	//要素数
	int num_elem;
	//線
	Line *line;
	//線数
	int num_line;
	//面
	Facet *facet;
	//面数
	int num_facet;

	//物体のサイズを描画空間に合わせるかどうか
	bool is_auto_scale;
	//選択モード
	int selection_mode;
	//物体の中心
	Vector3d center;
	//サイズ
	Vector3d size;
	//スケール
	double scale;

	bool is_view_line_extracted;
	bool is_view_facet_extracted;

	bool is_view_info;
	bool is_view_node;
	bool is_view_line;
	bool is_view_facet;
	bool is_view_smooth;

	void calCenter();
	//Calculate the scale
	void calScale();
	//線の抽出
	void calLine();
	//面の抽出
	void calFacet();
public:
	VolumeMesh();
	~VolumeMesh();

	void newMesh();
	void deleteMesh();

	//Clear FEM data
	void clear();
	void clearView();
	//Load FEM data
	bool load(const char* _filename);
	//Save FEM data
	bool save(const char* _filename);
	//Calculate the scale and the center coordinate of the object
	void setup();

	//Information about drawing object
	std::ostream& getInfo(std::ostream &stream);

	//Accessor
	void setNumNode(int _num_node) { this->num_node = _num_node; }
	int getNumNode(){return this->num_node;}
	void setNumElem(int _num_elem) { this->num_elem = _num_elem; }
	int getNumElem() { return this->num_elem; }
	void setNumLine(int _num_line) { this->num_line = _num_line; }
	int getNumLine() { return this->num_line; }
	void setNumFacet(int _num_facet) { this->num_facet = _num_facet; }
	int getNumFacet() { return this->num_facet; }
	Vector3d getCenter(){return this->center;}
	Vector3d getSize(){return this->size;}
	bool getIsLoaded(){return this->is_loaded;}
	Node* getNodeAt(int _elem){return (&this->node[_elem]);}
	Tetrahedra* getElemAt(int _elem){return (&this->elem[_elem]);}
	void setIsViewNode(bool _is_view_node){this->is_view_node=_is_view_node;}
	void setIsViewFacet(bool _is_view_facet) { this->is_view_facet = _is_view_facet; }
	void setIsViewLine(bool _is_view_line){this->is_view_line = _is_view_line;}
	void setIsViewInfo(bool _is_view_info){this->is_view_info = _is_view_info;}
	void setIsAutoScale(bool _is_auto_scale){ is_auto_scale=_is_auto_scale;}
	void setIsLoaded(bool _is_loaded) { this->is_loaded = _is_loaded; }

};

bool isSharedLine(Line _new, Line *_old, int _num);
bool isSharedFacet(Facet _new, Facet *_old, int _num);

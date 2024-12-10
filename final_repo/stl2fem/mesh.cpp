#include "mesh.h"


/****************Surface Mesh******************/
SurfMesh::SurfMesh(void)
	:num_node(0),num_normal(0),num_facet(0),num_line(0),num_material(0)
	,is_view(true),is_view_line(false),is_view_node(false),is_view_facet(true)
	,is_auto_scale(true),is_loaded(false),is_tri(false),is_identity(false)
	,is_alpha_blend(false),is_view_line_extracted(false)
	,is_listed(true),isUseDisplayList(false),is_vertex_color_enabled(false),
	is_view_label(false)
{
	this->newMesh();
}


SurfMesh::~SurfMesh(void)
{
	this->deleteMesh();
}

void SurfMesh::deleteMesh()
{
	this->is_loaded=false;
	delete []vertex;
	delete []facet;
	delete []normal;
	delete []labelIndex;
	delete []color;
	delete []line;
	delete []is_selected;
}

void SurfMesh::newMesh()
{
	this->vertex=new Vector3d[this->num_node+2];
	this->color=new Vector3f[this->num_node+2];
	this->labelIndex=new int[this->num_node+2];
	this->facet=new Facet[this->num_facet+2];
	this->normal=new Vector3d[this->num_normal+2];
	this->is_selected=new bool[this->num_node+2];
	this->line=new Line[this->num_line+2];

	for(int i=0;i<num_node+2;i++){
		is_selected[i]=false;
		labelIndex[i]=-1;
	}
}


void SurfMesh::clearMesh()
{
	this->is_listed=false;
	this->is_loaded=false;
	this->is_view_line_extracted=false;
	this->is_vertex_color_enabled=false;
	this->isUseDisplayList=false;
	num_node = 0;
	num_normal = 0;
	num_facet = 0;
	num_line = 0;
	this->scale=0;
	this->size=Vector3d(1,1,1);
	this->center=Vector3d(0,0,0);
}
void SurfMesh::clearView()
{
	this->is_view=true;
	this->is_view_facet=true;
	this->is_view_line=true;
	this->is_view_node=true;
	this->is_view_label=true;
}
void SurfMesh::addVertex(Vector3d &_vertex)
{
	num_node++;
	vertex = (Vector3d*)realloc(vertex, num_node*sizeof(Vector3d));
	vertex[num_node-1] = _vertex;
	is_selected = (bool*)realloc(is_selected, num_node*sizeof(bool));
	is_selected[num_node-1]=false;
}
void SurfMesh::addNormal(Vector3d &_normal)
{
	num_normal++;
	normal = (Vector3d*)realloc(normal, num_normal*sizeof(Vector3d));
	normal[num_normal-1] = _normal;
}
void SurfMesh::addFacet(Facet &_facet)
{
	num_facet++;
	facet = (Facet*)realloc(facet, num_facet*sizeof(Facet));
	facet[num_facet-1] = _facet;
}
std::ostream& SurfMesh::getInfo(std::ostream &stream)
{
	stream<<"Number of vertex:" << num_node << std::endl;
	stream<<"Number of normal:" << num_normal << std::endl;
	stream<<"Number of surface:" << num_facet << std::endl;
	return stream;
}

void SurfMesh::calFacetNormal()
{
	Vector3d temp[2];
	delete []normal;
	this->normal=new Vector3d[this->num_facet];
	for(int i=0;i<num_facet;i++){
		this->facet[i].normalType=NORMAL_Facet;
		this->facet[i].num_normal=1;
		for(int j=1;j<3;j++)
			temp[j-1]=vertex[facet[i].index_node[j]]-vertex[facet[i].index_node[0]];
		for(int j=0;j<3;j++){
			this->normal[i].X[j] =temp[1].X[(j+1)%3]*temp[0].X[(j+2)%3]
					-temp[1].X[(j+2)%3]*temp[0].X[(j+1)%3];
		}
		if(this->normal[i].abs()!=0)this->normal[i]/=this->normal[i].abs();
		this->facet[i].index_normal[0]=i;
		this->facet[i].normal[0]=this->normal[i];
	}
}
void SurfMesh::calFacetVertex()
{
	for(int i=0;i<this->num_facet;i++){
		for(int j=0;j<this->facet[i].num_node;j++){
			this->facet[i].vertex[j]=vertex[this->facet[i].index_node[j]];
		}
	}
}
void SurfMesh::calLine()
{
	if(this->num_line>1)
		delete []this->line;
	this->num_line=0;
	this->line= new Line[this->num_facet*4];//�l�p�`�܂ł����z�肵�Ă��Ȃ�

	for(int i=0;i<this->num_facet;i++){
		for(int j=0;j<facet[i].num_node;j++){
			facet[i].line[j].index_node[0]=facet[i].index_node[j];
			facet[i].line[j].index_node[1]=facet[i].index_node[(j+1)%facet[i].num_node];
			facet[i].line[j].vertex[0]=facet[i].vertex[j];
			facet[i].line[j].vertex[1]=facet[i].vertex[(j+1)%facet[i].num_node];
			if(!isSharedLine(facet[i].line[j],line,num_line)){
				this->line[this->num_line]=facet[i].line[j];
				this->num_line++;
			}
		}
	}
	this->is_view_line_extracted=true;
}

void SurfMesh::calVertex()
{
	int count=1;
	this->vertex[0]=this->facet[0].vertex[0];
	for(int i=0;i<this->num_facet;i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<count;k++){
				if(this->facet[i].vertex[j]==this->vertex[k]){
					this->facet[i].index_node[j]=k;
					k=count;
				}
				else if(k==count-1){
					this->vertex[count]=this->facet[i].vertex[j];
					this->facet[i].index_node[j]=count;
					count++;
				}
			}
		}
	}
	this->num_node=count;
	Vector3d *tempVertex=new Vector3d[this->num_node];
	memcpy(tempVertex,vertex,sizeof(Vector3d)*this->num_node);
	delete []vertex;
	this->vertex=tempVertex;
}
void SurfMesh::calCenter()
{
	Vector3d min(INT_MAX,INT_MAX,INT_MAX);
	Vector3d max(-INT_MAX,-INT_MAX,-INT_MAX);
	for(int i=0;i<this->num_node;i++){
		for(int k=0;k<3;k++){
			if(min.X[k]>this->vertex[i].X[k])
				min.X[k]=this->vertex[i].X[k];
			if(max.X[k]<this->vertex[i].X[k])
				max.X[k]=this->vertex[i].X[k];
		}
	}
	this->size.x=fabs(max.x-min.x);
	this->size.y=fabs(max.y-min.y);
	this->size.z=fabs(max.z-min.z);
	this->center=(max+min)/2.0;
}
void SurfMesh::calScale()
{
	double max=-INT_MAX;
	for(int i=0;i<3;i++)
		if(max<size.X[i])max=size.X[i];
	scale=max;
}

bool SurfMesh::load(const char *_filename)
{
	return true;
}

bool SurfMesh::save(const char *_filename)
{
	return true;
}

void SurfMesh::centerize()
{
	std::cout << "Centerizing mesh...";
	this->calCenter();
	for (int i = 0;i < this->num_node;i++) {
		this->vertex[i] -= this->center;
	}
	for (int i = 0;i < this->num_facet;i++) 
		for(int j=0;j<this->facet[i].num_node;j++)
			this->facet[i].vertex[j] -= this->center;
	std::cout << "[OK]" << std::endl;
}
/****************Surface Mesh******************/

/****************Volume Mesh******************/
VolumeMesh::VolumeMesh():num_elem(0),num_node(0),num_facet(0),num_line(0)
{
	this->newMesh();
	this->clear();
	this->clearView();
}

VolumeMesh::~VolumeMesh()
{
}

void VolumeMesh::newMesh()
{
	this->elem = new Tetrahedra[this->num_elem + 2];
	this->node = new Node[this->num_node + 2];
	this->line = new Line[this->num_line + 2];
	this->facet = new Facet[this->num_facet + 2];
}

void VolumeMesh::deleteMesh()
{
	this->is_loaded = false;
	delete []elem;
	delete []node;
	delete []line;
	delete []facet;
}

void VolumeMesh::clear()
{
	this->is_loaded = false;
	this->is_auto_scale = false;
	this->is_view_line_extracted = false;
	this->is_view_facet_extracted = false;
	this->center = Vector3d(0, 0, 0);
	this->size = Vector3d(1, 1, 1);
	this->scale = 1;
}

void VolumeMesh::clearView()
{
	this->is_view_line = true;
	this->is_view_node = true;
	this->is_view_facet = true;
	this->is_view_smooth = false;
}

bool VolumeMesh::load(const char *_filename)
{
	std::ifstream file;
	char buf[512];
	char dummy[256];
	int num;

	file.open(_filename, std::ios::in);
	if(!file.is_open())
	{
		std::cout<<"Fail to load volume mesh "<<_filename<<std::endl;
		return false;
	}
	this->deleteMesh();
	while(file.getline(buf, sizeof(buf))){
		if(strstr(buf, "nNodes ") || strstr(buf, "nVertex ")){
			sscanf(buf, "%s %d", &dummy, &num); 
			num_node = num;
			continue;
		}
		if(strstr(buf, "nTetrahedra ") || strstr(buf, "nTetrahedron ")){
			sscanf(buf, "%s %d", &dummy, &num);
			num_elem = num;
			break;
		}
	}
	this->newMesh();

	while(file.getline(buf, sizeof(buf)))
	{
		if(strstr(buf, "# Data ")) break;
	}
	while(!strstr(buf, "@1"))
	{
		file >> buf;
		if(file.eof()){
			std::cout<<"[FAIL]"<<std::endl;
			return false;
		}
	}
	for(int i=0; i< num_node; i++)
	{
		file 
			>> node[i].vertex.x 
			>> node[i].vertex.y 
			>> node[i].vertex.z;
	}
	while(!strstr(buf, "@2"))
	{
		file >> buf;
		if(file.eof()){
			std::cout<<"[FAIL]"<<std::endl;
			return false;
		}
	}
	for(int i=0; i< num_elem; i++){
		for(int j=0; j<4; j++){
			file >> elem[i].index_node[j];
			elem[i].index_node[j]--;
		}
	}
	for(int i=0;i<num_elem;i++)
		for(int j=0;j<4;j++)
			for(int k=0;k<3;k++)
				elem[i].position.X[3*j+k] = node[elem[i].index_node[j]].vertex.X[k];

	while(!strstr(buf, "@3")){
		file >> buf;
		if(file.eof()){
			std::cout<<"[FAIL]"<<std::endl;
			return false;
		}
	}
	for(int i=0; i< num_elem; i++)
		file >> elem[i].index_material;
	file.clear();
	file.close();

	this->setup();


	return true;
}

bool VolumeMesh::save(const char *_filename)
{
	std::ofstream file;
	std::cout<<"Saving mesh data...";
	file.open(_filename, std::ios::out);
	if(!this->is_loaded || !file.is_open())
	{
		std::cout<<"[FAIL]"<<std::endl;
		return false;
	}
	file<<"nNodes "<<this->num_node<<std::endl;
	file<<"nTetrahedra "<<this->num_elem<<std::endl;

	file<<"Nodes { float[3] Coordinates } @1"<<std::endl;
	file<<"Tetrahedra { int[4] Nodes } @2"<<std::endl;
	file<<"TetrahedronData { byte Materials } @3"<<std::endl;
	file<<"# Data section follows"<<std::endl;
	file<<"@1"<<std::endl;
	for(int i=0; i< num_node; i++)
		file<<node[i].vertex.X[0]<<" "<<node[i].vertex.X[1]<<" "<<node[i].vertex.X[2]<<std::endl;
	file<<"@2"<<std::endl;
	for(int i=0; i< num_elem; i++){
		for(int j=0; j<4; j++){
			file << elem[i].index_node[j]+1<<" ";
		}
		file<<std::endl;
	}
	file<<"@3"<<std::endl;
	for(int i=0; i< num_elem; i++)
		file << elem[i].index_material<<std::endl;
	file.clear();
	file.close();
	std::cout<<"[OK]"<<std::endl;
	return true;
}
void VolumeMesh::calCenter()
{
	Vector3d min(INT_MAX,INT_MAX,INT_MAX);
	Vector3d max(-INT_MAX,-INT_MAX,-INT_MAX);
	for(int i=0;i<this->num_node;i++){
		for(int k=0;k<3;k++){
			if(min.X[k]>this->node[i].vertex.X[k])
				min.X[k]=this->node[i].vertex.X[k];
			if(max.X[k]<this->node[i].vertex.X[k])
				max.X[k]=this->node[i].vertex.X[k];
		}
	}
	this->size.x=fabs(max.x-min.x);
	this->size.y=fabs(max.y-min.y);
	this->size.z=fabs(max.z-min.z);
	this->center=(max+min)/2.0;
}
void VolumeMesh::calScale()
{
	double max = -INT_MAX;
	for (int i = 0;i<3;i++)
		if (max<size.X[i])max = size.X[i];
	scale = max;
}
std::ostream& VolumeMesh::getInfo(std::ostream &stream)
{
	stream << "Number of vertices:" << this->num_node << std::endl;
	stream << "Number of tetrahedras:" << this->num_elem << std::endl;
	stream << "Number of surfaces:" << this->num_facet << std::endl;
	stream << "Number of lines:" << this->num_line << std::endl;
	stream << "Size:" << this->size << std::endl;

	return stream;
}

void VolumeMesh::calLine()
{
	std::cout<<"Extracting lines...";
	if(!this->is_loaded){
		this->is_view_facet_extracted = false;
		std::cout<<"[FAIL]"<<std::endl;
		return;
	}

	Line t_line;
	delete []this->line;
	this->num_line = 0;
	this->line = new Line[this->num_elem * 6 +2];//4�ʑ̂Ȃ̂�6�{

	for(int i=0;i<this->num_elem;i++){
		for(int j=0;j<4;j++){
			for(int k=j;k<4;k++){
				if(j!=k){
					t_line.index_node[0] = elem[i].index_node[j];
					t_line.index_node[1] = elem[i].index_node[k];
					t_line.vertex[0] = node[elem[i].index_node[j]].vertex;
					t_line.vertex[1] = node[elem[i].index_node[k]].vertex;
					if(!isSharedLine(t_line, this->line, this->num_line)){
						this->num_line++;
						this->line[this->num_line-1] = t_line;
					}
				}
			}
		}
	}
	this->is_view_line_extracted = true;
	std::cout<<"[OK]"<<std::endl;
}

void VolumeMesh::calFacet()
{
	std::cout<<"Extracting facets...";
	if(!this->is_loaded){
		this->is_view_facet_extracted = false;
		std::cout<<"[FAIL]"<<std::endl;
		return;
	}
	Facet t_facet;
	t_facet.setFacetTypeAsTriangle();
	delete []this->facet;
	this->num_facet = 0;
	this->facet= new Facet[this->num_elem * 4];
	for(int i=0; i<this->num_elem; i++){
		elem[i].calNormal();
		for(int j=0; j<4; j++){
			t_facet.normal[j] = elem[i].normal[j];
			t_facet.index_node[0] = elem[i].index_node[(j+0)%4];
			t_facet.index_node[1] = elem[i].index_node[(j+1)%4];
			t_facet.index_node[2] = elem[i].index_node[(j+2)%4];
			t_facet.vertex[0] = node[elem[i].index_node[(j+0)%4]].vertex;
			t_facet.vertex[1] = node[elem[i].index_node[(j+1)%4]].vertex;
			t_facet.vertex[2] = node[elem[i].index_node[(j+2)%4]].vertex;
			//t_facet.normal[j] = (t_facet.vertex[1] - t_facet.vertex[0]) % (t_facet.vertex[2] - t_facet.vertex[0]);
			//if (t_facet.normal[j] != Vector3d(0, 0, 0))t_facet.normal[j] /= t_facet.normal[j].abs();

			//std::cout<<t_facet.vertex[0]<<std::endl;

			if(!isSharedFacet(t_facet, this->facet, this->num_facet)){
				this->num_facet++;
				t_facet.index_facet = this->num_facet-1;
				t_facet.index_elem = i;
				this->facet[this->num_facet-1] = t_facet;
			}
		}
	}
	this->is_view_facet_extracted = true;
	std::cout<<"[OK]"<<std::endl;
}

/****************Volume Mesh******************/

bool isSharedLine(Line _new, Line *_old, int _num)
{
	for(int i=0;i<_num;i++){
		if(_old[i].index_node[0]==_new.index_node[0]&&_old[i].index_node[1]==_new.index_node[1])
			return true;
		else if(_old[i].index_node[0]==_new.index_node[1]&&_old[i].index_node[1]==_new.index_node[0])
			return true;
	}
	return false;
}

bool isSharedFacet(Facet _new, Facet *_old, int _num)
{
	int count=0;
	for(int i=0; i<_num; i++){
		if((_new.index_node[0]==_old[i].index_node[0] && _new.index_node[1] == _old[i].index_node[1] && _new.index_node[2] == _old[i].index_node[2]) ||
			(_new.index_node[0]==_old[i].index_node[1] && _new.index_node[1] == _old[i].index_node[2] && _new.index_node[2] == _old[i].index_node[0]) ||
			(_new.index_node[0]==_old[i].index_node[2] && _new.index_node[1] == _old[i].index_node[0] && _new.index_node[2] == _old[i].index_node[1]) ||
			(_new.index_node[0]==_old[i].index_node[0] && _new.index_node[1] == _old[i].index_node[2] && _new.index_node[2] == _old[i].index_node[1]) ||
			(_new.index_node[0]==_old[i].index_node[1] && _new.index_node[1] == _old[i].index_node[0] && _new.index_node[2] == _old[i].index_node[2]) ||
			(_new.index_node[0]==_old[i].index_node[2] && _new.index_node[1] == _old[i].index_node[1] && _new.index_node[2] == _old[i].index_node[0])){
			_old[i].is_shared = true;
				return true;
		}	
	}
	return false;
}

void VolumeMesh::setup()
{
	this->is_loaded = true;
	this->calCenter();
	this->calScale();
	this->calLine();
	this->calFacet();
}

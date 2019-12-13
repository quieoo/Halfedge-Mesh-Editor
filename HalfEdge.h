#ifndef _HALFEDGE_H_
#define _HALFEDGE_H_

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <set>
using namespace std;

class vertex;
class edge;
class face;
class halfedge;
class pos;
class vec3;
class vec4;
class mat4;


edge *uniquejoin(list<edge> *edges, edge &e);
struct pos
{
    float x;
    float y;
    float z;
    pos()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    pos(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
};

struct vec3
{
    float x;
    float y;
    float z;
    vec3() {}
    vec3(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
    vec3(pos a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
    }
    vec3 operator+(const vec3 a)
    {
        return vec3(x + a.x, y + a.y, z + a.z);
    }
    vec3 operator-(const vec3 a)
    {
        return vec3(x - a.x, y - a.y, z - a.z);
    }
    vec3 cross(const vec3 a)
    {
        return vec3(y * a.z - z * a.y, z * a.x - a.z * x, x * a.y - a.x * y);
    }
    float dot(const vec3 a)
    {
        return x * a.x + y * a.y + z * a.z;
    }
    void normalize()
    {
        float len = sqrt(x * x + y * y + z * z);
        x = x / len;
        y = y / len;
        z = z / len;
    }
};
struct vec4
{
    float x, y, z, t;
    vec4() {}
    vec4(float _x, float _y, float _z, float _t)
    {
        x = _x;
        y = _y;
        z = _z;
        t = _t;
    }
    vec4(vec3 a, float b)
    {
        x = a.x;
        y = a.y;
        z = a.z;
        t = b;
    }
    vector<float> getarray()
    {
        vector<float> a;
        a.push_back(x);
        a.push_back(y);
        a.push_back(z);
        a.push_back(t);
        return a;
    }
};
struct mat4
{
    float matrix[4][4];
    mat4()
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                matrix[i][j] = 0;
    }
    mat4(vec4 x)
    {
        vector<float> y = x.getarray();
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                matrix[i][j] = y[i] * y[j];
    }
    mat4 operator+(const mat4 &a)
    {
        mat4 r;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                r.matrix[i][j] = matrix[i][j] + a.matrix[i][j];
        return r;
    }
    void output()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
                cout << matrix[i][j] << " ";
            cout << endl;
        }
    }
};
pos operator+(const pos &a, const pos &b)
{
    return pos(a.x + b.x, a.y + b.y, a.z + b.z);
}
pos operator*(const pos &a, float b)
{
    return pos(a.x * b, a.y * b, a.z * b);
}

class halfedge
{
public:
    halfedge *_opposite;
    halfedge *_next;
    vertex *_vertex;
    edge *_edge;
    face *_face;
    long index;
    bool valid;
    halfedge(int c){}
    halfedge()
    {
        valid = true;
        _opposite = NULL;
        _next = NULL;
        _vertex = NULL;
        _edge = NULL;
        _face = NULL;
        index = -1;
    }
    halfedge(const halfedge& e)
    {
        _opposite=e._opposite;
        _next=e._opposite;
        _vertex=e._vertex;
        _edge=e._edge;
        _face=e._face;
        index=e.index;
        valid=e.valid;
    }
    halfedge *opposite()
    {
        return _opposite;
    }
    halfedge *next()
    {
        return _next;
    }
    vertex *vertexpointto()
    {
        return _vertex;
    }
    edge *edge_right()
    {
        return _edge;
    }
    face *face_left()
    {
        return _face;
    }

    bool operator==(const halfedge &a)
    {
        return index == a.index;
    }
    //判断半边是否在边缘，即半边不存在opposite
    bool isboundary()
    {
        return _opposite == NULL;
    }
    //获取指向同一个顶点的下一个顺时钟方向上的半边
    halfedge *CWgetNext()
    {
        //由于半边中储存指向的顶点
        //顺时钟方向上下一个指向同一个顶点的半边如此计算
        if (_next->_opposite != NULL)
            return _next->_opposite;
        else
            return NULL;
    }
    //获取指向同一个顶点的下一个逆时钟方向上的半边
    halfedge *RCWgetNext()
    {
        if (_opposite != NULL)
            return _opposite->_next->_next;
        else
            return NULL;
    }
    void output();
    int collapse();
    bool CheckSafe();
};

class vertex
{
public:
    float pos_x, pos_y, pos_z;
    long index;
    halfedge *_halfedge;
    pos newposition;
    mat4 QuardError;
    bool valid;
    vertex() { valid = 1; }
    vertex(float _x, float _y, float _z, int _i)
    {
        pos_x = _x;
        pos_y = _y;
        pos_z = _z;
        index = _i;
        valid = 1;
    }
    halfedge *tgethalfedge() { return _halfedge; }

    pos getposition()
    {
        pos position(pos_x, pos_y, pos_z);
        return position;
    }
    void setposition(pos &a)
    {
        pos_x = a.x;
        pos_y = a.y;
        pos_z = a.z;
    }
    void output()
    {
        cout <<valid<< "v(" << pos_x << "," << pos_y << "," << pos_z << ")" << index << endl;
    }
    //从链表中查找index的点
    bool operator==(const vertex &x)
    {
        return index == x.index;
    }
    //判断本顶点是否在边界
    bool isboundary()
    {
        //若是其周围的所有半边都存在对边，则返回假；否则返回真
/*        halfedge *_he = _halfedge;
        halfedge *h = _he;
        while (true)
        {

            //h->output();
            if (h->_opposite == NULL)
                return true;
            else
            {
                h = h->_next->_opposite;
                if (h == NULL)
                    return true;
                if (h == _he)
                    return false;
            }
        }*/
        vector<vertex*> vs=FindAllConnectvertexs();
        return vs.size()==3;
    }
    //返回所有与此顶点关联的边的指针
    vector<edge *> FindAllLinkEdges();
    vector<vertex *> FindAllConnectvertexs();
    vector<face *> FindAllConnectfaces();
    vector<halfedge *> FindAllLinkHalfEdges();
    /*计算每个顶点的误差矩阵*/
    void getQE();
    void update(vertex *x);
};

class edge
{
public:
    long index;
    halfedge *_halfedge;
    long point1;
    long point2;
    vertex newvertex;
    pos newposition; //边产生的新位置
    float cost;      //花费
    mat4 Qbar;       //相对应与新位置的最小误差矩阵
    bool valid;
    bool dirty; //表示此边的cost是否被更改过
    edge()
    {
        _halfedge = NULL;
        valid = true;
        dirty = 0;
    }
    edge(long p1, long p2, long in)
    {
        point1 = p1;
        point2 = p2;
        index = in;
        _halfedge = NULL;
        valid = true;
        dirty = 0;
    }
    halfedge *gethalfedge() { return _halfedge; }
    bool operator==(const edge &a)
    {
        return a.index == index;
    }

    void output()
    {
        cout <<valid<< "e(" << point1 << "," << point2 << ")" << index << endl;
    
    }
    bool isboundary()
    {
        return _halfedge->isboundary();
    }
    long getpoint1() { return point1; }
    long getpoint2() { return point2; }
    void GetNewPosition();
    void GetCost();
};

class face 
{
public:
    halfedge *gethalfedge() { return _halfedge; }
    unsigned int indices[3];
    long index;
    halfedge *_halfedge;
    mat4 Kp;
    bool valid;
    face() { valid = 1; }
    void output()
    {
        cout << "f " << index << endl;
    }
    bool operator==(const face &a)
    {
        return index == a.index;
    }
    /*计算每个面的矩阵Kp*/
    void getKp()
    {
        halfedge *ihe = _halfedge;
        vec3 v[3]; //面上的三个顶点向量
        for (int i = 0; i < 3; i++)
        {
            v[i] = vec3(ihe->_vertex->getposition());
            ihe = ihe->_next;
        }
        vec3 e10 = v[1] - v[0];
        vec3 e20 = v[2] - v[0];     //两条边向量
        vec3 norm = e10.cross(e20); //法向量
        norm.normalize();
        vec4 p = vec4(norm, (norm.dot(v[0])) * (-1)); //面的向量p
        Kp = mat4(p);
    }
};

class halfedgemesh
{
public:
    void output()
    {
        cout << "halfedges:" << endl;
        for (list<halfedge>::iterator it = halfedges.begin(); it != halfedges.end(); it++)
        {
            cout << it->index << endl;
            if (it->_vertex != NULL)
                (it->_vertex)->output();
            else
                cout << "NULL" << endl;

            if (it->_edge != NULL)
                (it->_edge)->output();
            else
                cout << "NULL" << endl;

            if (it->_face != NULL)
                (it->_face)->output();
            else
                cout << "NULL" << endl;

            if (it->_opposite != NULL)
                cout << (it->_opposite)->index << endl;
            else
                cout << "NULL" << endl;

            if (it->_next != NULL)
                cout << (it->_next)->index << endl;
            else
                cout << "NULL" << endl;

            cout << "-------------------------------------" << endl;
        }
    }
    halfedgemesh()
    {
        vertexindex = 1;
        edgeindex = 1;
        faceindex = 1;
        halfedgeindex = 1;
    }
    ~halfedgemesh()
    {
        halfedges.clear();
        vertexs.clear();
        edges.clear();
        faces.clear();
    }
    void ClearLists()
    {
        halfedges.clear();
        edges.clear();
        faces.clear();
        vertexs.clear();
    }
    void VertexsOutput()
    {
        for (list<vertex>::iterator t = vertexs.begin(); t != vertexs.end(); t++)
            t->output();
    }
    void halfedge_opposite();
    void createface(long *indexs);
    list<halfedge> halfedges;
    list<vertex> vertexs;
    list<edge> edges;
    list<face> faces;
    long vertexindex;
    long edgeindex;
    long faceindex;
    long halfedgeindex;
};

vertex *findvertex(list<vertex> *vertexs, long theindex)
{
    for (list<vertex>::iterator it = vertexs->begin(); it != vertexs->end(); it++)
        if (it->index == theindex)
            return &(*it);
    return NULL;
}
//根据两顶点索引在链表中寻找边的指针
edge *findedge(list<edge> *edges, long index1, long index2)
{
    //cout << "search edge (" << index1 << "," << index2 << ")" << endl;
    for (list<edge>::iterator it = edges->begin(); it != edges->end(); it++)
    {
        long x1 = it->getpoint1();
        long x2 = it->getpoint2();
        if (((x1 == index1) && (x2 == index2)) || ((x1 == index2) && (x2 == index1)))
        {
            //cout << "find edge:" << endl;
            //it->output();
            return &(*it);
        }
    }
    return NULL;
}

void halfedge::output()
{
    cout << "halfedge: " << index << endl;
    if (_vertex != NULL)
        _vertex->output();
    else
        cout << "NULL" << endl;

    if (_edge != NULL)
        _edge->output();
    else
        cout << "NULL" << endl;

    if (_face != NULL)
        _face->output();
    else
        cout << "NULL" << endl;

    if (_opposite != NULL)
        cout << _opposite->index << endl;
    else
        cout << "NULL" << endl;

    if (_next != NULL)
        cout << _next->index << endl;
    else
        cout << "NULL" << endl;

    cout << "-------------------------------------" << endl;
}

//collapse 此半边指向的边，并返回删除的面数.其中保留此半边指向的顶点作为新顶点
int halfedge::collapse()
{
//    output();
    int deleted_faces = 1;
    //对于两顶点，保存此半边指向的顶点，将其设为新顶点
    //更新位置与误差矩阵
//    _vertex->newposition=_edge->newposition;
    _vertex->setposition(_edge->newposition);
    _vertex->QuardError = _edge->Qbar;

    //删除另一顶点后将另一顶点关联的面的面片信息更新为保留的顶点
    vector<face*> fs=_opposite->_vertex->FindAllConnectfaces();
    for(vector<face*>::iterator it=fs.begin();it!=fs.end();it++)
    {
        for(int i=0;i<3;i++)
        {
            if((*it)->indices[i]==_opposite->_vertex->index)
                (*it)->indices[i]=_vertex->index;
        }
    }

        /*删除面、边、顶点、半边*/
    _face->valid = false;
    _next->_next->_edge->valid = false;
    _edge->valid = false;
    _opposite->_vertex->valid = false;
    valid = false;
    _next->valid = false;
    _next->_next->valid = false;


    /*将所有原本指向另一顶点的外部的半边的_vertex指向新的顶点，并将新顶点的指针_halfedge指向其中之一条半边*/
    _vertex->_halfedge=_next->_opposite;
    _opposite->_vertex->update(_vertex);

    /*将原本的外侧的半边的_opposite指针指向对面的边的外侧半边*/
    if (_next->_opposite != NULL)
        _next->_opposite->_opposite = _next->_next->_opposite;
    if (_next->_next->_opposite != NULL)
        _next->_next->_opposite->_opposite = _next->_opposite;

    /*确保边的_halfedge指针指向正确的半边——三角形外侧的两个半边之一*/
    if (_next->_opposite != NULL)
        _next->_edge->_halfedge = _next->_opposite;
    if(_next->_next->_opposite!=NULL)
        _next->_next->_opposite->_edge=_next->_edge;

    /*更新上侧顶点的_halfedge指针，不要指向三角形内部将要删除的半边*/
    _next->_vertex->_halfedge=_next->_next->_opposite;

    
    
    //若不是边缘半边，还需处理opposite半边
    if (_opposite != NULL)
    {
        deleted_faces++;

        _opposite->_face->valid = false;
        _opposite->_next->_edge->valid = false;
        _opposite->valid = false;
        _opposite->_next->valid = false;
        _opposite->_next->_next->valid = false;

        //更新opposite
        if(_opposite->_next->_opposite != NULL)
            _opposite->_next->_opposite->_opposite = _opposite->_next->_next->_opposite;
        if(_opposite->_next->_next->_opposite!=NULL)
            _opposite->_next->_next->_opposite->_opposite = _opposite->_next->_opposite;
        //更新边的halfedge指针
        if (_opposite->_next->_next->_opposite != NULL)
            _opposite->_next->_next->_edge->_halfedge = _opposite->_next->_next->_opposite;
        if (_opposite->_next->_opposite != NULL)
            _opposite->_next->_opposite->_edge =_opposite->_next->_next->_edge;

       //更新顶点的halfedge指针
        _opposite->_next->_vertex->_halfedge=_opposite->_next->_next->_opposite;


        

    }
    return deleted_faces;
}

//返回本半边指向的边是否会破坏图的拓扑结构
bool halfedge::CheckSafe()
{

    vertex *v1 = _vertex;
    vertex *v2 = _next->_next->_vertex;

    vector<vertex *> vs1 = v1->FindAllConnectvertexs();
    vector<vertex *> vs2 = v2->FindAllConnectvertexs();

    int joins = 0;
    for (vector<vertex *>::iterator i1 = vs1.begin(); i1 != vs1.end(); i1++)
    {
        for (vector<vertex *>::iterator i2 = vs2.begin(); i2 != vs2.end(); i2++)
        {
            if ((*i1) == (*i2))
                joins++;
        }
    }
    return joins == 4;
}
//返回所有指向此顶点的半边的指针
vector<halfedge*> vertex::FindAllLinkHalfEdges()
{
    set<halfedge*> shes;
    vector<halfedge*> hes;
    halfedge* he=_halfedge;
    shes.insert(he);
    while(he->_opposite->_next->_next != _halfedge)
    {
        he=he->_opposite->_next->_next;
        shes.insert(he);
    }
    for(set<halfedge*>::iterator it=shes.begin();it!=shes.end();it++)
        hes.push_back((*it));
    return hes;
}


vector<edge*> vertex::FindAllLinkEdges()
{
    vector<halfedge*> hes=FindAllLinkHalfEdges();
    set<edge*> es;
    for(vector<halfedge*>::iterator it=hes.begin();it!=hes.end();it++)
        es.insert((*it)->_edge);
    vector<edge*> ves;
    for(set<edge*>::iterator it=es.begin();it!=es.end();it++)
        ves.push_back((*it));
    return ves;
}


vector<vertex* >vertex::FindAllConnectvertexs()
{
    set<vertex*> vss;
    vector<vertex*> vs;
    halfedge* he=_halfedge;
    vs.push_back(he->_opposite->_vertex);
    while(he->_opposite->_next->_next!=_halfedge)
    {
        he=he->_opposite->_next->_next;
        vss.insert(he->_vertex);
        vss.insert(he->_opposite->_vertex);
    }
    for(set<vertex*>::iterator it=vss.begin();it!=vss.end();it++)
        vs.push_back(*it);
    return vs;
}
vector<face *> vertex::FindAllConnectfaces()
{
    vector<halfedge *> hfs = FindAllLinkHalfEdges();
    vector<face *> fs;
    for (vector<halfedge *>::iterator ie = hfs.begin(); ie != hfs.end(); ie++)
    {
        fs.push_back((*ie)->_face);
    }
    return fs;
}

void vertex::getQE()
{
    //为其相邻的所有面的Kp矩阵的和
    vector<face *> fs = FindAllConnectfaces();
    for (vector<face *>::iterator ifa = fs.begin(); ifa != fs.end(); ifa++)
        QuardError = QuardError + (*ifa)->Kp;
}

/*将此顶点关联的所有半边中储存的顶点值设为v*/
void vertex::update(vertex *v)
{
    halfedge* ih=_halfedge;
    do{
        ih->_vertex=v;
        ih=ih->_next->_opposite;
    }while(ih!=_halfedge);
}

//根据给出的三个顶点参数，创建面，半边，边，加入list
void halfedgemesh::createface(long *index)
{
    //创建半边，已知顶点与下一条半边，加入链表
    halfedge he1, he2, he3;
    list<halfedge>::iterator ih, ih1, ih2, ih3;

    //对于三个顶点构成的三条边，若其中已经有边存在了半边，就按与其相反的方向构建半边
    edge *e1 = findedge(&(edges), index[0], index[1]);
    edge *e2 = findedge(&(edges), index[1], index[2]);
    edge *e3 = findedge(&(edges), index[2], index[0]);

    bool FollowIncrease = true; //默认按照index0-2的顺序构建半边

    //若三条边中有边已经按照index0-2的顺序有了半边，就选择index2-0的顺序
    if (e1 != NULL && e1->_halfedge != NULL)
    {
        if (e1->_halfedge->_vertex->index == index[1])
        {
            FollowIncrease = false;
        }
    }
    if (e2 != NULL && e2->_halfedge != NULL && e2->_halfedge->_vertex->index == index[2])
        FollowIncrease = false;
    if (e3 != NULL && e3->_halfedge != NULL && e3->_halfedge->_vertex->index == index[0])
        FollowIncrease = false;

    //插入半边到链表
    if (FollowIncrease)
        he1._vertex = findvertex(&(vertexs), index[1]);
    else
        he1._vertex = findvertex(&(vertexs), index[0]);
    he1.index = halfedgeindex++;

//    halfedges.push_back(he1);
    halfedges.insert(halfedges.end(),halfedge(he1));
//    halfedges.insert(halfedges.end(),he1);
    //halfedges.push_front(he1);
    
    ih = find(halfedges.begin(), halfedges.end(), he1); //ih用于暂存指针位置
    ih1 = ih;
    if (FollowIncrease)
        he2._vertex = findvertex(&(vertexs), index[2]);
    else
        he2._vertex = findvertex(&(vertexs), index[1]);
    he2.index = halfedgeindex++;
    
    halfedges.push_back(he2);

    ih2 = ++ih;

    if (FollowIncrease)
        he3._vertex = findvertex(&(vertexs), index[0]);
    else
        he3._vertex = findvertex(&(vertexs), index[2]);
    he3.index = halfedgeindex++;
    halfedges.push_back(he3);
    ih3 = ++ih;

    //加入半边的next，同样需要考虑顺序
    if (FollowIncrease)
    {
        ih3->_next = &(*ih1);
        ih2->_next = &(*ih3);
        ih1->_next = &(*ih2);
    }
    else
    {
        ih3->_next = &(*ih2);
        ih2->_next = &(*ih1);
        ih1->_next = &(*ih3);
    }

    //更新顶点的半边指针
    (he1._vertex)->_halfedge = &(*ih1);
    (he2._vertex)->_halfedge = &(*ih2);
    (he3._vertex)->_halfedge = &(*ih3);

    //构建此面并插入链表
    face f;
    f.index = faceindex++;
    f._halfedge = &(*ih1);
    f.indices[0] = index[0];
    f.indices[1] = index[1];
    f.indices[2] = index[2];
    faces.push_back(f);
    list<face>::iterator ifa = find(faces.begin(), faces.end(), f);

    //为半边填入面指针
    ih1->_face = &(*ifa);
    ih2->_face = &(*ifa);
    ih3->_face = &(*ifa);

    //构建此面上的边，并在重复边不存在的情况下插入链表
    edge *ie1;
    edge *ie2;
    edge *ie3;
    edge ed1(index[0], index[1], edgeindex++);
    edge ed2(index[1], index[2], edgeindex++);
    edge ed3(index[2], index[0], edgeindex++);
    ed1._halfedge = &(*ih1);
    ed2._halfedge = &(*ih2);
    ed3._halfedge = &(*ih3);
    ie1 = uniquejoin(&(edges), ed1);
    ie2 = uniquejoin(&(edges), ed2);
    ie3 = uniquejoin(&(edges), ed3);

    //为半边插入边指针
    ih1->_edge = ie1;
    ih2->_edge = ie2;
    ih3->_edge = ie3;
}

/*若链表中不存在与本边的顶点相同的点就将本边插入链表，否则就不插入。但是都返回指向此边的指针*/
edge *uniquejoin(list<edge> *edges, edge &e)
{
    for (list<edge>::iterator it = edges->begin(); it != edges->end(); it++)
    {
        if (((it->point1 == e.point1) && (it->point2 == e.point2)) || ((it->point2 == e.point1) && (it->point1 == e.point2)))
        {
            return &(*it);
        }
    }
    edges->push_back(e);
    list<edge>::iterator ie = find(edges->begin(), edges->end(), e);
    return &(*ie);
}

//对于mesh中的所有halfedge，填上其opposite
void halfedgemesh::halfedge_opposite()
{
    for (list<halfedge>::iterator it = halfedges.begin(); it != halfedges.end(); it++)
    {
        if (it->_opposite != NULL)
        {
            continue;
        }
        for (list<halfedge>::iterator jt = halfedges.begin(); jt != halfedges.end(); jt++)
        {
            if ((it->_edge == jt->_edge) && (it->_vertex != jt->_vertex))
            {
                it->_opposite = &(*jt);
                jt->_opposite = &(*it);
                break;
            }
        }
    }
    //强行填上opposite
    for(list<halfedge>::iterator it=halfedges.begin();it!=halfedges.end();it++)
    {
        if(it->_opposite == NULL)
        {
            it->_opposite=&(*it);
        }
    }
}
//根据Q的一阶导数等于0的方程，计算出新顶点的最佳位置
//根据公式。若无法求解就返回错误
bool solve(mat4 Qbar, vec4 &Vbar)
{
    float b[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    for (int k = 0; k < 4; k++)
    {
        if (Qbar.matrix[k][k] < 0.00000001 && Qbar.matrix[k][k] > -0.00000001) //超出精度
            return false;
        for (int i = 0; i < 4; i++)
        {
            if (i == k)
                continue;
            float c = Qbar.matrix[i][k] / Qbar.matrix[k][k];
            for (int j = k + 1; j < 4; j++)
                Qbar.matrix[i][j] = Qbar.matrix[i][j] - c * Qbar.matrix[k][j];
            b[i] = b[i] - c * b[k];
        }
    }
    Vbar.x = b[0] / Qbar.matrix[0][0];
    Vbar.y = b[1] / Qbar.matrix[1][1];
    Vbar.z = b[2] / Qbar.matrix[2][2];
    Vbar.t = b[3] / Qbar.matrix[3][3];
    return true;
}
//计算QEM简化中每条边产生的误差最小的新顶点
void edge::GetNewPosition()
{
    mat4 _Qbar;
    vertex *v1 = _halfedge->_vertex;
    vertex *v2 = _halfedge->_next->_next->_vertex;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            _Qbar.matrix[i][j] = v1->QuardError.matrix[i][j] + v2->QuardError.matrix[i][j];
        }
    }
    _Qbar.matrix[3][0] = 0.0f;
    _Qbar.matrix[3][1] = 0.0f;
    _Qbar.matrix[3][2] = 0.0f;
    _Qbar.matrix[3][3] = 1.0f;
    vec4 Vbar;
    if (!solve(_Qbar, Vbar))
    {
        //cout << "can't solve" << endl;
        //如果不能求解，就使用中点作为最终结果
        Vbar.x = (v1->pos_x + v2->pos_x) / 2.0f;
        Vbar.y = (v1->pos_y + v2->pos_y) / 2.0f;
        Vbar.z = (v1->pos_z + v2->pos_z) / 2.0f;
        Vbar.t = 1.0f;
    }
    newposition = pos(Vbar.x, Vbar.y, Vbar.z); //设置边产生的新顶点位置
}
//计算边的cost
//Vbar(T) * Qbar * Vbar
//保存Qbar
void edge::GetCost()
{
    vertex *v1 = _halfedge->_vertex;
    vertex *v2 = _halfedge->_next->_next->_vertex;
    Qbar = v1->QuardError + v2->QuardError;

    float Vbar[4] = {newposition.x, newposition.y, newposition.z, 1.0f};
    float res = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res += Vbar[i] * Qbar.matrix[i][j] * Vbar[j];
    cost = res;
}
#endif
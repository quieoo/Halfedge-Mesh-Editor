/*
原来所有的顶点isnew设为0
计算所有原来的顶点的新位置，newposition
在所有边上计算新的中点的位置 newvertex
遍历原来的所有边，并对于每条边创建新的顶点，对于产生的三个顶点，创建三条新的边

对于新创建的边，若其两端分别为旧的和新的顶点

*/
/*
对于每个顶点，计算其新位置。将此新位置加入新mesh，更新原顶点中的newvertex项
对于每条边，计算新顶点，将此加入新mesh，更新边上的newvertex
对于每个面，创建四个新面，加入新mesh

*/
#include "HalfEdge.h"
#include <cmath>
float calculateAlpha(int n)
{
	float alpha;
	if (n > 3)
	{
		float center = (0.375f + (0.25f * cos((2.0f * 3.1415926f) / (float)n)));
		alpha = (0.625f - (center * center)) / (float)n;
	}
	else
	{
		alpha = 3.0f / 16.0f;
	}
	return alpha;
}

void subdivide(halfedgemesh &mesh, halfedgemesh &_mesh)
{
	//cout << "before subdivide:" << endl;
	//cout << "vertex number:" << mesh.vertexs.size() << endl;

	
	//cout << "face number:" << mesh.faces.size() << endl;
	//    halfedgemesh _mesh;
	//遍历所有的顶点，更新其新位置，并将其加入新mesh
	for (list<vertex>::iterator iv = mesh.vertexs.begin(); iv != mesh.vertexs.end(); iv++)
	{
		vertex newv;
		if (iv->isboundary())
		{
			//顶点不位于网格内部
			//v=3/4v+1/8v1+1/8v2
			//寻找其所有相邻的边中bondary的两条，取相对于他们的邻接点作为v1,v2
			vertex *v[2];
			int num = 0;
			/*vector<edge *> es = iv->FindAllLinkEdges();
			for (vector<edge *>::iterator ie = es.begin(); ie != es.end(); ie++)
			{
				if ((*ie)->isboundary())
				{
					if ((*ie)->_halfedge->_vertex == &(*iv))
						v[num++] = (*ie)->_halfedge->_next->_next->_vertex;
					else
						v[num++] = (*ie)->_halfedge->_vertex;
				}
			}*/
			v[0]=iv->_halfedge->_next->_vertex;
			v[1]=iv->_halfedge->_next->_next->_vertex;
			newv.pos_x = 3.0f / 4.0f * (iv->pos_x) + 1.0f / 8.0f * (v[0]->pos_x + v[1]->pos_x);
			newv.pos_y = 3.0f / 4.0f * (iv->pos_y) + 1.0f / 8.0f * (v[0]->pos_y + v[1]->pos_y);
			newv.pos_z = 3.0f / 4.0f * (iv->pos_z) + 1.0f / 8.0f * (v[0]->pos_z + v[1]->pos_z);
			newv.index = _mesh.vertexindex++;
			iv->newposition = pos(newv.pos_x, newv.pos_y, newv.pos_z);
			_mesh.vertexs.push_back(newv);
		}
		else
		{
			//顶点位于网格内部
			//v=(1-n@)v+@(v1+v2+v3....)邻接顶点
			vector<vertex *> vs = iv->FindAllConnectvertexs();
			float alpha = calculateAlpha(mesh.vertexs.size());
			pos newpos = (iv->getposition()) * (1.0f - mesh.vertexs.size() * alpha);
			pos _add;
			for (vector<vertex *>::iterator cvi = vs.begin(); cvi != vs.end(); cvi++)
			{
				_add = _add + ((*cvi)->getposition());
			}
			_add = _add * alpha;
			newpos = newpos + _add;
			newv.setposition(newpos);
			newv.index = _mesh.vertexindex++;
			iv->newposition = newpos;
			_mesh.vertexs.push_back(newv);
		}
	}

	//遍历所有边，为每一条边创建一个新顶点
	for (list<edge>::iterator ie = mesh.edges.begin(); ie != mesh.edges.end(); ie++)
	{
		vertex newv;
		pos newpos;
		if (ie->isboundary())
		{
			//若边不位于网格内部
			//v=1/2(v0+v1)
			vertex *v[2];
			v[0] = ie->_halfedge->_vertex;
			v[1] = ie->_halfedge->_next->_next->_vertex;
			newpos = (v[0]->getposition() + v[1]->getposition()) * (1.0f / 2.0f);
		}
		else
		{
			//若边位于网格内部
			//v=3/8(v0+v1)+1/8(v2+v3)
			vertex *v[4];
			v[0] = ie->_halfedge->_vertex;
			v[1] = ie->_halfedge->_opposite->_vertex;
			v[2] = ie->_halfedge->_next->_vertex;
			v[3] = ie->_halfedge->_opposite->_next->_vertex;
			newpos = (v[0]->getposition() + v[1]->getposition()) * (3.0f / 8.0f) +
						 (v[2]->getposition() + v[3]->getposition()) * (1.0f / 8.0f);
		}
		newv.index = _mesh.vertexindex++;
		newv.setposition(newpos);
		//newv.output();
		ie->newvertex = newv;
		_mesh.vertexs.push_back(newv);
	}
	//对于每个面，建立新的四个面
	for(list<face>::iterator ifa=mesh.faces.begin();ifa!=mesh.faces.end();ifa++)
	{
		long* index=new long[3];
		halfedge* ihe[3];
		ihe[0]=ifa->_halfedge;
		ihe[1]=ifa->_halfedge->_next;
		ihe[2]=ifa->_halfedge->_next->_next;

		//创建中间的面
		for(int i=0;i<3;i++)
			index[i]=ihe[i]->_edge->newvertex.index;
		_mesh.createface(index);

		//创建另外三个面
		for(int i=0;i<3;i++)
		{
			index[0]=ihe[i]->_edge->newvertex.index;
			index[1]=ihe[i]->_vertex->index;
			index[2]=ihe[i]->_next->_edge->newvertex.index;
			_mesh.createface(index);
		}

	}
	//对于新面中所有半边，添加其opposite
	_mesh.halfedge_opposite();
	cout<<"loop subdivide,get face:"<<_mesh.faces.size()<<endl;
}
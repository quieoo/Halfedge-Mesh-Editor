#include "HalfEdge.h"
#include <queue>
#include <utility>

struct edgewithcost
{
    float cost;
    edge *toedge;
    edgewithcost(edge *_e, float _c)
    {
        toedge = _e;
        cost = _c;
    }
    //因为默认是大根堆，因此重载小于号
    bool operator<(const edgewithcost &z) const
    {
        return cost > z.cost;
    }
};

void simplify(halfedgemesh &mesh, halfedgemesh &_mesh, float ratio = 0.5f)
{
    _mesh=mesh;
    int i=0;
    //cout<<i++<<" 次collap"<<endl;
    //计算图中所有顶点的误差矩阵
    for (list<face>::iterator ifa = mesh.faces.begin(); ifa != mesh.faces.end(); ifa++)
        ifa->getKp(); //首先计算每个面的Kp矩阵
    for (list<vertex>::iterator iv = mesh.vertexs.begin(); iv != mesh.vertexs.end(); iv++)
        iv->getQE(); //顶点的误差矩阵等于所有相邻面的Kp矩阵和

    //计算并保存每条边的cost与新位置
    for (list<edge>::iterator ie = mesh.edges.begin(); ie != mesh.edges.end(); ie++)
    {
        ie->GetNewPosition();
        ie->GetCost();
    }

    //创建优先队列（cost，edge*）
    priority_queue<edgewithcost> ErrorQueue;
    for (list<edge>::iterator ie = mesh.edges.begin(); ie != mesh.edges.end(); ie++)
    {
        edgewithcost e(&(*ie), ie->cost);
        ErrorQueue.push(e);
    }
    //迭代优先队列中cost最小的边，collapse相关的边(用设置valid来删除)并更新相关的cost，直到达到面要求
    int count = mesh.faces.size() * ratio;
    count=mesh.faces.size()-count;
    int DeletedFace = 0;
    while (DeletedFace < count)
    {
        if (!ErrorQueue.empty())
        {
            edge *ie = ErrorQueue.top().toedge; //取队列顶的边
            ErrorQueue.pop();
            if (ie->valid == true) //必须要求是未被删除的边
            {
                if (ie->dirty == false) //未被修改
                {
                    if (ie->_halfedge->CheckSafe()) //必须不能破坏拓扑结构
                    {
                        DeletedFace += ie->_halfedge->collapse();
                        //cout<<"collapse edge"<<endl;
                        //ie->output();
                        //cout<<"deletedface:"<<DeletedFace<<endl;
                        //cout<<i++<<" 次collap"<<endl;
                        //mesh.output();
                        //重新计算与此边有公共顶点的所有边的cost,并设置脏位
                        vertex* iv=ie->_halfedge->_vertex;
                        halfedge* ih=ie->_halfedge;
                        do{
                            ih->_edge->GetNewPosition();
                            ih->_edge->GetCost();
                            ih->_edge->dirty=true;

                            ih=ih->_next->_opposite;
                        }while(ih!=iv->_halfedge);

                    }
                    
                }
                else
                {
                    //此边在之前的过程中cost发生了变化,修改cost并重新插入队列
                    ie->dirty = false;
                    ErrorQueue.push(edgewithcost(ie, ie->cost));
                }
                
            }
            
        }
        else
        {
            //cout << "queue empty" << endl;
            break;
        } //没有足够的边
        
    }

    //复制mesh中valid为true的项到_mesh
    //cout<<"push vertexs"<<endl;
    /*for (list<vertex>::iterator iv = mesh.vertexs.begin(); iv != mesh.vertexs.end(); iv++)
    {
            _mesh.vertexs.push_back(vertex(iv->pos_x, iv->pos_y, iv->pos_z, iv->index));
            //iv->output();
    }
    //cout<<"push valid face"<<endl;
    for (list<face>::iterator iv = mesh.faces.begin(); iv != mesh.faces.end(); iv++)
    {
        if (iv->valid)
        {
            //cout<<"to create face"<<endl;
            long *index = new long[3];
            index[0] = iv->_halfedge->_vertex->index;
            index[1] = iv->_halfedge->_next->_vertex->index;
            index[2] = iv->_halfedge->_next->_next->_vertex->index;
            //cout<<index[0]<<" "<<index[1]<<" "<<index[2]<<endl;
            _mesh.createface(index);
        }
    }*/

    //删除invalid面
    //_mesh.faces.remove_if([](face f){return f.valid==0;});
    //cout<<"before erase"<<endl;
    for(list<face>::iterator it=mesh.faces.begin();it!=mesh.faces.end();)
    {
        if(!(it->valid))
            it=mesh.faces.erase(it);
        else
            it++;
    }
    cout<<"QEM simplify,get faces:"<<mesh.faces.size()<<endl;
}
# Halfedge-Edit-render
including halfedge mesh load, subdivide, simplify, and render with OPENGL.
As for now,this app is very simple,inefficient,and very possiblly wrong

usage
-
First，make sure GLFW is prepared,or rendering may not work.
```
g++ -o main glad.c stb_image.cpp main.cpp -lglfw3 -lpthread -lX11 -ldl -lGLEW
```
After that,main is generated.
```
./main input.obj
```
For now,only sportted obj file with "v" and "f".

halfedge
-
![](https://github.com/quieoo/Halfedge-Mesh-Editor/blob/master/halfedge.png)
A classic halfedge structure is like the picture above sys,which is from [OpenMesh](https://www.openmesh.org/media/Documentations/OpenMesh-6.3-Documentation/a00010.html).

But there is one thing to be noted.in my implementation ,all vertex pointer in halfedge class point to the vertex which point to it.
There are several methods need to be noted:
>halfedge* halfedge::CWgetNext().Find the next halfedge in clock wise ,which point to the same vertex.

>halfedge* halfedge::RCWgetNext().Find the next halfedge in reverse clock wise ,which point to the same vertex.

>void halfedge::collapse().collapse the edge halgedge point to.It will be introduced later.

>void halfedgemesh::createfaces(long* index).According to the vertex index,create a face,three edges(unique),three halfedges, and build their pointer to each other.

>void halfedgemesh::halfedge_opposite().For all the halfedge in mesh,fill their _opposite pointer.

>vector<face*> vertex::FindAllConnectface().

>vector<vertex*> vertex::FindAllConnectvertexs().

>vector<edge*> vertex::FindAllLinkEdges().

>vector<halfedge*> vertex::FindAllLinkHalfEdges()

>bool halfege::isBoundary().Return if _opposite is NULL.

>booll edge::isBoundary().Return if _halfedge->isBoundary().

>bool vertex::isBoundary().Return if FindAllConnnectvertex()==3.

Load
-
Build a halfedgemesh from *.obj file.
Those file and text process method are from [tinyobjloader](https://github.com/syoyo/tinyobjloader).And i only chose to take "v" and "f" data from a obj file.
For "v"line,build a vertex with position and index ,and push back to vertexs list.
For "f"line,call createface(long* index),with tree vertex index.
And finally ,call halfedge_opposite(),to cretea opposite pointer for all halfedge.

loop subdivide
-
> First,iterate through mesh::vertexs ,and get its new position.store new position in vertex.newposition.
>>If vertex.isboundary()
>>>newposition=3/4v+1/8(v1+v2),while v1,v2 share the same edge with v

>>else
>>>newposition=(1-n@)v+@(v1+v2+v3....).@is computed by calculateAlpha,and v1,v2,...are vertexs who connect with v.

>Secound Iterate through mesh::edges,generate newvertex for every edge.
>>if edge.isboundary()
>>>newpos=1/2(v0+v1).v0,v1 are two point for edge.

>>else
>>>newpos=3/8(v0+v1)+1/8(v2+v3).v2 and v3 are two of another point int two faces connect with this edge



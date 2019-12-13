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

>Third,Iterate through mesh.faces.create four new faces for every one.
>>for three vertex.newposition,and three edge.newvertex,call createfaces().

QEM simplify
-
Algorithm is from [Surface Simplification Using Quadric Error Metrics](https://www.cs.cmu.edu/~./garland/Papers/quadrics.pdf) by Michael Garland and Paul S. Heckbert

First,Compute the Qmatrices for all the initial vertices.

As defining error of vertex to the sum of squared distances to its connected faces,we get
![](https://github.com/quieoo/Halfedge-Mesh-Editor/blob/master/formula1.png)
where p=[a,b,c,d] represent the face defined by the equation ax+by+cz+d=0 and a^2+b^2+c^2=1.
since that,we can simply get p by compute its normal vector as [a,b,c] plus with normal vector multiply one of vexter.position() as d.
![](https://github.com/quieoo/Halfedge-Mesh-Editor/blob/master/formula2.png)
With the derivation above,we can now get Q matrix by sum all of its connnected face's Kp matrix,and Kp matrix is p*p^t.

Second,select all pair of vertexs(here we chose those pair connected to same edge),and compute a noewposition Vbar and Qbar for it.

We would like to choose a Vbar whose quadric error is smallest.since cost of collapse equal to Vbar*Qbar*Vbar^T,we only need to differentiate Qbar.So,we can get Vbar:
![](https://github.com/quieoo/Halfedge-Mesh-Editor/blob/master/formula3.png)
Than,what we need to do is to get Qbar.Qbar=(Q1+Q2)/2,where Q1,Q2 are two of linked vertex.QuardError.

Third,put all of edges eith cost in a priority queue,where the edge with smaller cost in top.

Last,every time we choose the first edge from pq,and call edge._halfedge->collapse().
But,there is one thing to be noted,only the edge with pair of vertexs whose shared number of  other vertex is 2 are safe.Others may cause the change of topologic 

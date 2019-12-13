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
A classic halfedge structure is like the picture above sys,which is from [OpenMesh](https://www.openmesh.org/media/Documentations/OpenMesh-6.3-Documentation/a00010.html)
But there is one thing to be noted.in my implementation ,all vertex pointer in halfedge class point to the vertex which point to it.
There are several methods need to be noted:
>halfedge::CWgetNext.Find the next halfedge in clock wise ,which point to the same vertex
>halfedge::RCWgetNext.Find the next halfedge in reverse clock wise ,which point to the same vertex


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

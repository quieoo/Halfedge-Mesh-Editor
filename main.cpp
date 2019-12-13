//#include "render_camera.h"
#include "render3.h"
#include "subdivide.h"
#include "QEMsimplify.h"
//#include "Loader.h"
//#include "render.h"

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "Needs input.obj\n"
              << std::endl;
    //return 0;
    //argv[1] = "cube.obj";
  }
  halfedgemesh mesh;
  halfedgemesh divided;
  halfedgemesh simplified;
  if (Load(mesh, argv[1]))
  {
    //vertex v=mesh.vertexs.front();
    //v.output();
    //vector<vertex*> vs=v.FindAllConnectvertexs();
    //for(vector<vertex*>::iterator it=vs.begin();it!=vs.end();it++)
      //(*it)->output();
    //mesh.output();
    subdivide(mesh, divided);
    simplify(mesh, simplified);
    render3(simplified, mesh, divided);
  }
}

//g++ -o main glad.c stb_image.cpp main.cpp -lglfw3 -lpthread -lX11 -ldl -lGLEW

// MAT201B: Final Project
// Instructor: Karl Yerkes
// Anthor:  Lu Liu (2017)

// Special thanks: Karl Yerkes

#ifndef __COMMON_STUFF__
#define __COMMON_STUFF__
#include <vector>

// define stuff that's common to both the simulator and renderer: definition of
// State and N.

#define MY_CUTTLEBONE_PORT (64537)
//#define PORT(65531)
#define N (1000)
#define MAXN (100000)
#define NUM (4000)
// N is the number of planes we cut the voronoi cell with
 // Set up constants for the container geometry
 const double x_min=-1,x_max=1;
 const double y_min=-1,y_max=1;
 const double z_min=-1,z_max=1;
 const double cvol=(x_max-x_min)*(y_max-y_min)*(x_max-x_min);

 // Set up the number of blocks that the container is divided into
 const int n_x=6,n_y=6,n_z=6;

struct State{
    int frame;
    Pose pose;
    Color background;
    //Vec3f vertex[MAXN];
    //int vertexCount;
    Vec3f Mesh_vertex[N * 300];
    Vec3f Wireframe_vertex[N * 30];
    float Wireframe_index[N * 100];
    int vertexCount_M[N];
    int vertexCount_wf[N];
    int indexCount_wf[N];
    //Vec4f Mesh_color[N];
    int numCells;
    int numVr;
    int sequence[N];
    int Stage;
    float Alpha[N];
    int temp_Lines;
    float Timer;
};



void fillContiner(container& con){
  double x, y, z;
  //Make a container to put points inside
  for(int i=0;i<N;i++) {
           x=x_min+rnd::uniform()*(x_max-x_min);
           y=y_min+rnd::uniform()*(y_max-y_min);
           z=z_min+rnd::uniform()*(z_max-z_min);
           con.put(i,x,y,z);
   }
}



#endif

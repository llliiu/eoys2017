// MAT201B: Final Project
// Instructor: Karl Yerkes
// Anthor:  Lu Liu (2017)

// Special thanks: Karl Yerkes

#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "allocore/al_Allocore.hpp"
#include "Cuttlebone/Cuttlebone.hpp"
#include "voro++/voro++.hh"
#include "voro_Lib_v2.cpp"

using namespace voro;
using namespace al;
using namespace std;


#include "common.hpp"
#include <set>
#include <vector>

struct AlloApp : OmniStereoGraphicsRenderer {

    Material material;
    cuttlebone::Taker<State, NUM ,MY_CUTTLEBONE_PORT > taker;
    State state;
    Light light;


    vector<Mesh> meshes_Mesh;
    vector<Mesh> meshes_Wframe;

    int idx;
    container con;
    int seq[N];
    int stage;
    int count;
    float alpha_Default[N];
    int showLines;
    float timer;

    AlloApp()
    : con(x_min,x_max,y_min,y_max,z_min,z_max,n_x,n_y,n_z,false,false,false,8)

    {
          pose.pos(0,0,0);
        light.pos(0, 0, 10);
      fillContiner(con);


      getCellsFromContainer(con,meshes_Mesh,2.5, false);
      getCellsFromContainer(con,meshes_Wframe,2.5, true);
      for(int i=0; i<N; i++){
        meshes_Mesh[i].generateNormals();
        meshes_Wframe[i].generateNormals();
      }

  initWindow();

    }

    virtual void onAnimate(double dt) {
    //  light.pos(0, 0, 10);
      taker.get(state);
      pose.set(state.pose);
      meshes_Mesh.clear();
      meshes_Wframe.clear();

      int vertex_M = 0;
      int vertex_wf = 0;
      int index_wf = 0;

      for (int i = 0; i < N; i++){
        meshes_Mesh.push_back(Mesh());
        meshes_Wframe.push_back(Mesh());

        for (int j = 0; j < state.vertexCount_M[i]; j++){
          meshes_Mesh[i].vertex(state.Mesh_vertex[vertex_M]);
          vertex_M++;
      }
        //  state.vertexCount[i]=meshes_Mesh[i].vertices().size();

        for (int j = 0; j < state.vertexCount_wf[i]; j++){
          //cout << state.Wireframe_vertex[index_wf]<< endl;
          meshes_Wframe[i].vertex(state.Wireframe_vertex[vertex_wf]);
          vertex_wf++;
      }

         for (int j = 0; j < state.indexCount_wf[i]; j++){
          meshes_Wframe[i].index(state.Wireframe_index[index_wf]);
          index_wf++;
    }

    }

      for(int i=0; i<N; i++){
        seq[i]=state.sequence[i];
        alpha_Default[i]=state.Alpha[i];
        //cout << state.sequence[i]<< endl;
      }
      idx=state.numCells;
      count=state.numVr;
      stage=state.Stage;
      showLines=state.temp_Lines;
      timer=state.Timer;


      for (auto& m : meshes_Mesh) {
        m.generateNormals();
      }
    }

    virtual void onDraw(Graphics& g) {
      light();
      material();
      shader().uniform("lighting", 0.2);


       g.blending(false);
      // g.blendModeTrans();

        //shader().uniform("lighting", 1.0);
        // g.clearColor(0.1, 0.1, 0.1, 1);
        // g.clear(Graphics::COLOR_BUFFER_BIT);

        if (stage == 0){
          if(showLines==1){
            for(int i = 0; i < idx; i++) {
                meshes_Wframe[seq[i]].primitive(Graphics::LINES);
                meshes_Wframe[seq[i]].color(0.1,0.4,1.0,1.0);
                g.draw(meshes_Wframe[seq[i]]);


          }}
          else{
        for(int i = 0; i < idx; i++) {
            meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
            meshes_Mesh[seq[i]].color(HSV(0.7, alpha_Default[i], 1.0));
            g.draw(meshes_Mesh[seq[i]]);

          }
        }
      }

      if(stage == 1){
        for(int i = count; i < idx; i++) {
            meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
            meshes_Mesh[seq[i]].color(Color(HSV(0.7, alpha_Default[i], 1.0)));

            //meshes_Mesh[seq[i]].colors()[0] = Color(alpha_Default[i], 0.07, 0.95 , alpha_Default[i]);
            g.draw(meshes_Mesh[seq[i]]);


          }
        for(int i = 0; i < count; i++) {
            meshes_Wframe[seq[i]].primitive(Graphics::LINES);
            meshes_Wframe[seq[i]].color(0.1,0.5,1.0,1.0);
            g.draw(meshes_Wframe[seq[i]]);

          }
        }

        if(stage==2){
          for(int i = 0; i < count; i++) {
              meshes_Wframe[seq[i]].primitive(Graphics::LINES);
              meshes_Wframe[seq[i]].color(0.1,0.5,1.0,1.0);
              //g.rotate(timer*0.005, 0, 1, 0 );
              g.draw(meshes_Wframe[seq[i]]);

            }
        }


              // if(stage == 2){
              //   for(int i = 0; i < count; i++) {
              //       meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
              //       meshes_Mesh[seq[i]].color(Color(HSV(alpha_Default[i], 1.0, 0.95),alpha_Default[i]));
              //       g.rotate(timer*0.1, 0, 1, 0 );
              //       g.draw(meshes_Mesh[seq[i]]);
              //
              //     }
              // }


        if(stage == 3){
          for(int i = count; i < idx; i++) {
            meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
            meshes_Mesh[seq[i]].color(Color(HSV(alpha_Default[i], 1.0, 0.95)));
            g.rotate(timer*0.005, 0, 1, 0 );
            //meshes_Mesh[seq[i]].colors()[0] = Color(alpha_Default[i], 0.07, 0.95 , alpha_Default[i]);
            g.draw(meshes_Mesh[seq[i]]);
          }
          for(int i = 0; i < count; i++) {
            meshes_Wframe[seq[i]].primitive(Graphics::LINES);
            meshes_Wframe[seq[i]].color(0.1,0.5,1.0,1.0);
            g.rotate(timer*0.005, 0, 1, 0 );
            g.draw(meshes_Wframe[seq[i]]);
          }
        }

        if(stage==4){
          for(int i = 0; i < idx; i++) {
              meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
              meshes_Mesh[seq[i]].color(Color(HSV(alpha_Default[i], 1.0, 0.95),alpha_Default[i]));
               g.rotate(timer*0.01, 0, 1, 0 );
               g.draw(meshes_Mesh[seq[i]]);
            }
        }



    }
};

int main() {
    AlloApp app;
    app.taker.start();
    app.start();

    }

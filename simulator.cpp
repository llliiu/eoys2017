// MAT201B: Final Project
// Instructor: Karl Yerkes
// Anthor:  Lu Liu (2017)

// Special thanks: Karl Yerkes


#include "allocore/al_Allocore.hpp"
#include "alloutil/al_Simulator.hpp"
#include "allocore/io/al_App.hpp"
#include "Cuttlebone/Cuttlebone.hpp"

#include "Gamma/Oscillator.h"
#include "Gamma/Effects.h"
#include "allocore/sound/al_Ambisonics.hpp"
#include "allocore/sound/al_Dbap.hpp"
#include "allocore/sound/al_Vbap.hpp"
#include "alloutil/al_AlloSphereAudioSpatializer.hpp"

#include "voro++/voro++.hh"
#include "voro_Lib_v2.cpp"
#include "common.hpp"

#include <set>
#include <vector>

using namespace al;
using namespace voro;
using namespace std;
using namespace gam;



 // Set the number of particles that are going to be randomly introduced

struct AlloApp : App , AlloSphereAudioSpatializer, InterfaceServerClient {
    SineDs<> src;
    SineDs<> src2;
    Accum<> tmr;
    Accum<> tmr2;
    Chorus<> chorus;
    SoundSource soundSource;
  //  Quatd srcQuat, destQuat;
    bool isSound;
    int soundSpeed;


    Material material;
    Light light;

    cuttlebone::Maker<State, NUM, MY_CUTTLEBONE_PORT> maker;
    State state;


    Mesh voronoi;

    vector<Mesh> meshes_Mesh;
    vector<Mesh> meshes_Wframe;

    float wallThickness=2.5;
    int idx;
    int status;
    container con;
    int seq[N];
    float timer = 0;
    int stage = 0;
    int count = 0;
    float alpha_Default[N];
    float alpha_changing[N];
    int showLines;

    AlloApp()
    : con(x_min,x_max,y_min,y_max,z_min,z_max,n_x,n_y,n_z,false,false,false,8),
    maker(Simulator::defaultBroadcastIP()),
    InterfaceServerClient(Simulator::defaultInterfaceServerIP())
    {

        state.frame = 0;

        fillContiner(con);

         //set the sequence of voronoi cells show up
         float cnt= al::rnd::uniform()*N;
         seq[0]=cnt;
         for(int i=0; i<N; i++){
           float cnt= al::rnd::uniform()*N;
           for(int j=0; j<i;j++){
            if (seq[j]==cnt) break;
            seq[i]=cnt;
            }
           }


           for(int i=0; i<N; i++){
             float cnt_alpha= al::rnd::uniform();
             alpha_Default[i]= cnt_alpha;
       }

       for(int i=0; i<N; i++){
         float cnt_alphaChanging= al::rnd::uniform(0.0005,0.007);
         alpha_changing[i]= cnt_alphaChanging;
   }

        // for(int i=0; i<N; i++){
        //   alpha_changing[i] = 0.004;
        // }
        tmr.freq(2.1);

        src.resize(N);

        tmr.finish();

        for(int i = 0; i < N; i++) {
          src.set(i, 0, 0, 0);
        }

        AlloSphereAudioSpatializer::initAudio();
        AlloSphereAudioSpatializer::initSpatialization();
        gam::Sync::master().spu(AlloSphereAudioSpatializer::audioIO().fps());

        scene()->addSource(soundSource);
        scene()->usePerSampleProcessing(false);
        //listener()->
        soundSource.dopplerType(DOPPLER_NONE);

        isSound= false;

        state.frame = 0;


        getCellsFromContainer(con,meshes_Mesh,wallThickness, false);
        getCellsFromContainer(con,meshes_Wframe,wallThickness, true);
        for(int i=0; i<N; i++){
          meshes_Mesh[i].generateNormals();
          meshes_Wframe[i].generateNormals();
        }



        nav().pos(0,0,0);
        light.pos(0,0,100);

        initWindow();
        InterfaceServerClient::setNav(nav());
        InterfaceServerClient::setLens(lens());

        //cout << 'here' << endl;
    }

    // float changeAlpha(float alpha){
    //       alpha=(int)(alpha*100+1)%100/100.0;
    //
    //       return alpha;
    //       alpha+=0.05;
    //         cout<< alpha<< endl;
    //        }

    void onAnimate(double dt) {
        timer+=dt;

        meshes_Mesh.clear();
        meshes_Wframe.clear();
        getCellsFromContainer(con,meshes_Mesh,wallThickness, false);
        getCellsFromContainer(con,meshes_Wframe,wallThickness, true);



        // Setting transparency of voro cells
        for(int i= 0; i<N; i++){
          alpha_Default[i] += alpha_changing[i];
          if ((alpha_Default[i]<= 0.3 && alpha_changing[i] < 0) || (alpha_Default[i] >= 1 && alpha_changing[i] >0))
          {
            alpha_changing [i]= -1 * alpha_changing[i];
          }
          //  state.Alpha[i]=alpha_Default[i];
        }

        // Animaiton
       if (timer <63){
         //nav().faceToward(Vec3d(10.0,10.0,20.0),0.01);
         stage = 0;
         // single cell appears
         if (timer < 3){
            idx= 0;}
         else if (timer<8) {
           soundSpeed=0;
           isSound=true;
           idx=1;
           if (timer>5 && timer <6){
             showLines = 1;
             isSound=false;
           }
           else { showLines = 0;
             isSound=true;

           }
         }
         // cells continuously appear one by one
         else{
          soundSpeed=1;
           if (status % 8== 0 ){
             if (idx< N*0.9) idx+=2;
           }
           if ((timer>15 && timer <16 )||(timer>23 && timer <24 )||(timer>30 && timer <31 )||(timer>37 && timer <38 )||(timer>43 && timer <44 )){
             showLines = 1;
             isSound=false;
           }
           else { showLines = 0;
             isSound=true;
           }
         }
         state.temp_Lines=showLines;
       }
       else if (timer < 110){
         if(wallThickness>1.004) {wallThickness-=0.001;}
         isSound=false;
          stage = 1;
          if (status % 4== 0 ){
          if(count<N*0.9) count++;
        }
      }
      //   //nav().move(sin(timer)*50,cos(timer)*50,0.0);
      else {
        if(wallThickness<1.006) {wallThickness+=0.001;}
        stage = 3;
        if(count>0){
            if (status % 4== 0 ){
            count--;
          }
        }
        // else{
        //       stage = 4;
        // }

      }

      int vertex_M = 0;
      int vertex_wf = 0;
      int index_wf = 0;

      for (int i = 0; i < N; i++){
        state.vertexCount_M[i] = meshes_Mesh[i].vertices().size();
        state.vertexCount_wf[i] = meshes_Wframe[i].vertices().size();
        state.indexCount_wf[i] = meshes_Wframe[i].indices().size();

       //cout << state.vertexCount_wf[i]<< endl;

        for (int j = 0; j < state.vertexCount_M[i]; j++){

          state.Mesh_vertex[vertex_M] = meshes_Mesh[i].vertices()[j];
           //cout << state.vertexCount[i]<< endl;
          vertex_M++;
      }
       for (int j = 0; j < state.vertexCount_wf[i]; j++){
        state.Wireframe_vertex[vertex_wf] = meshes_Wframe[i].vertices()[j];
         //cout << meshes_Wframe[i].vertices()[j]<< endl;
          vertex_wf++;
        }

    for (int j = 0; j < state.indexCount_wf[i]; j++){
       state.Wireframe_index[index_wf] = meshes_Wframe[i].indices()[j];
       index_wf++;
     }
 }

      for(int i=0; i<N; i++){
        state.sequence[i]=seq[i];
        state.Alpha[i]=alpha_Default[i];
         //cout << state.sequence[i]<< endl;
      }

      state.numCells=idx;
      state.numVr=count;
      state.Stage= stage;
      state.Timer=timer;

      state.pose = nav();
      maker.set(state);
      state.frame++;

      status++;

    }

    void onDraw(Graphics& g) {

      //  g.blending(true);
      //  g.blendModeTrans();
        // voronoi.primitive(Graphics::TRIANGLES);
        g.clearColor(0.1, 0.1, 0.1, 1);
        g.clear(Graphics::COLOR_BUFFER_BIT);

        material();
        light();

        if (stage == 0){
          if(showLines==1){
            for(int i = 0; i < idx; i++) {
                meshes_Wframe[seq[i]].primitive(Graphics::LINES);
                meshes_Wframe[seq[i]].colors()[0] = Color(0.1,0.4,1.0,1.0);
                g.draw(meshes_Wframe[seq[i]]);

          }}
          else{
        for(int i = 0; i < idx; i++) {
            meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
            meshes_Mesh[seq[i]].colors()[0] = Color(HSV(0.7, alpha_Default[i], 1.0));
            g.draw(meshes_Mesh[seq[i]]);
          }
        }
      }

      if(stage == 1){
        for(int i = count; i < idx; i++) {
            meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
            meshes_Mesh[seq[i]].colors()[0] = Color(HSV(0.7, alpha_Default[i], 1.0));

            //meshes_Mesh[seq[i]].colors()[0] = Color(alpha_Default[i], 0.07, 0.95 , alpha_Default[i]);
            g.draw(meshes_Mesh[seq[i]]);
          }
        for(int i = 0; i < count; i++) {
            meshes_Wframe[seq[i]].primitive(Graphics::LINES);
            meshes_Wframe[seq[i]].colors()[0] = Color(0.1,0.5,1.0,1.0);
            g.draw(meshes_Wframe[seq[i]]);

          }
        }

      // if(stage==2){
      //   for(int i = 0; i < count; i++) {
      //       meshes_Wframe[seq[i]].primitive(Graphics::LINES);
      //       meshes_Wframe[seq[i]].colors()[0] = Color(0.1,0.5,1.0,1.0);
      //       //g.rotate(timer*0.005, 0, 1, 0 );
      //       g.rotate(timer*0.005, 0, 1, 0 );
      //
      //       g.draw(meshes_Wframe[seq[i]]);
      //
      //     }
      // }


      if(stage == 3){
        for(int i = count; i < idx; i++) {
          meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
          meshes_Mesh[seq[i]].colors()[0] = Color(HSV(alpha_Default[i], 1.0, 0.95));
          //meshes_Mesh[seq[i]].colors()[0] = Color(alpha_Default[i], 0.07, 0.95 , alpha_Default[i]);
          g.draw(meshes_Mesh[seq[i]]);
        }
        for(int i = 0; i < count; i++) {
          meshes_Wframe[seq[i]].primitive(Graphics::LINES);
          meshes_Wframe[seq[i]].colors()[0] = Color(0.1,0.5,1.0,1.0);
          //g.rotate(timer*0.005, 0, 1, 0 );
          g.draw(meshes_Wframe[seq[i]]);
        }
      }
      //
      // if(stage==4){
      //   for(int i = 0; i < idx; i++) {
      //       meshes_Mesh[seq[i]].primitive(Graphics::TRIANGLES);
      //       meshes_Mesh[seq[i]].colors()[0] = Color(HSV(alpha_Default[i], 1.0, 0.95),alpha_Default[i]);
      //        g.rotate(timer*0.005, 0, 1, 0 );
      //        g.draw(meshes_Mesh[seq[i]]);
      //     }
      // }


    }
    virtual void onSound(al::AudioIOData& io) {
      while (io()) {
              if(isSound){
      if(soundSpeed==0) tmr.freq(0.2);
      if(soundSpeed==1) tmr.freq(1.9);

                if (tmr()){
            // for(int i=0; i<N; i++){

                  float freq =70.0+ al::rnd::uniform(1.50, 5.5)*100;
                  float amp = 0.8;
                  float decayLength;
                  if(soundSpeed==0) decayLength = 7.0f;
                  if(soundSpeed==1) decayLength = 1.0f;
                  //sine[i].freq(freq);
                  src.set(idx, freq, amp, decayLength);
                  soundSource.pos(meshes_Mesh[seq[idx]].vertices()[0].x, meshes_Mesh[seq[idx]].vertices()[0].y, meshes_Mesh[seq[idx]].vertices()[0].z);

          // }

        }
      }

	    	float s = 0;

	    	s = src() * 0.1;
    		s += chorus(s) * 0.2;
        soundSource.writeSample(s);


	    }


	    listener()->pose(nav().pos());

	    scene()->render(io);

    //   wallThickness++;
    }
};

int main() {
  AlloApp app;
  app.AlloSphereAudioSpatializer::audioIO().start();
  app.maker.start();
  app.start();
}

// // Voronoi Library for AlloSystem
//
// // Author: Lu Liu (2017)
//
// // Original Author: Chris H. Rycroft (LBL / UC Berkeley)(Voro++ Library)(2017)
//
// // Reference: openFrameworks' ofxVoro addon code found here:
// //   https://github.com/patriciogonzalezvivo/ofxVoro/blob/master/src/ofxVoro.cpp
//
// // Special thanks: Karl Yerkes, Keehong Youn


#include "allocore/al_Allocore.hpp"
#include "voro++/voro++.hh"
using namespace voro;
using namespace al;
using namespace std;
#include <set>
#include <vector>

void getCellVertices(Mesh& m,voronoicell& v){
    m.primitive(Graphics::TRIANGLES);

    double* raw = v.pts;
    for (int i = 0; i < v.p; i++, raw += 3) {
        Vec3f point;
        point.x = raw[0];  // * 0.5;
        point.y = raw[1];  // * 0.5;
        point.z = raw[2];  // * 0.5;
        m.vertex(point);
    }
}
//typedef unsigned __int128 uint128_t;

void getCellMesh(Mesh& m, voronoicell& v, bool isWireframe){
    if(v.p){
        if (!isWireframe){
            Mesh temp_mesh;
            temp_mesh.primitive(Graphics::TRIANGLES);
            getCellVertices(temp_mesh, v);
            //Add triangles using indeces
            int k, l, p, q;
            for(int i = 1;i< v.p; i++) {
                for(int j = 0; j< v.nu[i]; j++){
                  //  Vec3f trians;
                    k = v.ed[i][j];
                    if (k<0) continue;

                    //if( k >= 0 ) {
                        v.ed[i][j]=-1-k;
                        l = v.cycle_up( v.ed[i][ v.nu[i]+j], k);
                        p = v.ed[k][l];
                        v.ed[k][l]=-1-p;

                        while(p!=i) {
                            q = v.cycle_up( v.ed[k][ v.nu[k]+l],p);

                            temp_mesh.index(i);
                            temp_mesh.index(k);
                            temp_mesh.index(p);

                            k=p;
                            l=q;
                            p=v.ed[k][l];
                            v.ed[k][l]=-1-p;

                    }
                }
            }

            m.primitive(Graphics::TRIANGLES);
            m.reset();

            //Color color_ = Color(0.0,1.0,1.0,rnd::uniform(0.7));
            //Calculate Normals
             for (int i=0; i< temp_mesh.indices().size(); i+=3){

               auto a = temp_mesh.vertices()[temp_mesh.indices()[i]];
               auto b = temp_mesh.vertices()[temp_mesh.indices()[i+1]];
               auto c = temp_mesh.vertices()[temp_mesh.indices()[i+2]];

               auto nor = ((b-a).cross(c-a)).normalize() * -1.0;

               m.vertex(a);
               m.normal(nor);
               //m.color(color_);
               m.vertex(b);
               m.normal(nor);
               //m.color(color_);
               m.vertex(c);
               m.normal(nor);
               //m.color(color_);

               if (i%2==0){
                 m.texCoord(0.0, 0.0);
                 m.texCoord(0.0, 1.0);
                 m.texCoord(1.0, 0.0);
               }
               else{
                 m.texCoord(1.0, 0.0);
                 m.texCoord(0.0, 1.0);
                 m.texCoord(1.0, 1.0);
               }
             }

        }
        else{
            m.primitive(Graphics::LINES);
            getCellVertices(m, v);
          //  m.triFace();
            set<uint64_t> longset;
            for (int i = 1; i < v.p; i++) {
              //  for (int j = 0; j <3; j++) {
                for (int j = 0; j < v.nu[i]; j++) {
                    int k = v.ed[i][j];
                    if (k<0) continue;
                    uint64_t a = i, b = k;
                    uint64_t q = (a << 32) + b, p = a + (b << 32);
                    if (longset.count(q))
                    continue;
                    else
                    longset.insert(q);
                    if (longset.count(p))
                    continue;
                    else
                    longset.insert(p);

                    //if (k >= 0) {
                        m.index(i);
                        m.index(k);
                    }
                //}
              }
        }
    }
}

// Mesh getCellmesh(Mesh& m, voronoicell& v, bool isWireframe ){
//   if( v.p ) {
//     Mesh mesh;
//     getCellMesh(mesh, v, isWireframe);
//     return mesh;
// }
//     return Mesh();
// }

void getCellsFromContainer(container& con, Mesh& m, float wallsThickness, bool isWireframe){
    m.reset();

    c_loop_all vl( con );
    int i = 0;
	  if( vl.start() ){

        do {
            voronoicell c;
            if( !con.compute_cell(c, vl) ) {
                return;
            } else {
                double *pp = con.p[vl.ijk] + con.ps * vl.q;
                Mesh n;
                getCellMesh(n, c , isWireframe);
                n.translate(Vec3f(pp[0],pp[1],pp[2])*(float)(1.0+wallsThickness));

                m.merge(n);
                i++;
            }

        } while( vl.inc() );
    }
}

void getCellsFromContainer(container& con, std::vector<Mesh>& v, float wallsThickness, bool isWireframe){

    c_loop_all vl( con );
    int i = 0;
	  if( vl.start() ){
        do {
            voronoicell c;
            if( !con.compute_cell(c, vl) ) {
                return;
            } else {
                double *pp = con.p[vl.ijk] + con.ps * vl.q;
                Mesh n;
                getCellMesh(n, c , isWireframe);
                n.translate(Vec3f(pp[0],pp[1],pp[2])*(float)(1.0+wallsThickness));
                n.color(0.1,0.3,0.8,0.2);
                v.push_back(n);
                // m.merge(n);
                i++;
            }
        } while( vl.inc() );
    }
}


void addSphere(voronoicell& v, int N) {
  double x, y, z, rsq, r;
  // Initialize the Voronoi cell to be a cube of side length 2, centered
  // on the origin
  v.init(-1, 1, -1, 1, -1, 1);
  // Cut the cell by N random planes which are all a distance 1 away
  // from the origin, to make an approximation to a sphere
  for (int i = 0; i < N; i++) {
      x = 2 * rnd::uniform() - 1;
      y = 2 * rnd::uniform() - 1;
      z = 2 * rnd::uniform() - 1;
      rsq = x * x + y * y + z * z;
      if (rsq > 0.01 && rsq < 1) {
          r = 1 / sqrt(rsq);
          x *= r;
          y *= r;
          z *= r;
          v.plane(x, y, z, 1);
      }
  }

}

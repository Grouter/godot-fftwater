#ifndef OCEAN_H
#define OCEAN_H

#include <core/Godot.hpp>
#include <Reference.hpp>
#include <Ref.hpp>
#include <Vector2.hpp>
#include <MeshDataTool.hpp>
#include <MeshInstance.hpp>

#include <complex>
#include <random>

//#include "fft.h"
#include <fft.hpp>
#include <fft_settings.h>

/*struct vertex_ocean {
    float   x,   y,   z; // vertex
    float  nx,  ny,  nz; // normal
    float   a,   b,   c; // htilde0
    float  _a,  _b,  _c; // htilde0mk conjugate
    float  ox,  oy,  oz; // original position
};*/

namespace godot {
    class Tessendorf : public Reference {
        GODOT_CLASS(Tessendorf, Reference)
        private:
            /* Ocean simulation */
            float seed;
            float g;
            float amplitude;
            float wind_speed;
            float length;
            Vector2 wind_direction;
            std::complex<float> **htilde, **htilde_slopex, **htilde_slopez, **htilde_dx, **htilde_dz;
            std::complex<float> **gss;
            //vertex_ocean *vertices;
            unsigned int N, Nplus1;
            /* Gaussian random */
            std::default_random_engine gen;
            std::normal_distribution<float> dist;
            /* Varticies */
            Ref<MeshDataTool> mdt;

            float dispersion(Vector2 K);
            float phillips(Vector2 K);
            std::complex<float> gaussian();
            std::complex<float> h0_tilde(Vector2 K, int n, int m);
            std::complex<float> h_tilde(Vector2 K, int n, int m, float t);
        public:
            static void _register_methods();

            Tessendorf();
            ~Tessendorf();

            void _init();
            
            void update(float t);
            //float get_h(int m, int n);
            void set_mdt(Ref<MeshDataTool> mr);

            /* DEBUG */
            //Vector2 dh_tilde(Vector2 K, float time);
    };

}

#endif
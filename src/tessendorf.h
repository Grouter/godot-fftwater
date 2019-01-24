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
            float g;
            float amplitude;
            float wind_speed;
            float length;
            Vector2 wind_direction;
            unsigned int N, Nplus1;
            std::complex<float> **htilde;
            std::complex<float> **gss;
            
            /* Gaussian random */
            std::default_random_engine gen;
            std::normal_distribution<float> dist;
            
            std::complex<float> gaussian();
            float phillips(Vector2 K);
            std::complex<float> h0_tilde(Vector2 K, int n, int m);
            float dispersion(Vector2 K);
            std::complex<float> h_tilde(Vector2 K, int n, int m, float t);
        public:
            static void _register_methods();

            Tessendorf();
            ~Tessendorf();

            void _init();
            
            void update(float t, Ref<MeshDataTool> mdt);
    };

}

#endif
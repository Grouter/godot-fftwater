#ifndef OCEAN_H
#define OCEAN_H

#include <core/Godot.hpp>
#include <Reference.hpp>
#include <Ref.hpp>
#include <Vector2.hpp>
#include <MeshDataTool.hpp>
#include <ShaderMaterial.hpp>

#include <complex>
#include <random>

#include <fft.hpp>

namespace godot {
    class Tessendorf : public Reference {
        GODOT_CLASS(Tessendorf, Reference)
        private:
            /* Ocean simulation */
            float g;
            float length;
            unsigned int N, Nplus1;
            std::complex<float> **htilde;
            std::complex<float> **gauss;
            
            /* Gaussian random */
            std::default_random_engine gen;
            std::normal_distribution<float> dist;
            
            std::complex<float> gaussian();
            float phillips(Vector2 K);
            std::complex<float> h0_tilde(Vector2 K, int n, int m);
            float dispersion(Vector2 K);
            std::complex<float> h_tilde(Vector2 K, int n, int m, float t);
        public:
            float amplitude;
            float wind_speed;
            Vector2 wind_direction;

            static void _register_methods();

            Tessendorf();
            ~Tessendorf();

            void _init();
            
            void update(float t, Ref<MeshDataTool> mdt, Ref<ShaderMaterial> material);
    };

}

#endif
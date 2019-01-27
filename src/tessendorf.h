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
            unsigned int N;
            std::complex<float> **htilde;
            std::complex<float> **dx;
            std::complex<float> **dz;
            std::complex<float> **h0tk;
            std::complex<float> **h0tmk;
            
            /* Gaussian random */
            std::default_random_engine gen;
            std::normal_distribution<float> dist;
            
            /* Precalculation */
            std::complex<float> gaussian();
            float phillips(Vector2 K);
            std::complex<float> h0_tilde(Vector2 K);

            /* Realtime calculations */
            float dispersion(Vector2 K);
            std::complex<float> h_tilde(Vector2 K, int n, int m, float t);
        public:
            float amplitude;
            float wind_speed;
            float lambda;
            float smoothing;
            Vector2 wind_direction;

            static void _register_methods();

            Tessendorf();
            ~Tessendorf();

            void _init();
            
            void init(int freq_size);
            void calculate();
            void update(float t, Ref<MeshDataTool> mdt, Ref<ShaderMaterial> material);
    };

}

#endif
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

#include <fftw3.h>

namespace godot {
    class Tessendorf : public Reference {
        GODOT_CLASS(Tessendorf, Reference)
        private:
            /* Ocean simulation */
            double g;
            unsigned int N, Nsq;
            std::complex<double> *htilde, *htilde_i;
            std::complex<double> *dx, *dx_i;
            std::complex<double> *dz, *dz_i;
            std::complex<double> *h0tk;
            std::complex<double> *h0tmk;
            fftw_plan p_htilde, p_dx, p_dz;
            
            /* Gaussian random */
            std::default_random_engine gen;
            std::normal_distribution<double> dist;
            
            /* Precalculation */
            std::complex<double> gaussian();
            double phillips(Vector2 K);
            std::complex<double> h0_tilde(Vector2 K);

            /* Realtime calculations */
            double dispersion(Vector2 K);
            std::complex<double> h_tilde(Vector2 K, int index, double t);
        public:
            /* Ocean simulation tweaking variables */
            double amplitude;
            double wind_speed;
            double length;
            double lambda;
            double smoothing;
            Vector2 wind_direction;

            static void _register_methods();

            Tessendorf();
            ~Tessendorf();

            void _init();
            
            void init(int freq_size);
            void calculate();
            void update(double t, Ref<MeshDataTool> mdt, Ref<ShaderMaterial> material);
    };

}

#endif
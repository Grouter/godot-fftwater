#ifndef OCEAN_H
#define OCEAN_H

#include <core/Godot.hpp>
#include <Reference.hpp>
#include <Ref.hpp>
#include <Vector2.hpp>
#include <ShaderMaterial.hpp>
#include <String.hpp>
#include <Image.hpp>
#include <ImageTexture.hpp>

#include <complex>
#include <random>

#include <fftw3.h>

#define TWO_PI (2.0 * M_PI)

namespace godot {
    class Tessendorf : public Reference {
        GODOT_CLASS(Tessendorf, Reference)
        private:
            /* Ocean simulation */
            const double g = 9.81;
            const double norm = 10000.0; // scaling down displacement values
            const double signs[2] = { 1.0, -1.0 };

            ImageTexture *htex = ImageTexture::_new();

            unsigned int N = 1.0, Nsq;
            std::complex<double> *htilde, *htilde_i;
            std::complex<double> *dx, *dx_i;
            std::complex<double> *dz, *dz_i;
            std::complex<double> *h0tk;
            std::complex<double> *h0tmk;
            fftw_plan p_htilde, p_dx, p_dz;

            int get_index(int m, int n);
            
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
            void update_x();
            void update_y(double time);
            void update_z();
        public:
            /* Ocean simulation tweaking variables */
            double amplitude = 15.0;
            double wind_speed = 31.0;
            double length = 500.0;
            double lambda = -0.5;
            double smoothing = 2.0;
            Vector2 wind_direction = Vector2(1.0, 0.0);

            static void _register_methods();

            Tessendorf();
            ~Tessendorf();

            void _init();
            
            void create(int freq_size);
            void calculate();
            void update(double t);
            void send_displacement(Ref<ShaderMaterial> material, String uniform_name);
    };

}

#endif
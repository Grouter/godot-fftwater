#include "tessendorf.h"
#include <Vector3.hpp>
#include <MeshDataTool.hpp>
#include <Image.hpp>
#include <ImageTexture.hpp>
#include <thread>

using namespace godot;
using namespace std;

void Tessendorf::_register_methods() {
    register_method("create", &Tessendorf::create);
    register_method("calculate", &Tessendorf::calculate);
    register_method("update", &Tessendorf::update);
    register_method("send_displacement", &Tessendorf::send_displacement);

    register_property("amplitude", &Tessendorf::amplitude, 5.0);
    register_property("wind_speed", &Tessendorf::wind_speed, 31.0);
    register_property("length", &Tessendorf::length, 500.0);
    register_property("lambda", &Tessendorf::lambda, -0.5);
    register_property("smoothing", &Tessendorf::smoothing, 2.0);
    register_property("wind_direction", &Tessendorf::wind_direction, Vector2(1.0, 0.0));
}

Tessendorf::Tessendorf() {}

Tessendorf::~Tessendorf() {
    if (htilde)
        delete [] htilde;

    if (h0tk)
        delete [] h0tk;

    if (h0tmk)
        delete [] h0tmk;

    if (dx)
        delete [] dx;

    if (dz)
        delete [] dz;

    fftw_destroy_plan(p_htilde);
    fftw_destroy_plan(p_dx);
    fftw_destroy_plan(p_dz);
}

/**
 * Godot init function
 * - initialize default variables
 * - initialize variables that gdscript won't touch
*/
void Tessendorf::_init() {
    dist = normal_distribution<double>(0.0, 1.0);
}

/**
 * Create arrays and plans
 * - define N -> frequency resolution
 * - initialize arrays and fftw plans for FFT
*/
void Tessendorf::create(int freq_size) {
    N = freq_size;
    Nsq = N * N;

    //himg->create(N, N, false, Image::FORMAT_RGBF);

    htilde_i = new complex<double>[Nsq];
    htilde = new complex<double>[Nsq];
    dx = new complex<double>[Nsq];
    dx_i = new complex<double>[Nsq];
    dz = new complex<double>[Nsq];
    dz_i = new complex<double>[Nsq];
    h0tk = new complex<double>[Nsq];
    h0tmk = new complex<double>[Nsq];

    p_htilde = fftw_plan_dft_2d(
        N, N, reinterpret_cast<fftw_complex*>(htilde_i), reinterpret_cast<fftw_complex*>(htilde), FFTW_BACKWARD, FFTW_ESTIMATE
    );

    p_dx = fftw_plan_dft_2d(
        N, N, reinterpret_cast<fftw_complex*>(dx_i), reinterpret_cast<fftw_complex*>(dx), FFTW_BACKWARD, FFTW_ESTIMATE
    );

    p_dz = fftw_plan_dft_2d(
        N, N, reinterpret_cast<fftw_complex*>(dz_i), reinterpret_cast<fftw_complex*>(dz), FFTW_BACKWARD, FFTW_ESTIMATE
    );
}

/**
 * Calculate and store h0_tilde(K) and h0_tilde(-K)
 * - used when ocean simulation variables are
 * changed
*/
void Tessendorf::calculate() {
    int index;
    double kx, kz;

    // prevent zero division and weird behavior
    if (length <= 0.0)
        length = 1.0;

    for (int m = 0; m < N; m++) {
        kz = TWO_PI * (m - N / 2.0) / length;
        for (int n = 0; n < N; n++) {
            kx = TWO_PI * (n - N / 2.0) / length;
            index = m * N + n;

            h0tk[index] = h0_tilde(Vector2(kx, kz));
            h0tmk[index] = h0_tilde(-Vector2(kx, kz));
        }
    }
}

complex<double> Tessendorf::gaussian() {
    complex<double> ret(dist(gen), dist(gen));
    return ret;
}

double Tessendorf::phillips(Vector2 K) {
    double L = wind_speed * wind_speed / g;
    double kl = K.length();
    kl = (kl < 0.0001) ? 0.0001 : kl;
    double dt = K.normalized().dot(wind_direction.normalized());
    double kl2 = kl * kl;
    double powdt = dt * dt * dt * dt;   // more dt multiplications causes better wind direction alignment

    return amplitude * exp(-1.0 / (kl2 * L * L)) / (kl2 * kl2) * powdt * exp(-(kl * kl) * smoothing * smoothing);
}

complex<double> Tessendorf::h0_tilde(Vector2 K) {
    complex<double> gauss_pick = gaussian();
    return gauss_pick * (1.0 / sqrt(2.0)) * sqrt(phillips(K));
}

/* REAL TIME CALCULATIONS */

double Tessendorf::dispersion(Vector2 K) {
    return sqrt(g * K.length());
}

complex<double> Tessendorf::h_tilde(Vector2 K, int index, double time) {
    double omegakt = dispersion(K) * time;

    complex<double> rot(cos(omegakt), sin(omegakt));
    complex<double> roti(rot.real(), -rot.imag());
    return h0tk[index] * rot + conj(h0tmk[index]) * roti;
}

int Tessendorf::get_index(int m, int n) {
    return m * N + n;
}

void Tessendorf::update_x() {
    int index;
    double kx, kz, klen;

    for (int m = 0; m < N; m++) {
        kz = TWO_PI * (m - N / 2.0) / length;
        
        for (int n = 0; n < N; n++) {
            kx = TWO_PI * (n - N / 2.0) / length;
            klen = sqrt(kx * kx + kz * kz);
            index = get_index(m, n);

            if (klen < 0.000001) {
                dx_i[index] = complex<double>(0.0, 0.0);
            } 
            else {
                dx_i[index] = htilde_i[index] * complex<double>(0.0, -kx / klen);
            }
        }
    }

    fftw_execute(p_dx);

    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            index = get_index(m, n);

            dx[index] *= (double)signs[(n + m) & 1] * lambda / norm;
        }
    }
}

void Tessendorf::update_y(double time) {
    int index;
    double kx, kz;

    for (int m = 0; m < N; m++) {
        kz = TWO_PI * (m - N / 2.0) / length;
        
        for (int n = 0; n < N; n++) {
            kx = TWO_PI * (n - N / 2.0) / length;
            index = get_index(m, n);

            htilde_i[index] = h_tilde(Vector2(kx, kz), index, time);
        }
    }

    fftw_execute(p_htilde);

    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            index = get_index(m, n);

            htilde[index] *= (double)signs[(n + m) & 1] / norm;
        }
    }
}

void Tessendorf::update_z() {
    int index;
    double kx, kz, klen;

    for (int m = 0; m < N; m++) {
        kz = TWO_PI * (m - N / 2.0) / length;
        
        for (int n = 0; n < N; n++) {
            kx = TWO_PI * (n - N / 2.0) / length;
            klen = sqrt(kx * kx + kz * kz);
            index = get_index(m, n);

            if (klen < 0.000001) {
                dz_i[index] = complex<double>(0.0, 0.0);
            } 
            else {
                dz_i[index] = htilde_i[index] * complex<double>(0.0, -kz / klen);
            }
        }
    }

    fftw_execute(p_dz);

    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            index = get_index(m, n);

            dz[index] *= (double)signs[(n + m) & 1] * lambda / norm;
        }
    }
}

/**
 * Calculate x, y, z displacement with fft
*/
void Tessendorf::update(double time) {
    thread cx(&Tessendorf::update_x, this);
    thread cy(&Tessendorf::update_y, this, time);
    thread cz(&Tessendorf::update_z, this);

    cx.join();
    cy.join();
    cz.join();
}

/**
 * Send displacement texture to shader
 * @uniform_name is the name of uniform variable in shader, that
 * is used as diplacement map
 * 
 * float texture RGB = XYZ displacement
*/
void Tessendorf::send_displacement(Ref<ShaderMaterial> material, String uniform_name) {
    Image *himg = Image::_new();
    himg->create(N, N, false, Image::FORMAT_RGBF);
    
    himg->lock();
    
    int index;
    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            index = m * N + n;

            himg->set_pixel(n, m, Color(
                dx[index].real(),       // X
                htilde[index].real(),   // Y
                dz[index].real()        // Z
            ));
        }
    }

    himg->unlock();

    htex->create_from_image(himg);
    material->set_shader_param(uniform_name, htex);
}
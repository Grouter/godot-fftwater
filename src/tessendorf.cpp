#include "tessendorf.h"
#include <Vector3.hpp>
#include <MeshDataTool.hpp>
#include <Image.hpp>
#include <ImageTexture.hpp>

using namespace godot;
using namespace std;

void Tessendorf::_register_methods() {
    register_method("init", &Tessendorf::init);
    register_method("calculate", &Tessendorf::calculate);
    register_method("update", &Tessendorf::update);

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

void Tessendorf::_init() {
    g = 9.81;

    amplitude = 15.0;
    wind_speed = 31.0;
    length = 500.0;
    lambda = -0.5;
    smoothing = 2.0;
    wind_direction = Vector2(1.0, 0.0);

    dist = normal_distribution<double>(0.0, 1.0);
}

void Tessendorf::init(int freq_size) {
    N = freq_size;
    Nsq = N * N;

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

void Tessendorf::calculate() {
    int index;
    double kx, kz;
    for (int m = 0; m < N; m++) {
        kz = 2.0 * M_PI * (m - N / 2.0) / length;
        for (int n = 0; n < N; n++) {
            kx = 2.0 * M_PI * (n - N / 2.0) / length;
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
    double dt6 = dt * dt * dt * dt * dt * dt;

    return amplitude * exp(-1.0 / (kl2 * L * L)) / (kl2 * kl2) * dt6 * exp(-(kl * kl) * smoothing * smoothing);
}

complex<double> Tessendorf::h0_tilde(Vector2 K) {
    complex<double> gauss_pick = gaussian();
    return gauss_pick * (1.0 / sqrt(2.0)) * sqrt(phillips(K));
}

double Tessendorf::dispersion(Vector2 K) {
    return sqrt(g * K.length());
}

complex<double> Tessendorf::h_tilde(Vector2 K, int index, double time) {
    double omegakt = dispersion(K) * time;

    complex<double> rot(cos(omegakt), sin(omegakt));
    complex<double> roti(rot.real(), -rot.imag());
    return h0tk[index] * rot + conj(h0tmk[index]) * roti;
}

Vector3 Tessendorf::update(double time, Ref<MeshDataTool> mdt, Ref<ShaderMaterial> material) {
    int index;

    double kx, kz, klen;
    for (int m = 0; m < N; m++) {
        kz = 2.0 * M_PI * (m - N / 2.0) / length;
        for (int n = 0; n < N; n++) {
            kx = 2.0 * M_PI * (n - N / 2.0) / length;
            klen = sqrt(kx * kx + kz * kz);
            index = m * N + n;

            htilde_i[index] = h_tilde(Vector2(kx, kz), index, time);

            if (klen < 0.000001) {
                dx_i[index] = complex<double>(0.0, 0.0);
                dz_i[index] = complex<double>(0.0, 0.0);
            } else {
                dx_i[index] = htilde_i[index] * complex<double>(0.0, -kx / klen);
                dz_i[index] = htilde_i[index] * complex<double>(0.0, -kz / klen);
            }
        }
    }

    fftw_execute(p_htilde);
    fftw_execute(p_dx);
    fftw_execute(p_dz);

    int sign;
    double signs[] = { 1.0, -1.0 };
    Image *himg = Image::_new();
    himg->create(N, N, false, Image::FORMAT_RGBF);
    himg->lock();
    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            sign = signs[(n + m) & 1];
            index = m * N + n;

            htilde[index] *= (double)sign / 10000.0;
            dx[index] *= (double)sign * lambda / 10000.0;
            dz[index] *= (double)sign * lambda / 10000.0;

            himg->set_pixel(n, m, Color(
                dx[index].real(),
                htilde[index].real(),
                dz[index].real()
            ));
        }
    }

    ImageTexture *htex = ImageTexture::_new();
    htex->create_from_image(himg);
    material->set_shader_param("height_map", htex);

    return Vector3(
        dx[index].real() * lambda,
        htilde[index].real(),
        dz[index].real() * lambda
    );
}
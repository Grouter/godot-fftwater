#include "tessendorf.h"
#include <Vector3.hpp>
#include <MeshDataTool.hpp>
#include <Image.hpp>
#include <ImageTexture.hpp>

using namespace godot;
using namespace std;

void Tessendorf::_register_methods() {
    register_method("update", &Tessendorf::update);

    register_property("amplitude", &Tessendorf::amplitude, 5.0f);
    register_property("wind_speed", &Tessendorf::wind_speed, 31.0f);
    register_property("wind_direction", &Tessendorf::wind_direction, Vector2(1.0f, 0.0f));
}

Tessendorf::Tessendorf() {}

Tessendorf::~Tessendorf() {
    if (htilde) {
        for(int i = 0; i < N; i++)
            delete [] htilde[i];
        delete [] htilde;
    }

    if (gauss) {
        for(int i = 0; i < N; i++)
            delete [] gauss[i];
        delete [] gauss;
    }
}

void Tessendorf::_init() {
    g = 9.81f;
    amplitude = 15.0f;
    wind_speed = 31.0f;
    length = 500.0f;
    wind_direction = Vector2(1.0f, 0.0f);

    N = 128;
    Nplus1 = N + 1;

    htilde = new complex<float>*[N];
    gauss = new complex<float>*[N];
    for(int i = 0; i < N; i++) {
        htilde[i] = new complex<float>[N];
        gauss[i] = new complex<float>[N];
        for(int j = 0; j < N; j++) {
            gauss[i][j] = gaussian();
        }
    }
    
    dist = normal_distribution<float>(0.0f, 1.0f);
}

complex<float> Tessendorf::gaussian() {
    complex<float> ret(dist(gen), dist(gen));
    return ret;
}

float Tessendorf::phillips(Vector2 K) {
    float L = wind_speed * wind_speed / g;
    float kl = K.length();
    kl = (kl < 0.0001f) ? 0.0001f : kl;
    float dt = K.normalized().dot(wind_direction.normalized());
    float kl2 = kl * kl;
    float l = pow(10, -4) * L;

    return amplitude * exp(-1.0f / (kl2 * L * L)) / (kl2 * kl2) * pow(dt, 6) * exp(-(kl * kl) * l * l);
}

complex<float> Tessendorf::h0_tilde(Vector2 K, int n, int m) {
    complex<float> gauss_pick = gauss[n][m];
    return gauss_pick * (1.0f / sqrt(2.0f)) * sqrt(phillips(K));
}

float Tessendorf::dispersion(Vector2 K) {
    return sqrt(g * K.length());
}

complex<float> Tessendorf::h_tilde(Vector2 K, int n, int m, float time) {
    float omegakt = dispersion(K) * time;

    complex<float> rot(cos(omegakt), sin(omegakt));
    complex<float> roti(rot.real(), -rot.imag());
    return h0_tilde(K, n, m) * rot + conj(h0_tilde(-K, m, n)) * roti;
}

void Tessendorf::update(float time, Ref<MeshDataTool> mdt, Ref<ShaderMaterial> material) {
    int index;

    float kx, kz;
    for (int m = 0; m < N; m++) {
        kz = 2.0f * M_PI * (m - N / 2.0f) / length;
        for (int n = 0; n < N; n++) {
            kx = 2.0f * M_PI * (n - N / 2.0f) / length;
            htilde[n][m] = h_tilde(Vector2(kx, kz), n, m, time);
        }
    }

    const char * error = NULL;
    simple_fft::IFFT(htilde, htilde, N, N, error);

    int sign;
    float signs[] = { 1.0f, -1.0f };
    Image *himg = Image::_new();
    himg->create(N, N, false, Image::FORMAT_RGBF);
    himg->lock();
    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            sign = signs[(n + m) & 1];
            htilde[n][m] *= (float)sign;
            
            himg->set_pixel(n, m, Color(
                htilde[n][m].real(),
                htilde[n][m].real(),
                htilde[n][m].real()
            ));
        }
    }

    ImageTexture *htex = ImageTexture::_new();
    htex->create_from_image(himg);
    material->set_shader_param("height_map", htex);
}
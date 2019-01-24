#include "tessendorf.h"
#include <Vector3.hpp>
#include <MeshDataTool.hpp>
#include <math.h>

using namespace godot;
using namespace std;

void Tessendorf::_register_methods() {
    register_method("update", &Tessendorf::update);
    //register_method("get_h", &Tessendorf::get_h);
    register_method("set_mdt", &Tessendorf::set_mdt);
    //register_method("randm", &Tessendorf::randm);
    /*register_property<Tessendorf, float>("amplitude", &Tessendorf::amplitude, 10.0);
    register_property<Tessendorf, float>("wind_speed", &Tessendorf::wind_speed, 10.0);
    register_property<Tessendorf, float>("length", &Tessendorf::length, 512.0);*/
    //aaregister_property<Tessendorf, Vector2>("wind_direction", &Tessendorf::wind_direction, Vector2(1.0, 0.0));
}

Tessendorf::Tessendorf() {}

Tessendorf::~Tessendorf() {
    if (htilde) delete [] htilde;
    /*if (h_tilde_slopex) delete [] h_tilde_slopex;
    if (h_tilde_slopez) delete [] h_tilde_slopez;
    if (h_tilde_dx) delete [] h_tilde_dx;
    if (h_tilde_dz) delete [] h_tilde_dz;*/
    //if (vertices) delete [] vertices;
}

void Tessendorf::_init() {
    seed = 1.0f;
    g = 9.81f;
    amplitude = 100.0f;
    wind_speed = 31.0f;
    wind_direction = Vector2(1, 0);
    length = 128.0f;

    N = 128;
    Nplus1 = N + 1;

    htilde = new complex<float>*[N];
    htilde_slopex = new complex<float>*[N];
    htilde_slopez = new complex<float>*[N];
    htilde_dx  = new complex<float>*[N];
    htilde_dz = new complex<float>*[N];
    gss = new complex<float>*[N];
    for(int i = 0; i < N; i++) {
        htilde[i] = new complex<float>[N];
        htilde_slopex[i] = new complex<float>[N];
        htilde_slopez[i] = new complex<float>[N];
        htilde_dx[i] = new complex<float>[N];
        htilde_dz[i] = new complex<float>[N];
        gss[i] = new complex<float>[N];
        for(int j = 0; j < N; j++) {
            gss[i][j] = gaussian();
        }
    }
    

    dist = normal_distribution<float>(0.0f, 1.0f);
}

void Tessendorf::set_mdt(Ref<MeshDataTool> mr) {
    mdt = mr;
}

float Tessendorf::phillips(Vector2 K) {
    float L = pow(wind_speed, 2) / g;
    float kl = K.length();
    kl = (kl < 0.0001f) ? 0.0001f : kl;
    float dt = K.normalized().dot(wind_direction.normalized());
    float kl2 = kl * kl;
    float l = pow(10, -4) * L;

    return amplitude * exp(-1.0f / (kl2 * L * L)) / (kl2 * kl2) * (dt * dt) * exp(-(kl * kl) * l * l);
}

complex<float> Tessendorf::gaussian() {
    /*random_device rd{};
    mt19937 gen{rd()};*/
    complex<float> ret(dist(gen), dist(gen));
    return ret;
}

complex<float> Tessendorf::h0_tilde(Vector2 K, int n, int m) {
    complex<float> gauss = gss[n][m];
    return gauss * (1.0f / sqrt(2.0f)) * sqrt(phillips(K));
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

void Tessendorf::update(float time) {
    float kx, kz, len;
    int index1;

    for (int m_prime = 0; m_prime < N; m_prime++) {
        kz = 2.0f * M_PI * (m_prime - N / 2.0f) / length;
        for (int n_prime = 0; n_prime < N; n_prime++) {
            kx = 2.0f * M_PI * (n_prime - N / 2.0f) / length;
            len = sqrt(kx * kx + kz * kz);
            htilde[n_prime][m_prime] = h_tilde(Vector2(kx, kz), n_prime, m_prime, time);
            //htilde_slopex[n_prime][m_prime] = htilde[n_prime][m_prime] * complex<float>(0.0f, kx);
            //htilde_slopez[n_prime][m_prime] = htilde[n_prime][m_prime] * complex<float>(0.0f, kz);
        }
    }
    
    const char * error = NULL;
    simple_fft::IFFT(htilde, htilde, N, N, error);
    //simple_fft::IFFT(htilde_slopex, htilde_slopex, N, N, error);
    //simple_fft::IFFT(htilde_slopez, htilde_slopez, N, N, error);
    /*simple_fft::IFFT(htilde_dx, htilde_dx, N, N, error);
    simple_fft::IFFT(htilde_dz, htilde_dz, N, N, error);*/

    int sign;
    float signs[] = { 1.0f, -1.0f };
    Vector3 vert;
    for (int m_prime = 0; m_prime < N; m_prime++) {
        kz = 2.0f * M_PI * (m_prime - N / 2.0f) / length;
        for (int n_prime = 0; n_prime < N; n_prime++) {
            kx = 2.0f * M_PI * (n_prime - N / 2.0f) / length;
            index1 = m_prime * Nplus1 + n_prime;    // index into vertices
            sign = signs[(n_prime + m_prime) & 1];
            
            htilde[n_prime][m_prime] *= (float)sign;
            //htilde_slopex[n_prime][m_prime] *= (float)sign;
            //htilde_slopez[n_prime][m_prime] *= (float)sign;

            mdt->set_vertex_color(index1, Color(
                htilde[n_prime][m_prime].real(),
                htilde[n_prime][m_prime].real(),
                htilde[n_prime][m_prime].real()
            ));

            //vert = mdt->get_vertex(index1);

            /* NORMAL */

            /*float hL = htilde[n_prime][m_prime - 1 < 0 ? 0 : m_prime - 1].real();
            float hR = htilde[n_prime][m_prime + 1 >= N ? N-1 : m_prime + 1].real();
            float hU = htilde[n_prime - 1 < 0 ? 0 : n_prime - 1][m_prime].real();
            float hD = htilde[n_prime + 1 >= N ? N-1 : n_prime + 1][m_prime].real();

            Vector3 nor = Vector3(
                hL - hR,
                2.0f,
                hD - hU
            ).normalized();

            /*Vector3 nor = Vector3(
                0.0f - htilde_slopex[n_prime][m_prime].real(),
                1.0f,
                0.0f - htilde_slopez[n_prime][m_prime].real()
            ).normalized();
            mdt->set_vertex_normal(index1, nor);*/

            /* DISPLACEMENT */
            /*htilde_dx[n_prime][m_prime] *= (float)sign;
            htilde_dz[n_prime][m_prime] *= (float)sign;
            vert.x += htilde_dx[n_prime][m_prime].real() * -1.0f;
            vert.z += htilde_dz[n_prime][m_prime].real() * -1.0f;*/

            /* HEIGHT */
            /*vert.y = htilde[n_prime][m_prime].real();
            mdt->set_vertex(index1, vert);*/
        }
    }
}
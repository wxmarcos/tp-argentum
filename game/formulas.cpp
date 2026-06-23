#include "formulas.h"

#include <cmath>
#include <random>

static std::mt19937& rng() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

// ------------- Vida y Mana -------------
int Formulas::calcularVidaMax(int constitucion, float fClaseVida,
                              float fRazaVida, int nivel) {
    return static_cast<int>(constitucion * fClaseVida * fRazaVida * nivel);
}

int Formulas::calcularManaMax(int inteligencia, float fClaseMana,
                              float fRazaMana, int nivel) {
    return static_cast<int>(inteligencia * fClaseMana * fRazaMana * nivel);
}

// ------------- Recuperacion pasiva -------------
float Formulas::calcularRecuperacionVida(float fRazaRecuperacion, float dt) {
    return fRazaRecuperacion * dt;
}

float Formulas::calcularRecuperacionMana(float fRazaRecuperacion, float dt) {
    return fRazaRecuperacion * dt;
}

float Formulas::calcularRecuperacionManaMeditando(float fClaseMeditacion,
                                                  int inteligencia, float dt) {
    return fClaseMeditacion * inteligencia * dt;
}

//------------- Combate - Ataque -------------
int Formulas::calcularDanio(int fuerza, int danioMin, int danioMax) {
    if (danioMin > danioMax) return 0;
    int rango = danioMax - danioMin + 1;
    int tirada =
        danioMin + std::uniform_int_distribution<int>(0, rango - 1)(rng());
    return fuerza * tirada;
}

bool Formulas::calcularEsquive(int agilidad, double umbral) {
    double r = std::uniform_real_distribution<double>(0.0, 1.0)(rng());
    return std::pow(r, agilidad) < umbral;
}

bool Formulas::calcularCritico(int porcentaje) {
    return std::uniform_int_distribution<int>(0, 99)(rng()) < porcentaje;
}

// Combate - Defensa
static int tiradaRango(int minVal, int maxVal) {
    if (minVal > maxVal || (minVal == 0 && maxVal == 0)) return 0;
    int rango = maxVal - minVal + 1;
    return minVal + std::uniform_int_distribution<int>(0, rango - 1)(rng());
}

int Formulas::calcularDefensa(int armaduraMin, int armaduraMax, int escudoMin,
                              int escudoMax, int cascoMin, int cascoMax) {
    return tiradaRango(armaduraMin, armaduraMax) +
           tiradaRango(escudoMin, escudoMax) + tiradaRango(cascoMin, cascoMax);
}

// Experiencia
int Formulas::calcularLimiteExp(int nivel, double coeficiente,
                                double exponente) {
    return static_cast<int>(coeficiente * std::pow(nivel, exponente));
}
int Formulas::calcularExpAtaque(int danio, int nivelOtro, int nivelPropio,
                                int nivelOffset) {
    if (nivelPropio <= 0) return 0;
    int factor = std::max(nivelOtro - nivelPropio + nivelOffset, 0);
    return danio * factor;
}
int Formulas::calcularExpMatar(int vidaMaxOtro, int nivelOtro, int nivelPropio,
                               int nivelOffset, double factorExp) {
    if (nivelPropio <= 0) return 0;
    int factor = std::max(nivelOtro - nivelPropio + nivelOffset, 0);
    double r = std::uniform_real_distribution<double>(0.0, factorExp)(rng());
    return static_cast<int>(r * vidaMaxOtro * factor);
}

int Formulas::calcularExpPerdida(int expActual, int porcentaje) {
    return std::max(0, expActual * porcentaje / 100);
}

// Oro
int Formulas::calcularOroMax(int nivel, double coeficiente, double exponente) {
    return static_cast<int>(coeficiente * std::pow(nivel, exponente));
}
int Formulas::calcularOroDropNPC(int vidaMaxNPC, int divisor) {
    if (divisor <= 0) return 0;
    double r = std::uniform_real_distribution<double>(0.0, 1.0)(rng());
    return static_cast<int>(r * vidaMaxNPC / divisor);
}
int Formulas::calcularOroExceso(int oro, int oroMax) {
    if (oro <= oroMax) return 0;
    return oro - oroMax;
}

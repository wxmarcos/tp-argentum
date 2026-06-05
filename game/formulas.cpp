#include "formulas.h"

#include <cmath>
#include <cstdlib>

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
    int tirada = danioMin + (rand() % rango);
    return fuerza * tirada;
}

bool Formulas::calcularEsquive(int agilidad) {
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    return std::pow(r, agilidad) < 0.001;
}

bool Formulas::calcularCritico() {
    // TODO: leer porcentaje de Config cuando este disponible
    return (rand() % 100) < 20;
}

// Combate - Defensa
static int tiradaRango(int minVal, int maxVal) {
    if (minVal > maxVal || (minVal == 0 && maxVal == 0)) return 0;
    int rango = maxVal - minVal + 1;
    return minVal + (rand() % rango);
}

int Formulas::calcularDefensa(int armaduraMin, int armaduraMax, int escudoMin,
                              int escudoMax, int cascoMin, int cascoMax) {
    return tiradaRango(armaduraMin, armaduraMax) +
           tiradaRango(escudoMin, escudoMax) + tiradaRango(cascoMin, cascoMax);
}

// Experiencia
int Formulas::calcularLimiteExp(int nivel) {
    return static_cast<int>(1000.0 * std::pow(nivel, 1.8));
}
int Formulas::calcularExpAtaque(int danio, int nivelOtro, int nivelPropio) {
    if (nivelPropio <= 0) return 0;
    int factor = std::max(nivelOtro - nivelPropio + 10, 0);
    return danio * factor;
}
int Formulas::calcularExpMatar(int vidaMaxOtro, int nivelOtro,
                               int nivelPropio) {
    if (nivelPropio <= 0) return 0;
    int factor = std::max(nivelOtro - nivelPropio + 10, 0);
    double r = (static_cast<double>(rand()) / RAND_MAX) * 0.1;
    return static_cast<int>(r * vidaMaxOtro * factor);
}

// Oro
int Formulas::calcularOroMax(int nivel) {
    return static_cast<int>(100.0 * std::pow(nivel, 1.1));
}
int Formulas::calcularOroDropNPC(int vidaMaxNPC) {
    double r = 0.01 + (static_cast<double>(rand()) / RAND_MAX) *
                          (0.2 - 0.01);  // Entre 0.01 y 0.1
    return static_cast<int>(r * vidaMaxNPC);
}
int Formulas::calcularOroExceso(int oro, int oroMax) {
    if (oro <= oroMax) return 0;
    return oro - oroMax;
}

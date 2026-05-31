#include "formulas.h"

#include <cmath>
#include <cstdlib>

// ------------- Vida y Mana -------------
int Formulas::calcularVidaMax(int constitucion, float fClaseVida, float fRazaVida, int nivel) {
    return static_cast<int>(constitucion * fClaseVida * fRazaVida * nivel);
}

int Formulas::calcularManaMax(int inteligencia, float fClaseMana, float fRazaMana, int nivel) {
    return static_cast<int>(inteligencia * fClaseMana * fRazaMana * nivel);
}

// ------------- Recuperacion pasiva -------------
int Formulas::calcularRecuperacionVida(int fRazaRecuperacion, float dt) {
    return static_cast<int>(fRazaRecuperacion * dt);
}

int Formulas::calcularRecuperacionMana(int fRazaRecuperacion, float dt) {
    return static_cast<int>(fRazaRecuperacion * dt);
}

int Formulas::calcularRecuperacionManaMeditando(int fClaseMeditacion, int inteligencia, float dt) {
    return static_cast<int>(fClaseMeditacion * inteligencia * dt);
}

float Formulas::calcularRecuperacionVidaF(float fRazaRecuperacion, float dt) {
    return fRazaRecuperacion * dt;
}

float Formulas::calcularRecuperacionManaF(float fRazaRecuperacion, float dt) {
    return fRazaRecuperacion * dt;
}

float Formulas::calcularRecuperacionManaMeditandoF(float fClaseMeditacion, int inteligencia, float dt) {
    return fClaseMeditacion * inteligencia * dt;
}

//------------- Combate - Ataque -------------
int Formulas::calcularDanio(int fuerza, float danioMin, float danioMax) {
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

int Formulas::calcularDefensa(int armaduraMin, int armaduraMax, int escudoMin, int escudoMax, int cascoMin, int cascoMax) {
    return tiradaRango(armaduraMin, armaduraMax) + tiradaRango(escudoMin, escudoMax) + tiradaRango(cascoMin, cascoMax);
}

// Experiencia
int Formulas::calcularLimiteExp(int nivel) {
    return static_cast<int>(1000.0 * std::pow(nivel, 1.8));
}
int Formulas::calcularExpAtaque(int danio, int nivelOtro, int nivelPropio) {
    if (nivelPropio <= 0) return 0;
    return static_cast<int>((danio * static_cast<double>(nivelOtro)) / nivelPropio);
}
int Formulas::calcularExpMatar(int vidaMaxOtro, int nivelOtro, int nivelPropio) {
    if (nivelPropio <= 0) return 0;
    return static_cast<int>((vidaMaxOtro * static_cast<double>(nivelOtro)) / nivelPropio);
}

// Oro
int Formulas::calcularOroMax(int nivel) {
    return static_cast<int>(100.0 * std::pow(nivel, 1.1));
}
int Formulas::calcularOroDropNPC(int vidaMaxNPC) {
    return vidaMaxNPC / 10;
}
int Formulas::calcularOroExceso(int oro, int oroMax) {
    if (oro <= oroMax) return 0;
    return oro - oroMax;
}
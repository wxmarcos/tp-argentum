#pragma once

class Formulas {
public:
    Formulas() = delete;

    // Vida y Mana
    static int calcularVidaMax(int constitucion, float fClaseVida, float fRazaVida, int nivel);
    static int calcularManaMax(int inteligencia, float fClaseMana, float fRazaMana, int nivel);

    // Recuperacion pasiva
    static int calcularRecuperacionVida(int fRazaRecuperacion, float dt);
    static int calcularRecuperacionMana(int fRazaRecuperacion, float dt);
    static int calcularRecuperacionManaMeditando(int fClaseMeditacion, int inteligencia, float dt);

    // Combate - Ataque
    static int calcularDanio(int fuerza, float danioMin, float danioMax);
    static bool calcularEsquive(int agilidad);
    static bool calcularCritico();

    // Combate - Defensa
    static int calcularDefensa(int armaduraMin, int armaduraMax, int escudoMin, int escudoMax, int cascoMin, int cascoMax);

    // Experiencia
    static int calcularLimiteExp(int nivel);
    static int calcularExpAtaque(int danio, int nivelOtro, int nivelPropio);
    static int calcularExpMatar(int vidaMaxOtro, int nivelOtro, int nivelPropio);

    // Oro
    static int calcularOroMax(int nivel);
    static int calcularOroDropNPC(int vidaMaxNPC);
    static int calcularOroExceso(int oro, int oroMax);
};
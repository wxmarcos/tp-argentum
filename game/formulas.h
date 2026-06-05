#pragma once

class Formulas {
public:
    Formulas() = delete;

    // Vida y Mana
    static int calcularVidaMax(int constitucion, float fClaseVida,
                               float fRazaVida, int nivel);
    static int calcularManaMax(int inteligencia, float fClaseMana,
                               float fRazaMana, int nivel);

    // Recuperacion pasiva
    static float calcularRecuperacionVida(float fRazaRecuperacion, float dt);
    static float calcularRecuperacionMana(float fRazaRecuperacion, float dt);
    static float calcularRecuperacionManaMeditando(float fClaseMeditacion,
                                                   int inteligencia, float dt);

    // Combate - Ataque
    static int calcularDanio(int fuerza, int danioMin, int danioMax);
    static bool calcularEsquive(int agilidad);
    static bool calcularCritico();

    // Combate - Defensa
    static int calcularDefensa(int armaduraMin, int armaduraMax, int escudoMin,
                               int escudoMax, int cascoMin, int cascoMax);

    // Experiencia
    static int calcularLimiteExp(int nivel);
    static int calcularExpAtaque(int danio, int nivelOtro, int nivelPropio);
    static int calcularExpMatar(int vidaMaxOtro, int nivelOtro,
                                int nivelPropio);

    // Oro
    static int calcularOroMax(int nivel);
    static int calcularOroDropNPC(int vidaMaxNPC);
    static int calcularOroExceso(int oro, int oroMax);
};

#pragma once
#include "character.h"
#include "../razas/raza.h"
#include "../clases/charClase.h"

class Jugador : public Character {
private:
    const Raza* raza;
    const charClase* clase;
    int manaActual;
    int manaMax;
    int nivel;
    int experiencia;
    int oro;
    // Inventario* inventario; // TODO: agregar cuando este implementado

    int expParaSiguienteNivel() const;
    void verificarSubidaNivel();

public:
    Jugador(const std::string& nombre, int posX, int posY,
            const Raza* raza, const charClase* clase);


    const Raza* getRaza() const;
    const charClase* getClase() const;

    int getManaActual() const;
    int getManaMax() const;
    void gastarMana(int cantidad);
    void recuperarMana(int cantidad);

    int getNivel() const;
    int getExperiencia() const;
    void ganarExperiencia(int exp);

    int getOro() const;
    void agregarOro(int cantidad);
    bool gastarOro(int cantidad);

    void recuperacionPasiva();
};
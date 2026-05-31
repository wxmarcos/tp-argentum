#pragma once
#include "characters/character.h"
#include "razas/raza.h"
#include "clases/charClase.h"
#include "items/inventario.h"

class Jugador : public Character {
private:
    const Raza* raza;
    const CharClase* clase;
    int constitucion;
    int inteligencia;
    int fuerza;
    int agilidad;
    int manaActual;
    int manaMax;
    int nivel;
    int experiencia;
    int oro;
    float vidaAcumulada;
    float manaAcumulado;
    bool meditando;
    Inventario inventario;

    void recalcularStats();
    int expParaSiguienteNivel() const;
    void verificarSubidaNivel();

public:
    Jugador(const std::string& nombre, int posX, int posY,
            const Raza* raza, const CharClase* clase);


    const Raza* getRaza() const;
    const CharClase* getClase() const;
    int getConstitucion() const;
    int getInteligencia() const;
    int getFuerza() const;
    int getAgilidad() const;

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

    void iniciarMeditacion();
    void interrumpirMeditacion();
    bool estaMeditando() const;

    bool agarrarItem(std::unique_ptr<Item> item, int cantidad = 1);
    std::optional<SlotInventario> soltarItem(int indice, int cantidad = -1);
    std::vector<SlotInventario> soltarTodosLosItems();

    bool equiparArma(int indice);
    bool equiparBaculo(int indice);
    bool equiparArmadura(int indice);
    bool equiparCasco(int indice);
    bool equiparEscudo(int indice);
    
    void desequiparArma();
    void desequiparArmadura();
    void desequiparCasco();
    void desequiparEscudo();

    bool usarPocion(int indice);

    const Inventario& getInventario() const;

    void recuperacionPasiva(float dt);

    void morir() override;
};
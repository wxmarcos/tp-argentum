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

    // Resurreccion
    bool resucitando;
    float tiempoResucitando;
    int destinoMapaId;
    int destinoPosX;
    int destinoPosY;

    // Cheats
    bool cheatVidaInfinita;
    bool cheatManaInfinito;

    void recalcularStats();
    int expParaSiguienteNivel() const;
    void verificarSubidaNivel();

public:
    Jugador(const std::string& nombre, int posX, int posY,
            const Raza* raza, const CharClase* clase,
            int capacidadInventario = 30);
            
    void restaurarEstado(
        int nivel, int vida, int vidaMax,
        int mana, int manaMax,
        int experiencia, int oro,
        int constitucion, int inteligencia,
        int fuerza, int agilidad);
    
    // Getters de raza/clase/stats
    const Raza* getRaza() const;
    const CharClase* getClase() const;
    int getConstitucion() const;
    int getInteligencia() const;
    int getFuerza() const;
    int getAgilidad() const;

    // Mana
    int getManaActual() const;
    int getManaMax() const;
    bool gastarMana(int cantidad);
    void recuperarMana(int cantidad);

    // Nivel y Experiencia
    int getNivel() const;
    int getExperiencia() const;
    void ganarExperiencia(int exp);

    // Oro
    int getOro() const;
    int getOroMax() const;
    void agregarOro(int cantidad);
    bool gastarOro(int cantidad);

    // Meditacion
    void iniciarMeditacion();
    void interrumpirMeditacion();
    bool estaMeditando() const;

    // Inventario
    std::optional<int> agarrarItem(std::unique_ptr<Item> item, int cantidad = 1);
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

    // Recuperacion pasiva
    void recuperacionPasiva(float dt);

    // Resurreccion
    int getDestinoMapaId() const;
    int getDestinoPosX() const;
    int getDestinoPosY() const;

    void iniciarResurreccion(float tiempo, int mapaId, int posX, int posY);
    void tickResurreccion(float dt);
    bool estaResucitando() const;
    bool resurreccionCompleta() const;

    // Cheats
    void activarCheatVidaInfinita();
    void activarCheatManaInfinito();

    void morir() override;
};
#pragma once
#include <string>

enum class Direccion { NORTE, ESTE, SUR, OESTE };

class Character {
protected:
    std::string nombre;
    int mapaId;
    int posX;
    int posY;
    Direccion direccion;
    int vidaActual;
    int vidaMax;
    bool vivo;

public:
    Character(const std::string& nombre, int posX, int posY, int vidaMax);
    virtual ~Character() = default;

    const std::string& getNombre() const;
    int getMapaId() const;
    int getPosX() const;
    int getPosY() const;
    Direccion getDireccion() const;
    int getVidaActual() const;
    int getVidaMax() const;
    bool estaVivo() const;

    void setMapaId(int id);
    void setPosicion(int x, int y);
    void setDireccion(Direccion dir);
    void recibirDanio(int danio);
    void curar(int cantidad);
    virtual void morir();
    void revivir(int vidaInicial);
};
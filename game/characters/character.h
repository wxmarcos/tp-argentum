#pragma once
#include <string>

enum class Direccion { NORTE, SUR, ESTE, OESTE };

class Character {
protected:
    std::string nombre;
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
    int getPosX() const;
    int getPosY() const;
    Direccion getDireccion() const;
    int getVidaActual() const;
    int getVidaMax() const;
    bool estaVivo() const;

    void setPosicion(int x, int y);
    void setDireccion(Direccion dir);
    void recibirDanio(int danio);
    void curar(int cantidad);
    void morir();
    void revivir(int vidaInicial);
};
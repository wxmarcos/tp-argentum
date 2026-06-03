#include "criaturas/criatura.h"

Criatura::Criatura(const std::string& nombre, int posX, int posY,
                   int vidaMax, int nivel, int danioMin, int danioMax, int fuerza)
    : Character(nombre, posX, posY, vidaMax)
    , nivel(nivel), danioMin(danioMin), danioMax(danioMax), fuerza(fuerza) {}

int Criatura::getNivel() const { return nivel; }
int Criatura::getDanioMin() const { return danioMin; }
int Criatura::getDanioMax() const { return danioMax; }
int Criatura::getFuerza() const { return fuerza; }

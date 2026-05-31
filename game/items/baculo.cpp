#include "baculo.h"

Baculo::Baculo(const std::string& nombre, const std::string& nombreHechizo,
               TipoHechizo tipoHechizo, int efectoMin, int efectoMax, int costoMana)
    : nombre(nombre), nombreHechizo(nombreHechizo), tipoHechizo(tipoHechizo),
      efectoMin(efectoMin), efectoMax(efectoMax), costoMana(costoMana) {}

std::string Baculo::getNombre() const { return nombre; }
TipoItem Baculo::getTipo() const { return TipoItem::BACULO; }
bool Baculo::esApilable() const { return false; }
const std::string& Baculo::getNombreHechizo() const { return nombreHechizo; }
TipoHechizo Baculo::getTipoHechizo() const { return tipoHechizo; }
int Baculo::getEfectoMin() const { return efectoMin; }
int Baculo::getEfectoMax() const { return efectoMax; }
int Baculo::getCostoMana() const { return costoMana; }
#include "arma.h"
#include "formulas.h"

Arma::Arma(const std::string& nombre, int danioMin, int danioMax)
    : nombre(nombre), danioMin(danioMin), danioMax(danioMax) {}

std::string Arma::getNombre() const { return nombre; }
TipoItem Arma::getTipo() const { return TipoItem::ARMA; }
int Arma::getDanioMin() const { return danioMin; }
int Arma::getDanioMax() const { return danioMax; }

int Arma::calcularDanio(int fuerza) const {
    return Formulas::calcularDanio(fuerza, danioMin, danioMax);
}
#include "razas/raza.h"

#include "game/config.h"

Raza::Raza(const std::string& nombre, const Config& config):
    nombre_(nombre), constitucionBase_(config.getRazaConstitucionBase(nombre)),
    inteligenciaBase_(config.getRazaInteligenciaBase(nombre)),
    fuerzaBase_(config.getRazaFuerzaBase(nombre)),
    agilidadBase_(config.getRazaAgilidadBase(nombre)),
    fRazaVida_(config.getRazaFVida(nombre)),
    fRazaMana_(config.getRazaFMana(nombre)),
    fRazaRecuperacion_(config.getRazaFRecuperacion(nombre)) {}

std::string Raza::getNombre() const { return nombre_; }
int Raza::getConstitucionBase() const { return constitucionBase_; }
int Raza::getInteligenciaBase() const { return inteligenciaBase_; }
int Raza::getFuerzaBase() const { return fuerzaBase_; }
int Raza::getAgilidadBase() const { return agilidadBase_; }
float Raza::getFRazaVida() const { return fRazaVida_; }
float Raza::getFRazaMana() const { return fRazaMana_; }
float Raza::getFRazaRecuperacion() const { return fRazaRecuperacion_; }

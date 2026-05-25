#include "razas/gnomo.h"

Gnomo::Gnomo(const Config& config)
    : constitucionBase(config.getRazaConstitucionBase("gnomo")),
      inteligenciaBase(config.getRazaInteligenciaBase("gnomo")),
      fuerzaBase(config.getRazaFuerzaBase("gnomo")),
      agilidadBase(config.getRazaAgilidadBase("gnomo")),
      fRazaVida(config.getRazaFVida("gnomo")),
      fRazaMana(config.getRazaFMana("gnomo")),
      fRazaRecuperacion(config.getRazaFRecuperacion("gnomo")) {}

std::string Gnomo::getNombre() const { return "Gnomo"; }
int Gnomo::getConstitucionBase() const { return constitucionBase; }
int Gnomo::getInteligenciaBase() const { return inteligenciaBase; }
int Gnomo::getFuerzaBase() const { return fuerzaBase; }
int Gnomo::getAgilidadBase() const { return agilidadBase; }
float Gnomo::getFRazaVida() const { return fRazaVida; }
float Gnomo::getFRazaMana() const { return fRazaMana; }
float Gnomo::getFRazaRecuperacion() const { return fRazaRecuperacion; }
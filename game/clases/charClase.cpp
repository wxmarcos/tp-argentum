#include "clases/charClase.h"

#include "game/config.h"

charClase::charClase(const std::string& nombre, const Config& config,
                     bool puedeMeditar, bool puedeUsarMagia):
    nombre_(nombre),
    fClaseVida_(config.getClaseFVida(nombre)),
    fClaseMana_(config.getClaseFMana(nombre)),
    fClaseMeditacion_(config.getClaseFMeditacion(nombre)),
    puedeMeditar_(puedeMeditar), puedeUsarMagia_(puedeUsarMagia) {}

std::string charClase::getNombre() const { return nombre_; }
float charClase::getFClaseVida() const { return fClaseVida_; }
float charClase::getFClaseMana() const { return fClaseMana_; }
float charClase::getFClaseMeditacion() const { return fClaseMeditacion_; }
bool charClase::puedeMeditar() const { return puedeMeditar_; }
bool charClase::puedeUsarMagia() const { return puedeUsarMagia_; }

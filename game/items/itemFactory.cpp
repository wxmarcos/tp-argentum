#include "itemFactory.h"

// Armas cuerpo a cuerpo
std::unique_ptr<Arma> ItemFactory::crearEspada() {
    return std::make_unique<Arma>(item_defs::ESPADA, 2, 5);
}

std::unique_ptr<Arma> ItemFactory::crearHacha() {
    return std::make_unique<Arma>(item_defs::HACHA, 4, 5);
}

std::unique_ptr<Arma> ItemFactory::crearMartillo() {
    return std::make_unique<Arma>(item_defs::MARTILLO, 1, 9);
}

// Baculos
std::unique_ptr<Baculo> ItemFactory::crearVaraDeFresno() {
    return std::make_unique<Baculo>(item_defs::VARA_DE_FRESNO, "Flecha magica",
                                    TipoHechizo::DANIO, 2, 4, 5);
}

std::unique_ptr<Baculo> ItemFactory::crearFlautaElfica() {
    return std::make_unique<Baculo>(item_defs::FLAUTA_ELFICA, "Curar",
                                    TipoHechizo::CURACION, 50, 100, 100);
}

std::unique_ptr<Baculo> ItemFactory::crearBaculoNudoso() {
    return std::make_unique<Baculo>(item_defs::BACULO_NUDOSO, "Misil",
                                    TipoHechizo::DANIO, 4, 8, 15);
}

std::unique_ptr<Baculo> ItemFactory::crearBaculoEngarzado() {
    return std::make_unique<Baculo>(item_defs::BACULO_ENGARZADO, "Explosion",
                                    TipoHechizo::DANIO, 8, 20, 30);
}

// Armas a distancia
std::unique_ptr<Arma> ItemFactory::crearArcoSimple() {
    return std::make_unique<Arma>(item_defs::ARCO_SIMPLE, 1, 4, true);
}

std::unique_ptr<Arma> ItemFactory::crearArcoCompuesto() {
    return std::make_unique<Arma>(item_defs::ARCO_COMPUESTO, 4, 16, true);
}

// Armaduras
std::unique_ptr<Armadura> ItemFactory::crearArmaduraDeCuero() {
    return std::make_unique<Armadura>(item_defs::ARMADURA_DE_CUERO, 2, 6);
}

std::unique_ptr<Armadura> ItemFactory::crearArmaduraDePlacas() {
    return std::make_unique<Armadura>(item_defs::ARMADURA_DE_PLACAS, 15, 30);
}

std::unique_ptr<Armadura> ItemFactory::crearTunicaAzul() {
    return std::make_unique<Armadura>(item_defs::TUNICA_AZUL, 6, 10);
}

// Cascos
std::unique_ptr<Casco> ItemFactory::crearCapucha() {
    return std::make_unique<Casco>(item_defs::CAPUCHA, 1, 4);
}

std::unique_ptr<Casco> ItemFactory::crearCascoDeHierro() {
    return std::make_unique<Casco>(item_defs::CASCO_DE_HIERRO, 4, 8);
}

std::unique_ptr<Casco> ItemFactory::crearSombreroMagico() {
    return std::make_unique<Casco>(item_defs::SOMBRERO_MAGICO, 4, 12);
}

// Escudos
std::unique_ptr<Escudo> ItemFactory::crearEscudoDeTortuga() {
    return std::make_unique<Escudo>(item_defs::ESCUDO_DE_TORTUGA, 1, 2);
}

std::unique_ptr<Escudo> ItemFactory::crearEscudoDeHierro() {
    return std::make_unique<Escudo>(item_defs::ESCUDO_DE_HIERRO, 1, 4);
}

// Pociones
std::unique_ptr<Pocion> ItemFactory::crearPocionDeVida() {
    return std::make_unique<Pocion>(TipoPocion::VIDA, 100);
}

std::unique_ptr<Pocion> ItemFactory::crearPocionDeMana() {
    return std::make_unique<Pocion>(TipoPocion::MANA, 100);
}

// Oro
std::unique_ptr<Oro> ItemFactory::crearOro(int cantidad) {
    return std::make_unique<Oro>(cantidad);
}
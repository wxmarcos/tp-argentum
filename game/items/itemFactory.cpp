#include "itemFactory.h"

// Armas cuerpo a cuerpo
std::unique_ptr<Arma> ItemFactory::crearEspada() {
    return std::make_unique<Arma>("Espada", 2, 5);
}

std::unique_ptr<Arma> ItemFactory::crearHacha() {
    return std::make_unique<Arma>("Hacha", 4, 5);
}

std::unique_ptr<Arma> ItemFactory::crearMartillo() {
    return std::make_unique<Arma>("Martillo", 1, 9);
}

// Baculos
std::unique_ptr<Baculo> ItemFactory::crearVaraDeFresno() {
    return std::make_unique<Baculo>("Vara de fresno", "Flecha magica", TipoHechizo::DANIO, 2, 4, 5);
}

std::unique_ptr<Baculo> ItemFactory::crearFlautaElfica() {
    return std::make_unique<Baculo>("Flauta elfica", "Curar", TipoHechizo::CURACION, 50, 100, 100);
}

std::unique_ptr<Baculo> ItemFactory::crearBaculoNudoso() {
    return std::make_unique<Baculo>("Baculo nudoso", "Misil", TipoHechizo::DANIO, 4, 8, 15);
}

std::unique_ptr<Baculo> ItemFactory::crearBaculoEngarzado() {
    return std::make_unique<Baculo>("Baculo engarzado", "Explosion", TipoHechizo::DANIO, 8, 20, 30);
}

// Armas a distancia
std::unique_ptr<Arma> ItemFactory::crearArcoSimple() {
    return std::make_unique<Arma>("Arco simple", 1, 4, true);
}

std::unique_ptr<Arma> ItemFactory::crearArcoCompuesto() {
    return std::make_unique<Arma>("Arco compuesto", 4, 16, true);
}

// Armaduras
std::unique_ptr<Armadura> ItemFactory::crearArmaduraDeCuero() {
    return std::make_unique<Armadura>("Armadura de cuero", 2, 6);
}

std::unique_ptr<Armadura> ItemFactory::crearArmaduraDePlacas() {
    return std::make_unique<Armadura>("Armadura de placas", 15, 30);
}

std::unique_ptr<Armadura> ItemFactory::crearTunicaAzul() {
    return std::make_unique<Armadura>("Túnica azul", 6, 10);
}

// Cascos
std::unique_ptr<Casco> ItemFactory::crearCapucha() {
    return std::make_unique<Casco>("Capucha", 1, 4);
}

std::unique_ptr<Casco> ItemFactory::crearCascoDeHierro() {
    return std::make_unique<Casco>("Casco de hierro", 4, 8);
}

std::unique_ptr<Casco> ItemFactory::crearSombreroMagico() {
    return std::make_unique<Casco>("Sombrero mágico", 4, 12);
}

// Escudos
std::unique_ptr<Escudo> ItemFactory::crearEscudoDeTortuga() {
    return std::make_unique<Escudo>("Escudo de tortuga", 1, 2);
}

std::unique_ptr<Escudo> ItemFactory::crearEscudoDeHierro() {
    return std::make_unique<Escudo>("Escudo de hierro", 1, 4);
}

// Pociones
std::unique_ptr<Pocion> ItemFactory::crearPocionDeVida() {
    return std::make_unique<Pocion>(TipoPocion::VIDA, 100);
}

std::unique_ptr<Pocion> ItemFactory::crearPocionDeMana() {
    return std::make_unique<Pocion>(TipoPocion::MANA, 100);
}
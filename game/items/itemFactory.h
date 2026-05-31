#pragma once
#include <memory>
#include "arma.h"
#include "baculo.h"
#include "armadura.h"
#include "casco.h"
#include "escudo.h"
#include "pocion.h"

class ItemFactory {
public:
    ItemFactory() = delete;

    static std::unique_ptr<Arma> crearEspada();
    static std::unique_ptr<Arma> crearHacha();
    static std::unique_ptr<Arma> crearMartillo();

    static std::unique_ptr<Baculo> crearVaraDeFresno();
    static std::unique_ptr<Baculo> crearFlautaElfica();
    static std::unique_ptr<Baculo> crearBaculoNudoso();
    static std::unique_ptr<Baculo> crearBaculoEngarzado();

    static std::unique_ptr<Arma> crearArcoSimple();
    static std::unique_ptr<Arma> crearArcoCompuesto();

    static std::unique_ptr<Armadura> crearArmaduraDeCuero();
    static std::unique_ptr<Armadura> crearArmaduraDePlacas();
    static std::unique_ptr<Armadura> crearTunicaAzul();

    static std::unique_ptr<Casco> crearCapucha();
    static std::unique_ptr<Casco> crearCascoDeHierro();
    static std::unique_ptr<Casco> crearSombreroMagico();

    static std::unique_ptr<Escudo> crearEscudoDeTortuga();
    static std::unique_ptr<Escudo> crearEscudoDeHierro();

    static std::unique_ptr<Pocion> crearPocionDeVida();
    static std::unique_ptr<Pocion> crearPocionDeMana();
};
#include "jugador.h"
#include <algorithm>

// vidaMax = suma de lo que aportan raza y clase
// TODO: confirmar formula exacta entre todos
static int calcularVidaMax(const Raza* raza, const charClase* clase) {
    return raza->getMaxVida() + clase->getMaxVida();
}

static int calcularManaMax(const Raza* raza, const charClase* clase) {
    return raza->getMaxMana() + clase->getMaxMana();
}

Jugador::Jugador(const std::string& nombre, int posX, int posY,
                 const Raza* raza, const charClase* clase)
    : Character(nombre, posX, posY, calcularVidaMax(raza, clase)),
      raza(raza),
      clase(clase),
      manaActual(calcularManaMax(raza, clase)),
      manaMax(calcularManaMax(raza, clase)),
      nivel(1),
      experiencia(0),
      oro(0) {}

const Raza* Jugador::getRaza() const { return raza; }
const charClase* Jugador::getClase() const { return clase; }

int Jugador::getManaActual() const { return manaActual; }
int Jugador::getManaMax() const { return manaMax; }

void Jugador::gastarMana(int cantidad) {
    if (!clase->puedeUsarMagia()) return;
    manaActual = std::max(0, manaActual - cantidad);
}

void Jugador::recuperarMana(int cantidad) {
    if (!clase->puedeMeditar()) return;
    manaActual = std::min(manaMax, manaActual + cantidad);
}

int Jugador::getNivel() const { return nivel; }
int Jugador::getExperiencia() const { return experiencia; }

int Jugador::expParaSiguienteNivel() const {
    return nivel * 1000;    // TODO: cargar formula desde Config 
}

void Jugador::verificarSubidaNivel() {
    while (experiencia >= expParaSiguienteNivel()) {
        experiencia -= expParaSiguienteNivel();
        nivel++;
        // TODO: recalcular vidaMax y manaMax al subir de nivel
    }
}

void Jugador::ganarExperiencia(int exp) {
    experiencia += exp;
    verificarSubidaNivel();
}

int Jugador::getOro() const { return oro; }

void Jugador::agregarOro(int cantidad) {
    if (cantidad > 0) oro += cantidad;
}

bool Jugador::gastarOro(int cantidad) {
    if (cantidad > oro) return false;
    oro -= cantidad;
    return true;
}

void Jugador::recuperacionPasiva() {
    if (!vivo) return;
    float factor = raza->getFRecuperacion();
    int recuperacionVida = static_cast<int>(5 * factor);        // TODO: base desde Config
    int recuperacionMana = static_cast<int>(5 * factor);        // TODO: base desde Config
    curar(recuperacionVida);
    recuperarMana(recuperacionMana);
}
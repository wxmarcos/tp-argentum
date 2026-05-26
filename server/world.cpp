#include "world.h"
World::World()
    : mapa(100, 100) // tamaño default
{}

Mapa& World::get_map() {
    return mapa;
}

void World::process_command(const Command& cmd) {

    // obtener el personaje por id capaz 
    Character* c = mapa.getPersonajeEnPosicion(
        0, 0 // por ahora 0 , 0, pero luego se debería buscar la posicion correcta
    ); 

    switch (cmd.get_type()) {

        case CommandType::Move:
            if (c) {
                Direccion dir = static_cast<Direccion>(cmd.get_direction()); // traduci el uint16_t a Direccion
                mapa.moverPersonaje(c, dir);
            }
            break;

        case CommandType::Attack:
            // futura lógica de combate
            break;

        case CommandType::Disconnect:
            // futura limpieza de personaje
            break;
    }
}

void World::update() {
    // ticks del mundo (regen, NPCs, etc.)
}

Snapshot World::build_snapshot() const {
    Snapshot s;
    return s;
}
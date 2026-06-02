#include "snapshot_factory.h"

Snapshot SnapshotFactory::entity_created(const std::string& nombre,
                                         const Jugador* jugador) {
    if (!jugador) {
        return Snapshot::entity_remove(nombre);
    }

    return Snapshot::entity_created(
        nombre,
        static_cast<uint16_t>(jugador->getPosX()),
        static_cast<uint16_t>(jugador->getPosY()),
        static_cast<uint8_t>(jugador->getDireccion()));
}

Snapshot SnapshotFactory::entity_login(const std::string& nombre,
                                       const Jugador* jugador) {
    if (!jugador) {
        return Snapshot::entity_remove(nombre);
    }

    return Snapshot::entity_login(
        nombre,
        static_cast<uint16_t>(jugador->getPosX()),
        static_cast<uint16_t>(jugador->getPosY()),
        static_cast<uint8_t>(jugador->getDireccion()));
}

Snapshot SnapshotFactory::entity_move(const std::string& nombre,
                                      const Jugador* jugador) {
    if (!jugador) {
        return Snapshot::entity_remove(nombre);
    }

    return Snapshot::entity_move(
        nombre,
        static_cast<uint16_t>(jugador->getPosX()),
        static_cast<uint16_t>(jugador->getPosY()),
        static_cast<uint8_t>(jugador->getDireccion()));
}
#include "snapshot_factory.h"

#include "characters/jugador.h"

Snapshot SnapshotFactory::player_stats_from_player(
    const Jugador& jugador
) {
    return Snapshot::player_stats(
        jugador.getNombre(),
        jugador.getRaza()->getNombre(),
        jugador.getClase()->getNombre(),
        static_cast<uint16_t>(jugador.getMapaId()),
        static_cast<uint16_t>(jugador.getPosX()),
        static_cast<uint16_t>(jugador.getPosY()),
        static_cast<uint8_t>(jugador.getDireccion()),
        static_cast<uint16_t>(jugador.getNivel()),
        static_cast<uint16_t>(jugador.getVidaActual()),
        static_cast<uint16_t>(jugador.getVidaMax()),
        static_cast<uint16_t>(jugador.getManaActual()),
        static_cast<uint16_t>(jugador.getManaMax()),
        static_cast<uint32_t>(jugador.getExperiencia()),
        static_cast<uint32_t>(jugador.getOro()),
        static_cast<uint16_t>(jugador.getConstitucion()),
        static_cast<uint16_t>(jugador.getInteligencia()),
        static_cast<uint16_t>(jugador.getFuerza()),
        static_cast<uint16_t>(jugador.getAgilidad())
    );
}
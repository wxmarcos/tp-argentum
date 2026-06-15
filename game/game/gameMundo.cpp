#include "game/game.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "common/protocol_defs.h"
#include "game/formulas.h"
#include "game/items/inventario.h"
#include "game/items/itemFactory.h"
#include "game/items/item_defs.h"
#include "game/snapshot_factory.h"

// ----------------- Spawn de criaturas -----------------

void Game::spawnCriaturas(std::vector<Snapshot>& snapshots) {
    for (auto& [mapaId, info] : infoSpawn) {
        if (info.criaturasPosibles.empty()) continue;

        int poblacionActual = 0;
        for (auto& [id, c] : criaturas) {
            if (c->getMapaId() == mapaId && c->estaVivo()) {
                poblacionActual++;
            }
        }

        if (poblacionActual >= info.poblacionMax) continue;

        Mapa* mapa = mundo.getMapa(mapaId);
        if (!mapa) continue;
        if (mapa->esZonaSegura()) continue;

        const std::string& tipo =
            info.criaturasPosibles[rand() % info.criaturasPosibles.size()];

        for (int intento = 0; intento < 20; intento++) {
            int x = rand() % mapa->getAncho();
            int y = rand() % mapa->getAlto();

            if (mapa->esTransitable(x, y) && !mapa->estaOcupada(x, y)) {
                std::string id = agregarCriatura(tipo, mapaId, x, y);

                if (!id.empty()) {
                    snapshots.push_back(Snapshot::entity_created(
                        id, mapaId, static_cast<uint16_t>(x),
                        static_cast<uint16_t>(y), 2));
                }

                break;
            }
        }
    }
}

// ----------------- Tick de criaturas -----------------

void Game::tickCriaturas(float dt, std::vector<Snapshot>& snapshots) {
    int rango = config.getCriaturaRangoDeteccion();
    float cdAtq = config.getCriaturaCooldownAtaque();
    float cdMov = config.getCriaturaCooldownMovimiento();

    std::vector<std::string> criaturasMuertas;

    for (auto& [id, criatura] : criaturas) {
        if (!criatura->estaVivo()) {
            criaturasMuertas.push_back(id);
            continue;
        }

        criatura->actualizarTiempoAtaque(dt);
        criatura->actualizarTiempoMovimiento(dt);

        Jugador* objetivo = nullptr;
        double distanciaMin = static_cast<double>(rango) + 1.0;

        for (auto& [nombre, jugador] : jugadores) {
            if (!jugador->estaVivo()) continue;
            if (jugador->getMapaId() != criatura->getMapaId()) continue;

            double dx = jugador->getPosX() - criatura->getPosX();
            double dy = jugador->getPosY() - criatura->getPosY();
            double distancia = std::sqrt(dx * dx + dy * dy);

            if (distancia <= rango && distancia < distanciaMin) {
                distanciaMin = distancia;
                objetivo = jugador.get();
            }
        }

        if (!objetivo) continue;

        int dx = objetivo->getPosX() - criatura->getPosX();
        int dy = objetivo->getPosY() - criatura->getPosY();
        bool adyacente = (std::abs(dx) + std::abs(dy)) == 1;

        if (adyacente && criatura->getTiempoDesdeUltimoAtaque() >= cdAtq) {
            int danio = criaturaAtacaJugador(criatura.get(), objetivo);
            criatura->resetearCooldownAtaque();

            if (danio == 0) {
                snapshots.push_back(
                    Snapshot::dodge_event(id, objetivo->getNombre()));
            } else {
                snapshots.push_back(Snapshot::damage_event(
                    id, objetivo->getNombre(), static_cast<uint16_t>(danio),
                    false));
            }

            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*objetivo));

            if (!objetivo->estaVivo()) {
                objetivo->perderExperiencia(Formulas::calcularExpPerdida(
                    objetivo->getExperiencia(),
                    config.getFormulaExpPenalidadPorcentaje()));

                snapshots.push_back(
                    SnapshotFactory::player_stats_from_player(*objetivo));

                snapshots.push_back(
                    Snapshot::death_event(objetivo->getNombre()));
                snapshots.push_back(
                    Snapshot::entity_remove(objetivo->getNombre()));

                auto items = objetivo->soltarTodosLosItems();
                for (auto& item : items) {
                    std::string nombreItem = item.item->getNombre();
                    uint16_t cantidad = item.cantidad;

                    mundo.tirarItem(objetivo->getMapaId(), objetivo->getPosX(),
                                    objetivo->getPosY(), std::move(item));

                    snapshots.push_back(Snapshot::item_event(
                        static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                        objetivo->getNombre(), nombreItem,
                        static_cast<uint16_t>(objetivo->getMapaId()),
                        static_cast<uint16_t>(objetivo->getPosX()),
                        static_cast<uint16_t>(objetivo->getPosY()), cantidad));
                }

                int oroExceso = Formulas::calcularOroExceso(
                    objetivo->getOro(), objetivo->getOroMax());

                if (oroExceso > 0) {
                    objetivo->gastarOro(oroExceso);
                    mundo.tirarItem(
                        objetivo->getMapaId(), objetivo->getPosX(),
                        objetivo->getPosY(),
                        SlotInventario(ItemFactory::crearOro(oroExceso)));

                    snapshots.push_back(Snapshot::item_event(
                        static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                        objetivo->getNombre(), item_defs::ORO,
                        static_cast<uint16_t>(objetivo->getMapaId()),
                        static_cast<uint16_t>(objetivo->getPosX()),
                        static_cast<uint16_t>(objetivo->getPosY()),
                        static_cast<uint16_t>(oroExceso)));
                }
            }

            continue;
        }

        if (!adyacente && criatura->getTiempoDesdeUltimoMovimiento() >= cdMov) {
            std::vector<Direccion> direcciones;

            if (std::abs(dx) >= std::abs(dy)) {
                if (dx != 0) {
                    direcciones.push_back((dx > 0) ? Direccion::ESTE
                                                   : Direccion::OESTE);
                }
                if (dy != 0) {
                    direcciones.push_back((dy > 0) ? Direccion::SUR
                                                   : Direccion::NORTE);
                }
            } else {
                if (dy != 0) {
                    direcciones.push_back((dy > 0) ? Direccion::SUR
                                                   : Direccion::NORTE);
                }
                if (dx != 0) {
                    direcciones.push_back((dx > 0) ? Direccion::ESTE
                                                   : Direccion::OESTE);
                }
            }

            bool seMovio = false;

            for (Direccion dir : direcciones) {
                if (mundo.moverPersonaje(criatura.get(), dir)) {
                    seMovio = true;
                    break;
                }
            }

            if (!seMovio) {
                continue;
            }

            criatura->resetearCooldownMovimiento();

            snapshots.push_back(Snapshot::entity_move(
                id, criatura->getMapaId(),
                static_cast<uint16_t>(criatura->getPosX()),
                static_cast<uint16_t>(criatura->getPosY()),
                static_cast<uint8_t>(criatura->getDireccion())));
        }
    }

    for (const auto& id : criaturasMuertas) {
        snapshots.push_back(Snapshot::entity_remove(id));
        removerCriatura(id);
    }
}

// ----------------- Sacerdote más cercano -----------------

bool Game::encontrarSacerdoteMasCercano(const Jugador* fantasma,
                                        InfoNPC& destino,
                                        float& distancia) const {
    bool encontrado = false;
    float distMin = std::numeric_limits<float>::max();

    for (const auto& s : sacerdotes) {
        float dx = static_cast<float>(s.x - fantasma->getPosX());
        float dy = static_cast<float>(s.y - fantasma->getPosY());
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < distMin) {
            distMin = dist;
            destino = s;
            encontrado = true;
        }
    }

    distancia = distMin;
    return encontrado;
}

// ----------------- Tick resurrección -----------------

void Game::tickResucitando(float dt, std::vector<Snapshot>& snapshots) {
    for (auto& [nombre, jugador] : jugadores) {
        if (!jugador->estaResucitando()) continue;

        jugador->tickResurreccion(dt);

        if (jugador->resurreccionCompleta()) {
            mundo.removerPersonaje(jugador.get());
            jugador->setPosicion(jugador->getDestinoPosX(),
                                 jugador->getDestinoPosY());
            jugador->setMapaId(jugador->getDestinoMapaId());
            mundo.agregarPersonaje(jugador.get());
            jugador->revivir(jugador->getVidaMax());

            snapshots.push_back(Snapshot::entity_move(
                nombre, static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            snapshots.push_back(
                SnapshotFactory::player_inventory_from_player(*jugador));
        }
    }
}

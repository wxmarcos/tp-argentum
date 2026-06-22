#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <set>

#include "common/protocol_defs.h"
#include "game/formulas.h"
#include "game/game.h"
#include "game/items/inventario.h"
#include "game/items/itemFactory.h"
#include "game/items/item_defs.h"
#include "game/snapshot_factory.h"

// ----------------- Spawn de criaturas -----------------

void Game::spawnCriaturas(std::vector<OutgoingSnapshot>& snapshots) {
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
                    push_broadcast(snapshots,
                                   Snapshot::entity_created(
                                       id, mapaId, static_cast<uint16_t>(x),
                                       static_cast<uint16_t>(y), 2));
                }

                break;
            }
        }
    }
}

// ----------------- Tick de criaturas -----------------

void Game::tickCriaturas(float dt, std::vector<OutgoingSnapshot>& snapshots) {
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
                push_broadcast(snapshots, Snapshot::dodge_event(
                                              id, objetivo->getNombre()));
            } else {
                push_broadcast(snapshots,
                               Snapshot::damage_event(
                                   id, objetivo->getNombre(),
                                   static_cast<uint16_t>(danio), false));
            }

            auto itObjetivoId = nick_to_player_id.find(objetivo->getNombre());
            if (itObjetivoId != nick_to_player_id.end()) {
                push_unicast(
                    snapshots,
                    SnapshotFactory::player_stats_from_player(*objetivo),
                    itObjetivoId->second);
            }

            if (!objetivo->estaVivo()) {
                push_broadcast(snapshots,
                               Snapshot::death_event(objetivo->getNombre()));

                push_broadcast(snapshots,
                               Snapshot::entity_remove(objetivo->getNombre()));

                objetivo->perderExperiencia(Formulas::calcularExpPerdida(
                    objetivo->getExperiencia(),
                    config.getFormulaExpPenalidadPorcentaje()));

                auto items = objetivo->soltarTodosLosItems();
                std::set<std::pair<int, int>> tilesUsados;
                for (auto& item : items) {
                    std::string nombreItem = item.item->getNombre();
                    uint16_t cantidad = item.cantidad;
                    auto [tx, ty] = buscarTileParaItem(
                        objetivo->getMapaId(), objetivo->getPosX(),
                        objetivo->getPosY(), tilesUsados);

                    mundo.tirarItem(objetivo->getMapaId(), tx, ty,
                                    std::move(item));

                    push_broadcast(
                        snapshots,
                        Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            objetivo->getNombre(), nombreItem,
                            static_cast<uint16_t>(objetivo->getMapaId()),
                            static_cast<uint16_t>(tx),
                            static_cast<uint16_t>(ty), cantidad));
                    if (itObjetivoId != nick_to_player_id.end()) {
                        push_unicast(
                            snapshots,
                            SnapshotFactory::player_inventory_from_player(
                                *objetivo),
                            itObjetivoId->second);
                    }
                }

                int oroExceso = Formulas::calcularOroExceso(
                    objetivo->getOro(), objetivo->getOroMax());

                if (oroExceso > 0) {
                    objetivo->gastarOro(oroExceso);
                    auto [tx, ty] = buscarTileParaItem(
                        objetivo->getMapaId(), objetivo->getPosX(),
                        objetivo->getPosY(), tilesUsados);

                    mundo.tirarItem(
                        objetivo->getMapaId(), tx, ty,
                        SlotInventario(ItemFactory::crearOro(oroExceso)));

                    push_broadcast(
                        snapshots,
                        Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            objetivo->getNombre(), item_defs::ORO,
                            static_cast<uint16_t>(objetivo->getMapaId()),
                            static_cast<uint16_t>(tx),
                            static_cast<uint16_t>(ty),
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

            if (!seMovio) continue;

            criatura->resetearCooldownMovimiento();

            push_broadcast(snapshots,
                           Snapshot::entity_move(
                               id, criatura->getMapaId(),
                               static_cast<uint16_t>(criatura->getPosX()),
                               static_cast<uint16_t>(criatura->getPosY()),
                               static_cast<uint8_t>(criatura->getDireccion())));
        }
    }

    for (const auto& id : criaturasMuertas) {
        push_broadcast(snapshots, Snapshot::entity_remove(id));
        removerCriatura(id);
    }
}

// ----------------- Sacerdote más cercano -----------------

bool Game::encontrarSacerdoteMasCercano(const Jugador* fantasma,
                                        InfoNPC& destino,
                                        float& distancia) const {
    bool encontrado = false;
    float distMin = std::numeric_limits<float>::max();
    const float penalizacion = config.getPenalizacionMapaDistinto();

    for (const auto& s : sacerdotes) {
        float dx = static_cast<float>(s.x - fantasma->getPosX());
        float dy = static_cast<float>(s.y - fantasma->getPosY());
        float dist = std::sqrt(dx * dx + dy * dy);

        if (s.mapaId != fantasma->getMapaId()) {
            dist += penalizacion;
        }

        if (dist < distMin) {
            distMin = dist;
            destino = s;
            encontrado = true;
        }
    }

    distancia = distMin;
    return encontrado;
}

std::pair<int, int> Game::buscarTileParaItem(
    int mapaId, int cx, int cy, std::set<std::pair<int, int>>& usados) const {
    const Mapa* mapa = mundo.getMapa(mapaId);
    if (!mapa) return {cx, cy};

    const int maxRadioDrop = config.getMaxRadioDrop();
    for (int r = 0; r <= maxRadioDrop; ++r) {
        for (int dx = -r; dx <= r; ++dx) {
            for (int dy = -r; dy <= r; ++dy) {
                if (std::abs(dx) != r && std::abs(dy) != r) continue;
                int nx = cx + dx;
                int ny = cy + dy;
                if (!mapa->esPosicionValida(nx, ny)) continue;
                if (!mapa->esTransitable(nx, ny)) continue;
                std::pair<int, int> tile{nx, ny};
                if (usados.count(tile)) continue;
                if (mapa->hayItemEnPosicion(nx, ny)) continue;
                usados.insert(tile);
                return tile;
            }
        }
    }
    return {cx, cy};
}

bool Game::buscarPosicionLibreCerca(int mapaId, int x, int y, int& outX,
                                    int& outY) const {
    const Mapa* mapa = mundo.getMapa(mapaId);
    if (!mapa) return false;

    static const int dirs[8][2] = {{0, 1}, {1, 0},  {0, -1}, {-1, 0},
                                   {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (const auto& d : dirs) {
        int nx = x + d[0];
        int ny = y + d[1];

        if (!mapa->esPosicionValida(nx, ny)) continue;
        if (!mapa->esTransitable(nx, ny)) continue;
        if (mapa->estaOcupada(nx, ny)) continue;

        outX = nx;
        outY = ny;
        return true;
    }

    return false;
}

// ----------------- Tick resurrección -----------------

void Game::tickResucitando(float dt, std::vector<OutgoingSnapshot>& snapshots) {
    for (auto& [nombre, jugador] : jugadores) {
        if (!jugador->estaResucitando()) continue;

        jugador->tickResurreccion(dt);

        if (!jugador->resurreccionCompleta()) continue;

        const int mapaAnterior = jugador->getMapaId();
        const int destinoMapa = jugador->getDestinoMapaId();
        const int destinoX = jugador->getDestinoPosX();
        const int destinoY = jugador->getDestinoPosY();

        mundo.removerPersonaje(jugador.get());

        jugador->setMapaId(destinoMapa);
        jugador->setPosicion(destinoX, destinoY);

        mundo.agregarPersonaje(jugador.get());
        jugador->revivir(jugador->getVidaMax());

        auto itId = nick_to_player_id.find(nombre);

        if (itId != nick_to_player_id.end()) {
            const uint16_t playerId = itId->second;

            if (mapaAnterior != destinoMapa) {
                push_unicast(snapshots,
                             Snapshot::map_change(
                                 nombre, static_cast<uint16_t>(destinoMapa),
                                 static_cast<uint16_t>(destinoX),
                                 static_cast<uint16_t>(destinoY),
                                 static_cast<uint8_t>(jugador->getDireccion())),
                             playerId);

                agregarReplayDeJugadores(snapshots, nombre, destinoMapa,
                                         playerId);
                agregarReplayNpcs(snapshots, destinoMapa, playerId);
                agregarReplayCriaturas(snapshots, destinoMapa, playerId);
                agregarReplayItems(snapshots, destinoMapa, playerId);
            } else {
                push_unicast(snapshots,
                             Snapshot::entity_move(
                                 nombre, static_cast<uint16_t>(destinoMapa),
                                 static_cast<uint16_t>(destinoX),
                                 static_cast<uint16_t>(destinoY),
                                 static_cast<uint8_t>(jugador->getDireccion())),
                             playerId);
            }

            push_unicast(snapshots,
                         SnapshotFactory::player_stats_from_player(*jugador),
                         playerId);

            push_unicast(
                snapshots,
                SnapshotFactory::player_inventory_from_player(*jugador),
                playerId);
        }

        if (mapaAnterior != destinoMapa) {
            push_broadcast(snapshots, Snapshot::entity_remove(nombre));
        }

        push_broadcast(snapshots,
                       Snapshot::entity_created(
                           nombre, static_cast<uint16_t>(destinoMapa),
                           static_cast<uint16_t>(destinoX),
                           static_cast<uint16_t>(destinoY),
                           static_cast<uint8_t>(jugador->getDireccion())));
    }
}

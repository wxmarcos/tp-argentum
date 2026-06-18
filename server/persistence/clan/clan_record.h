#pragma once

#include <cstddef>
#include <cstdint>

constexpr std::size_t CLAN_NAME_SIZE = 32;
constexpr std::size_t CLAN_NICK_SIZE = 32;
constexpr std::size_t CLAN_MAX_MIEMBROS = 16;
constexpr std::size_t CLAN_MAX_SOLICITUDES = 50;
constexpr std::size_t CLAN_MAX_BANEADOS = 50;

struct ClanRecord {
    char nombre[CLAN_NAME_SIZE];
    char fundador[CLAN_NICK_SIZE];

    uint32_t miembros_count;
    char miembros[CLAN_MAX_MIEMBROS][CLAN_NICK_SIZE];

    uint32_t solicitudes_count;
    char solicitudes[CLAN_MAX_SOLICITUDES][CLAN_NICK_SIZE];

    uint32_t baneados_count;
    char baneados[CLAN_MAX_BANEADOS][CLAN_NICK_SIZE];
};
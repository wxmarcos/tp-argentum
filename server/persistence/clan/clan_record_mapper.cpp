#include "server/persistence/clan/clan_record_mapper.h"

#include <algorithm>
#include <cstring>
#include <string>

static void copy_string(char* dest, std::size_t size, const std::string& src) {
    std::memset(dest, 0, size);
    std::strncpy(dest, src.c_str(), size - 1);
}

ClanRecord ClanRecordMapper::to_record(const Clan& clan) {
    ClanRecord record{};

    copy_string(record.nombre, CLAN_NAME_SIZE, clan.getNombre());
    copy_string(record.fundador, CLAN_NICK_SIZE, clan.getFundador());

    record.miembros_count = 0;
    for (const auto& miembro : clan.getMiembros()) {
        if (record.miembros_count >= CLAN_MAX_MIEMBROS) break;

        copy_string(record.miembros[record.miembros_count],
                    CLAN_NICK_SIZE,
                    miembro);

        record.miembros_count++;
    }

    record.solicitudes_count = 0;
    for (const auto& solicitud : clan.getSolicitudes()) {
        if (record.solicitudes_count >= CLAN_MAX_SOLICITUDES) break;

        copy_string(record.solicitudes[record.solicitudes_count],
                    CLAN_NICK_SIZE,
                    solicitud);

        record.solicitudes_count++;
    }

    record.baneados_count = 0;
    for (const auto& baneado : clan.getBaneados()) {
        if (record.baneados_count >= CLAN_MAX_BANEADOS) break;

        copy_string(record.baneados[record.baneados_count],
                    CLAN_NICK_SIZE,
                    baneado);

        record.baneados_count++;
    }

    return record;
}

Clan ClanRecordMapper::from_record(const ClanRecord& record) {
    Clan clan(record.nombre, record.fundador);

    const uint32_t miembros_count =
        std::min<uint32_t>(record.miembros_count, CLAN_MAX_MIEMBROS);

    for (uint32_t i = 0; i < miembros_count; ++i) {
        std::string miembro = record.miembros[i];

        if (!miembro.empty() && miembro != record.fundador) {
            clan.agregarMiembro(miembro);
        }
    }

    const uint32_t solicitudes_count =
        std::min<uint32_t>(record.solicitudes_count, CLAN_MAX_SOLICITUDES);

    for (uint32_t i = 0; i < solicitudes_count; ++i) {
        std::string solicitud = record.solicitudes[i];

        if (!solicitud.empty()) {
            clan.agregarSolicitud(solicitud);
        }
    }

    const uint32_t baneados_count =
        std::min<uint32_t>(record.baneados_count, CLAN_MAX_BANEADOS);

    for (uint32_t i = 0; i < baneados_count; ++i) {
        std::string baneado = record.baneados[i];

        if (!baneado.empty()) {
            clan.banear(baneado);
        }
    }

    return clan;
}
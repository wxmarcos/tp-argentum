#include "server/persistence/persistence_record_mapper.h"

#include <algorithm>
#include <cstring>
#include <string>

static void copy_string(char* dest, std::size_t size, const std::string& src) {
    std::memset(dest, 0, size);
    std::strncpy(dest, src.c_str(), size - 1);
}

PersistencePlayerRecord PersistenceRecordMapper::to_record(
    const PersistenceTask& task) {
    PersistencePlayerRecord record{};

    copy_string(record.nick, PERSISTENCE_NICK_SIZE, task.nick);
    copy_string(record.raza, PERSISTENCE_RAZA_SIZE, task.raza);
    copy_string(record.clase, PERSISTENCE_CLASE_SIZE, task.clase);

    record.mapa_id = task.mapa_id;
    record.x = task.x;
    record.y = task.y;
    record.direction = task.direction;

    record.nivel = task.nivel;
    record.vida = task.vida;
    record.vida_max = task.vida_max;
    record.mana = task.mana;
    record.mana_max = task.mana_max;

    record.experiencia = task.experiencia;
    record.oro = task.oro;

    record.constitucion = task.constitucion;
    record.inteligencia = task.inteligencia;
    record.fuerza = task.fuerza;
    record.agilidad = task.agilidad;

    record.inventario_count = 0;

    for (const auto& item : task.inventario) {
        if (record.inventario_count >= PERSISTENCE_MAX_INVENTORY) {
            break;
        }

        auto& slot = record.inventario[record.inventario_count];

        slot.slot_id = item.slot_id;
        copy_string(slot.item, PERSISTENCE_ITEM_SIZE, item.item);
        slot.cantidad = item.cantidad;
        slot.equipado = item.equipado ? 1 : 0;

        record.inventario_count++;
    }

    return record;
}

PersistenceTask PersistenceRecordMapper::from_record(
    const PersistencePlayerRecord& record) {
    PersistenceTask task;

    task.nick = record.nick;
    task.raza = record.raza;
    task.clase = record.clase;

    task.mapa_id = record.mapa_id;
    task.x = record.x;
    task.y = record.y;
    task.direction = record.direction;

    task.nivel = record.nivel;
    task.vida = record.vida;
    task.vida_max = record.vida_max;
    task.mana = record.mana;
    task.mana_max = record.mana_max;

    task.experiencia = record.experiencia;
    task.oro = record.oro;

    task.constitucion = record.constitucion;
    task.inteligencia = record.inteligencia;
    task.fuerza = record.fuerza;
    task.agilidad = record.agilidad;

    const uint32_t count =
        std::min<uint32_t>(record.inventario_count, PERSISTENCE_MAX_INVENTORY);

    for (uint32_t i = 0; i < count; ++i) {
        PersistenceInventoryItem item;
        item.slot_id = record.inventario[i].slot_id;
        item.item = record.inventario[i].item;
        item.cantidad = record.inventario[i].cantidad;
        item.equipado = record.inventario[i].equipado != 0;

        if (!item.item.empty() && item.cantidad > 0) {
            task.inventario.push_back(item);
        }
    }

    return task;
}

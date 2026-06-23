#include "itemFactory.h"

#include "game/items/arma.h"
#include "game/items/armadura.h"
#include "game/items/baculo.h"
#include "game/items/casco.h"
#include "game/items/escudo.h"
#include "game/items/item_defs.h"
#include "game/items/pocion.h"

std::unique_ptr<Item> ItemFactory::crear(const std::string& nombre,
                                         const Config& config) {
    // Armas cuerpo a cuerpo
    if (nombre == item_defs::ESPADA)
        return std::make_unique<Arma>(nombre, config.getItemDanioMin("espada"),
                                      config.getItemDanioMax("espada"));
    if (nombre == item_defs::HACHA)
        return std::make_unique<Arma>(nombre, config.getItemDanioMin("hacha"),
                                      config.getItemDanioMax("hacha"));
    if (nombre == item_defs::MARTILLO)
        return std::make_unique<Arma>(nombre,
                                      config.getItemDanioMin("martillo"),
                                      config.getItemDanioMax("martillo"));

    // Armas a distancia
    if (nombre == item_defs::ARCO_SIMPLE)
        return std::make_unique<Arma>(
            nombre, config.getItemDanioMin("arco_simple"),
            config.getItemDanioMax("arco_simple"), true);
    if (nombre == item_defs::ARCO_COMPUESTO)
        return std::make_unique<Arma>(
            nombre, config.getItemDanioMin("arco_compuesto"),
            config.getItemDanioMax("arco_compuesto"), true);

    // Baculos
    if (nombre == item_defs::VARA_DE_FRESNO)
        return std::make_unique<Baculo>(
            nombre, "Flecha magica", TipoHechizo::DANIO,
            config.getItemEfectoMin("vara_de_fresno"),
            config.getItemEfectoMax("vara_de_fresno"),
            config.getItemCostoMana("vara_de_fresno"));
    if (nombre == item_defs::FLAUTA_ELFICA)
        return std::make_unique<Baculo>(
            nombre, "Curar", TipoHechizo::CURACION,
            config.getItemEfectoMin("flauta_elfica"),
            config.getItemEfectoMax("flauta_elfica"),
            config.getItemCostoMana("flauta_elfica"));
    if (nombre == item_defs::BACULO_NUDOSO)
        return std::make_unique<Baculo>(
            nombre, "Misil", TipoHechizo::DANIO,
            config.getItemEfectoMin("baculo_nudoso"),
            config.getItemEfectoMax("baculo_nudoso"),
            config.getItemCostoMana("baculo_nudoso"));
    if (nombre == item_defs::BACULO_ENGARZADO)
        return std::make_unique<Baculo>(
            nombre, "Explosion", TipoHechizo::DANIO,
            config.getItemEfectoMin("baculo_engarzado"),
            config.getItemEfectoMax("baculo_engarzado"),
            config.getItemCostoMana("baculo_engarzado"));

    // Armaduras
    if (nombre == item_defs::ARMADURA_DE_CUERO)
        return std::make_unique<Armadura>(
            nombre, config.getItemDefensaMin("armadura_de_cuero"),
            config.getItemDefensaMax("armadura_de_cuero"));
    if (nombre == item_defs::ARMADURA_DE_PLACAS)
        return std::make_unique<Armadura>(
            nombre, config.getItemDefensaMin("armadura_de_placas"),
            config.getItemDefensaMax("armadura_de_placas"));
    if (nombre == item_defs::TUNICA_AZUL)
        return std::make_unique<Armadura>(
            nombre, config.getItemDefensaMin("tunica_azul"),
            config.getItemDefensaMax("tunica_azul"));

    // Cascos
    if (nombre == item_defs::CAPUCHA)
        return std::make_unique<Casco>(nombre,
                                       config.getItemDefensaMin("capucha"),
                                       config.getItemDefensaMax("capucha"));
    if (nombre == item_defs::CASCO_DE_HIERRO)
        return std::make_unique<Casco>(
            nombre, config.getItemDefensaMin("casco_de_hierro"),
            config.getItemDefensaMax("casco_de_hierro"));
    if (nombre == item_defs::SOMBRERO_MAGICO)
        return std::make_unique<Casco>(
            nombre, config.getItemDefensaMin("sombrero_magico"),
            config.getItemDefensaMax("sombrero_magico"));

    // Escudos
    if (nombre == item_defs::ESCUDO_DE_TORTUGA)
        return std::make_unique<Escudo>(
            nombre, config.getItemDefensaMin("escudo_de_tortuga"),
            config.getItemDefensaMax("escudo_de_tortuga"));
    if (nombre == item_defs::ESCUDO_DE_HIERRO)
        return std::make_unique<Escudo>(
            nombre, config.getItemDefensaMin("escudo_de_hierro"),
            config.getItemDefensaMax("escudo_de_hierro"));

    // Pociones
    if (nombre == item_defs::POCION_DE_VIDA)
        return std::make_unique<Pocion>(
            TipoPocion::VIDA, config.getItemCantidad("pocion_de_vida"));
    if (nombre == item_defs::POCION_DE_MANA)
        return std::make_unique<Pocion>(
            TipoPocion::MANA, config.getItemCantidad("pocion_de_mana"));

    return nullptr;
}

std::unique_ptr<Oro> ItemFactory::crearOro(int cantidad) {
    return std::make_unique<Oro>(cantidad);
}

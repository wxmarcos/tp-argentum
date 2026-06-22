#include <gtest/gtest.h>

#include "common/command/command.h"
#include "common/command/command_codec.h"
#include "common/snapshot/snapshot.h"

// ============================================================
// Helpers
// ============================================================

// Encode a Command and decode it back.
static Command roundtrip(const Command& cmd) {
    auto payload = build_command_payload(cmd);
    return parse_command_payload(payload, cmd.get_type(), cmd.get_player_id());
}

// ============================================================
// Tests de Command — round-trip encode/decode
// ============================================================

TEST(CommandCodec, Login) {
    auto cmd = Command::login("Pedro");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::LOGIN);
    EXPECT_EQ(parsed.get_nick(), "Pedro");
}

TEST(CommandCodec, LoginNickVacio) {
    auto cmd = Command::login("");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_nick(), "");
}

TEST(CommandCodec, LoginNickConEspacios) {
    auto cmd = Command::login("Lionel Messi");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_nick(), "Lionel Messi");
}

TEST(CommandCodec, CreateCharacter) {
    auto cmd = Command::create_character("Drope37", "enano", "guerrero");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::CREATE_CHARACTER);
    EXPECT_EQ(parsed.get_nick(), "Drope37");
    EXPECT_EQ(parsed.get_raza(), "enano");
    EXPECT_EQ(parsed.get_clase(), "guerrero");
}

TEST(CommandCodec, CreateCharacterTodasLasRazasYClases) {
    const std::vector<std::pair<std::string, std::string>> combos = {
        {"humano", "guerrero"},
        {"elfo", "mago"},
        {"enano", "clerigo"},
        {"gnomo", "paladin"},
    };
    for (const auto& [raza, clase] : combos) {
        auto cmd = Command::create_character("Nick", raza, clase);
        auto parsed = roundtrip(cmd);
        EXPECT_EQ(parsed.get_raza(), raza);
        EXPECT_EQ(parsed.get_clase(), clase);
    }
}

TEST(CommandCodec, Move) {
    for (uint8_t dir = 0; dir <= 3; ++dir) {
        auto cmd = Command::move(dir);
        auto parsed = roundtrip(cmd);

        EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::MOVE);
        EXPECT_EQ(parsed.get_direction(), dir);
    }
}

TEST(CommandCodec, Attack) {
    auto cmd = Command::attack("Zombie_0");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::ATTACK);
    EXPECT_EQ(parsed.get_nick(), "Zombie_0");
}

TEST(CommandCodec, DropItem) {
    auto cmd = Command::drop_item(5);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::DROP_ITEM);
    EXPECT_EQ(parsed.get_slot(), 5);
}

TEST(CommandCodec, EquipItem) {
    auto cmd = Command::equip_item(3);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::EQUIP_ITEM);
    EXPECT_EQ(parsed.get_slot(), 3);
}

TEST(CommandCodec, PickItem) {
    auto cmd = Command::pick_item();
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::PICK_ITEM);
}

TEST(CommandCodec, BuyItem) {
    auto cmd = Command::buy_item("espada");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::BUY_ITEM);
    EXPECT_EQ(parsed.get_text(), "espada");
}

TEST(CommandCodec, SellItem) {
    auto cmd = Command::sell_item(2);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::SELL_ITEM);
    EXPECT_EQ(parsed.get_slot(), 2);
}

TEST(CommandCodec, DepositItem) {
    auto cmd = Command::deposit_item(7);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::DEPOSIT_ITEM);
    EXPECT_EQ(parsed.get_slot(), 7);
}

TEST(CommandCodec, WithdrawItem) {
    auto cmd = Command::withdraw_item(4);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::WITHDRAW_ITEM);
    EXPECT_EQ(parsed.get_item_id(), 4);
}

TEST(CommandCodec, DepositGold) {
    auto cmd = Command::deposit_gold(500);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::DEPOSIT_GOLD);
    EXPECT_EQ(parsed.get_amount(), 500u);
}

TEST(CommandCodec, WithdrawGold) {
    auto cmd = Command::withdraw_gold(250);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::WITHDRAW_GOLD);
    EXPECT_EQ(parsed.get_amount(), 250u);
}

TEST(CommandCodec, GoldMaxValue) {
    uint32_t max = std::numeric_limits<uint32_t>::max();
    auto cmd = Command::deposit_gold(max);
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_amount(), max);
}

TEST(CommandCodec, PrivateMessage) {
    auto cmd = Command::private_message("Lean", "hola, como estas?");
    auto parsed = roundtrip(cmd);

    EXPECT_EQ(parsed.get_type(), protocol::ClientOpcode::PRIVATE_MESSAGE);
    EXPECT_EQ(parsed.get_nick(), "Lean");
    EXPECT_EQ(parsed.get_text(), "hola, como estas?");
}

TEST(CommandCodec, ComandosSinPayload) {
    const std::vector<std::pair<protocol::ClientOpcode, Command>> comandos = {
        {protocol::ClientOpcode::MEDITATE, Command::meditate()},
        {protocol::ClientOpcode::RESURRECT, Command::resurrect()},
        {protocol::ClientOpcode::HEAL, Command::heal()},
        {protocol::ClientOpcode::CHEAT_GOD, Command::cheat_god()},
        {protocol::ClientOpcode::CHEAT_MANA, Command::cheat_mana()},
        {protocol::ClientOpcode::CHEAT_DIE, Command::cheat_die()},
        {protocol::ClientOpcode::CHEAT_RESURRECT, Command::cheat_resurrect()},
        {protocol::ClientOpcode::DISCONNECT, Command::disconnect()},
    };

    for (const auto& [opcode, cmd] : comandos) {
        auto payload = build_command_payload(cmd);
        EXPECT_TRUE(payload.empty()) << "Opcode " << static_cast<int>(opcode)
                                     << " debería tener payload vacío";
        auto parsed = parse_command_payload(payload, opcode, 1);
        EXPECT_EQ(parsed.get_type(), opcode);
    }
}

TEST(CommandCodec, PayloadExtraBytesLanzaExcepcion) {
    // Un LOGIN válido más un byte extra
    auto cmd = Command::login("test");
    auto payload = build_command_payload(cmd);
    payload.push_back(0xFF);  // byte extra

    EXPECT_THROW(
        parse_command_payload(payload, protocol::ClientOpcode::LOGIN, 1),
        std::runtime_error);
}

TEST(CommandCodec, OpcodeDesconocidoLanzaExcepcion) {
    std::vector<uint8_t> payload;
    EXPECT_THROW(parse_command_payload(
                     payload, static_cast<protocol::ClientOpcode>(0x42), 1),
                 std::runtime_error);
}

// ============================================================
// Tests de Snapshot — construcción y getters
// ============================================================

TEST(Snapshot, EntityCreated) {
    auto s = Snapshot::entity_created("jugador1", 2, 10, 20, 3);

    EXPECT_EQ(s.get_opcode(), protocol::ServerOpcode::ENTITY_CREATED);
    EXPECT_EQ(s.get_nick(), "jugador1");
    EXPECT_EQ(s.get_mapa_id(), 2);
    EXPECT_EQ(s.get_x(), 10);
    EXPECT_EQ(s.get_y(), 20);
    EXPECT_EQ(s.get_direction(), 3);
    EXPECT_TRUE(s.is_entity_created());
}

TEST(Snapshot, EntityMove) {
    auto s = Snapshot::entity_move("jugador1", 1, 5, 7, 0);

    EXPECT_EQ(s.get_opcode(), protocol::ServerOpcode::ENTITY_MOVE);
    EXPECT_TRUE(s.is_entity_move());
    EXPECT_EQ(s.get_x(), 5);
    EXPECT_EQ(s.get_y(), 7);
}

TEST(Snapshot, EntityRemove) {
    auto s = Snapshot::entity_remove("goblin_0");

    EXPECT_EQ(s.get_opcode(), protocol::ServerOpcode::ENTITY_REMOVE);
    EXPECT_TRUE(s.is_entity_remove());
    EXPECT_EQ(s.get_nick(), "goblin_0");
}

TEST(Snapshot, DamageEvent) {
    auto s = Snapshot::damage_event("atacante", "victima", 42, false);

    EXPECT_EQ(s.get_opcode(), protocol::ServerOpcode::DAMAGE_EVENT);
    EXPECT_TRUE(s.is_damage_event());
    EXPECT_EQ(s.get_attacker(), "atacante");
    EXPECT_EQ(s.get_target(), "victima");
    EXPECT_EQ(s.get_damage(), 42);
    EXPECT_FALSE(s.is_critical());
}

TEST(Snapshot, DamageEventCritico) {
    auto s = Snapshot::damage_event("a", "b", 100, true);

    EXPECT_TRUE(s.is_critical());
    EXPECT_EQ(s.get_damage(), 100);
}

TEST(Snapshot, DodgeEvent) {
    auto s = Snapshot::dodge_event("a", "b");

    EXPECT_EQ(s.get_opcode(), protocol::ServerOpcode::DODGE_EVENT);
    EXPECT_TRUE(s.is_dodge_event());
    EXPECT_EQ(s.get_attacker(), "a");
    EXPECT_EQ(s.get_target(), "b");
}

TEST(Snapshot, DeathEvent) {
    auto s = Snapshot::death_event("victima");

    EXPECT_EQ(s.get_opcode(), protocol::ServerOpcode::DEATH_EVENT);
    EXPECT_TRUE(s.is_death_event());
    EXPECT_EQ(s.get_target(), "victima");
}

TEST(Snapshot, ErrorMessage) {
    auto s = Snapshot::error_message("Pedro", "No se pudo mover");

    EXPECT_TRUE(s.is_error_message());
    EXPECT_EQ(s.get_nick(), "Pedro");
    EXPECT_EQ(s.get_text(), "No se pudo mover");
}

TEST(Snapshot, PlayerStats) {
    auto s =
        Snapshot::player_stats("Pedro", "humano", "guerrero",
                               /*mapa*/ 1, /*x*/ 10, /*y*/ 20, /*dir*/ 2,
                               /*nivel*/ 5, /*vida*/ 80, /*vida_max*/ 100,
                               /*mana*/ 30, /*mana_max*/ 50,
                               /*exp*/ 1500, /*exp_limite*/ 2000, /*oro*/ 200,
                               /*con*/ 10, /*int*/ 8, /*fue*/ 12, /*agi*/ 9);

    EXPECT_TRUE(s.is_player_stats());
    EXPECT_EQ(s.get_nick(), "Pedro");
    EXPECT_EQ(s.get_nivel(), 5);
    EXPECT_EQ(s.get_vida(), 80);
    EXPECT_EQ(s.get_vida_max(), 100);
    EXPECT_EQ(s.get_mana(), 30);
    EXPECT_EQ(s.get_mana_max(), 50);
    EXPECT_EQ(s.get_experiencia(), 1500u);
    EXPECT_EQ(s.get_oro(), 200u);
    EXPECT_EQ(s.get_fuerza(), 12);
    EXPECT_EQ(s.get_agilidad(), 9);
}

TEST(Snapshot, InventoryUpdate) {
    std::vector<InventorySnapshotItem> items = {
        {0, "espada", 1, true},
        {1, "pocion_de_vida", 3, false},
    };
    auto s = Snapshot::inventory_update("Pedro", items);

    EXPECT_TRUE(s.is_inventory_update());
    ASSERT_EQ(s.get_inventory_items().size(), 2u);
    EXPECT_EQ(s.get_inventory_items()[0].item, "espada");
    EXPECT_EQ(s.get_inventory_items()[0].equipado, true);
    EXPECT_EQ(s.get_inventory_items()[1].cantidad, 3);
}

TEST(Snapshot, ChatMessage) {
    auto s = Snapshot::chat_message("Pedro", "Lean", "Vamos a matar lobos?");

    EXPECT_TRUE(s.is_chat_message());
    EXPECT_EQ(s.get_nick(), "Pedro");
    EXPECT_EQ(s.get_text(), "Vamos a matar lobos?");
}

TEST(Snapshot, MeditationStatus) {
    auto s_on = Snapshot::meditation_status("Pedro", true);
    auto s_off = Snapshot::meditation_status("Pedro", false);

    EXPECT_TRUE(s_on.is_meditation_status());
    EXPECT_TRUE(s_on.is_meditating());
    EXPECT_FALSE(s_off.is_meditating());
}

TEST(Snapshot, ItemEventDrop) {
    auto s = Snapshot::item_event(
        static_cast<uint8_t>(protocol::ItemEventAction::DROP), "goblin_0",
        "espada", 1, 5, 5, 1);

    EXPECT_TRUE(s.is_item_event());
    EXPECT_EQ(s.get_item_action(),
              static_cast<uint8_t>(protocol::ItemEventAction::DROP));
    EXPECT_EQ(s.get_item_name(), "espada");
    EXPECT_EQ(s.get_amount(), 1);
}

TEST(Snapshot, ItemEventPick) {
    auto s = Snapshot::item_event(
        static_cast<uint8_t>(protocol::ItemEventAction::PICK), "Pedro", "oro",
        1, 3, 3, 50);

    EXPECT_EQ(s.get_item_action(),
              static_cast<uint8_t>(protocol::ItemEventAction::PICK));
    EXPECT_EQ(s.get_amount(), 50);
}

TEST(Snapshot, MapChange) {
    auto s = Snapshot::map_change("Pedro", 2, 15, 30, 1);

    EXPECT_TRUE(s.is_map_change());
    EXPECT_EQ(s.get_mapa_id(), 2);
    EXPECT_EQ(s.get_x(), 15);
    EXPECT_EQ(s.get_y(), 30);
}

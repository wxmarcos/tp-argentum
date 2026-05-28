#include "game_loop.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "server_config.h"

GameLoop::GameLoop(
    Queue<Command>& commands_queue,
    std::vector<std::unique_ptr<ClientHandler>>& clients,
    Config& config,
    int anchoMapa,
    int altoMapa)
    : commands_queue(commands_queue),
      clients(clients),
      game(config, anchoMapa, altoMapa) {}

void GameLoop::run() {

    std::cout << "[GameLoop] iniciado\n";

    const auto tick_duration =
        std::chrono::milliseconds(
            1000 / ServerConfig::TICKS_PER_SECOND);

    while (should_keep_running()) {

        auto tick_start =
            std::chrono::steady_clock::now();

        try {

            Command cmd =
                commands_queue.pop();

            std::vector<Snapshot> snapshots =
                game.process(cmd);

            for (const Snapshot& snapshot : snapshots) {
                broadcast_snapshot(snapshot);
            }

        } catch (const ClosedQueue&) {
            break;
        }

        // update lógica del juego
        float dt =
            std::chrono::duration<float>(
                tick_duration).count();

        game.tick(dt);

        auto tick_end =
            std::chrono::steady_clock::now();

        auto elapsed =
            tick_end - tick_start;

        if (elapsed < tick_duration) {
            std::this_thread::sleep_for(
                tick_duration - elapsed);
        }
    }

    std::cout << "[GameLoop] finalizado\n";
}

void GameLoop::process(const Command& cmd) {

    if (cmd.is_disconnect()) {
        std::cout
            << "[GameLoop] Player "
            << cmd.get_player_id()
            << " disconnected\n";
        return;
    }

    switch (cmd.get_type()) {

        case CommandType::Move:
            std::cout
                << "[GameLoop] MOVE dir="
                << static_cast<int>(cmd.get_direction())
                << "\n";
            break;

        case CommandType::Attack:
            std::cout
                << "[GameLoop] ATTACK target="
                << cmd.get_target()
                << "\n";
            break;

        case CommandType::Meditate:
            std::cout << "[GameLoop] MEDITATE\n";
            break;

        case CommandType::Resurrect:
            std::cout << "[GameLoop] RESURRECT\n";
            break;

        case CommandType::Heal:
            std::cout << "[GameLoop] HEAL\n";
            break;

        case CommandType::PickItem:
            std::cout << "[GameLoop] PICK_ITEM\n";
            break;

        case CommandType::DropItem:
            std::cout
                << "[GameLoop] DROP_ITEM item="
                << cmd.get_item_id()
                << "\n";
            break;

        case CommandType::EquipItem:
            std::cout
                << "[GameLoop] EQUIP_ITEM item="
                << cmd.get_item_id()
                << "\n";
            break;

        case CommandType::BuyItem:
            std::cout
                << "[GameLoop] BUY_ITEM item="
                << cmd.get_item_id()
                << "\n";
            break;

        case CommandType::SellItem:
            std::cout
                << "[GameLoop] SELL_ITEM item="
                << cmd.get_item_id()
                << "\n";
            break;

        case CommandType::DepositItem:
            std::cout
                << "[GameLoop] DEPOSIT_ITEM item="
                << cmd.get_item_id()
                << " amount="
                << cmd.get_amount()
                << "\n";
            break;

        case CommandType::WithdrawItem:
            std::cout
                << "[GameLoop] WITHDRAW_ITEM item="
                << cmd.get_item_id()
                << " amount="
                << cmd.get_amount()
                << "\n";
            break;

        case CommandType::PrivateMessage:
            std::cout
                << "[GameLoop] PRIVATE_MESSAGE to="
                << cmd.get_nick()
                << " text="
                << cmd.get_text()
                << "\n";
            break;

        case CommandType::ClanCreate:
            std::cout
                << "[GameLoop] CLAN_CREATE name="
                << cmd.get_clan_name()
                << "\n";
            break;

        case CommandType::ClanJoin:
            std::cout
                << "[GameLoop] CLAN_JOIN name="
                << cmd.get_clan_name()
                << "\n";
            break;

        case CommandType::ClanReview:
            std::cout << "[GameLoop] CLAN_REVIEW\n";
            break;

        case CommandType::ClanAccept:
            std::cout
                << "[GameLoop] CLAN_ACCEPT nick="
                << cmd.get_nick()
                << "\n";
            break;

        case CommandType::ClanReject:
            std::cout
                << "[GameLoop] CLAN_REJECT nick="
                << cmd.get_nick()
                << "\n";
            break;

        case CommandType::ClanBan:
            std::cout
                << "[GameLoop] CLAN_BAN nick="
                << cmd.get_nick()
                << "\n";
            break;

        case CommandType::ClanKick:
            std::cout
                << "[GameLoop] CLAN_KICK nick="
                << cmd.get_nick()
                << "\n";
            break;

        case CommandType::ClanLeave:
            std::cout << "[GameLoop] CLAN_LEAVE\n";
            break;

        case CommandType::Disconnect:
            std::cout
                << "[GameLoop] DISCONNECT player="
                << cmd.get_player_id()
                << "\n";
            break;
    }
}

void GameLoop::broadcast_snapshot(
    const Snapshot& snapshot) {

    for (auto& client : clients) {

        if (client) {
            client->push(snapshot);
        }
    }
}
import struct

from protocol_test_utils import (
    OP_LOGIN,
    OP_CREATE_CHARACTER,
    OP_PICK_ITEM,
    OP_DROP_ITEM,
    OP_EQUIP_ITEM,
    connect,
    pack_string,
    send_packet,
    print_packet,
    read_available_packets,
)


def print_responses(sock, title):
    print()
    print(f"=== {title} ===")

    packets = read_available_packets(sock)

    if not packets:
        print("Sin respuestas")
        return

    for packet in packets:
        print_packet(packet)


def login(sock, nick):
    send_packet(sock, OP_LOGIN, pack_string(nick))
    print_responses(sock, "LOGIN")


def create_character(sock, nick, raza, clase):
    payload = pack_string(nick) + pack_string(raza) + pack_string(clase)
    send_packet(sock, OP_CREATE_CHARACTER, payload)
    print_responses(sock, "CREATE_CHARACTER")


def pick_item(sock):
    send_packet(sock, OP_PICK_ITEM)
    print_responses(sock, "PICK_ITEM")


def drop_item(sock, slot_id):
    payload = struct.pack(">H", slot_id)
    send_packet(sock, OP_DROP_ITEM, payload)
    print_responses(sock, f"DROP_ITEM slot={slot_id}")


def equip_item(sock, slot_id):
    payload = struct.pack(">H", slot_id)
    send_packet(sock, OP_EQUIP_ITEM, payload)
    print_responses(sock, f"EQUIP_ITEM slot={slot_id}")


def main():
    nick = "InventoryTest"
    raza = "humano"
    clase = "mago"

    with connect() as sock:
        login(sock, nick)
        create_character(sock, nick, raza, clase)

        pick_item(sock)
        equip_item(sock, 0)
        equip_item(sock, 1)
        drop_item(sock, 3)


if __name__ == "__main__":
    main()
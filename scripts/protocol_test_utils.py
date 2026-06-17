import socket
import struct

HOST = "127.0.0.1"
PORT = 8080

OP_LOGIN = 0
OP_CREATE_CHARACTER = 1
OP_PICK_ITEM = 10
OP_DROP_ITEM = 11
OP_EQUIP_ITEM = 12

SERVER_ENTITY_CREATED = 5
SERVER_ENTITY_LOGIN = 6
SERVER_PLAYER_STATS = 10
SERVER_INVENTORY_UPDATE = 11
SERVER_ERROR_MESSAGE = 250


def pack_string(text):
    data = text.encode("utf-8")
    return struct.pack(">H", len(data)) + data


def connect():
    return socket.create_connection((HOST, PORT))


def send_packet(sock, opcode, payload=b""):
    header = struct.pack(">BH", opcode, len(payload))
    sock.sendall(header + payload)


def recv_exact(sock, size):
    data = b""

    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return None
        data += chunk

    return data


def recv_packet(sock):
    header = recv_exact(sock, 3)
    if header is None:
        return None

    opcode, payload_size = struct.unpack(">BH", header)
    payload = recv_exact(sock, payload_size)

    if payload is None:
        return None

    return opcode, payload


def read_u8(payload, offset):
    return payload[offset], offset + 1


def read_u16(payload, offset):
    return struct.unpack_from(">H", payload, offset)[0], offset + 2


def read_u32(payload, offset):
    return struct.unpack_from(">I", payload, offset)[0], offset + 4


def read_string(payload, offset):
    length, offset = read_u16(payload, offset)
    text = payload[offset:offset + length].decode("utf-8")
    return text, offset + length


def decode_entity_position(payload, event_type):
    offset = 0
    nick, offset = read_string(payload, offset)
    x, offset = read_u16(payload, offset)
    y, offset = read_u16(payload, offset)
    direction, offset = read_u8(payload, offset)

    return {
        "type": event_type,
        "nick": nick,
        "x": x,
        "y": y,
        "direction": direction,
    }


def decode_packet(opcode, payload):
    offset = 0

    if opcode == SERVER_ERROR_MESSAGE:
        nick, offset = read_string(payload, offset)
        message, offset = read_string(payload, offset)
        return {
            "type": "ERROR_MESSAGE",
            "nick": nick,
            "message": message,
        }

    if opcode == SERVER_ENTITY_CREATED:
        return decode_entity_position(payload, "ENTITY_CREATED")

    if opcode == SERVER_ENTITY_LOGIN:
        return decode_entity_position(payload, "ENTITY_LOGIN")

    if opcode == SERVER_PLAYER_STATS:
        nick, offset = read_string(payload, offset)
        raza, offset = read_string(payload, offset)
        clase, offset = read_string(payload, offset)

        mapa_id, offset = read_u16(payload, offset)
        x, offset = read_u16(payload, offset)
        y, offset = read_u16(payload, offset)
        direction, offset = read_u8(payload, offset)

        level, offset = read_u16(payload, offset)

        vida, offset = read_u16(payload, offset)
        vida_max, offset = read_u16(payload, offset)
        mana, offset = read_u16(payload, offset)
        mana_max, offset = read_u16(payload, offset)

        exp, offset = read_u32(payload, offset)
        oro, offset = read_u32(payload, offset)

        constitucion, offset = read_u16(payload, offset)
        inteligencia, offset = read_u16(payload, offset)
        fuerza, offset = read_u16(payload, offset)
        agilidad, offset = read_u16(payload, offset)

        return {
            "type": "PLAYER_STATS",
            "nick": nick,
            "raza": raza,
            "clase": clase,
            "mapa_id": mapa_id,
            "level": level,
            "position": {"x": x, "y": y},
            "direction": direction,
            "vida": f"{vida}/{vida_max}",
            "mana": f"{mana}/{mana_max}",
            "exp": exp,
            "oro": oro,
            "constitucion": constitucion,
            "inteligencia": inteligencia,
            "fuerza": fuerza,
            "agilidad": agilidad,
        }

    if opcode == SERVER_INVENTORY_UPDATE:
        nick, offset = read_string(payload, offset)
        slot_count, offset = read_u16(payload, offset)

        slots = []

        for _ in range(slot_count):
            slot_id, offset = read_u16(payload, offset)
            item_name, offset = read_string(payload, offset)
            quantity, offset = read_u16(payload, offset)
            equipped, offset = read_u8(payload, offset)

            slots.append({
                "slot_id": slot_id,
                "item": item_name,
                "quantity": quantity,
                "equipped": bool(equipped),
            })

        return {
            "type": "INVENTORY_UPDATE",
            "nick": nick,
            "slot_count": slot_count,
            "slots": slots,
        }

    return {
        "type": "UNKNOWN",
        "opcode": opcode,
        "payload_hex": payload.hex(" "),
    }


def print_packet(packet):
    if packet is None:
        print("No se recibió respuesta")
        return

    opcode, payload = packet
    decoded = decode_packet(opcode, payload)

    print(f"opcode={opcode} payload_size={len(payload)}")

    if decoded["type"] == "INVENTORY_UPDATE":
        print(f"type={decoded['type']}")
        print(f"nick={decoded['nick']}")
        print(f"slot_count={decoded['slot_count']}")

        for slot in decoded["slots"]:
            if slot["item"] or slot["quantity"] != 0 or slot["equipped"]:
                print(
                    f"  slot={slot['slot_id']} "
                    f"item={slot['item']} "
                    f"quantity={slot['quantity']} "
                    f"equipped={slot['equipped']}"
                )

        print("  slots vacíos omitidos")
        return

    print(decoded)


def read_available_packets(sock, max_packets=10, timeout=0.5):
    packets = []
    old_timeout = sock.gettimeout()
    sock.settimeout(timeout)

    try:
        for _ in range(max_packets):
            try:
                packet = recv_packet(sock)
                if packet is None:
                    break
                packets.append(packet)
            except socket.timeout:
                break
    finally:
        sock.settimeout(old_timeout)

    return packets
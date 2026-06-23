# Manual de Usuario — Argentum Online

## Índice

1. [Introducción](#1-introducción)
2. [Instalación y arranque](#2-instalación-y-arranque)
3. [Creación de personaje](#3-creación-de-personaje)
4. [Interfaz de juego](#4-interfaz-de-juego)
5. [Controles](#5-controles)
6. [Comandos de consola](#6-comandos-de-consola)
7. [Combate](#7-combate)
8. [Items e inventario](#8-items-e-inventario)
9. [NPCs](#9-npcs)
10. [Clanes](#10-clanes)
11. [Cheats](#11-cheats)

---

## 1. Introducción

Argentum Online es un juego de rol multijugador en tiempo real (MMORPG) 2D con arquitectura cliente-servidor. Los jugadores crean un personaje, exploran un mundo con múltiples mapas interconectados, combaten criaturas y otros jugadores, acumulan experiencia, oro e ítems, y pueden organizarse en clanes.

---

## 2. Instalación y arranque

### Requisitos

- Linux (Ubuntu/Xubuntu 24.04 recomendado)
- GCC con soporte C++20
- CMake 3.24 o superior

### Instalación

El script `installer.sh` instala todas las dependencias, compila el proyecto y lo instala en el sistema del usuario:

```bash
chmod +x installer.sh
./installer.sh
```

Tras la instalación los archivos quedan en:
- **Lanzadores**: `~/.local/bin/argentum-{client,server}`
- **Assets y datos**: `~/.local/share/argentum/`
- **Configuración**: `~/.config/argentum/config.toml` (servidor) y `client.toml` (cliente)

Asegurate de que `~/.local/bin` esté en tu `PATH`.

### Iniciar el servidor

```bash
argentum-server <PUERTO>
```

Ejemplo: `argentum-server 8080`

### Iniciar el cliente

```bash
argentum-client
```

### Multijugador en red local

1. En la PC del servidor, levantá el servidor y anotá su IP con `hostname -I`.
2. En cada PC cliente, editá `~/.config/argentum/client.toml`:

```toml
[server]
host = "192.168.X.X"
port = "8080"
```

3. Asegurate de que el firewall permita el puerto: `sudo ufw allow 8080`.

---

## 3. Creación de personaje

Al conectarse por primera vez se elige un nombre, una **raza** y una **clase**.

### Razas

| Raza   | Constitución | Inteligencia | Fuerza | Agilidad | Mult. vida | Mult. maná | Recuperación |
|--------|-------------|--------------|--------|----------|-----------|-----------|--------------|
| Humano | 25          | 25           | 5      | 3        | ×1.0      | ×1.0      | ×1.0         |
| Elfo   | 18          | 35           | 4      | 5        | ×0.8      | ×1.3      | ×1.2         |
| Enano  | 35          | 15           | 7      | 1        | ×1.3      | ×0.7      | ×0.8         |
| Gnomo  | 22          | 30           | 3      | 4        | ×0.9      | ×1.2      | ×0.9         |

### Clases

| Clase    | Mult. vida | Mult. maná | Meditación | Medita | Usa magia |
|----------|-----------|-----------|------------|--------|-----------|
| Guerrero | ×1.5      | ×0.0      | ×0.0       | No     | No        |
| Mago     | ×0.8      | ×1.5      | ×2.0       | Sí     | Sí        |
| Clérigo  | ×1.0      | ×1.0      | ×1.5       | Sí     | Sí        |
| Paladín  | ×1.3      | ×0.7      | ×1.0       | Sí     | Sí        |

- **Vida máxima** = `constitución × mult_clase_vida × mult_raza_vida × nivel`
- **Maná máximo** = `inteligencia × mult_clase_maná × mult_raza_maná × nivel`

---

## 4. Interfaz de juego

- **Vista del mundo**: mapa y personajes animados en tiempo real.
- **HUD**: barra de vida, barra de maná, nivel, experiencia y oro.
- **Inventario**: slots con los ítems en bolsa y equipados.
- **Consola/chat**: muestra mensajes del sistema y combate. Se abre con `Enter`.

---

## 5. Controles

| Tecla / Acción                        | Efecto                                    |
|---------------------------------------|------------------------------------------|
| `↑` `↓` `←` `→` / `W` `A` `S` `D`  | Mover al personaje                        |
| Click izquierdo (sobre el mundo)      | Atacar a la criatura o jugador en esa posición |
| Click izquierdo (sobre el inventario) | Equipar o usar el ítem del casillero      |
| `Enter`                               | Abrir la consola para escribir comandos   |
| Rueda del mouse (sobre el chat)       | Desplazar el historial de mensajes        |

Al llegar al borde de un mapa el personaje pasa automáticamente al mapa vecino.

---

## 6. Comandos de consola

Los comandos se escriben abriendo la consola con `Enter`. El número de `<slot>` es la posición del ítem en el inventario (empezando en 0).

### Personaje

| Comando           | Descripción                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| `/meditar`        | Inicia la meditación para recuperar maná (solo clases mágicas). Se interrumpe al moverse o recibir daño. |
| `/equipar <slot>` | Equipa el arma, armadura, casco o escudo del slot indicado. Si es una poción, la consume. |
| `/tomar`          | Recoge del suelo el objeto en la posición del personaje.                    |
| `/tirar <slot>`   | Tira al suelo el objeto del slot indicado.                                  |

### NPCs

Debés estar cerca del NPC para que los comandos funcionen.

| Comando                      | NPC                                | Descripción                                       |
|------------------------------|------------------------------------|---------------------------------------------------|
| `/curar`                     | Sacerdote                          | Restaura la vida y el maná al máximo.             |
| `/resucitar`                 | Sacerdote                          | Revive al personaje si está muerto.               |
| `/listar`                    | Comerciante / Sacerdote / Banquero | Lista los objetos disponibles en el NPC.          |
| `/comprar <objeto>`          | Comerciante / Sacerdote            | Compra el objeto indicado por su nombre.          |
| `/vender <slot>`             | Comerciante                        | Vende el objeto del slot indicado.                |
| `/depositar <slot>`          | Banquero                           | Deposita en el banco el objeto del slot indicado. |
| `/depositar oro <cantidad>`  | Banquero                           | Deposita la cantidad de oro indicada.             |
| `/retirar <slot>`            | Banquero                           | Retira del banco el objeto del slot indicado.     |
| `/retirar oro <cantidad>`    | Banquero                           | Retira la cantidad de oro indicada.               |

### Mensajes privados

| Comando                  | Descripción                                    |
|--------------------------|------------------------------------------------|
| `@<nick> <mensaje>`      | Envía un mensaje privado al jugador indicado.  |

### Clanes

| Comando                   | Descripción                                                  |
|---------------------------|--------------------------------------------------------------|
| `/fundar-clan <nombre>`   | Funda un nuevo clan. El jugador pasa a ser fundador.         |
| `/unirse <nombre>`        | Envía una solicitud de ingreso al clan indicado.             |
| `/revisar-clan`           | Muestra la información y los miembros del clan.              |
| `/clan-aceptar <nick>`    | (Fundador) Acepta la solicitud de un jugador.                |
| `/clan-rechazar <nick>`   | (Fundador) Rechaza la solicitud de un jugador.               |
| `/clan-ban <nick>`        | (Fundador) Banea a un miembro del clan.                      |
| `/clan-kick <nick>`       | (Fundador) Expulsa a un miembro del clan.                    |
| `/dejar-clan`             | Abandona el clan actual.                                     |

---

## 7. Combate

### Atacar

Click izquierdo sobre una entidad para atacarla. El personaje debe tener un arma equipada.

- **Armas cuerpo a cuerpo**: el personaje debe estar adyacente al objetivo.
- **Arcos y báculos**: pueden atacar a distancia.
- **Flauta Élfica**: en lugar de dañar al objetivo, cura al propio atacante.

### Fórmulas

- **Daño base** = `fuerza × tirada(daño_min, daño_max)`
- **Golpe crítico** (20% de probabilidad por defecto): duplica el daño.
- **Defensa** = suma de tiradas de armadura + escudo + casco del defensor.
- **Daño final** = `max(0, daño_base − defensa)`
- **Esquive**: probabilidad basada en la agilidad del defensor; si esquiva, el daño es 0.
- **Bonus de clan**: +5% de daño y defensa por cada compañero de clan presente en el mismo mapa.

### PvP

El combate entre jugadores está restringido:
- Ambos deben tener nivel mayor a 12.
- La diferencia de nivel no puede superar 10.
- No pueden estar en zona segura.
- No pueden pertenecer al mismo clan.

### Criaturas

Las criaturas patrullan el mapa y persiguen a los jugadores cercanos. Al morir pueden dropear oro, pociones, armas o escudos. Al subir de nivel o matar criaturas se obtiene experiencia.

| Criatura         | Nivel | Vida | Daño      |
|------------------|-------|------|-----------|
| Goblin           | 1     | 25   | 2–4       |
| Zombie           | 1     | 35   | 2–5       |
| Araña            | 1     | 15   | 1–3       |
| Goblin Jorobado  | 2     | 60   | 3–7       |
| Esqueleto        | 2     | 70   | 3–7       |
| Araña Blanca     | 2     | 50   | 2–6       |
| Esqueleto Hacha  | 3     | 100  | 5–10      |
| Orco             | 3     | 140  | 5–12      |
| Golem            | 5     | 250  | 8–20      |
| Golem Demoníaco  | 7     | 350  | 12–28     |

### Muerte

Al morir el jugador pierde un porcentaje de su experiencia actual y suelta todos sus ítems al suelo. Aparece como fantasma y debe acercarse a un sacerdote y usar `/resucitar` para volver a jugar.

---

## 8. Items e inventario

### Armas

| Arma             | Tipo          | Descripción                         |
|------------------|---------------|-------------------------------------|
| Espada           | Cuerpo a cuerpo | —                                |
| Hacha            | Cuerpo a cuerpo | —                                |
| Martillo         | Cuerpo a cuerpo | —                                |
| Arco Simple      | Rango         | Ataca a distancia.                  |
| Arco Compuesto   | Rango         | Ataca a distancia, mayor daño.      |
| Vara de Fresno   | Báculo        | Hechizo de ataque.                  |
| Flauta Élfica    | Báculo        | Cura al portador en lugar de dañar. |
| Báculo Nudoso    | Báculo        | Hechizo de ataque.                  |
| Báculo Engarzado | Báculo        | Hechizo de ataque, mayor efecto.    |

### Protección

| Ítem                | Tipo     |
|---------------------|----------|
| Armadura de Cuero   | Armadura |
| Armadura de Placas  | Armadura |
| Túnica Azul         | Armadura |
| Capucha             | Casco    |
| Casco de Hierro     | Casco    |
| Sombrero Mágico     | Casco    |
| Escudo de Tortuga   | Escudo   |
| Escudo de Hierro    | Escudo   |

### Consumibles

- **Poción de Vida**: restaura puntos de vida. Se usa con `/equipar <slot>` o click en el inventario.
- **Poción de Maná**: restaura puntos de maná. Solo es útil para clases que usan magia.

### Precios de compra

El precio de venta (lo que recibe el jugador al vender) es la mitad del precio de compra.

| Ítem               | Precio compra |
|--------------------|--------------|
| Espada             | 40           |
| Hacha              | 70           |
| Martillo           | 90           |
| Arco Simple        | 80           |
| Arco Compuesto     | 350          |
| Vara de Fresno     | 120          |
| Flauta Élfica      | 300          |
| Báculo Nudoso      | 250          |
| Báculo Engarzado   | 600          |
| Armadura de Cuero  | 120          |
| Armadura de Placas | 800          |
| Túnica Azul        | 220          |
| Capucha            | 40           |
| Casco de Hierro    | 120          |
| Sombrero Mágico    | 300          |
| Escudo de Tortuga  | 30           |
| Escudo de Hierro   | 80           |
| Poción de Vida     | 25           |
| Poción de Maná     | 40           |

### Oro

La moneda del juego. Se obtiene matando criaturas, vendiendo ítems y retirando del banco. El jugador tiene un límite de oro según su nivel; el exceso se suelta al morir.

---

## 9. NPCs

Los NPCs están en zonas seguras. Debés estar adyacente para interactuar.

### Sacerdote

- `/curar`: restaura vida y maná al máximo.
- `/resucitar`: revive al personaje muerto.
- `/listar` y `/comprar <objeto>`: vende pociones.

### Comerciante

- `/listar`: muestra los ítems disponibles con sus precios.
- `/comprar <objeto>`: compra un ítem por su nombre.
- `/vender <slot>`: vende el ítem del slot indicado (recibís la mitad del precio base).

### Banquero

- `/depositar <slot>` / `/depositar oro <cantidad>`: guarda ítems u oro en el banco.
- `/retirar <slot>` / `/retirar oro <cantidad>`: recupera ítems u oro del banco.
- Los ítems en el banco **no se pierden** al morir.

---

## 10. Clanes

Los clanes agrupan jugadores. Los miembros del mismo clan no pueden atacarse entre sí y se benefician del bonus grupal en combate (+5% daño y defensa por compañero en el mapa).

Un clan puede tener hasta 16 miembros. El fundador es quien lo administra: acepta solicitudes, expulsa miembros y puede banearlos.

Consultá la sección de [Comandos de consola → Clanes](#clanes-1) para la lista completa de comandos.

---

## 11. Cheats

Los cheats se activan con combinaciones de teclado durante la partida.

| Combinación  | Efecto                                              |
|--------------|-----------------------------------------------------|
| `Ctrl + G`   | Toggle god mode (vida infinita — el personaje no puede morir) |
| `Ctrl + M`   | Toggle maná infinito (los báculos no consumen maná) |
| `Ctrl + D`   | Morir instantáneamente                              |
| `Ctrl + R`   | Resucitar instantáneamente                          |

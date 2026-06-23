# Documentación Técnica

## Arquitectura General del servidor

Argentum Online utiliza una arquitectura cliente-servidor donde la lógica del juego se ejecuta exclusivamente en el servidor. Los clientes actúan como interfaces gráficas encargadas de capturar la entrada del usuario y representar el estado del mundo.

La arquitectura fue diseñada siguiendo el principio de autoridad del servidor (server authoritative), evitando que los clientes puedan modificar directamente el estado del juego.

### Componentes principales

* Cliente SDL2
* Servidor
* Protocolo binario
* Sistema de persistencia

El servidor mantiene el estado completo del mundo, incluyendo personajes, criaturas, NPCs, inventarios, clanes y cuentas bancarias.

---

# Modelo de Concurrencia

El servidor utiliza una arquitectura híbrida:

* Multihilo para la comunicación de red.
* Monohilo para la lógica del juego.

Esto permite atender múltiples clientes simultáneamente sin introducir problemas de sincronización sobre el estado del mundo.

## Acceptor

El hilo Acceptor escucha conexiones entrantes.

Cuando un cliente se conecta:

1. Se acepta el socket.
2. Se crea un ClientHandler.
3. Se asigna un identificador único al jugador.
4. El cliente queda registrado en el monitor de conexiones.

## ClientHandler

Cada cliente conectado posee un ClientHandler.

Internamente contiene:

* Receiver
* Sender

### Receiver

Lee paquetes desde el socket.

Su única responsabilidad es:

* Decodificar paquetes.
* Construir objetos Command.
* Insertarlos en la cola compartida de comandos.

El Receiver nunca modifica el estado del juego.

### Sender

Recibe Snapshots generados por el servidor y los envía al cliente.

El Sender nunca consulta ni modifica el mundo del juego.

## GameLoop

El GameLoop es el núcleo del servidor.

Es el único componente autorizado para modificar el estado del juego.

Su ciclo principal consiste en:

1. Consumir Commands.
2. Ejecutar Game::process().
3. Generar Snapshots.
4. Enviarlos a los clientes.
5. Ejecutar actualizaciones periódicas del mundo.

La centralización de la lógica evita race conditions y simplifica el modelo de concurrencia.

---

# Protocolo de Comunicación

La comunicación entre cliente y servidor utiliza un protocolo binario propio.

Todos los paquetes poseen la siguiente estructura:

* opcode (1 byte)
* payload_size (2 bytes)
* payload (N bytes)

## Commands

Los Commands representan acciones solicitadas por un cliente.

Ejemplos:

* LOGIN
* CREATE_CHARACTER
* MOVE
* ATTACK
* PICK_ITEM
* DROP_ITEM
* BUY_ITEM
* PRIVATE_MESSAGE

El Receiver transforma los paquetes recibidos en objetos Command y los inserta en la cola de comandos.

## Snapshots

Los Snapshots representan eventos o cambios de estado generados por el servidor.

Ejemplos:

* ENTITY_CREATED
* ENTITY_MOVE
* ENTITY_REMOVE
* PLAYER_STATS
* INVENTORY_UPDATE
* DAMAGE_EVENT
* DEATH_EVENT
* CHAT_MESSAGE
* MAP_CHANGE

Los clientes nunca generan Snapshots.

---

# Distribución de Eventos

Los Snapshots se encapsulan dentro de objetos OutgoingSnapshot.

Existen tres modos de distribución:

## Broadcast

Envía el Snapshot a todos los clientes conectados.

Ejemplos:

* Movimiento visible de un personaje.
* Aparición de criaturas.
* Eventos de daño.

## Unicast

Envía el Snapshot a un único cliente.

Ejemplos:

* Mensajes de error.
* Actualización del inventario propio.
* Cambio de mapa.

## Multicast

Envía el Snapshot a un conjunto específico de clientes.

Ejemplos:

* Mensajes privados.

---

# Sistema de Replay

Cuando un jugador entra al juego o cambia de mapa, necesita sincronizarse con el estado actual del mundo.

Para resolver este problema el servidor implementa un sistema de replay.

El replay reconstruye para el cliente:

* Jugadores existentes.
* NPCs.
* Criaturas.
* Objetos en el piso.
* Equipamiento visible de otros personajes.

Esto garantiza que un jugador que entra tarde vea exactamente el mismo estado que el resto.

---

# Persistencia

La persistencia se encuentra desacoplada de la lógica principal mediante una cola de trabajos.

## Objetivos

* Evitar bloqueos del GameLoop.
* Evitar escrituras concurrentes.
* Mantener consistencia de los datos.

## PersistenceJob

Representa una operación de persistencia pendiente.

Los principales tipos son:

* Persistencia de jugador.
* Persistencia de clanes.

## PersistenceWorker

Existe un único PersistenceWorker.

Su responsabilidad es:

1. Consumir PersistenceJobs.
2. Escribir datos en disco.
3. Actualizar índices.

Al existir un único escritor se evita la corrupción de archivos por acceso concurrente.

---

# Archivos Persistidos

## players.bin

Almacena los registros de personajes.

Cada registro contiene:

* Nick
* Raza
* Clase
* Posición
* Nivel
* Vida
* Maná
* Experiencia
* Oro
* Inventario
* Banco

## index.bin

Mantiene una tabla:

nick -> offset

Esto permite acceder a un jugador específico sin recorrer todo el archivo.

## clanes.bin

Almacena:

* Nombre del clan
* Fundador
* Miembros
* Solicitudes pendientes
* Usuarios baneados

---

# Justificación de Diseño

La arquitectura elegida separa claramente:

* Comunicación de red
* Lógica del juego
* Persistencia

Esta división permite:

* Escalar el número de clientes conectados.
* Simplificar el manejo de concurrencia.
* Evitar corrupción de datos.
* Mantener un protocolo consistente entre cliente y servidor.

La lógica centralizada en GameLoop constituye el principal mecanismo para garantizar consistencia en el estado del mundo.

# Arquitectura del Cliente

El cliente es responsable de:

* Capturar la entrada del usuario.
* Comunicarse con el servidor.
* Mantener una representación local del estado del mundo.
* Renderizar la interfaz gráfica.

El cliente no posee lógica autoritativa del juego. Todas las decisiones importantes son tomadas por el servidor.

---

## Componentes principales

### Input

Captura:

* Teclado
* Mouse
* Comandos de consola

Las acciones del usuario son transformadas en objetos Command y enviadas al servidor.

Ejemplos:

* Movimiento
* Ataque
* Compra de objetos
* Mensajes de chat

---

### Cliente de Red

Mantiene una conexión TCP persistente con el servidor.

Posee dos responsabilidades:

#### Sender

Codifica Commands utilizando el protocolo binario y los envía al servidor.

#### Receiver

Recibe Snapshots enviados por el servidor y actualiza el estado local.

---

### Estado del Juego

El cliente mantiene una copia local simplificada del mundo.

Incluye:

* Jugadores visibles
* Criaturas
* NPCs
* Objetos en el suelo
* Inventario
* Estadísticas del personaje

Este estado se actualiza exclusivamente mediante Snapshots provenientes del servidor.

---

### Renderizado

El renderizador utiliza SDL2.

Sus responsabilidades son:

* Dibujar mapas TMX.
* Dibujar personajes.
* Dibujar criaturas.
* Dibujar equipamiento visible.
* Dibujar efectos visuales.
* Dibujar interfaz gráfica.

---

### Sistema de Mapas

Los mapas se cargan desde archivos TMX exportados desde Tiled.

El cliente utiliza estos archivos para:

* Dibujar el terreno.
* Dibujar capas visuales.
* Dibujar decoraciones.

Las colisiones y reglas de juego son responsabilidad del servidor.

---

### Interfaz de Usuario

La interfaz contiene:

* Login.
* Creación de personaje.
* Inventario.
* Chat.
* Consola de comandos.
* Barra de vida.
* Barra de maná.
* Información del personaje.

---

# Sincronización Cliente-Servidor

El cliente nunca modifica el estado real del juego.

Flujo típico:

Usuario presiona W
↓
Cliente envía MOVE
↓
Servidor valida movimiento
↓
Servidor genera ENTITY_MOVE
↓
Cliente actualiza posición

Esto garantiza que todos los jugadores vean un estado consistente del mundo.

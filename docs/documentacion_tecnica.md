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

La comunicación entre el cliente y el servidor se realiza mediante un **protocolo binario propio** sobre una conexión TCP persistente. 

Todos los mensajes, independientemente de su tipo, poseen un encabezado común seguido de un payload cuyo contenido depende del opcode.

## Estructura de un paquete

Cada paquete transmitido tiene el siguiente formato:

```
+------------+----------------+----------------------+
| Opcode     | Payload Size   | Payload              |
| 1 byte     | 2 bytes        | N bytes              |
+------------+----------------+----------------------+
```

donde:

* **Opcode**: identifica el tipo de mensaje enviado.
* **Payload Size**: indica la cantidad de bytes que ocupa el payload.
* **Payload**: contiene los datos específicos del mensaje.

El encabezado permite al receptor conocer qué operación debe ejecutar y cuántos bytes debe leer para reconstruir correctamente el mensaje.

Los valores numéricos se serializan utilizando un tamaño fijo definido por el protocolo. Las cadenas de texto se envían precedidas por su longitud para permitir su reconstrucción sin utilizar caracteres de terminación.

---

# Commands

Los **Commands** representan acciones solicitadas por un cliente.

Cada vez que el jugador realiza una acción (por ejemplo moverse, atacar o enviar un mensaje), el cliente construye un paquete utilizando el opcode correspondiente y los datos necesarios para dicha operación.

Algunos ejemplos de Commands son:

| Command          | Descripción                                         |
| ---------------- | --------------------------------------------------- |
| LOGIN            | Solicita iniciar sesión con un personaje existente. |
| CREATE_CHARACTER | Crea un nuevo personaje.                            |
| MOVE             | Solicita mover el personaje en una dirección.       |
| ATTACK           | Solicita atacar un objetivo.                        |
| PICK_ITEM        | Solicita recoger un objeto del suelo.               |
| DROP_ITEM        | Solicita dejar un objeto en el mapa.                |
| BUY_ITEM         | Solicita comprar un objeto a un NPC comerciante.    |
| PRIVATE_MESSAGE  | Envía un mensaje privado a otro jugador.            |

Cuando un paquete llega al servidor, el **Receiver** realiza las siguientes etapas:

1. Lee el encabezado del paquete.
2. Obtiene el opcode y el tamaño del payload.
3. Deserializa el payload.
4. Construye un objeto `Command`.
5. Inserta el comando en la cola compartida de comandos.

Es importante destacar que el Receiver **no modifica el estado del juego**. Su única responsabilidad consiste en transformar los datos binarios recibidos desde la red en objetos de alto nivel que posteriormente serán procesados por el GameLoop.

---

# Snapshots

Los **Snapshots** representan eventos o cambios de estado generados por el servidor luego de procesar uno o más Commands.

A diferencia de los Commands, los Snapshots son enviados exclusivamente por el servidor y permiten mantener sincronizado el estado del mundo entre todos los clientes conectados.

Algunos ejemplos de Snapshots son:

| Snapshot         | Descripción                                       |
| ---------------- | ------------------------------------------------- |
| ENTITY_CREATED   | Notifica la creación de una nueva entidad.        |
| ENTITY_MOVE      | Informa el desplazamiento de una entidad.         |
| ENTITY_REMOVE    | Indica que una entidad debe eliminarse del mundo. |
| PLAYER_STATS     | Actualiza las estadísticas del jugador.           |
| INVENTORY_UPDATE | Actualiza el inventario del personaje.            |
| DAMAGE_EVENT     | Informa que una entidad recibió daño.             |
| DEATH_EVENT      | Notifica la muerte de una entidad.                |
| CHAT_MESSAGE     | Envía un mensaje de chat.                         |
| MAP_CHANGE       | Indica que el jugador cambió de mapa.             |

Luego de ejecutar `Game::process()`, el servidor genera uno o varios Snapshots representando los cambios producidos sobre el estado del juego.

Estos Snapshots son distribuidos mediante los objetos `OutgoingSnapshot`, que determinan si el mensaje debe enviarse a un único cliente (Unicast), a un conjunto específico de clientes (Multicast) o a todos los jugadores conectados (Broadcast).

---

# Flujo de un mensaje

El siguiente flujo resume el procesamiento de una acción típica del jugador:

```
Jugador
    │
    ▼
Cliente
    │
    ▼
Construcción del Command
    │
    ▼
Receiver del servidor
    │
    ▼
Queue<Command>
    │
    ▼
GameLoop
    │
    ▼
Game::process()
    │
    ▼
Generación de Snapshots
    │
    ▼
Sender
    │
    ▼
Clientes conectados
```

Esta separación entre Commands y Snapshots permite desacoplar completamente la comunicación de red de la lógica del juego. Mientras los Receiver únicamente reciben solicitudes y las encolan, el GameLoop centraliza todas las modificaciones sobre el estado del mundo, evitando problemas de concurrencia y garantizando que todos los clientes observen un estado consistente.


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
  
## Métodos principales

Las siguientes funciones representan los puntos centrales de la arquitectura del proyecto, tanto del servidor como del cliente.

### Servidor

| Clase               | Método                    | Responsabilidad                                                                                                                                    |
| ------------------- | ------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------|
| `GameLoop`          | `run()`                   | Ejecuta el ciclo principal del servidor. Consume los `Command`, invoca `Game::process()`, genera los `Snapshot` correspondientes y los distribuye. |
| `Game`              | `process(const Command&)` | Interpreta cada comando recibido, valida la acción solicitada, modifica el estado del mundo y genera los `Snapshot`.                               |
| `World`             | `movePlayer()`            | Gestiona el movimiento de un personaje, verificando colisiones, límites del mapa y cambios entre mapas antes de actualizar su posición.            |
| `PersistenceWorker` | `run()`                   | Consume los trabajos de persistencia pendientes y actualiza los archivos binarios del juego sin bloquear el `GameLoop`.                            |

### Cliente

| Clase          | Método          | Responsabilidad                                                                                                                                            |
| -------------- | --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ClientGame`   | `run()`         | Ejecuta el ciclo principal del cliente. Procesa los eventos de entrada, actualiza el estado local y coordina el renderizado del juego.                     |
| `InputHandler` | `handleEvent()` | Captura la entrada del usuario, interpreta sus acciones y genera los `Command` que serán enviados al servidor.                                             |
| `Receiver`     | `run()`         | Recibe los `Snapshot` enviados por el servidor, los deserializa y actualiza el estado local del juego para mantener la sincronización con el mundo remoto. |
| `Renderer`     | `render()`      | Renderiza el mapa, las entidades, los efectos visuales y la interfaz gráfica utilizando la información almacenada en el estado local del cliente.          |



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

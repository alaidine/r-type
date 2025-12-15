```text
╰─ cat protocol.txt
```

# PROTOCOLE UDP CUSTOM – R-TYPE

---

## Cycle de vie complet

```text
CLIENT                                    SERVEUR
  │                                          │
  │  1. MSG_CONNECT                         │
  │  ────────────────────────────>          │
  │     [Type: 0]                            │
  │                                          │
  │                    2. Crée le client     │
  │                       Assigne ID         │
  │                       Choisit spawn      │
  │                                          │
  │  3. MSG_CONNECT (réponse)               │
  │  <────────────────────────────           │
  │     [Type: 0, ID: 1, X: 50, Y: 50]      │
  │                                          │
  │  ✅ CONNECTÉ & SPAWNÉ                   │
  │                                          │
  ╠══════════════ GAME LOOP (60 FPS) ═══════════════╣
  │                                          │
  │  4. MSG_UPDATE_STATE (×60/sec)          │
  │  ────────────────────────────>          │
  │     [Type: 1, X, Y, missiles...]        │
  │                                          │
  │                    5. Met à jour état   │
  │                       Update mobs       │
  │                       Check collisions  │
  │                                          │
  │  6. MSG_GAME_STATE (×60/sec)            │
  │  <────────────────────────────           │
  │     [Type: 2, all clients, mobs...]     │
  │                                          │
  │  7. Affiche tout                         │
  │                                          │
  ╠═══════════════════════════════════════════════╣
  │                                          │
  │  8. Déconnexion (timeout/crash)         │
  │  ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─>           │
  │                                          │
  │                    9. Retire le client  │
  │                                          │
  │  10. MSG_GAME_STATE (sans ce client)    │
  │  <────────────────────────────           │
  │     [autres clients voient déco]        │
```

---

## Message 0 – MSG_CONNECT

### Client → Serveur (demande de connexion)

| Champ     | Taille  | Description |
| --------- | ------- | ----------- |
| Type      | 1 byte  | `0x00`      |
| Client ID | 4 bytes | `uint32`    |
| Spawn X   | 4 bytes | `uint32`    |
| Spawn Y   | 4 bytes | `uint32`    |

**Total : 13 bytes**

**Exemple**

```text
[0x00][0x00 0x00 0x00 0x01][0x00 0x00 0x00 0x32][0x00 0x00 0x00 0x32]
 Type        ID=1               X=50               Y=50
```

---

### Serveur → Client (confirmation)

| Champ             | Taille       | Description     |
| ----------------- | ------------ | --------------- |
| Type              | 1 byte       | `0x01`          |
| Position X        | 4 bytes      | `int32`         |
| Position Y        | 4 bytes      | `int32`         |
| Missile Count (N) | 4 bytes      | `uint32`        |
| Missile Data      | N × 36 bytes | Voir ci-dessous |

#### Missile Data (36 bytes)

| Champ         | Taille           |
| ------------- | ---------------- |
| pos.x         | 4 bytes (float)  |
| pos.y         | 4 bytes (float)  |
| rect.x        | 4 bytes (float)  |
| rect.y        | 4 bytes (float)  |
| rect.width    | 4 bytes (float)  |
| rect.height   | 4 bytes (float)  |
| currentFrame  | 4 bytes (uint32) |
| framesSpeed   | 4 bytes (uint32) |
| framesCounter | 4 bytes (uint32) |

**Total : 13 + (N × 36) bytes**

**Exemple (0 missile)**

```text
[0x01][0x00 0x00 0x00 0x64][0x00 0x00 0x00 0x32][0x00 0x00 0x00 0x00]
 Type        X=100               Y=50               0 missiles
```

---

## Message 1 – MSG_UPDATE_STATE

📤 **Client → Serveur (État du joueur)**

| Champ            | Taille          |
| ---------------- | --------------- |
| Type             | 1 byte (`0x02`) |
| Client Count (N) | 4 bytes         |
| Client State     | N × variable    |
| Mob Count (M)    | 4 bytes         |
| Mob State        | M × 12 bytes    |

### Client State

| Champ         | Taille           |
| ------------- | ---------------- |
| client_id     | 4 bytes (uint32) |
| x             | 4 bytes (int32)  |
| y             | 4 bytes (int32)  |
| missile_count | 4 bytes (uint32) |
| missiles      | 36 × count       |

### Mob State

| Champ  | Taille           |
| ------ | ---------------- |
| mob_id | 4 bytes (uint32) |
| x      | 4 bytes (float)  |
| y      | 4 bytes (float)  |

**Exemple (2 clients, 1 mob)**

```text
[0x02]
[0x00 0x00 0x00 0x02]
  [Client 1 data]
  [Client 2 data]
[0x00 0x00 0x00 0x01]
  [Mob ID=10, X=300.0, Y=100.0]
```

---

## Message 2 – MSG_GAME_STATE

📥 **Serveur → Clients (État complet du jeu)**

| Champ | Taille          |
| ----- | --------------- |
| Type  | 1 byte (`0x03`) |
| Code  | 4 bytes         |

**Codes**

* `0` : Server Full
* `1` : Timeout
* `2` : Kicked

**Total : 5 bytes**

---

## Message 3 – MSG_DISCONNECT

📥 **Serveur → Client (notification de déconnexion)**

---

## Récapitulatif des messages

| ID   | Nom              | Direction | Fréquence        |
| ---- | ---------------- | --------- | ---------------- |
| 0x00 | MSG_CONNECT      | C→S, S→C  | Une fois         |
| 0x01 | MSG_UPDATE_STATE | C→S       | 60/sec           |
| 0x02 | MSG_GAME_STATE   | S→C       | 60/sec           |
| 0x03 | MSG_DISCONNECT   | S→C       | À la déconnexion |

---

## Timeline exemple

```text
0ms    Client ouvre socket UDP
1ms    → MSG_CONNECT
2ms    ← MSG_CONNECT (ID + spawn)
16ms   → MSG_UPDATE
17ms   ← MSG_GAME_STATE
...
5000ms Client crash
5016ms ← MSG_GAME_STATE (sans le client)
```

---

## Paramètres globaux

| Paramètre    | Valeur           |
| ------------ | ---------------- |
| Transport    | UDP (SOCK_DGRAM) |
| Port         | 4242             |
| Endianness   | Little-endian    |
| Tick Rate    | 60 Hz            |
| Max Clients  | 4                |
| Max Missiles | 32 / client      |
| Max Mobs     | 50               |
| Fiabilité    | Aucune           |
| Ordre        | Non garanti      |
| Broadcast    | 60/sec           |


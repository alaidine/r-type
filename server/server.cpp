#include "server.h"
#include <csignal>
#include <cstdlib>
#include <cstring>

#include "MovementStrategies/CircularStrategy.hpp"
#include "MovementStrategies/DiveStrategy.hpp"
#include "MovementStrategies/Figure8Strategy.hpp"
#include "MovementStrategies/LinearStrategy.hpp"
#include "MovementStrategies/SinusoidalStrategy.hpp"
#include "MovementStrategies/ZigZagStrategy.hpp"

Server *g_serverInstance = nullptr;

static void SigintHandler(int dummy) {
  (void)dummy;
  g_serverInstance->running = false;
}

Server::Server() {
  g_serverInstance = this;
  signal(SIGINT, SigintHandler);
  m_spawns = {{50, 50},
              {GAME_WIDTH - 100, 50},
              {50, GAME_HEIGHT - 100},
              {GAME_WIDTH - 100, GAME_HEIGHT - 100}};
  tick_dt = 1.0f / TICK_RATE;

  // Initialize mobs
  m_mobCount = 0;
  m_nextMobId = 0;
  for (auto &mob : m_mobs) {
    mob.active = false;
  }

  // Initialize wave manager
  m_waveManager.Init();
}

Server::~Server() {}

bool Server::AddressEquals(const sockaddr_in &a, const sockaddr_in &b) {
  return a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port;
}

ConnectedClient *Server::FindClientByAddress(const sockaddr_in &addr) {
  for (auto &[id, client] : m_clients) {
    if (AddressEquals(client.address, addr))
      return &client;
  }
  return nullptr;
}

uint32_t Server::GetClientIdByAddress(const sockaddr_in &addr) {
  ConnectedClient *client = FindClientByAddress(addr);
  return client ? client->client_id : 0;
}

void Server::HandleConnectRequest(const sockaddr_in &from) {
  TraceLog(LOG_INFO, "New connection request");

  // Check if already connected
  if (FindClientByAddress(from) != nullptr) {
    TraceLog(LOG_INFO, "Client already connected, ignoring");
    return;
  }

  // Check if server is full
  if (m_clients.size() >= MAX_CLIENTS) {
    TraceLog(LOG_INFO, "Server full, rejecting connection");

    NetBuffer rejectBuffer;
    rejectBuffer.WriteUInt8(MSG_CONNECT_REJECT);
    rejectBuffer.WriteInt32(SERVER_FULL_CODE);
    m_netServer->SendTo(rejectBuffer, from);
    return;
  }

  // Create new client
  uint32_t client_id = m_nextClientId++;
  Vector2 spawn = m_spawns[client_id % MAX_CLIENTS];

  ConnectedClient newClient;
  newClient.client_id = client_id;
  newClient.address = from;
  newClient.state.client_id = client_id;
  newClient.state.x = (int)spawn.x;
  newClient.state.y = (int)spawn.y;
  newClient.state.missile_count = 0;
  memset(newClient.state.missiles, 0, sizeof(newClient.state.missiles));
  newClient.last_heard_tick = m_currentTick;

  m_clients[client_id] = newClient;

  // Send accept message
  NetBuffer acceptBuffer;
  acceptBuffer.WriteUInt8(MSG_CONNECT_ACCEPT);

  ConnectAcceptData acceptData;
  acceptData.client_id = client_id;
  acceptData.spawn_x = (int)spawn.x;
  acceptData.spawn_y = (int)spawn.y;
  SerializeConnectAcceptData(acceptBuffer, acceptData);

  m_netServer->SendTo(acceptBuffer, from);

  TraceLog(LOG_INFO, "Connection accepted (ID: %d)", client_id);
}

void Server::HandleDisconnect(uint32_t client_id) {
  auto it = m_clients.find(client_id);
  if (it != m_clients.end()) {
    TraceLog(LOG_INFO, "Client disconnected (ID: %d)", client_id);
    m_clients.erase(it);
  }
}

void Server::HandleUpdateStateMessage(NetBuffer &buffer, uint32_t client_id) {
  auto it = m_clients.find(client_id);
  if (it == m_clients.end())
    return;

  UpdateStateMessage msg = DeserializeUpdateStateMessage(buffer);

  // Update client state
  it->second.state.x = msg.x;
  it->second.state.y = msg.y;
  it->second.state.missile_count = msg.missile_count;
  memset(it->second.state.missiles, 0, sizeof(it->second.state.missiles));
  memcpy(it->second.state.missiles, msg.missiles,
         msg.missile_count * sizeof(Missile));
  it->second.last_heard_tick = m_currentTick;
}

void Server::HandleReceivedMessage(NetBuffer &buffer, const sockaddr_in &from) {
  if (!buffer.CanRead(1))
    return;

  uint8_t msgType = buffer.ReadUInt8();

  switch (msgType) {
  case MSG_CONNECT_REQUEST:
    HandleConnectRequest(from);
    break;

  case MSG_DISCONNECT: {
    uint32_t client_id = GetClientIdByAddress(from);
    if (client_id != 0)
      HandleDisconnect(client_id);
    break;
  }

  case MSG_UPDATE_STATE: {
    uint32_t client_id = GetClientIdByAddress(from);
    if (client_id != 0) {
      HandleUpdateStateMessage(buffer, client_id);
    }
    break;
  }

  case MSG_HEARTBEAT: {
    uint32_t client_id = GetClientIdByAddress(from);
    if (client_id != 0) {
      auto it = m_clients.find(client_id);
      if (it != m_clients.end())
        it->second.last_heard_tick = m_currentTick;
    }
    break;
  }
  }
}

void Server::CheckClientTimeouts(void) {
  std::vector<uint32_t> toRemove;

  for (auto &[id, client] : m_clients) {
    if (m_currentTick - client.last_heard_tick > CLIENT_TIMEOUT_TICKS) {
      TraceLog(LOG_INFO, "Client timed out (ID: %d)", id);
      toRemove.push_back(id);
    }
  }

  for (uint32_t id : toRemove) {
    m_clients.erase(id);
  }
}

std::unique_ptr<IMovementStrategy>
Server::CreateStrategy(MovementPattern pattern, float startX, float startY) {
  float baseSpeed = 80.0f;

  switch (pattern) {
  case MovementPattern::LINEAR:
    return std::make_unique<LinearStrategy>(baseSpeed, 0.0f, startX, startY);
  case MovementPattern::SINUSOIDAL:
    return std::make_unique<SinusoidalStrategy>(baseSpeed, 60.0f, 3.0f, startY,
                                                startX);
  case MovementPattern::ZIGZAG:
    return std::make_unique<ZigZagStrategy>(baseSpeed, 50.0f, 4.0f, startX,
                                            startY);
  case MovementPattern::CIRCULAR:
    return std::make_unique<CircularStrategy>(baseSpeed * 0.8f, 40.0f, 4.0f,
                                              startX, startY);
  case MovementPattern::DIVE:
    return std::make_unique<DiveStrategy>(baseSpeed, 120.0f, 3.0f, startX,
                                          startY);
  case MovementPattern::FIGURE8:
    return std::make_unique<Figure8Strategy>(baseSpeed * 0.6f, 50.0f, 30.0f,
                                             2.5f, startX, startY);
  default:
    return std::make_unique<LinearStrategy>(baseSpeed, 0.0f, startX, startY);
  }
}

void Server::SpawnMobAtPosition(MovementPattern pattern, float yPosition) {
  for (unsigned int i = 0; i < MAX_MOBS; i++) {
    if (!m_mobs[i].active) {
      float startX = (float)GAME_WIDTH;
      float startY = yPosition;

      // Clamp Y position to valid range
      if (startY < 0)
        startY = 0;
      if (startY > GAME_HEIGHT - MOB_HEIGHT)
        startY = (float)(GAME_HEIGHT - MOB_HEIGHT);

      m_mobs[i].mob_id = this->m_nextMobId++;
      m_mobs[i].x = startX;
      m_mobs[i].y = startY;
      m_mobs[i].active = true;
      m_mobs[i].spawnTime = this->m_totalTime;
      m_mobs[i].pattern = pattern;
      m_mobs[i].strategy = CreateStrategy(pattern, startX, startY);
      this->m_mobCount++;

      const char *patternNames[] = {"LINEAR",   "SINUSOIDAL", "ZIGZAG",
                                    "CIRCULAR", "DIVE",       "FIGURE8"};
      TraceLog(LOG_INFO,
               "Spawned mob (ID: %d) with pattern %s at position (%f, %f)",
               m_mobs[i].mob_id, patternNames[static_cast<int>(pattern)],
               m_mobs[i].x, m_mobs[i].y);
      break;
    }
  }
}

void Server::UpdateMobs(void) {
  this->m_totalTime += this->tick_dt;

  // Only update wave manager if there are connected clients
  if (!m_clients.empty()) {
    // Update wave manager - spawns mobs based on wave definitions
    this->m_waveManager.Update(this->tick_dt,
                               [this](MovementPattern pattern, float yPos) {
                                 SpawnMobAtPosition(pattern, yPos);
                               });
  }

  for (unsigned int i = 0; i < MAX_MOBS; i++) {
    if (m_mobs[i].active && m_mobs[i].strategy) {
      float mobTime = this->m_totalTime - m_mobs[i].spawnTime;
      Position currentPos = {m_mobs[i].x, m_mobs[i].y};
      Position newPos = m_mobs[i].strategy->update(currentPos, mobTime);

      m_mobs[i].x = newPos.x;
      m_mobs[i].y = newPos.y;

      if (m_mobs[i].x < -MOB_WIDTH || m_mobs[i].x > GAME_WIDTH + 100 ||
          m_mobs[i].y < -MOB_HEIGHT || m_mobs[i].y > GAME_HEIGHT + MOB_HEIGHT) {
        m_mobs[i].active = false;
        m_mobs[i].strategy.reset();
        this->m_mobCount--;
        TraceLog(LOG_INFO, "Mob (ID: %d) went off screen", m_mobs[i].mob_id);
      }
    }
  }
}

void Server::CheckMissileCollisions(void) {
  for (auto &[id, client] : m_clients) {
    for (unsigned int m = 0; m < client.state.missile_count; m++) {
      Missile &missile = client.state.missiles[m];
      Rectangle missileRect = {missile.pos.x, missile.pos.y, missile.rect.width,
                               missile.rect.height};

      for (unsigned int i = 0; i < MAX_MOBS; i++) {
        if (m_mobs[i].active) {
          Rectangle mobRect = {m_mobs[i].x, m_mobs[i].y, (float)MOB_WIDTH,
                               (float)MOB_HEIGHT};

          if (missileRect.x < mobRect.x + mobRect.width &&
              missileRect.x + missileRect.width > mobRect.x &&
              missileRect.y < mobRect.y + mobRect.height &&
              missileRect.y + missileRect.height > mobRect.y) {
            m_mobs[i].active = false;
            m_mobCount--;
            TraceLog(LOG_INFO, "Mob (ID: %d) destroyed by client %d",
                     m_mobs[i].mob_id, client.client_id);
          }
        }
      }
    }
  }
}

int Server::BroadcastGameState(void) {
  if (m_clients.empty())
    return 0;

  // Build game state message
  GameStateMessage gameState;
  gameState.client_count = 0;

  for (auto &[id, client] : m_clients) {
    if (gameState.client_count < MAX_CLIENTS) {
      gameState.client_states[gameState.client_count] = client.state;
      gameState.client_count++;
    }
  }

  // Build mob states
  gameState.mob_count = 0;
  for (unsigned int i = 0; i < MAX_MOBS; i++) {
    if (m_mobs[i].active && gameState.mob_count < MAX_MOBS) {
      gameState.mobs[gameState.mob_count].mob_id = m_mobs[i].mob_id;
      gameState.mobs[gameState.mob_count].x = m_mobs[i].x;
      gameState.mobs[gameState.mob_count].y = m_mobs[i].y;
      gameState.mobs[gameState.mob_count].active = true;
      gameState.mob_count++;
    }
  }

  // Wave system info
  gameState.countdown_timer = this->m_waveManager.GetCountdownTimer();
  gameState.current_wave =
      this->m_waveManager.GetCurrentWaveIndex() + 1; // 1-indexed for display
  gameState.wave_active = this->m_waveManager.IsWaveActive();

  // Send to all clients
  for (auto &[id, client] : m_clients) {
    NetBuffer buffer;
    buffer.WriteUInt8(MSG_GAME_STATE);
    SerializeGameStateMessage(buffer, gameState);
    m_netServer->SendTo(buffer, client.address);
  }

  return 0;
}

void Server::Run(void) {
  NetBuffer recvBuffer;
  sockaddr_in from;

  while (running) {
    // Receive and process messages
    while (m_netServer->Receive(recvBuffer, from) > 0) {
      HandleReceivedMessage(recvBuffer, from);
      recvBuffer.Clear();
    }

    // Check for client timeouts
    CheckClientTimeouts();

    // Update mobs
    UpdateMobs();

    // Check missile-mob collisions
    CheckMissileCollisions();

    // Broadcast game state
    if (BroadcastGameState() < 0) {
      TraceLog(LOG_ERROR, "Error broadcasting game states. Exit");
      break;
    }

    m_currentTick++;

    // Cap simulation rate
#if defined(_WIN32) || defined(_WIN64)
    Sleep((DWORD)(tick_dt * 1000));
#else
    long nanos = (long)(tick_dt * 1e9);
    struct timespec t = {.tv_sec = nanos / 999999999,
                         .tv_nsec = nanos % 999999999};
    nanosleep(&t, &t);
#endif
  }
}

void Server::Init(int argc, char **argv) {
  if (ReadCommandLine(argc, argv)) {
    printf("Usage: r-type_server [--packet_loss <value>] [--packet_duplication "
           "<value>] [--ping <value>]"
           " [--jitter <value>]\n");
    return;
  }

  SetTraceLogLevel(LOG_DEBUG);

  m_netServer = std::make_unique<NetServer>(PORT);
  if (!m_netServer->Start()) {
    TraceLog(LOG_ERROR, "Failed to start server on port %d", PORT);
    running = false;
    return;
  }

  TraceLog(LOG_INFO, "Server started on port %d", PORT);
}

int main(int argc, char *argv[]) {
  std::unique_ptr<Server> server = std::make_unique<Server>();

  server->Init(argc, argv);
  server->Run();
  return 0;
}

#include "client.h"
#include <algorithm>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

Client::Client()
{
    m_clientInitialized = false;
    m_connected = false;
    m_disconnected = false;
    m_spawned = false;
    m_serverCloseCode = 0;
    m_localClientId = 0;
    m_heartbeatTimer = 0;
    m_currentScreen = TITLE;
    m_updatedIds = { 0 };
    m_clientCount = 0;
    m_fireMissileKeyPressed = false;
    m_tickDt = 1.0f / TICK_RATE;
    m_acc = 0;
    m_letterCount = 0;
    m_textBox = { GAME_WIDTH / 2.0f - 170, 180, 365, 50 };
    m_framesCounter = 0;
    m_displayHUD = false;
    m_player = { 0 };
    m_background = { 0 };
    m_mob = { 0 };
    m_localPlayerEntity = 0;

    m_missileAnimationRectangles[0] = { 0, 128, 25, 22 };
    m_missileAnimationRectangles[1] = { 25, 128, 31, 22 };
    m_missileAnimationRectangles[2] = { 56, 128, 40, 22 };
    m_missileAnimationRectangles[3] = { 96, 128, 55, 22 };
    m_missileAnimationRectangles[4] = { 151, 128, 72, 22 };

    memset(m_serverIp, 0, MAX_INPUT_CHARS + 1);

    m_mobBox = { 22, 115, 33, 29 };
}

Client::~Client()
{
    UnloadTexture(m_player);
    UnloadTexture(m_background);
    UnloadTexture(m_mob);
    
    // Send disconnect message if connected
    if (m_clientInitialized && m_connected && !m_disconnected)
    {
        NetBuffer buffer;
        buffer.WriteUInt8(MSG_DISCONNECT);
        m_netClient->Send(buffer);
    }
    
    CloseWindow();
}

void Client::InitECS(void)
{
    m_ecsCore.Init();

    MiniBuilder::RegisterComponentBuilder registerBuilder;
    registerBuilder.RegisterComponents<
        Position,
        Sprite,
        PlayerSprite,
        AnimationComponent,
        InputController,
        Tag,
        MissileTag,
        playerCooldown,
        NetworkedClient,
        LocalPlayerTag,
        RemotePlayerTag
    >(m_ecsCore);

    m_clientRendererSystem = m_ecsCore.RegisterSystem<ClientRendererSystem>();
    m_clientRendererSystem->order = 1;
    m_clientRendererSystem->SetPlayerTexture(&m_player);

    Signature clientRendererSignature;
    clientRendererSignature.set(m_ecsCore.GetComponentType<Position>(), true);
    clientRendererSignature.set(m_ecsCore.GetComponentType<Sprite>(), true);
    clientRendererSignature.set(m_ecsCore.GetComponentType<AnimationComponent>(), true);
    clientRendererSignature.set(m_ecsCore.GetComponentType<NetworkedClient>(), true);
    m_ecsCore.SetSystemSignature<ClientRendererSystem>(clientRendererSignature);
}

void Client::SpawnLocalClient(int x, int y, uint32_t client_id)
{
    TraceLog(LOG_INFO, "Received spawn message, position: (%d, %d), client id: %d", x, y, client_id);

    m_localClientId = client_id;

    m_localPlayerEntity = Prefab::MakeClient(m_ecsCore, (float)x, (float)y, client_id, true, m_player);
    m_clientEntities[client_id] = m_localPlayerEntity;

    m_spawned = true;
}

void Client::HandleConnectAccept(NetBuffer& buffer)
{
    ConnectAcceptData data = DeserializeConnectAcceptData(buffer);
    
    TraceLog(LOG_INFO, "Connection accepted by server");
    
    SpawnLocalClient(data.spawn_x, data.spawn_y, data.client_id);
    m_connected = true;
}

void Client::HandleConnectReject(NetBuffer& buffer)
{
    int code = buffer.ReadInt32();
    
    TraceLog(LOG_INFO, "Connection rejected by server (code: %d)", code);
    
    m_disconnected = true;
    m_serverCloseCode = code;
}

bool Client::ClientExists(uint32_t client_id)
{
    return m_clientEntities.find(client_id) != m_clientEntities.end();
}

void Client::CreateClient(ClientState state)
{
    TraceLog(LOG_DEBUG, "CreateClient %d", state.client_id);
    assert(m_clientCount < MAX_CLIENTS - 1);

    Entity clientEntity = Prefab::MakeClient(m_ecsCore, (float)state.x, (float)state.y, state.client_id, false, m_player);
    m_clientEntities[state.client_id] = clientEntity;

    m_clientCount++;

    TraceLog(LOG_INFO, "New remote client (ID: %d)", state.client_id);
}

void Client::UpdateClient(ClientState state)
{
    auto it = m_clientEntities.find(state.client_id);
    if (it == m_clientEntities.end()) {
        TraceLog(LOG_WARNING, "UpdateClient: client_id %d not found", state.client_id);
        return;
    }

    Entity entity = it->second;

    auto& pos = m_ecsCore.GetComponent<Position>(entity);
    pos.position.x = (float)state.x;
    pos.position.y = (float)state.y;

    m_remoteMissiles[state.client_id].clear();
    for (unsigned int i = 0; i < state.missile_count; i++)
    {
        m_remoteMissiles[state.client_id].push_back(state.missiles[i]);
    }
}

void Client::DestroyClient(uint32_t client_id)
{
    auto it = m_clientEntities.find(client_id);
    if (it != m_clientEntities.end())
    {
        TraceLog(LOG_INFO, "Destroy disconnected client (ID: %d)", client_id);

        m_ecsCore.DestroyEntity(it->second);
        m_clientEntities.erase(it);
        m_remoteMissiles.erase(client_id);
        m_clientCount--;
    }
}

void Client::DestroyDisconnectedClients(void)
{
    std::vector<uint32_t> toDestroy;

    for (auto& [client_id, entity] : m_clientEntities)
    {
        if (client_id == m_localClientId)
            continue;

        bool disconnected = true;

        for (int j = 0; j < MAX_CLIENTS; j++)
        {
            if ((int)client_id == m_updatedIds[j])
            {
                disconnected = false;
                break;
            }
        }

        if (disconnected)
            toDestroy.push_back(client_id);
    }

    for (uint32_t client_id : toDestroy)
    {
        DestroyClient(client_id);
    }
}

void Client::HandleGameStateMessage(NetBuffer& buffer)
{
    if (!m_spawned)
        return;

    GameStateMessage msg = DeserializeGameStateMessage(buffer);

    for (int i = 0; i < MAX_CLIENTS; i++)
        m_updatedIds[i] = -1;

    for (unsigned int i = 0; i < msg.client_count; i++)
    {
        ClientState state = msg.client_states[i];

        if (state.client_id != m_localClientId)
        {
            if (ClientExists(state.client_id))
                UpdateClient(state);
            else
                CreateClient(state);

            m_updatedIds[i] = state.client_id;
        }
        else
        {
            if (ClientExists(m_localClientId))
            {
                // Correct the local player position with the position sent by the server.
                auto it = m_clientEntities.find(m_localClientId);
                if (it == m_clientEntities.end()) {
                    TraceLog(LOG_WARNING, "UpdateClient: local client not found");
                    return;
                }

                Entity entity = it->second;

                auto& pos = m_ecsCore.GetComponent<Position>(entity);

                TraceLog(
                    LOG_INFO, "Position after correction: x: %d, y %d, Position before correction x: %d, y: %d",
                    state.x,
                    state.y,
                    (int)pos.position.x,
                    (int)pos.position.y);

                pos.position.x = (float)state.x;
                pos.position.y = (float)state.y;

                m_remoteMissiles[m_localClientId].clear();
                for (unsigned int i = 0; i < state.missile_count; i++)
                {
                    m_remoteMissiles[m_localClientId].push_back(state.missiles[i]);
                }
            }
        }
    }

    m_mobs.clear();
    for (unsigned int i = 0; i < msg.mob_count; i++)
    {
        m_mobs.push_back(msg.mobs[i]);
    }

    DestroyDisconnectedClients();
}

void Client::HandleReceivedMessages(void)
{
    NetBuffer buffer;
    
    while (m_netClient->Receive(buffer) > 0)
    {
        if (!buffer.CanRead(1))
        {
            buffer.Clear();
            continue;
        }

        uint8_t msgType = buffer.ReadUInt8();

        switch (msgType)
        {
        case MSG_CONNECT_ACCEPT:
            HandleConnectAccept(buffer);
            break;

        case MSG_CONNECT_REJECT:
            HandleConnectReject(buffer);
            break;

        case MSG_GAME_STATE:
            HandleGameStateMessage(buffer);
            break;
        }

        buffer.Clear();
    }
}

int Client::SendConnectRequest(void)
{
    NetBuffer buffer;
    buffer.WriteUInt8(MSG_CONNECT_REQUEST);
    
    if (!m_netClient->Send(buffer))
    {
        TraceLog(LOG_WARNING, "Failed to send connect request");
        return -1;
    }
    
    return 0;
}

int Client::SendPositionUpdate(void)
{
    if (!m_connected || m_disconnected)
        return 0;

    NetBuffer buffer;
    buffer.WriteUInt8(MSG_UPDATE_STATE);

    auto& pos = m_ecsCore.GetComponent<Position>(m_localPlayerEntity);

    UpdateStateMessage msg;
    msg.x = (int)pos.position.x;
    msg.y = (int)pos.position.y;
    msg.missile_count = std::min((unsigned int)m_missiles.size(), (unsigned int)MAX_MISSILES_CLIENT);
    memcpy(msg.missiles, m_missiles.data(), msg.missile_count * sizeof(Missile));

    SerializeUpdateStateMessage(buffer, msg);

    if (!m_netClient->Send(buffer))
        return -1;

    return 0;
}

void Client::SendHeartbeat(void)
{
    if (!m_connected || m_disconnected)
        return;

    NetBuffer buffer;
    buffer.WriteUInt8(MSG_HEARTBEAT);
    m_netClient->Send(buffer);
}

int Client::UpdateGameplay(void)
{
    if (!m_spawned)
        return 0;

    auto& pos = m_ecsCore.GetComponent<Position>(m_localPlayerEntity);

    if (IsKeyDown(KEY_SPACE) && !m_fireMissileKeyPressed)
    {
        m_fireMissileKeyPressed = true;
        TraceLog(LOG_INFO, "Firing missile");
        Fire();
    }

    if (IsKeyUp(KEY_SPACE))
        m_fireMissileKeyPressed = false;

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        pos.position.y = MAX(0, pos.position.y - 5);
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        pos.position.y = MIN(GAME_HEIGHT - 50, pos.position.y + 5);

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        pos.position.x = MAX(0, pos.position.x - 5);
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        pos.position.x = MIN(GAME_WIDTH - 50, pos.position.x + 5);

    if (SendPositionUpdate() < 0)
    {
        TraceLog(LOG_WARNING, "Failed to send client state update");
        return -1;
    }

    // Send heartbeat every 60 ticks
    m_heartbeatTimer++;
    if (m_heartbeatTimer >= 60)
    {
        SendHeartbeat();
        m_heartbeatTimer = 0;
    }

    return 0;
}

void Client::DrawClient(Entity entity)
{
    m_clientRendererSystem->RenderClient(m_ecsCore, entity);
}

void Client::DrawHUD(void)
{
    DrawText(TextFormat("FPS: %d", GetFPS()), 450, 350, 32, MAROON);
    DrawText(TextFormat("Client ID: %d", m_localClientId), 450, 400, 32, MAROON);
    DrawText(TextFormat("Connected: %s", m_connected ? "Yes" : "No"), 450, 450, 32, MAROON);
}

void Client::DrawBackground(void)
{
    float frameWidth = GAME_WIDTH / 4;
    float frameHeight = GAME_HEIGHT / 4;
    Rectangle source_rect = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };
    Rectangle dest_rect = { 0.0f , 0.0f, GAME_WIDTH, GAME_HEIGHT};
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(m_background, source_rect, dest_rect, origin, 0.0f, WHITE);
}

void Client::DrawGameplay(void)
{
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    if (m_disconnected)
    {
        if (m_serverCloseCode == SERVER_FULL_CODE)
        {
            DrawText("Cannot connect, server is full", 265, 280, 20, RED);
        }
        else
        {
            DrawText("Connection to the server was lost", 265, 280, 20, RED);
        }
    }
    else if (m_connected && m_spawned)
    {
        DrawBackground();

        for (auto& [client_id, entity] : m_clientEntities)
        {
            DrawClient(entity);
        }

        DrawMissiles();
        DrawMobs();

        if (m_displayHUD)
        {
            DrawHUD();
        }
    }
    else
    {
        DrawText("Connecting to server...", 265, 280, 20, RED);
    }

    EndDrawing();
}

void Client::UpdateAndDraw(void)
{
    switch (m_currentScreen)
    {
    case TITLE:
    {
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        {
            m_currentScreen = IP_ADDRESS;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("R-Type", 190, 200, 20, DARKGRAY);
        EndDrawing();
    } break;

    case IP_ADDRESS:
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitClient(m_serverIp);
            m_currentScreen = GAMEPLAY;
        }

        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();

        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (m_letterCount < MAX_INPUT_CHARS))
            {
                m_serverIp[m_letterCount] = (char)key;
                m_serverIp[m_letterCount + 1] = '\0';
                m_letterCount++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            m_letterCount--;
            if (m_letterCount < 0) m_letterCount = 0;
            m_serverIp[m_letterCount] = '\0';
        }

        m_framesCounter++;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(m_textBox, LIGHTGRAY);
        DrawRectangleLines((int)m_textBox.x, (int)m_textBox.y, (int)m_textBox.width, (int)m_textBox.height, RED);

        DrawText(m_serverIp, (int)m_textBox.x + 5, (int)m_textBox.y + 8, 40, MAROON);
        DrawText(TextFormat("INPUT CHARS: %i/%i", m_letterCount, MAX_INPUT_CHARS), 315, 250, 20, DARKGRAY);

        if (m_letterCount < MAX_INPUT_CHARS)
        {
            if (((m_framesCounter / 20) % 2) == 0)
                DrawText("_", (int)m_textBox.x + 8 + MeasureText(m_serverIp, 40), (int)m_textBox.y + 12, 40, MAROON);
        }
        else
            DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);

        EndDrawing();
    } break;

    case GAMEPLAY:
    {
        m_acc += GetFrameTime();

        while (m_acc >= m_tickDt)
        {
            // Handle received messages
            HandleReceivedMessages();

            // Send connect request if not connected yet
            if (!m_connected && !m_disconnected)
            {
                SendConnectRequest();
            }

            if (m_connected && !m_disconnected)
            {
                if (UpdateGameplay() < 0)
                    break;
            }

            m_acc -= m_tickDt;
        }

        DrawGameplay();
    } break;

    default: break;
    }
}

void Client::InitClient(char* serverIp)
{
    m_netClient = std::make_unique<NetClient>();
    
    if (!m_netClient->Connect(serverIp, PORT))
    {
        TraceLog(LOG_WARNING, "Failed to connect to server at %s:%d", serverIp, PORT);
        m_disconnected = true;
        m_serverCloseCode = -1;
        return;
    }

    TraceLog(LOG_INFO, "Connecting to server at %s:%d", serverIp, PORT);
    m_clientInitialized = true;
}

void Client::Init(void)
{
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(GAME_WIDTH, GAME_HEIGHT, "R-Type");
    SetTargetFPS(TARGET_FPS);

    m_player = LoadTexture("resources/sprites/player_r-9c_war-head.png");
    m_background = LoadTexture("resources/sprites/space_background.png");
    m_mob = LoadTexture("resources/sprites/mob_bydo_minions.png");

    InitECS();
}

void Client::Run(void)
{
    while (!WindowShouldClose())
    {
        UpdateAndDraw();
    }
}

void Client::Fire(void)
{
    auto& playerPos = m_ecsCore.GetComponent<Position>(m_localPlayerEntity);

    Missile missile = {
        .pos = { 0 },
        .rect = { 0 },
        .currentFrame = 0,
        .framesSpeed = 8,
        .framesCounter = 0
    };

    missile.pos.x = playerPos.position.x;
    missile.pos.y = playerPos.position.y;
    missile.rect.x = m_missileAnimationRectangles[missile.currentFrame].x;
    missile.rect.y = m_missileAnimationRectangles[missile.currentFrame].y;
    missile.rect.height = m_missileAnimationRectangles[missile.currentFrame].height;
    missile.rect.width = m_missileAnimationRectangles[missile.currentFrame].width;
    m_missiles.push_back(missile);
}

void Client::DrawMissiles(void)
{
    m_missiles.erase(std::remove_if(m_missiles.begin(), m_missiles.end(),
        [](Missile missile) {
            if (missile.pos.x > GAME_WIDTH)
            {
                TraceLog(LOG_INFO, "Missile out of map... Deleting missile...");
                return true;
            }
            return false;
        }), m_missiles.end());

    for (Missile& missile : m_missiles)
    {
        missile.framesCounter++;

        if (missile.framesCounter >= (TARGET_FPS / missile.framesSpeed))
        {
            missile.framesCounter = 0;
            missile.currentFrame++;
            if (missile.currentFrame > 4) missile.currentFrame = 0;
            missile.rect.x = m_missileAnimationRectangles[missile.currentFrame].x;
            missile.rect.y = m_missileAnimationRectangles[missile.currentFrame].y;
            missile.rect.height = m_missileAnimationRectangles[missile.currentFrame].height;
            missile.rect.width = m_missileAnimationRectangles[missile.currentFrame].width;
        }

        missile.pos.x += 5;

        float frameWidth = missile.rect.width;
        float frameHeight = missile.rect.height;
        Rectangle sourceRec = { missile.rect.x, missile.rect.y, frameWidth, frameHeight };
        Rectangle destRec = { (float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f };
        Vector2 origin = { 0.0f, 0.0f };

        DrawTexturePro(m_player, sourceRec, destRec, origin, 0.0f, WHITE);
    }

    for (auto& [client_id, missiles] : m_remoteMissiles)
    {
        for (const Missile& missile : missiles)
        {
            float frameWidth = missile.rect.width;
            float frameHeight = missile.rect.height;
            Rectangle sourceRec = { missile.rect.x, missile.rect.y, frameWidth, frameHeight };
            Rectangle destRec = { (float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(m_player, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
}

void Client::DrawMobs(void)
{
    for (const MobState& mob : m_mobs)
    {
        if (mob.active)
        {
            float frameWidth = m_mobBox.width;
            float frameHeight = m_mobBox.height;
            Rectangle sourceRec = { m_mobBox.x, m_mobBox.y, frameWidth, frameHeight };
            Rectangle destRec = { mob.x, mob.y, (float)MOB_WIDTH, (float)MOB_HEIGHT };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(m_mob, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    Client client;

    client.Init();
    client.Run();
    return 0;
}

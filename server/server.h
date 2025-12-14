#pragma once

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>

#include "shared.h"

// A simple structure to represent connected clients
struct ConnectedClient
{
	uint32_t client_id;
	sockaddr_in address;
	ClientState state;
	unsigned int last_heard_tick; // For timeout detection
};

// Mob structure for server-side management
typedef struct
{
	uint32_t mob_id;
	float x;
	float y;
	bool active;
} Mob;

class Server
{
private:
	std::unique_ptr<NetServer> m_netServer;
	std::unordered_map<uint32_t, ConnectedClient> m_clients;
	uint32_t m_nextClientId = 1;
	unsigned int m_currentTick = 0;

	// Mob management
	std::array<Mob, MAX_MOBS> m_mobs;
	unsigned int m_mobCount = 0;
	uint32_t m_nextMobId = 0;
	unsigned int m_mobSpawnTimer = 0;

	float tick_dt;

	// Spawn positions
	std::vector<Vector2> m_spawns;

	// Helper to compare sockaddr_in
	static bool AddressEquals(const sockaddr_in& a, const sockaddr_in& b);
	ConnectedClient* FindClientByAddress(const sockaddr_in& addr);
	uint32_t GetClientIdByAddress(const sockaddr_in& addr);

public:
	Server();
	~Server();

	bool running = true;

	void Init(int argc, char **argv);
	void Run(void);

	// Connection handling
	void HandleConnectRequest(const sockaddr_in& from);
	void HandleDisconnect(uint32_t client_id);
	void HandleUpdateStateMessage(NetBuffer& buffer, uint32_t client_id);
	void HandleReceivedMessage(NetBuffer& buffer, const sockaddr_in& from);
	void CheckClientTimeouts(void);

	// Game state broadcasting
	int BroadcastGameState(void);

	// Mob management
	void SpawnMob(void);
	void UpdateMobs(void);
	void CheckMissileCollisions(void);
};

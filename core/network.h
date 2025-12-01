#pragma once

#include "raylib.h"
#include "framework.h"

#define NBN_LogInfo(...) TraceLog(LOG_INFO, __VA_ARGS__)
#define NBN_LogError(...) TraceLog(LOG_ERROR, __VA_ARGS__)
#define NBN_LogWarning(...) TraceLog(LOG_WARNING, __VA_ARGS__)
#define NBN_LogDebug(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#define NBN_LogTrace(...) TraceLog(LOG_TRACE, __VA_ARGS__)

#include "nbnet.h"
#include "udp.h"

enum NetworkMode
{
	SERVER,
	CLIENT,
};

class Network
{
private:
	NetworkMode m_mode;
public:
	Network(NetworkMode mode);
	~Network(void);

	void Init();
	void Init(int argc, char** argv);
};

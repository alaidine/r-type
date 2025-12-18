/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Wave Manager Header - Gestion des vagues d'ennemis
*/

#ifndef WAVEMANAGER_HPP_
#define WAVEMANAGER_HPP_

#include <functional>
#include <vector>

enum class MovementPattern;

struct MobWaveEnemy {
  MovementPattern pattern;
  float spawnDelay;
  float yPosition;
};

struct MobWave {
  std::vector<MobWaveEnemy> enemies;
  float delayBeforeNextWave;
  MovementPattern mainPattern; // Pattern principal de la vague
};

class WaveManager {
public:
  WaveManager();
  ~WaveManager() = default;

  void Init(void);
  void Update(float deltaTime,
              std::function<void(MovementPattern, float)> spawnCallback);
  bool IsWaveActive(void) const;
  unsigned int GetCurrentWaveIndex(void) const;
  unsigned int GetTotalWaves(void) const;
  float GetCountdownTimer(void) const;
  bool IsGameStarted(void) const;
  void Reset(void);

private:
  void LoadWaves(void);
  void StartNextWave(void);
  void GenerateWave(unsigned int waveIndex);

  std::vector<MobWave> _waves;
  unsigned int _currentWaveIndex;
  unsigned int _currentEnemyIndex;
  float _waveTimer;
  float _enemySpawnTimer;
  bool _waveActive;
  bool _waitingForNextWave;
  float _nextWaveDelay;
  float _initialDelay;
  bool _gameStarted;
  float _baseSpawnInterval;
  float _difficultyMultiplier;
};

#endif // WAVEMANAGER_HPP_

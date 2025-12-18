/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Wave Manager Implementation - Gestion des vagues d'ennemis
*/

#include "WaveManager.hpp"
#include "../server.h"

WaveManager::WaveManager()
    : _currentWaveIndex(0), _currentEnemyIndex(0), _waveTimer(0.0f),
      _enemySpawnTimer(0.0f), _waveActive(false), _waitingForNextWave(false),
      _nextWaveDelay(0.0f), _initialDelay(10.0f), _gameStarted(false),
      _baseSpawnInterval(0.5f), _difficultyMultiplier(1.0f)
{
}

void WaveManager::Init(void)
{
    this->_waves.clear();
    this->_currentWaveIndex = 0;
    this->_currentEnemyIndex = 0;
    this->_waveTimer = 0.0f;
    this->_enemySpawnTimer = 0.0f;
    this->_waveActive = false;
    this->_waitingForNextWave = true;
    this->_nextWaveDelay = this->_initialDelay;
    this->_gameStarted = false;
    this->_difficultyMultiplier = 1.0f;
}

void WaveManager::GenerateWave(unsigned int waveIndex)
{
    MobWave wave;
    
    // Augmenter la difficulté progressivement
    float speedMultiplier = 1.0f + (waveIndex * 0.1f);
    float spawnInterval = this->_baseSpawnInterval / speedMultiplier;
    
    // Nombre d'ennemis augmente avec les vagues
    unsigned int enemyCount = 5 + (waveIndex * 2);
    if (enemyCount > 15) enemyCount = 15;
    
    // Choisir le pattern principal selon la vague
    MovementPattern mainPattern;
    switch (waveIndex % 6)
    {
        case 0: mainPattern = MovementPattern::LINEAR; break;
        case 1: mainPattern = MovementPattern::SINUSOIDAL; break;
        case 2: mainPattern = MovementPattern::ZIGZAG; break;
        case 3: mainPattern = MovementPattern::CIRCULAR; break;
        case 4: mainPattern = MovementPattern::DIVE; break;
        case 5: mainPattern = MovementPattern::FIGURE8; break;
        default: mainPattern = MovementPattern::LINEAR; break;
    }
    
    wave.mainPattern = mainPattern;
    
    // Générer les ennemis avec le même pattern pour la vague
    float currentDelay = 0.0f;
    for (unsigned int i = 0; i < enemyCount; i++)
    {
        MobWaveEnemy enemy;
        enemy.pattern = mainPattern;
        enemy.spawnDelay = currentDelay;
        
        // Positions Y variées mais organisées
        float yPos = 80.0f + (i % 5) * 100.0f;
        enemy.yPosition = yPos;
        
        wave.enemies.push_back(enemy);
        currentDelay += spawnInterval;
    }
    
    // Délai avant prochaine vague (diminue avec la difficulté, minimum 5 secondes)
    wave.delayBeforeNextWave = 8.0f - (waveIndex * 0.3f);
    if (wave.delayBeforeNextWave < 5.0f) wave.delayBeforeNextWave = 5.0f;
    
    this->_waves.push_back(wave);
}

void WaveManager::LoadWaves(void)
{
    this->_waves.clear();
    // On génère les vagues dynamiquement lors de StartNextWave
}

void WaveManager::StartNextWave(void)
{
    // Générer la vague si elle n'existe pas encore
    if (this->_currentWaveIndex >= this->_waves.size())
    {
        GenerateWave(this->_currentWaveIndex);
    }

    this->_currentEnemyIndex = 0;
    this->_enemySpawnTimer = 0.0f;
    this->_waveActive = true;
    this->_waitingForNextWave = false;
    this->_gameStarted = true;
}

void WaveManager::Update(float deltaTime, std::function<void(MovementPattern, float)> spawnCallback)
{
    if (this->_waitingForNextWave)
    {
        this->_nextWaveDelay -= deltaTime;
        if (this->_nextWaveDelay <= 0.0f)
        {
            StartNextWave();
        }
        return;
    }

    if (!this->_waveActive)
        return;

    // S'assurer que la vague existe
    if (this->_currentWaveIndex >= this->_waves.size())
    {
        this->_waveActive = false;
        this->_waitingForNextWave = true;
        this->_nextWaveDelay = 5.0f;
        return;
    }

    MobWave& currentWave = this->_waves[this->_currentWaveIndex];

    if (this->_currentEnemyIndex >= currentWave.enemies.size())
    {
        this->_waveActive = false;
        this->_waitingForNextWave = true;
        this->_nextWaveDelay = currentWave.delayBeforeNextWave;
        this->_currentWaveIndex++;
        return;
    }

    this->_enemySpawnTimer += deltaTime;

    while (this->_currentEnemyIndex < currentWave.enemies.size())
    {
        const MobWaveEnemy& enemy = currentWave.enemies[this->_currentEnemyIndex];

        if (this->_enemySpawnTimer >= enemy.spawnDelay)
        {
            spawnCallback(enemy.pattern, enemy.yPosition);
            this->_currentEnemyIndex++;
        }
        else
        {
            break;
        }
    }
}

bool WaveManager::IsWaveActive(void) const
{
    return this->_waveActive;
}

unsigned int WaveManager::GetCurrentWaveIndex(void) const
{
    return this->_currentWaveIndex;
}

unsigned int WaveManager::GetTotalWaves(void) const
{
    return static_cast<unsigned int>(this->_waves.size());
}

float WaveManager::GetCountdownTimer(void) const
{
    if (this->_waitingForNextWave)
    {
        return this->_nextWaveDelay;
    }
    return 0.0f;
}

bool WaveManager::IsGameStarted(void) const
{
    return this->_gameStarted;
}

void WaveManager::Reset(void)
{
    Init();
}

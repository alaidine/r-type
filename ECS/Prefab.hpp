#pragma once
    #include "Core.hpp"

namespace Prefab {
    Entity MakePlayer(Core& _core, float posX, float posY);
    Entity MakeEnemy(Core& _core, float posX, float posY);
    Entity MakeMilssile(Core& _core);
    Entity MakeClient(Core& _core, float x, float y);
}
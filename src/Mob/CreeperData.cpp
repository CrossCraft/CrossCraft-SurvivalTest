#include "CreeperData.hpp"
#include "../World/World.hpp"

namespace CrossCraft::Mob {
    void CreeperData::OnDeath(World *w, bool playerKill) {
        MobData::OnDeath(w, playerKill);
        pos.y -= 1.4625f;
        w->explode(pos);
    }
}// namespace CrossCraft::Mob

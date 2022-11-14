#include "MobManager.hpp"
#include "../Player/Player.hpp"

namespace CrossCraft::Mob {

MobManager::MobManager() {
    mobs.clear();
    srand(time(NULL));
}
MobManager::~MobManager() {
    for (auto m : mobs) {
        delete m;
    }

    mobs.clear();
}

void MobManager::add_mob(MobData *mobData) { mobs.push_back(mobData); }

const int MOB_DIST_NEAR = 24.0f;
const int MOB_CAP = 20;
const int MOB_DIST_FAR = 64.0f;

void MobManager::update(float dt, Player *p, World *w) {

    // Spawn
    if (mobs.size() < MOB_CAP) {
        MobData *mobData = MobData::make_mob((MobType)(rand() % 7));

        auto pos = p->pos;
        int rx = rand() % 49 - 24;
        int rz = rand() % 49 - 24;

        if (rx < 0)
            rx -= 12;
        else
            rx += 12;

        if (rz < 0)
            rz -= 12;
        else
            rz += 12;

        pos.x += rx;
        pos.z += rz;

        for (int i = 63; i >= 0; i--) {
            auto idx = w->getIdx(pos.x, i, pos.z);
            auto blk = w->worldData[idx];

            if (blk != 0) {
                pos.y = i + 3;
                break;
            }
        }

        mobData->pos = pos;

        mobs.push_back(mobData);
    }
    int toRemove = -1;

    // Update
    for (size_t i = 0; i < mobs.size(); i++) {
        auto &m = mobs[i];
        m->hitCD -= dt;

        // Self-collide physics
        for (size_t c = 0; c < mobs.size(); c++) {
            if (c == i)
                continue;
            auto m2 = mobs[c];

            auto diff = m->pos - m2->pos;
            auto len = sqrtf(diff.x * diff.x + diff.z * diff.z);

            if (len < 0.6f) {
                m->vel.x += diff.x / 2.0f;
                m->vel.z += diff.z / 2.0f;
                m2->vel.x -= diff.x / 2.0f;
                m2->vel.z -= diff.z / 2.0f;
            }
        }

        auto diff = p->pos - m->pos;

        auto lenS = sqrtf(diff.x * diff.x + diff.z * diff.z);
        auto lenF = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

        if (lenS < 0.6f) {
            if (lenF < 1.5f) {
                if (m->mobType == MobType::Creeper ||
                    m->mobType == MobType::Zombie ||
                    m->mobType == MobType::Spider)
                    p->OnHit(w, 4, glm::normalize(diff) * 3.0f, false);
            }
        }

        auto len = sqrtf(diff.x * diff.x + diff.z * diff.z);

        if (len > 24.0f && len < 64.0f)
            m->inRange = false;
        else if (len > 64.0f) {
            m->inRange = false;

            m->HP -= 1;
            if (m->HP < 0 && toRemove == -1) {
                m->despawned = true;
                m->isAlive = false;
                toRemove = i;
            }
        } else
            m->inRange = true;

        if (m->vel.x != 0 && m->vel.z != 0 && !m->isAnimating) {
            m->isAnimating = true;
            m->animationTime = 3.0f;
        }
        m->animationTime -= dt;

        if (m->animationTime < 0)
            m->isAnimating = false;

        if (!m->isAlive) {
            m->deathTime += dt;
            m->rot.x += 180.0f * dt;
            if (m->deathTime > 0.5) {
                ParticleManager::get().spawn_particles(
                    Particle_Death, {m->pos.x, m->pos.y - 1.0f, m->pos.z}, 0);
                if (toRemove == -1)
                    toRemove = i;
            }
        }

        m->update(dt, p, w);
    }

    // Remove
    if (toRemove != -1) {
        delete mobs[toRemove];
        mobs.erase(mobs.begin() + toRemove);
    }
}
void MobManager::draw() {
    for (auto m : mobs) {
        if (!m->inRange)
            continue;

        switch (m->mobType) {
        case MobType::Steve:
            Model::Steve::get().draw((SteveData *)m);
            break;

        case MobType::Skeleton:
            Model::Skeleton::get().draw((SkeletonData *)m);
            break;

        case MobType::Zombie:
            Model::Zombie::get().draw((ZombieData *)m);
            break;

        case MobType::Creeper:
            Model::Creeper::get().draw((CreeperData *)m);
            break;

        case MobType::Pig:
            Model::Pig::get().draw((PigData *)m);
            break;

        case MobType::Sheep:
            Model::Sheep::get().draw((SheepData *)m);
            break;

        case MobType::Spider:
            Model::Spider::get().draw((SpiderData *)m);
            break;

        default:
            break;
        }
    }
}
} // namespace CrossCraft::Mob
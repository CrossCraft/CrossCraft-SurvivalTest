#pragma once
#include "Drops.hpp"
#include "../BlockConst.hpp"

namespace CrossCraft
{

    void lookup(uint8_t broken, DropData &d)
    {
        switch (broken)
        {

        case Block::Grass: {
            d.type = Block::Dirt;
            d.quantity = 1;
            break;
        }

        case Block::Stone: {
            d.type = Block::Cobblestone;
            d.quantity = 1;
            break;
        }

        case Block::Coal_Ore: {
            d.type = Block::Slab;
            d.quantity = 1 + rand() % 3;
            break;
        }


        case Block::Gold_Ore: {
            d.type = Block::Gold;
            d.quantity = 1 + rand() % 3;
            break;
        }

        case Block::Iron_Ore: {
            d.type = Block::Iron;
            d.quantity = 1 + rand() % 3;
            break;
        }

        case Block::Logs: {
            d.type = Block::Wood;
            d.quantity = 1 + rand() % 3;
            break;
        }

        case Block::Leaves: {
            d.quantity = 0;
            break;
        }
        case Block::Glass: {
            d.quantity = 0;
            break;
        }

        case Block::Obsidian: {
            d.type = Block::Cobblestone;
            d.quantity = 1;
            break;
        }

        default:
        {
            d.type = broken;
            d.quantity = 1;
            break;
        }
        }
    }

}
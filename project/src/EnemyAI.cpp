#include "EnemyAI.h"
#include "World.h"

extern World world;

EnemyAI::EnemyAI()
{
    //ctor
}

EnemyAI::~EnemyAI()
{
    //dtor
}

void EnemyAI::clearAI()
{
    m_battlefield.clear();
    m_aiSquads.clear();
    m_playerSquads.clear();
}

void EnemyAI::takeBattlefield()
{
    clearAI();
    m_battlefield = world.m_battle.m_tiles;
    for (vector<Squad*> :: iterator it = world.m_battle.m_squads.begin(); it != world.m_battle.m_squads.end(); it++)
    {
        if ((*it)->m_owner == PLAYER1)
        {
            m_playerSquads.push_back((*it));
        }
        else
        {
            m_aiSquads.push_back((*it));
        }
    }
}

void EnemyAI::chooseBestActionForUnit(Squad* squad)
{
    Tile* startPosition = squad->m_tileTaken;
    vector<Tile*> availableToWalkTiles = world.m_battle.showAvailableWalkTiles(squad);
    int bestScore = 0;
    Tile* bestPosition;
    Squad* bestVictim;
    int score;
    /// Go trough all available to walk tiles and choose the best move
    for (vector <Tile*> :: iterator it = availableToWalkTiles.begin(); it != availableToWalkTiles.end(); it ++)
    {
        /// Move the squad
        squad->m_mapCoor = (*it)->m_mapCoordinates;
        squad->m_tileTaken = (*it);
        for(vector <Squad*> :: iterator squadIt = m_playerSquads.begin(); squadIt != m_playerSquads.end(); squadIt ++ )
        {
            /// calculate the score for this move
            score = 0;
            if (world.m_battle.canShoot(squad, (*squadIt)->m_mapCoor))
            {
                score = squad->m_attackDamage;
                if (world.m_battle.canShoot((*squadIt), squad->m_mapCoor))
                {
                    score -= (*squadIt)->m_attackDamage / 2;
                }
            }
            if (score > bestScore)
            {
                bestScore = score;
                bestPosition = (*it);
                bestVictim = (*squadIt);
            }
        }
    }
    squad->m_tileTaken = startPosition;
    squad->m_mapCoor = squad->m_tileTaken->m_mapCoordinates;
    if(bestScore > 0)
    {
        world.m_battle.canTravel(squad, bestPosition->m_mapCoordinates);
        if (bestVictim != NULL)
        {
            squad->attack(bestVictim);
        }
    }
}

void EnemyAI::makeTurn()
{
    for (vector<Squad*> :: iterator it = m_aiSquads.begin(); it != m_aiSquads.end(); it ++)
    {
        chooseBestActionForUnit((*it));
    }
}

void EnemyAI::returnBattlefield()
{
    world.m_battle.m_squads.clear();
    for (vector<Squad*> :: iterator it = m_aiSquads.begin(); it != m_aiSquads.end(); it ++)
    {
        world.m_battle.m_squads.push_back((*it));
    }
    for (vector<Squad*> :: iterator it = m_playerSquads.begin(); it != m_playerSquads.end(); it ++)
    {
        world.m_battle.m_squads.push_back((*it));
    }
}

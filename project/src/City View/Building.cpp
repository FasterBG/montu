#include "Building.h"
#include "WorldBuilding.h"
extern WorldBuilding cityView;

Building::Building()
{
    //ctor
}

Building::Building(const Building& model/*, Tile* tile, OWNER owner*/)//:Tile(model)
{
    m_img = model.m_img;
    m_ownerTextureP1 = model.m_ownerTextureP1;
    m_ownerTextureP2 = model.m_ownerTextureP2;
    m_buildTexture = model.m_buildTexture;
    m_objectTexture = model.m_objectTexture;
    m_damageTexture = model.m_damageTexture;
    m_height = model.m_height;
    m_width = model.m_width;
    m_adjustment = model.m_adjustment;
    m_drawState = model.m_drawState;
    m_maxHealth = model.m_maxHealth;
    m_healthPerState = model.m_healthPerState;
    m_states = model.m_states;
    m_maxState = model.m_maxState;
    m_duration = model.m_duration;
    m_getPosition = model.m_getPosition;
    m_currState = model.m_currState;
    m_health = model.m_health;
    m_constructing = model.m_constructing;
    m_useObject = model.m_useObject;
    m_currTimePoint = steady_clock::now();

    m_objectRect = model.m_objectRect;

    m_type = model.m_type;

   // D(m_type);
}

Building::~Building()
{
    SDL_DestroyTexture(m_objectTexture);
    SDL_DestroyTexture(m_ownerTextureP1);
    SDL_DestroyTexture(m_ownerTextureP2);
    SDL_DestroyTexture(m_buildTexture);
    SDL_DestroyTexture(m_damageTexture);
}

void Building::load(ifstream& stream)
{
    string tmp;
    stream >> tmp >> m_walkDifficulty;
    stream >> tmp >> m_img;
    stream >> tmp >> m_ownerImgP1;
    stream >> tmp >> m_ownerImgP2;
    stream >> tmp >> m_buildImg;
    stream >> tmp >> m_damageImg;
    stream >> tmp >> m_height;
    stream >> tmp >> m_width;
    stream >> tmp >> m_adjustment;
    stream >> tmp >> m_maxHealth;
    stream >> tmp >> m_states;
    stream >> tmp >> m_maxState;
    stream >> tmp >> m_duration;
    stream >> tmp >> m_getPosition;
    stream >> tmp >> m_currState;
    stream >> tmp >> m_health;
    stream >> tmp >> m_constructing;
}

void Building::load(string configFile, SDL_Renderer* renderer)
{

    m_type = configFile;
    configFile = "config\\" + configFile;

    ifstream stream;
    stream.open(configFile.c_str());

    load(stream);

    stream.close();

    m_objectTexture = LoadTexture(m_img, renderer);

    m_ownerTextureP1 = LoadTexture(m_ownerImgP1, renderer);
    m_ownerTextureP2 = LoadTexture(m_ownerImgP2, renderer);

    m_buildTexture = LoadTexture(m_buildImg, renderer);

    m_damageTexture = LoadTexture(m_damageImg, renderer);

    m_healthPerState = m_maxHealth / m_maxState;

    m_drawState.h = m_height;
    m_drawState.w = m_width;
    m_drawState.x = 0;
}

void Building::setTextureDetails(SDL_Renderer* renderer)
{
    if (m_owner == PLAYER1)
    {
        SDL_RenderCopyEx(renderer, m_ownerTextureP1, &m_drawState, &m_objectRect, NULL, NULL, SDL_FLIP_NONE);
    }
    else if (m_owner == PLAYER2)
    {
        SDL_RenderCopyEx(renderer, m_ownerTextureP2, &m_drawState, &m_objectRect, NULL, NULL, SDL_FLIP_NONE);
    }
    if (m_constructing)
    {
        SDL_RenderCopyEx(renderer, m_buildTexture, &m_drawState, &m_objectRect, NULL, NULL, SDL_FLIP_NONE);
    }
    else if (m_health < m_maxHealth)
    {
        SDL_RenderCopyEx(renderer, m_damageTexture, &m_drawState, &m_objectRect, NULL, NULL, SDL_FLIP_NONE);
    }
}

void Building::limitHealth()
{
    if (m_health >= m_maxHealth)
    {
        m_health = m_maxHealth;
        m_healthPerState = m_maxHealth / (m_states - m_maxState);
        m_constructing = false;
    }
}

void Building::select()
{
    if (cityView.m_selected.x == m_objectRect.x && cityView.m_selected.y == m_objectRect.y)
    {
        if (!m_getPosition)
        {
            if (GetAsyncKeyState(VK_RETURN))
            {
                m_getPosition = true;
            }
            if (GetAsyncKeyState(VK_ESCAPE))
            {
                m_health = 0;
                m_useObject = false;
                m_getPosition = true;
            }
            Sleep(100);
        }
    }
    else
    {
        m_getPosition = true;
    }
}

void Building::getState()
{
   limitHealth();
    if (m_constructing &&
            milliseconds(duration_cast<milliseconds>(steady_clock::now() - m_currTimePoint).count()) >= milliseconds(m_duration))
    {
        m_health += m_healthPerState;
        m_currTimePoint = steady_clock::now();
    }

    if (m_constructing)
    {
        m_currState = m_health / m_healthPerState;
        m_drawState.y = (m_maxState - m_currState) * (m_height / (m_maxState + 1));
        m_objectRect.h = (cityView.m_hexagonHeight / (m_maxState + 1)) * (m_currState + 1);
        m_objectRect.y -= m_adjustment;
        m_adjustment = cityView.m_hexagonHeight - (cityView.m_hexagonHeight / (m_maxState + 1)) * (m_currState + 1);
        m_objectRect.y += m_adjustment;
    }
    else
    {
        m_currState = (m_maxHealth - m_health) / m_healthPerState + m_maxState;
        m_drawState.y = (m_currState - m_maxState) * (m_height / (m_states - m_maxState + 1));
        m_objectRect.h = (cityView.m_hexagonHeight / (m_states - m_maxState)) * (m_states - m_currState);
        m_objectRect.y -= m_adjustment;
        m_adjustment = - cityView.m_hexagonHeight + (cityView.m_hexagonHeight / (m_maxState + 1)) * (m_currState + 1);
        m_objectRect.y += m_adjustment;
    }
}

void Building::update()
{
    select();
    if (!m_getPosition)
    {
        m_currTimePoint = steady_clock::now();
    }
    if (m_useObject && m_getPosition)
    {
        getState();
    }
   // cout<<__LINE__<<endl;
}

void Building::draw(SDL_Renderer* renderer)
{
    //cout<<"DrawObjRect: "<<m_objectRect.x<<" "<<m_objectRect.y<<" "<<m_objectRect.w<<" "<<m_objectRect.h<<endl;
    if (!m_getPosition)
    {
        //cout<<__LINE__<<endl;
        SDL_RenderCopy(renderer, m_objectTexture, NULL, &m_objectRect);
    }
    else
    {
        //cout<<__LINE__<<endl;

        SDL_RenderCopy(renderer, m_objectTexture, NULL, &m_objectRect);
        setTextureDetails(renderer);
    }


    //cout<<"objRect"<<m_objectRect.x<<" "<<m_objectRect.y<<" "<<m_objectRect.w<<" "<<m_objectRect.h<<endl;
}

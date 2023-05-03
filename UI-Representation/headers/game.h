#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <sqlite3.h>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <utility>

class Game{
    private:
        sf::RenderWindow *window;
        sf::VideoMode videoMode;
        sf::Event ev;
        sf::Vector2f mousePos;
        std::vector<sf::Vector2f> points;
        int WINDOW_WIDTH, WINDOW_HEIGHT;
        double MIN_LATITUDE,MAX_LATITUDE,MIN_LONGITUDE,MAX_LONGITUDE;
        std::unordered_map<int,std::vector<std::pair<double,double>>> db;
        sf::Texture map;
        float zoom;
        sf::Vector2f pan;
        sf::Clock gameClock;

        void insertIntoDB(double d);
        void initWindow();
        void setConsts();
        void initVariables();
        void loadTextures();
        void loadData();
        void displayData();
        void crash(std::string error);
        sf::Vector2f convertToScreen(double lat, double lon);
    public:
        Game();
        virtual ~Game();

        const bool isRunning()const;

        void update();
        void updateMousePos();
        void pollEvents();
        void render();

};

#endif


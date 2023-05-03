#include "headers/game.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Window/Keyboard.hpp>

Game::Game(){
    this->setConsts();
    this->initVariables();
    this->initWindow();
    this->loadTextures();
    this->loadData();
    // this->displayData();
}

Game::~Game(){
    delete this->window;
}

void Game::setConsts(){
    this->WINDOW_HEIGHT = 1024;
    this->WINDOW_WIDTH = 1024;
    this->MIN_LATITUDE = 76.25;
    this->MAX_LATITUDE = 77.25;
    this->MIN_LONGITUDE = 30.25;
    this->MAX_LONGITUDE = 31.25;
}

void Game::initVariables(){
    this->window = nullptr;
    this->zoom = 2;
    this->pan = sf::Vector2f(0,0);
}

void Game::initWindow(){
    this->videoMode.height = this->WINDOW_HEIGHT;
    this->videoMode.width = this->WINDOW_WIDTH;
    this->window = new sf::RenderWindow(this->videoMode,"mapathon",sf::Style::Close);
}
void Game::loadTextures(){
    if (!this->map.loadFromFile("map.png")){
        this->crash("Failed to load map texture");
    }
}

void Game::loadData(){
    sqlite3 *db;
    this->db.clear();
    int rc = sqlite3_open("network.db",&db);
    std::cout << std::setprecision(9);
    if (rc != SQLITE_OK) {
        this->crash("Unable to load database file: roads.db");
    }

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, "SELECT * FROM roads;", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        this->crash("Error preparing statement: ");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 1);
        this->db[id].push_back(std::pair<double,double>(sqlite3_column_double(stmt,3),sqlite3_column_double(stmt, 4)));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void Game::displayData(){
    for (auto& pair : this->db) {
        std::cout << "ID = " << pair.first << std::endl;
        for(auto x: pair.second){
            std::cout << "Coords -> " << "Lat=" << x.first << " Long=" << x.second << std::endl;
        }
    }
    std::cout << "Size: " << this->db.size();
}

void Game::crash(std::string error){
    std::cerr << "crash: " << error << std::endl;
    this->window->close();
    std::exit(1);
}

const bool Game::isRunning() const{
    return this->window->isOpen();
}

void Game::update(){
    this->updateMousePos();
    this->pollEvents();

    // sf::Time elapsed = this->gameClock.restart();
    // float dt = elapsed.asSeconds();
    // float fps = 1.0f/dt;
    // std::cout << "FPS: " << fps << std::endl;
}

void Game::updateMousePos(){

    this->mousePos = this->window->mapPixelToCoords(sf::Mouse::getPosition(*this->window));
}

void Game::pollEvents(){
    while(this->window->pollEvent(this->ev)){
        if(this->ev.type == sf::Event::Closed) this->window->close();
        if(this->ev.type == sf::Event::KeyPressed){
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) this->zoom += 0.1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) this->zoom -= 0.1;
        } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            pan.x -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            pan.x += 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            pan.y -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            pan.y += 10;
        }
    }
}
sf::Vector2f Game::convertToScreen(double lat, double lon) {
    float latCenter = (this->MIN_LATITUDE + this->MAX_LATITUDE) / 2.0f;
    float lonCenter = (this->MIN_LONGITUDE + this->MAX_LONGITUDE) / 2.0f;
    float latRange = this->MAX_LATITUDE - this->MIN_LATITUDE;
    float lonRange = this->MAX_LONGITUDE - this->MIN_LONGITUDE;

    float x = (lon - lonCenter) / lonRange * this->zoom * this->WINDOW_WIDTH + this->WINDOW_WIDTH / 2 - this->pan.x;
    float y = this->WINDOW_WIDTH - (lat - latCenter) / latRange * this->zoom * this->WINDOW_WIDTH - this->WINDOW_WIDTH / 2 - this->pan.y;
    return sf::Vector2f(x, y);
}
void Game::render(){
    this->window->clear();
    
    //Render map
    sf::Sprite mapSprite(this->map);
    mapSprite.setScale(this->WINDOW_WIDTH / this->map.getSize().x, this->WINDOW_HEIGHT / this->map.getSize().y);
    this->window->draw(mapSprite);


    //Draw points
    sf::CircleShape circle(1);
    circle.setFillColor(sf::Color::Red);
    for (const auto& kv : this->db){
        sf::VertexArray points(sf::LineStrip);
        for (const auto& point : kv.second)
        {
            double latitude = point.first;
            double longitude = point.second;
            if (latitude >= this->MIN_LATITUDE && latitude <= this->MAX_LATITUDE && longitude >= this->MIN_LONGITUDE && longitude <= this->MAX_LATITUDE)
            {
                sf::Vector2f pos = convertToScreen(latitude, longitude);
                if(pos.x == 0 || pos.y == 0){
                    points.clear();
                    continue;
                }
                circle.setPosition(pos.x, pos.y);
                points.append(sf::Vertex(pos,sf::Color::Blue));
                this->window->draw(circle);
            }
        }
        if(points.getVertexCount() > 0)this->window->draw(points);
    }

    this->window->display();
}


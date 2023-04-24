#include "headers/game.h"
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
    this->MIN_LATITUDE = 75.0;
    this->MAX_LATITUDE = 77.0;
    this->MIN_LONGITUDE = 30.0;
    this->MAX_LONGITUDE = 32.0;
}

void Game::initVariables(){
    this->window = nullptr;
    this->zoom = 2;
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
std::vector<double> split(const std::string& input, char delimiter) {
    std::vector<double> result;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(std::stod(item));
    }
    return result;
}

void removeSquareBracketsAndSpaces(std::string& str)
{
    str.erase(std::remove(str.begin(), str.end(), '['), str.end());
    str.erase(std::remove(str.begin(), str.end(), ']'), str.end());
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}
std::vector<std::pair<double, double>> convertToPairs(const std::vector<double>& inputVector) {
    std::vector<std::pair<double, double>> outputVector;

    // Iterate over the elements of the input vector, creating pairs from every two adjacent elements
    for (std::vector<double>::size_type i = 0; i < inputVector.size(); i += 2) {
        double first = inputVector[i];
        double second = (i + 1 < inputVector.size()) ? inputVector[i + 1] : 0.0;
        outputVector.emplace_back(first, second);
    }

    return outputVector;
}
void Game::loadData(){
    sqlite3 *db;
    this->db.clear();
    int rc = sqlite3_open("roads.db",&db);
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
        int id = sqlite3_column_int(stmt, 0);
        const char* coordsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string st = std::string(coordsStr);
        removeSquareBracketsAndSpaces(st);
        std::vector<double> split_string = split(st, ',');
        this->db[id] = convertToPairs(split_string);
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
    }
}
sf::Vector2f Game::convertToScreen(float lat, float lon) {
    float latCenter = (75 + 77) / 2.0f;
    float lonCenter = (30 + 32) / 2.0f;
    float latRange = this->MAX_LATITUDE - this->MIN_LATITUDE;
    float lonRange = this->MAX_LONGITUDE - this->MIN_LONGITUDE;

    float x = (lon - lonCenter) / lonRange * this->zoom * this->WINDOW_WIDTH + this->WINDOW_WIDTH / 2;
    float y = this->WINDOW_WIDTH - (lat - latCenter) / latRange * this->zoom * this->WINDOW_WIDTH - this->WINDOW_WIDTH / 2;
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
        for (const auto& point : kv.second)
        {
            double latitude = point.first;
            double longitude = point.second;
            if (latitude >= this->MIN_LATITUDE && latitude <= this->MAX_LATITUDE && longitude >= this->MIN_LONGITUDE && longitude <= this->MAX_LATITUDE)
            {
                sf::Vector2f pos = convertToScreen(latitude, longitude);
                circle.setPosition(pos.x, pos.y);
                this->window->draw(circle);
            }
        }
    }

    this->window->display();
}


#pragma once
#include "Cell.h"
#include <vector>

struct SuppressionEffect {
    double water_level;     // 0.0 to 1.0
    double retardant_level; // 0.0 to 1.0
    double remaining_time;  // Seconds remaining
    bool is_firebreak;      // Permanent barrier
};

class Grid {
private:
    int width, height;
    std::vector<std::vector<Cell>> cells;
    std::vector<std::vector<SuppressionEffect>> suppression_effects;
    double wind_speed;      // m/s
    double wind_direction;  // degrees (0 = north, 90 = east)
    double ambient_temp;    // Celsius
    double humidity;        // 0.0 to 1.0
    
public:
    Grid(int w, int h);
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Cell& getCell(int x, int y) { return cells[y][x]; }
    const Cell& getCell(int x, int y) const { return cells[y][x]; }
    double getWindSpeed() const { return wind_speed; }
    double getWindDirection() const { return wind_direction; }
    double getAmbientTemp() const { return ambient_temp; }
    double getHumidity() const { return humidity; }
    
    // Setters
    void setWindSpeed(double speed) { wind_speed = speed; }
    void setWindDirection(double direction) { wind_direction = direction; }
    void setAmbientTemp(double temp) { ambient_temp = temp; }
    void setHumidity(double humid) { humidity = humid; }
    
    // Grid operations
    bool isValidPosition(int x, int y) const;
    void initializeRandom();
    void initializeTerrain();
    void igniteCell(int x, int y);
    void display() const;
    
    // Fire spread simulation
    void update(double dt);
    double calculateSpreadProbability(int from_x, int from_y, int to_x, int to_y) const;
    std::vector<std::pair<int, int>> getNeighbors(int x, int y) const;
    
    // Suppression methods
    void applyWaterDrop(int x, int y, int radius, double effectiveness, double duration);
    void applyRetardant(int x, int y, int radius, double effectiveness, double duration);
    void createFirebreak(int x1, int y1, int x2, int y2);
    void displayWithCrews(const class HumanFactorManager& human_manager) const;
    bool hasSuppressionEffect(int x, int y) const;
    double getSuppressionModifier(int x, int y) const;
};
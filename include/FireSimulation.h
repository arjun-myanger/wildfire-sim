#pragma once
#include "Grid.h"
#include "FirefightingCrew.h"
#include <chrono>

class FireSimulation {
private:
    Grid grid;
    HumanFactorManager human_manager;
    double time_step;           // Simulation time step in seconds
    double total_time;          // Total simulation time elapsed
    bool running;
    
    // Statistics
    int cells_burning;
    int cells_burned;
    int total_fuel_cells;
    
public:
    FireSimulation(int width, int height, double dt = 0.1);
    
    // Simulation control
    void start();
    void stop();
    void reset();
    void step();
    void run(double duration = -1); // -1 for indefinite
    
    // Getters
    Grid& getGrid() { return grid; }
    const Grid& getGrid() const { return grid; }
    HumanFactorManager& getHumanManager() { return human_manager; }
    const HumanFactorManager& getHumanManager() const { return human_manager; }
    double getTotalTime() const { return total_time; }
    bool isRunning() const { return running; }
    
    // Statistics
    void updateStatistics();
    int getCellsBurning() const { return cells_burning; }
    int getCellsBurned() const { return cells_burned; }
    int getTotalFuelCells() const { return total_fuel_cells; }
    double getBurnPercentage() const;
    
    // Scenario setup
    void setupGrassland();
    void setupForest();
    void setupMixed();
    void addFirebreak(int x1, int y1, int x2, int y2);
    void addIgnitionPoint(int x, int y);
    
    // Display and output
    void printStatus() const;
    void saveToFile(const std::string& filename) const;
};
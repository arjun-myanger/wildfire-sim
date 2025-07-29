#include "FireSimulation.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

FireSimulation::FireSimulation(int width, int height, double dt) 
    : grid(width, height), time_step(dt), total_time(0.0), running(false),
      cells_burning(0), cells_burned(0), total_fuel_cells(0) {
}

void FireSimulation::start() {
    running = true;
    updateStatistics();
}

void FireSimulation::stop() {
    running = false;
}

void FireSimulation::reset() {
    stop();
    total_time = 0.0;
    cells_burning = 0;
    cells_burned = 0;
    total_fuel_cells = 0;
}

void FireSimulation::step() {
    if (running) {
        grid.update(time_step);
        human_manager.updateCrews(time_step);
        human_manager.updateEvacuations(time_step);
        total_time += time_step;
        updateStatistics();
    }
}

void FireSimulation::run(double duration) {
    start();
    
    auto start_time = std::chrono::steady_clock::now();
    double elapsed = 0.0;
    
    while (running && (duration < 0 || elapsed < duration)) {
        step();
        
        // Check if fire has burned out
        if (cells_burning == 0) {
            std::cout << "Fire has burned out after " << total_time << " seconds.\n";
            break;
        }
        
        // Update display every 10 steps
        static int step_count = 0;
        if (++step_count % 10 == 0) {
            system("clear"); // Unix/Linux/Mac
            // system("cls"); // Windows alternative
            printStatus();
            human_manager.printStatus();
            grid.displayWithCrews(human_manager);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        auto current_time = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration<double>(current_time - start_time).count();
    }
    
    stop();
}

void FireSimulation::updateStatistics() {
    cells_burning = 0;
    cells_burned = 0;
    total_fuel_cells = 0;
    
    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            const Cell& cell = grid.getCell(x, y);
            
            if (cell.canBurn() || cell.getState() == CellState::BURNING || 
                cell.getState() == CellState::BURNED) {
                total_fuel_cells++;
            }
            
            if (cell.getState() == CellState::BURNING) {
                cells_burning++;
            } else if (cell.getState() == CellState::BURNED) {
                cells_burned++;
            }
        }
    }
}

double FireSimulation::getBurnPercentage() const {
    if (total_fuel_cells == 0) return 0.0;
    return (double)(cells_burned + cells_burning) / total_fuel_cells * 100.0;
}

void FireSimulation::setupGrassland() {
    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            grid.getCell(x, y) = Cell(FuelType::GRASS, 0.8, 0.2);
        }
    }
}

void FireSimulation::setupForest() {
    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            grid.getCell(x, y) = Cell(FuelType::TREE, 0.9, 0.3);
        }
    }
}

void FireSimulation::setupMixed() {
    grid.initializeRandom();
}

void FireSimulation::addFirebreak(int x1, int y1, int x2, int y2) {
    // Simple line drawing algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int x = x1;
    int y = y1;
    int x_inc = (x1 < x2) ? 1 : -1;
    int y_inc = (y1 < y2) ? 1 : -1;
    int error = dx - dy;
    
    dx *= 2;
    dy *= 2;
    
    while (true) {
        if (grid.isValidPosition(x, y)) {
            grid.getCell(x, y) = Cell(FuelType::ROCK, 0.0, 0.0);
        }
        
        if (x == x2 && y == y2) break;
        
        if (error > 0) {
            x += x_inc;
            error -= dy;
        } else {
            y += y_inc;
            error += dx;
        }
    }
}

void FireSimulation::addIgnitionPoint(int x, int y) {
    grid.igniteCell(x, y);
}

void FireSimulation::printStatus() const {
    std::cout << "=== Wildfire Simulation Status ===\n";
    std::cout << "Time: " << total_time << "s\n";
    std::cout << "Cells burning: " << cells_burning << "\n";
    std::cout << "Cells burned: " << cells_burned << "\n";
    std::cout << "Burn percentage: " << getBurnPercentage() << "%\n";
    std::cout << "Wind: " << grid.getWindSpeed() << " m/s at " 
              << grid.getWindDirection() << "°\n";
    std::cout << "Temperature: " << grid.getAmbientTemp() << "°C\n";
    std::cout << "Humidity: " << grid.getHumidity() * 100 << "%\n";
    std::cout << "\nLegend: . = grass, o = shrub, T = tree, * = fire, x = burned\n";
    std::cout << "        ~ = water/suppression, # = rock/firebreak, R = retardant\n";
    std::cout << "        G = ground crew, W = water tanker, A = air tanker, H = helicopter\n\n";
}

void FireSimulation::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Time," << total_time << "\n";
        file << "Burning," << cells_burning << "\n";
        file << "Burned," << cells_burned << "\n";
        file << "BurnPercentage," << getBurnPercentage() << "\n";
        
        file << "Grid:\n";
        for (int y = 0; y < grid.getHeight(); ++y) {
            for (int x = 0; x < grid.getWidth(); ++x) {
                file << grid.getCell(x, y).getDisplayChar();
            }
            file << "\n";
        }
        file.close();
    }
}
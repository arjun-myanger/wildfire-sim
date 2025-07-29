#include "Grid.h"
#include "FirefightingCrew.h"
#include <iostream>
#include <random>
#include <cmath>

Grid::Grid(int w, int h) : width(w), height(h), wind_speed(5.0), 
                           wind_direction(90.0), ambient_temp(25.0), humidity(0.4) {
    cells.resize(height);
    suppression_effects.resize(height);
    for (int y = 0; y < height; ++y) {
        cells[y].resize(width);
        suppression_effects[y].resize(width);
        for (int x = 0; x < width; ++x) {
            suppression_effects[y][x] = {0.0, 0.0, 0.0, false};
        }
    }
}

bool Grid::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Grid::initializeRandom() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> fuel_dist(0.3, 1.0);
    std::uniform_real_distribution<> moisture_dist(0.1, 0.6);
    std::uniform_int_distribution<> type_dist(0, 2);
    std::uniform_real_distribution<> special_dist(0.0, 1.0);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            FuelType type;
            
            // Add some water and rock obstacles
            if (special_dist(gen) < 0.05) {
                type = FuelType::WATER;
            } else if (special_dist(gen) < 0.08) {
                type = FuelType::ROCK;
            } else {
                switch (type_dist(gen)) {
                    case 0: type = FuelType::GRASS; break;
                    case 1: type = FuelType::SHRUB; break;
                    case 2: type = FuelType::TREE; break;
                    default: type = FuelType::GRASS; break;
                }
            }
            
            cells[y][x] = Cell(type, fuel_dist(gen), moisture_dist(gen));
        }
    }
}

void Grid::initializeTerrain() {
    // Create a simple terrain with rivers and patches
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            FuelType type = FuelType::GRASS;
            double density = 0.7;
            double moisture = 0.3;
            
            // Create a river diagonally
            if (abs((x - y)) < 2) {
                type = FuelType::WATER;
            }
            // Forest patch in upper right
            else if (x > width/2 && y < height/2) {
                type = FuelType::TREE;
                density = 0.9;
                moisture = 0.2;
            }
            // Shrub area in lower left
            else if (x < width/3 && y > 2*height/3) {
                type = FuelType::SHRUB;
                density = 0.8;
                moisture = 0.4;
            }
            
            cells[y][x] = Cell(type, density, moisture);
        }
    }
}

void Grid::igniteCell(int x, int y) {
    if (isValidPosition(x, y)) {
        cells[y][x].ignite();
    }
}

void Grid::display() const {
    std::cout << "+";
    for (int x = 0; x < width; ++x) std::cout << "-";
    std::cout << "+\n";
    
    for (int y = 0; y < height; ++y) {
        std::cout << "|";
        for (int x = 0; x < width; ++x) {
            std::cout << cells[y][x].getDisplayChar();
        }
        std::cout << "|\n";
    }
    
    std::cout << "+";
    for (int x = 0; x < width; ++x) std::cout << "-";
    std::cout << "+\n";
}

std::vector<std::pair<int, int>> Grid::getNeighbors(int x, int y) const {
    std::vector<std::pair<int, int>> neighbors;
    
    // 8-directional neighbors
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue; // Skip center cell
            
            int nx = x + dx;
            int ny = y + dy;
            
            if (isValidPosition(nx, ny)) {
                neighbors.push_back({nx, ny});
            }
        }
    }
    
    return neighbors;
}

double Grid::calculateSpreadProbability(int from_x, int from_y, int to_x, int to_y) const {
    const Cell& from_cell = cells[from_y][from_x];
    const Cell& to_cell = cells[to_y][to_x];
    
    if (from_cell.getState() != CellState::BURNING || !to_cell.canBurn()) {
        return 0.0;
    }
    
    // Check for firebreaks
    const SuppressionEffect& to_suppression = suppression_effects[to_y][to_x];
    if (to_suppression.is_firebreak) {
        return 0.0; // Firebreaks completely block spread
    }
    
    double base_prob = to_cell.getIgnitionProbability() * 0.1; // Base spread rate
    
    // Wind effect
    double dx = to_x - from_x;
    double dy = to_y - from_y;
    double spread_angle = atan2(dy, dx) * 180.0 / M_PI;
    double wind_effect = cos((spread_angle - wind_direction) * M_PI / 180.0);
    
    // Wind increases probability in wind direction
    if (wind_effect > 0) {
        base_prob *= (1.0 + wind_speed * wind_effect * 0.1);
    }
    
    // Distance effect (diagonal neighbors are farther)
    double distance = sqrt(dx*dx + dy*dy);
    base_prob /= distance;
    
    // Temperature effect from burning cell
    double temp_effect = (from_cell.getTemperature() - ambient_temp) / 100.0;
    base_prob *= (1.0 + temp_effect * 0.2);
    
    // Apply suppression effects
    double suppression_modifier = getSuppressionModifier(to_x, to_y);
    base_prob *= (1.0 - suppression_modifier);
    
    return std::min(1.0, std::max(0.0, base_prob));
}

void Grid::update(double dt) {
    // Create a copy to avoid updating cells as we read from neighbors
    std::vector<std::vector<bool>> will_ignite(height, std::vector<bool>(width, false));
    
    // First pass: determine which cells will ignite
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (cells[y][x].getState() == CellState::BURNING) {
                auto neighbors = getNeighbors(x, y);
                
                for (auto& [nx, ny] : neighbors) {
                    if (cells[ny][nx].canBurn()) {
                        double prob = calculateSpreadProbability(x, y, nx, ny);
                        
                        // Use probability to determine ignition
                        static std::random_device rd;
                        static std::mt19937 gen(rd());
                        std::uniform_real_distribution<> dist(0.0, 1.0);
                        
                        if (dist(gen) < prob * dt) {
                            will_ignite[ny][nx] = true;
                        }
                    }
                }
            }
        }
    }
    
    // Second pass: ignite cells and update all cells
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (will_ignite[y][x]) {
                cells[y][x].ignite();
            }
            cells[y][x].update(dt);
            
            // Update suppression effects
            SuppressionEffect& effect = suppression_effects[y][x];
            if (effect.remaining_time > 0) {
                effect.remaining_time -= dt;
                if (effect.remaining_time <= 0) {
                    effect.water_level = 0.0;
                    effect.retardant_level = 0.0;
                }
            }
            
            // Water and retardant also extinguish existing fires
            if (cells[y][x].getState() == CellState::BURNING) {
                double suppression = getSuppressionModifier(x, y);
                if (suppression > 0.5) { // Strong suppression can extinguish fires
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_real_distribution<> dist(0.0, 1.0);
                    
                    if (dist(gen) < suppression * dt * 2.0) {
                        cells[y][x].setState(CellState::BURNED);
                    }
                }
            }
        }
    }
}

void Grid::applyWaterDrop(int x, int y, int radius, double effectiveness, double duration) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int target_x = x + dx;
            int target_y = y + dy;
            
            if (isValidPosition(target_x, target_y)) {
                double distance = sqrt(dx*dx + dy*dy);
                if (distance <= radius) {
                    SuppressionEffect& effect = suppression_effects[target_y][target_x];
                    double distance_factor = 1.0 - (distance / radius);
                    
                    effect.water_level = std::max(effect.water_level, 
                                                effectiveness * distance_factor);
                    effect.remaining_time = std::max(effect.remaining_time, duration);
                }
            }
        }
    }
}

void Grid::applyRetardant(int x, int y, int radius, double effectiveness, double duration) {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int target_x = x + dx;
            int target_y = y + dy;
            
            if (isValidPosition(target_x, target_y)) {
                double distance = sqrt(dx*dx + dy*dy);
                if (distance <= radius) {
                    SuppressionEffect& effect = suppression_effects[target_y][target_x];
                    double distance_factor = 1.0 - (distance / radius);
                    
                    effect.retardant_level = std::max(effect.retardant_level, 
                                                    effectiveness * distance_factor);
                    effect.remaining_time = std::max(effect.remaining_time, duration);
                }
            }
        }
    }
}

void Grid::createFirebreak(int x1, int y1, int x2, int y2) {
    // Simple line drawing algorithm (Bresenham's)
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
        if (isValidPosition(x, y)) {
            suppression_effects[y][x].is_firebreak = true;
            cells[y][x] = Cell(FuelType::ROCK, 0.0, 0.0);
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

bool Grid::hasSuppressionEffect(int x, int y) const {
    if (!isValidPosition(x, y)) return false;
    
    const SuppressionEffect& effect = suppression_effects[y][x];
    return effect.water_level > 0.0 || effect.retardant_level > 0.0 || effect.is_firebreak;
}

double Grid::getSuppressionModifier(int x, int y) const {
    if (!isValidPosition(x, y)) return 0.0;
    
    const SuppressionEffect& effect = suppression_effects[y][x];
    return std::min(1.0, effect.water_level * 0.8 + effect.retardant_level * 0.9);
}

void Grid::displayWithCrews(const HumanFactorManager& human_manager) const {
    std::cout << "+";
    for (int x = 0; x < width; ++x) std::cout << "-";
    std::cout << "+\n";
    
    for (int y = 0; y < height; ++y) {
        std::cout << "|";
        for (int x = 0; x < width; ++x) {
            char crew_char = human_manager.getCrewDisplayChar(x, y);
            if (crew_char != ' ') {
                std::cout << crew_char; // Show crew if present
            } else if (hasSuppressionEffect(x, y)) {
                const SuppressionEffect& effect = suppression_effects[y][x];
                if (effect.is_firebreak) {
                    std::cout << '#'; // Firebreak
                } else if (effect.water_level > 0.5) {
                    std::cout << '~'; // Water effect
                } else if (effect.retardant_level > 0.5) {
                    std::cout << 'R'; // Retardant effect
                } else {
                    std::cout << cells[y][x].getDisplayChar();
                }
            } else {
                std::cout << cells[y][x].getDisplayChar();
            }
        }
        std::cout << "|\n";
    }
    
    std::cout << "+";
    for (int x = 0; x < width; ++x) std::cout << "-";
    std::cout << "+\n";
}
#include "FirefightingCrew.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

FirefightingCrew::FirefightingCrew(int crew_id, const std::string& crew_name, 
                                   CrewType crew_type, int start_x, int start_y)
    : id(crew_id), name(crew_name), type(crew_type), x(start_x), y(start_y),
      fatigue(0.0), available(true) {
    
    switch (type) {
        case CrewType::GROUND_CREW:
            water_capacity = 500.0;
            retardant_capacity = 0.0;
            effectiveness = 0.6;
            speed = 2.0;
            break;
        case CrewType::WATER_TANKER:
            water_capacity = 3000.0;
            retardant_capacity = 0.0;
            effectiveness = 0.8;
            speed = 4.0;
            break;
        case CrewType::AIR_TANKER:
            water_capacity = 1000.0;
            retardant_capacity = 2000.0;
            effectiveness = 0.9;
            speed = 8.0;
            break;
        case CrewType::HELICOPTER:
            water_capacity = 1500.0;
            retardant_capacity = 500.0;
            effectiveness = 0.7;
            speed = 6.0;
            break;
    }
    
    current_water = water_capacity;
    current_retardant = retardant_capacity;
}

void FirefightingCrew::moveTo(int target_x, int target_y) {
    if (!available) return;
    
    x = target_x;
    y = target_y;
    fatigue += 0.05; // Movement causes fatigue
    fatigue = std::min(fatigue, 1.0);
}

SuppressionAction FirefightingCrew::deployWater(int target_x, int target_y, int radius) {
    SuppressionAction action;
    action.type = SuppressionType::WATER;
    action.x = target_x;
    action.y = target_y;
    action.radius = radius;
    action.effectiveness = getEffectiveness() * 0.8;
    action.duration = 300.0; // 5 minutes
    action.cost = 500.0;
    
    double water_used = std::min(current_water, water_capacity * 0.3);
    current_water -= water_used;
    fatigue += 0.1;
    fatigue = std::min(fatigue, 1.0);
    
    return action;
}

SuppressionAction FirefightingCrew::deployRetardant(int target_x, int target_y, int radius) {
    SuppressionAction action;
    action.type = SuppressionType::RETARDANT;
    action.x = target_x;
    action.y = target_y;
    action.radius = radius;
    action.effectiveness = getEffectiveness() * 0.9;
    action.duration = 1800.0; // 30 minutes
    action.cost = 2000.0;
    
    double retardant_used = std::min(current_retardant, retardant_capacity * 0.4);
    current_retardant -= retardant_used;
    fatigue += 0.15;
    fatigue = std::min(fatigue, 1.0);
    
    return action;
}

SuppressionAction FirefightingCrew::createFirebreak(int start_x, int start_y, int end_x, int end_y) {
    SuppressionAction action;
    action.type = SuppressionType::FIREBREAK;
    action.x = start_x;
    action.y = start_y;
    action.radius = abs(end_x - start_x) + abs(end_y - start_y);
    action.effectiveness = getEffectiveness() * 0.7;
    action.duration = -1.0; // Permanent
    action.cost = 1000.0;
    
    fatigue += 0.2;
    fatigue = std::min(fatigue, 1.0);
    
    return action;
}

void FirefightingCrew::refill() {
    current_water = water_capacity;
    current_retardant = retardant_capacity;
}

void FirefightingCrew::rest(double time) {
    fatigue -= time * 0.1; // Recover 10% per time unit
    fatigue = std::max(fatigue, 0.0);
}

void FirefightingCrew::update(double dt) {
    fatigue += dt * 0.01; // Gradual fatigue over time
    fatigue = std::min(fatigue, 1.0);
    
    if (fatigue > 0.8) {
        available = false; // Too tired to work
    } else if (fatigue < 0.3) {
        available = true;
    }
}

bool FirefightingCrew::canDeploy(SuppressionType type) const {
    if (!available) return false;
    
    switch (type) {
        case SuppressionType::WATER:
            return current_water > water_capacity * 0.1;
        case SuppressionType::RETARDANT:
            return current_retardant > retardant_capacity * 0.1;
        case SuppressionType::FIREBREAK:
            return fatigue < 0.7;
        case SuppressionType::EVACUATION:
            return fatigue < 0.5;
    }
    return false;
}

double FirefightingCrew::getResourceLevel(SuppressionType type) const {
    switch (type) {
        case SuppressionType::WATER:
            return current_water / water_capacity;
        case SuppressionType::RETARDANT:
            return current_retardant / retardant_capacity;
        default:
            return 1.0 - fatigue;
    }
}

std::string FirefightingCrew::getStatusString() const {
    std::stringstream ss;
    ss << name << " (" << id << ") - ";
    
    switch (type) {
        case CrewType::GROUND_CREW: ss << "Ground"; break;
        case CrewType::WATER_TANKER: ss << "Water"; break;
        case CrewType::AIR_TANKER: ss << "Air"; break;
        case CrewType::HELICOPTER: ss << "Heli"; break;
    }
    
    ss << " [" << x << "," << y << "] ";
    ss << "Water: " << (int)(getWaterLevel() * 100) << "% ";
    if (retardant_capacity > 0) {
        ss << "Retardant: " << (int)(getRetardantLevel() * 100) << "% ";
    }
    ss << "Fatigue: " << (int)(fatigue * 100) << "%";
    
    if (!available) ss << " (RESTING)";
    
    return ss.str();
}

// HumanFactorManager implementation
HumanFactorManager::HumanFactorManager(double initial_budget) 
    : total_budget(initial_budget), spent_budget(0.0), next_crew_id(1) {
}

void HumanFactorManager::addCrew(const std::string& name, CrewType type, int x, int y) {
    crews.emplace_back(next_crew_id++, name, type, x, y);
}

void HumanFactorManager::deployCrewToLocation(int crew_id, int x, int y) {
    for (auto& crew : crews) {
        if (crew.getId() == crew_id && crew.isAvailable()) {
            crew.moveTo(x, y);
            break;
        }
    }
}

SuppressionAction HumanFactorManager::orderSuppression(int crew_id, SuppressionType type, 
                                                       int x, int y, int radius) {
    for (auto& crew : crews) {
        if (crew.getId() == crew_id && crew.canDeploy(type)) {
            SuppressionAction action;
            
            switch (type) {
                case SuppressionType::WATER:
                    action = crew.deployWater(x, y, radius);
                    break;
                case SuppressionType::RETARDANT:
                    action = crew.deployRetardant(x, y, radius);
                    break;
                case SuppressionType::FIREBREAK:
                    action = crew.createFirebreak(x, y, x + radius, y);
                    break;
                default:
                    action.effectiveness = 0.0;
                    action.cost = 0.0;
                    break;
            }
            
            if (canAfford(action.cost)) {
                spendBudget(action.cost);
                return action;
            }
        }
    }
    
    SuppressionAction failed_action;
    failed_action.effectiveness = 0.0;
    failed_action.cost = 0.0;
    return failed_action;
}

void HumanFactorManager::updateCrews(double dt) {
    for (auto& crew : crews) {
        crew.update(dt);
    }
}

void HumanFactorManager::addEvacuationZone(const std::string& name, int x, int y, 
                                           int radius, int population) {
    EvacuationZone zone;
    zone.name = name;
    zone.x = x;
    zone.y = y;
    zone.radius = radius;
    zone.population = population;
    zone.evacuated = 0;
    zone.evacuation_ordered = false;
    zone.danger_level = 0.0;
    evacuation_zones.push_back(zone);
}

void HumanFactorManager::orderEvacuation(int zone_index) {
    if (zone_index >= 0 && zone_index < static_cast<int>(evacuation_zones.size())) {
        evacuation_zones[zone_index].evacuation_ordered = true;
    }
}

void HumanFactorManager::updateEvacuations(double dt) {
    (void)dt; // Suppress unused parameter warning
    for (auto& zone : evacuation_zones) {
        if (zone.evacuation_ordered && zone.evacuated < zone.population) {
            int evacuation_rate = std::max(1, zone.population / 100); // 1% per update
            zone.evacuated = std::min(zone.population, zone.evacuated + evacuation_rate);
        }
    }
}

bool HumanFactorManager::canAfford(double cost) const {
    return (spent_budget + cost) <= total_budget;
}

void HumanFactorManager::spendBudget(double amount) {
    spent_budget += amount;
}

void HumanFactorManager::printStatus() const {
    std::cout << "=== Human Factors Status ===\n";
    std::cout << "Budget: $" << (int)getRemainingBudget() << " / $" << (int)total_budget << "\n\n";
    
    std::cout << "Firefighting Crews (" << crews.size() << "):\n";
    for (const auto& crew : crews) {
        std::cout << "  " << crew.getStatusString() << "\n";
    }
    
    if (!evacuation_zones.empty()) {
        std::cout << "\nEvacuation Zones (" << evacuation_zones.size() << "):\n";
        for (const auto& zone : evacuation_zones) {
            std::cout << "  " << zone.name << " [" << zone.x << "," << zone.y << "] ";
            std::cout << "Pop: " << zone.evacuated << "/" << zone.population;
            if (zone.evacuation_ordered) std::cout << " (EVACUATING)";
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

char HumanFactorManager::getCrewDisplayChar(int x, int y) const {
    for (const auto& crew : crews) {
        if (crew.getX() == x && crew.getY() == y) {
            switch (crew.getType()) {
                case CrewType::GROUND_CREW: return 'G';
                case CrewType::WATER_TANKER: return 'W';
                case CrewType::AIR_TANKER: return 'A';
                case CrewType::HELICOPTER: return 'H';
            }
        }
    }
    return ' '; // No crew at this location
}
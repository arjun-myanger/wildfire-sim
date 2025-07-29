#include "Cell.h"
#include <algorithm>
#include <cmath>

Cell::Cell(FuelType type, double density, double moisture_level) 
    : state(CellState::FUEL), fuel_type(type), fuel_density(density), 
      moisture(moisture_level), temperature(20.0), burn_time(0.0) {
    
    // Water and rock can't burn
    if (type == FuelType::WATER || type == FuelType::ROCK) {
        state = CellState::EMPTY;
        fuel_density = 0.0;
    }
}

void Cell::ignite() {
    if (canBurn()) {
        state = CellState::BURNING;
        burn_time = 0.0;
        temperature = 300.0; // Initial fire temperature
    }
}

void Cell::update(double dt) {
    if (state == CellState::BURNING) {
        burn_time += dt;
        
        // Different fuel types burn for different durations
        double burn_duration;
        switch (fuel_type) {
            case FuelType::GRASS: burn_duration = 30.0; break;  // 30 seconds
            case FuelType::SHRUB: burn_duration = 120.0; break; // 2 minutes
            case FuelType::TREE:  burn_duration = 300.0; break; // 5 minutes
            default: burn_duration = 30.0; break;
        }
        
        // Fuel density affects burn duration
        burn_duration *= fuel_density;
        
        // High moisture slows burning
        burn_duration *= (1.0 + moisture);
        
        // Temperature decreases over time
        double burn_progress = burn_time / burn_duration;
        temperature = 300.0 * (1.0 - burn_progress) + 20.0;
        
        // Burn out when fuel is consumed
        if (burn_time >= burn_duration) {
            state = CellState::BURNED;
            temperature = 20.0;
            fuel_density = 0.0;
        }
    }
}

bool Cell::canBurn() const {
    return (state == CellState::FUEL) && 
           (fuel_type != FuelType::WATER) && 
           (fuel_type != FuelType::ROCK) &&
           (fuel_density > 0.1); // Need minimum fuel
}

double Cell::getIgnitionProbability() const {
    if (!canBurn()) return 0.0;
    
    double base_prob = fuel_density;
    
    // Fuel type affects ignition
    switch (fuel_type) {
        case FuelType::GRASS: base_prob *= 1.2; break;
        case FuelType::SHRUB: base_prob *= 1.0; break;
        case FuelType::TREE:  base_prob *= 0.8; break;
        default: break;
    }
    
    // Moisture reduces ignition probability
    base_prob *= (1.0 - moisture * 0.8);
    
    // Temperature affects ignition (higher temp = easier ignition)
    if (temperature > 50.0) {
        base_prob *= (1.0 + (temperature - 50.0) / 100.0);
    }
    
    return std::min(1.0, std::max(0.0, base_prob));
}

char Cell::getDisplayChar() const {
    switch (state) {
        case CellState::EMPTY:
            if (fuel_type == FuelType::WATER) return '~';
            if (fuel_type == FuelType::ROCK) return '#';
            return ' ';
        case CellState::FUEL:
            switch (fuel_type) {
                case FuelType::GRASS: return '.';
                case FuelType::SHRUB: return 'o';
                case FuelType::TREE:  return 'T';
                default: return '.';
            }
        case CellState::BURNING: return '*';
        case CellState::BURNED:  return 'x';
        default: return '?';
    }
}
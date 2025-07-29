#pragma once

enum class CellState {
    EMPTY,      // No fuel
    FUEL,       // Unburned vegetation
    BURNING,    // Currently on fire
    BURNED      // Already burned out
};

enum class FuelType {
    GRASS,      // Fast burning, low intensity
    SHRUB,      // Medium burning, medium intensity
    TREE,       // Slow burning, high intensity
    WATER,      // Non-flammable
    ROCK        // Non-flammable
};

class Cell {
private:
    CellState state;
    FuelType fuel_type;
    double fuel_density;    // 0.0 to 1.0
    double moisture;        // 0.0 to 1.0
    double temperature;     // In Celsius
    double burn_time;       // How long it's been burning
    
public:
    Cell(FuelType type = FuelType::GRASS, double density = 0.8, double moisture = 0.3);
    
    // Getters
    CellState getState() const { return state; }
    FuelType getFuelType() const { return fuel_type; }
    double getFuelDensity() const { return fuel_density; }
    double getMoisture() const { return moisture; }
    double getTemperature() const { return temperature; }
    double getBurnTime() const { return burn_time; }
    
    // Setters
    void setState(CellState new_state) { state = new_state; }
    void setFuelType(FuelType type) { fuel_type = type; }
    void setFuelDensity(double density) { fuel_density = density; }
    void setMoisture(double moisture_level) { moisture = moisture_level; }
    void setTemperature(double temp) { temperature = temp; }
    
    // Fire simulation methods
    void ignite();
    void update(double dt);
    bool canBurn() const;
    double getIgnitionProbability() const;
    char getDisplayChar() const;
};
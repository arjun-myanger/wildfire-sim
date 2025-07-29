#pragma once
#include <string>
#include <vector>

enum class CrewType {
    GROUND_CREW,    // Manual firefighting, firebreaks
    WATER_TANKER,   // Water drops
    AIR_TANKER,     // Retardant drops
    HELICOPTER      // Versatile water/rescue operations
};

enum class SuppressionType {
    WATER,          // Reduces fire intensity and spread
    RETARDANT,      // Long-term fire suppression
    FIREBREAK,      // Physical barrier creation
    EVACUATION      // Civilian rescue operations
};

struct SuppressionAction {
    SuppressionType type;
    int x, y;           // Target coordinates
    int radius;         // Effect radius
    double effectiveness; // 0.0 to 1.0
    double duration;    // How long the effect lasts
    double cost;        // Resource cost
};

class FirefightingCrew {
private:
    int id;
    std::string name;
    CrewType type;
    int x, y;               // Current position
    double water_capacity;  // Liters
    double retardant_capacity; // Liters
    double current_water;
    double current_retardant;
    double effectiveness;   // Base effectiveness 0.0 to 1.0
    double fatigue;        // 0.0 (fresh) to 1.0 (exhausted)
    bool available;        // Can take new assignments
    double speed;          // Movement speed (cells per time unit)
    
public:
    FirefightingCrew(int crew_id, const std::string& crew_name, CrewType crew_type, 
                     int start_x, int start_y);
    
    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    CrewType getType() const { return type; }
    int getX() const { return x; }
    int getY() const { return y; }
    double getWaterLevel() const { return current_water / water_capacity; }
    double getRetardantLevel() const { return current_retardant / retardant_capacity; }
    double getEffectiveness() const { return effectiveness * (1.0 - fatigue); }
    double getFatigue() const { return fatigue; }
    bool isAvailable() const { return available; }
    
    // Operations
    void moveTo(int target_x, int target_y);
    SuppressionAction deployWater(int target_x, int target_y, int radius);
    SuppressionAction deployRetardant(int target_x, int target_y, int radius);
    SuppressionAction createFirebreak(int start_x, int start_y, int end_x, int end_y);
    void refill(); // Refill water/retardant at base
    void rest(double time); // Reduce fatigue
    void update(double dt); // Update crew state over time
    
    // Status
    bool canDeploy(SuppressionType type) const;
    double getResourceLevel(SuppressionType type) const;
    std::string getStatusString() const;
};

struct EvacuationZone {
    int x, y;           // Center coordinates
    int radius;         // Zone radius
    int population;     // Number of civilians
    int evacuated;      // Number already evacuated
    bool evacuation_ordered; // Evacuation status
    double danger_level;     // 0.0 (safe) to 1.0 (critical)
    std::string name;        // Zone identifier
};

class HumanFactorManager {
private:
    std::vector<FirefightingCrew> crews;
    std::vector<EvacuationZone> evacuation_zones;
    double total_budget;    // Available resources
    double spent_budget;    // Resources used
    int next_crew_id;
    
public:
    HumanFactorManager(double initial_budget = 100000.0);
    
    // Crew management
    void addCrew(const std::string& name, CrewType type, int x, int y);
    void deployCrewToLocation(int crew_id, int x, int y);
    SuppressionAction orderSuppression(int crew_id, SuppressionType type, 
                                     int x, int y, int radius);
    void updateCrews(double dt);
    
    // Evacuation management
    void addEvacuationZone(const std::string& name, int x, int y, int radius, int population);
    void orderEvacuation(int zone_index);
    void updateEvacuations(double dt);
    
    // Resource management
    bool canAfford(double cost) const;
    void spendBudget(double amount);
    double getRemainingBudget() const { return total_budget - spent_budget; }
    
    // Status and display
    std::vector<FirefightingCrew>& getCrews() { return crews; }
    std::vector<EvacuationZone>& getEvacuationZones() { return evacuation_zones; }
    void printStatus() const;
    char getCrewDisplayChar(int x, int y) const;
};
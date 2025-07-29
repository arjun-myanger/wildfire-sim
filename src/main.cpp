#include "FireSimulation.h"
#include <iostream>
#include <string>

void printMenu() {
    std::cout << "\n=== Wildfire Simulation ===\n";
    std::cout << "1. Run grassland simulation\n";
    std::cout << "2. Run forest simulation\n";
    std::cout << "3. Run mixed terrain simulation\n";
    std::cout << "4. Custom simulation\n";
    std::cout << "5. Run human factors demo\n";
    std::cout << "6. Exit\n";
    std::cout << "Choose option (1-6): ";
}

void setupFirefightingCrews(FireSimulation& sim, const std::string& scenario) {
    HumanFactorManager& hm = sim.getHumanManager();
    
    if (scenario == "grassland") {
        hm.addCrew("Alpha", CrewType::GROUND_CREW, 5, 5);
        hm.addCrew("Bravo", CrewType::WATER_TANKER, 35, 15);
    } else if (scenario == "forest") {
        hm.addCrew("Charlie", CrewType::GROUND_CREW, 5, 5);
        hm.addCrew("Delta", CrewType::AIR_TANKER, 35, 15);
        hm.addCrew("Echo", CrewType::HELICOPTER, 20, 2);
    } else if (scenario == "mixed") {
        hm.addCrew("Foxtrot", CrewType::GROUND_CREW, 3, 3);
        hm.addCrew("Golf", CrewType::WATER_TANKER, 25, 12);
        hm.addCrew("Hotel", CrewType::AIR_TANKER, 45, 20);
        
        // Add evacuation zones
        hm.addEvacuationZone("Residential Area", 10, 10, 5, 250);
        hm.addEvacuationZone("Camp Ground", 40, 15, 3, 80);
    } else if (scenario == "demo") {
        hm.addCrew("Alpha Team", CrewType::GROUND_CREW, 10, 5);
        hm.addCrew("Water-1", CrewType::WATER_TANKER, 30, 5);
        hm.addCrew("Air-1", CrewType::AIR_TANKER, 15, 2);
        hm.addCrew("Rescue-1", CrewType::HELICOPTER, 5, 15);
        
        hm.addEvacuationZone("Town Center", 20, 10, 4, 500);
        hm.addEvacuationZone("School", 35, 15, 2, 120);
    }
}

void runSimulation(FireSimulation& sim, const std::string& scenario) {
    std::cout << "\nStarting " << scenario << " simulation...\n";
    
    // Set up firefighting crews for this scenario
    setupFirefightingCrews(sim, scenario);
    
    // Show initial crew deployment
    std::cout << "Firefighting crews deployed:\n";
    sim.getHumanManager().printStatus();
    
    std::cout << "\nPress Ctrl+C to stop the simulation\n\n";
    
    // Start fire in the center
    int center_x = sim.getGrid().getWidth() / 2;
    int center_y = sim.getGrid().getHeight() / 2;
    sim.addIgnitionPoint(center_x, center_y);
    
    // Simulate some suppression actions
    if (scenario != "custom") {
        // Deploy suppression near fire center
        HumanFactorManager& hm = sim.getHumanManager();
        auto& crews = hm.getCrews();
        
        if (crews.size() > 0) {
            hm.orderSuppression(crews[0].getId(), SuppressionType::WATER, 
                              center_x - 3, center_y - 3, 2);
        }
        if (crews.size() > 1) {
            hm.orderSuppression(crews[1].getId(), SuppressionType::RETARDANT, 
                              center_x + 5, center_y + 5, 3);
        }
        
        // Order evacuations if zones exist
        auto& zones = hm.getEvacuationZones();
        for (size_t i = 0; i < zones.size(); ++i) {
            hm.orderEvacuation(i);
        }
    }
    
    // Run simulation for 5 minutes (300 seconds) or until fire burns out
    sim.run(300.0);
    
    std::cout << "\nSimulation finished!\n";
    sim.printStatus();
    sim.getHumanManager().printStatus();
    
    std::cout << "\nSave results to file? (y/n): ";
    char save;
    std::cin >> save;
    if (save == 'y' || save == 'Y') {
        std::string filename = scenario + "_results.txt";
        sim.saveToFile(filename);
        std::cout << "Results saved to " << filename << "\n";
    }
}

void customSimulation() {
    std::cout << "\n=== Custom Simulation Setup ===\n";
    
    int width, height;
    std::cout << "Grid width (10-100): ";
    std::cin >> width;
    std::cout << "Grid height (10-100): ";
    std::cin >> height;
    
    width = std::max(10, std::min(100, width));
    height = std::max(10, std::min(100, height));
    
    FireSimulation sim(width, height);
    
    std::cout << "\nTerrain setup:\n";
    std::cout << "1. Random mixed terrain\n";
    std::cout << "2. Predefined terrain\n";
    std::cout << "Choose (1-2): ";
    int terrain_choice;
    std::cin >> terrain_choice;
    
    if (terrain_choice == 1) {
        sim.setupMixed();
    } else {
        sim.getGrid().initializeTerrain();
    }
    
    // Weather settings
    double wind_speed, wind_dir, temp, humidity;
    std::cout << "\nWind speed (0-20 m/s): ";
    std::cin >> wind_speed;
    std::cout << "Wind direction (0-360 degrees): ";
    std::cin >> wind_dir;
    std::cout << "Temperature (0-50 °C): ";
    std::cin >> temp;
    std::cout << "Humidity (0.0-1.0): ";
    std::cin >> humidity;
    
    sim.getGrid().setWindSpeed(std::max(0.0, std::min(20.0, wind_speed)));
    sim.getGrid().setWindDirection(wind_dir);
    sim.getGrid().setAmbientTemp(std::max(0.0, std::min(50.0, temp)));
    sim.getGrid().setHumidity(std::max(0.0, std::min(1.0, humidity)));
    
    // Ignition points
    std::cout << "\nNumber of ignition points (1-5): ";
    int num_ignitions;
    std::cin >> num_ignitions;
    num_ignitions = std::max(1, std::min(5, num_ignitions));
    
    for (int i = 0; i < num_ignitions; ++i) {
        int x, y;
        std::cout << "Ignition point " << (i+1) << " - X coordinate (0-" << (width-1) << "): ";
        std::cin >> x;
        std::cout << "Ignition point " << (i+1) << " - Y coordinate (0-" << (height-1) << "): ";
        std::cin >> y;
        
        x = std::max(0, std::min(width-1, x));
        y = std::max(0, std::min(height-1, y));
        sim.addIgnitionPoint(x, y);
    }
    
    runSimulation(sim, "custom");
}

void humanFactorsDemo() {
    std::cout << "\n=== Human Factors Demonstration ===\n";
    std::cout << "This demo shows firefighting crews and evacuation zones in action.\n\n";
    
    FireSimulation sim(40, 25);
    sim.setupMixed();
    sim.getGrid().setWindSpeed(10.0);
    sim.getGrid().setWindDirection(45.0);
    sim.getGrid().setHumidity(0.3);
    
    runSimulation(sim, "demo");
}

int main() {
    std::cout << "Welcome to the Wildfire Simulation!\n";
    std::cout << "This simulation models fire spread across different terrains.\n";
    
    while (true) {
        printMenu();
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                FireSimulation sim(40, 20);
                sim.setupGrassland();
                sim.getGrid().setWindSpeed(8.0);
                sim.getGrid().setWindDirection(45.0);
                runSimulation(sim, "grassland");
                break;
            }
            case 2: {
                FireSimulation sim(40, 20);
                sim.setupForest();
                sim.getGrid().setWindSpeed(3.0);
                sim.getGrid().setWindDirection(90.0);
                sim.getGrid().setHumidity(0.6);
                runSimulation(sim, "forest");
                break;
            }
            case 3: {
                FireSimulation sim(50, 25);
                sim.setupMixed();
                sim.getGrid().setWindSpeed(12.0);
                sim.getGrid().setWindDirection(135.0);
                runSimulation(sim, "mixed");
                break;
            }
            case 4: {
                customSimulation();
                break;
            }
            case 5: {
                humanFactorsDemo();
                break;
            }
            case 6: {
                std::cout << "Thank you for using the Wildfire Simulation!\n";
                return 0;
            }
            default: {
                std::cout << "Invalid choice. Please select 1-6.\n";
                break;
            }
        }
    }
    
    return 0;
}
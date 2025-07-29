# 🔥 Wildfire Simulation with Human Factors

A comprehensive C++ wildfire simulation that models realistic fire spread dynamics, environmental factors, and human intervention strategies including firefighting operations and civilian evacuations.

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)

## 🌟 Features

### 🔥 Fire Dynamics
- **Realistic fire spread** based on fuel type, moisture, and environmental conditions
- **Multiple terrain types**: Grass, shrubs, trees, water bodies, and rock formations
- **Environmental factors**: Wind speed/direction, temperature, and humidity effects
- **Physics-based modeling** with time-stepped simulation

### 👨‍🚒 Human Factors & Firefighting
- **Firefighting crews** with different specializations:
  - 🏃‍♂️ **Ground Crews**: Manual firefighting and firebreak creation
  - 🚛 **Water Tankers**: Large-scale water deployment
  - ✈️ **Air Tankers**: Retardant drops for long-term suppression
  - 🚁 **Helicopters**: Versatile water drops and rescue operations

### 🛡️ Suppression Systems
- **Water drops** for immediate fire suppression
- **Retardant deployment** for long-term fire prevention
- **Firebreak creation** as permanent barriers
- **Resource management** with budget constraints and crew fatigue

### 🏘️ Evacuation Management
- **Evacuation zones** with civilian populations
- **Automatic evacuation orders** based on fire proximity
- **Real-time evacuation progress** tracking
- **Risk assessment** for populated areas

### 📊 Real-time Visualization
- **ASCII-based display** with intuitive symbols
- **Crew position tracking** (G/W/A/H indicators)
- **Suppression effect visualization** (~/#/R symbols)
- **Live statistics** showing burn progress, crew status, and evacuation data

## 🎮 Demo Scenarios

1. **🌾 Grassland Simulation**: Fast-spreading fire in open terrain
2. **🌲 Forest Simulation**: Dense vegetation with challenging suppression
3. **🗺️ Mixed Terrain**: Complex landscape with varied fuel types
4. **⚙️ Custom Simulation**: User-defined parameters and conditions
5. **👨‍🚒 Human Factors Demo**: Full-featured scenario with crews and evacuations

## 🚀 Quick Start

### Prerequisites
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Make or CMake build system

### 🔨 Build Instructions

#### Using Make
```bash
# Clone the repository
git clone https://github.com/yourusername/wildfire-simulation.git
cd wildfire-simulation

# Build the simulation
make

# Run the simulation
./wildfire_sim
```

#### Using CMake
```bash
# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the simulation
./wildfire_sim
```

### 🎯 Usage

1. **Launch the simulation**: `./wildfire_sim`
2. **Choose a scenario** from the menu (1-6)
3. **Watch the simulation** unfold in real-time
4. **Observe firefighting operations** and evacuation progress
5. **Save results** to file when prompted

## 🎨 Visual Legend

| Symbol | Meaning |
|--------|---------|
| `.` | Grass terrain |
| `o` | Shrub terrain |
| `T` | Tree/forest |
| `*` | Active fire |
| `x` | Burned area |
| `~` | Water/suppression effect |
| `#` | Rock/firebreak |
| `R` | Retardant effect |
| `G` | Ground crew |
| `W` | Water tanker |
| `A` | Air tanker |
| `H` | Helicopter |

## 📈 Example Output

```
=== Human Factors Status ===
Budget: $95500 / $100000

Firefighting Crews (4):
  Alpha Team (1) - Ground [10,5] Water: 70% Fatigue: 25%
  Water-1 (2) - Water [30,5] Water: 85% Fatigue: 40%
  Air-1 (3) - Air [15,2] Water: 100% Retardant: 60% Fatigue: 60%
  Rescue-1 (4) - Heli [5,15] Water: 90% Retardant: 100% Fatigue: 30%

Evacuation Zones (2):
  Town Center [20,10] Pop: 324/500 (EVACUATING)
  School [35,15] Pop: 120/120 ✅ COMPLETE
```

## 🛠️ Technical Details

### Architecture
- **Modular design** with separate classes for simulation components
- **Grid-based cellular automaton** for fire spread modeling
- **Object-oriented crew management** system
- **Event-driven evacuation** processing

### Key Classes
- `FireSimulation`: Main simulation controller
- `Grid`: Terrain and fire state management
- `Cell`: Individual terrain cell properties
- `FirefightingCrew`: Crew behavior and resource management
- `HumanFactorManager`: Coordinates all human intervention activities

### Algorithms
- **Probabilistic fire spread** based on environmental factors
- **Dijkstra-inspired pathfinding** for optimal crew deployment
- **Resource optimization** for suppression effectiveness
- **Real-time fatigue modeling** for crew management

## 🔧 Customization

The simulation supports extensive customization:

- **Terrain generation**: Modify `Grid::initializeRandom()` for custom landscapes
- **Fire behavior**: Adjust spread probabilities in `Grid::calculateSpreadProbability()`
- **Crew capabilities**: Customize crew types in `FirefightingCrew` constructor
- **Environmental conditions**: Set wind, temperature, and humidity parameters

## 🤝 Contributing

Contributions are welcome! Areas for enhancement:

- 🗺️ **GIS integration** for real-world terrain data
- 🌡️ **Advanced weather modeling** with dynamic conditions
- 🛰️ **Satellite imagery support** for realistic scenarios
- 📱 **Web interface** for remote simulation control
- 🔬 **Machine learning** for optimal suppression strategies

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by real wildfire management practices
- Fire behavior models based on forestry research
- Built with modern C++ best practices

---

⭐ **Star this repository** if you find it useful for wildfire research, education, or simulation purposes!

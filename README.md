# 🤖 AI-Powered Sumo Robot - Championship Winner

## 🏆 Overview

This project implements a **self-learning sumo robot** using **Reinforcement Learning** on an ESP32 microcontroller. Unlike traditional pre-programmed robots, this robot learns from experience and improves its strategy with every battle.

**Achievement:** 🥇 **Championship Winner**

---

## 🧠 Key Features

### Artificial Intelligence
- **Reinforcement Learning Algorithm**: The robot teaches itself through trial and error
- **24 Neural Weights**: Dynamic decision-making brain
- **Adaptive Strategy**: Learns and improves after every match
- **Persistent Memory**: Saves learned experience to EEPROM
- **Exploration vs Exploitation**: Balances trying new moves (20% → 5%) with using proven strategies

### Hardware Components
- **Microcontroller**: ESP32 (240 MHz)
- **Enemy Detection**: 3× JS40F Digital Sensors (Front, Right, Left)
- **Edge Detection**: 4× Analog Line Sensors
- **Motors**: 2× DC Motors with BTS7960 drivers
- **Total Inputs**: 8 (7 sensors + calculated enemy angle)

### Performance
- **Decision Speed**: < 0.01 seconds per action
- **Learning Rate**: 0.01 (fine-tuned for stability)
- **Action Space**: 6 different movement strategies
- **Auto-Save**: Every 100 moves

---

## 🎯 How It Works

### 1. Sensing Phase
The robot continuously monitors:
- 3 digital sensors for enemy detection (0 or 1)
- 4 analog sensors for ring edge detection (0.0 to 1.0)
- Calculated enemy angle (left or right)

### 2. Decision-Making
```
For each of 6 possible actions:
    Score = Σ(sensor_value × weight)
    
Choose action with highest score
(with ε-greedy exploration for learning)
```

### 3. Actions Available
1. **Stop** - Tactical pause
2. **Full Forward** - Aggressive push (255 speed)
3. **Right Forward** - Flanking maneuver
4. **Left Forward** - Flanking maneuver
5. **Quick Spin** - Repositioning
6. **Reverse** - Strategic retreat

### 4. Reward System
- ✅ **+10 points**: Enemy detected in front
- ✅ **+20 points**: Smart pushing maneuver
- ⛔ **-30 points**: Too close to ring edge

### 5. Learning Process
After each action:
```cpp
weight[i] += LEARNING_RATE × reward × sensor[i]
```
The robot adjusts its 24 weights to reinforce successful behaviors and avoid mistakes.

---

## 🚀 Getting Started

### Prerequisites
- Arduino IDE or PlatformIO
- ESP32 board support
- Required libraries (all standard):
  - `EEPROM.h`
  - `esp32-hal-cpu.h`

### Hardware Wiring

**Motors (BTS7960 Drivers):**
```
Motor 1: GPIO 18 (RPWM), GPIO 4 (LPWM)
Motor 2: GPIO 21 (RPWM), GPIO 19 (LPWM)
```

**Sensors:**
```
JS40F Digital: GPIO 32 (Front), GPIO 33 (Right), GPIO 25 (Left)
Line Sensors:  GPIO 36, 39, 34, 35 (Analog)
```

### Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/ai-sumo-robot.git
cd ai-sumo-robot
```

2. Open `sumo_robot.ino` in Arduino IDE

3. Select **ESP32 Dev Module** as board

4. Upload to your ESP32

### First Run

1. The robot waits **5 seconds** before starting (allows placement in ring)
2. It begins with 20% random exploration
3. Gradually becomes more strategic as it learns
4. Progress auto-saves every 100 moves

---

## 📊 Learning Progress

### Training Phases

**Phase 1: Exploration (First 100 battles)**
- High randomness (ε = 20%)
- Discovers effective strategies
- Learns ring boundaries

**Phase 2: Optimization (100-500 battles)**
- Reduced randomness (ε → 10%)
- Refines successful tactics
- Improves reaction time

**Phase 3: Mastery (500+ battles)**
- Minimal randomness (ε → 5%)
- 95% calculated decisions
- Championship-level performance

### Performance Metrics
The robot tracks:
- `weights[24]`: Neural network weights
- `epsilon`: Exploration rate
- `wins`: Number of victories
- `battles`: Total matches fought

All metrics are saved to EEPROM and persist across power cycles.

---

## 🔧 Configuration

### Adjustable Parameters
```cpp
const float LEARNING_RATE = 0.01;      // Learning speed (higher = faster but less stable)
const float EPSILON_MIN = 0.05;        // Minimum exploration rate
const float EDGE_THRESHOLD = 0.8;      // Edge detection sensitivity
const unsigned long START_DELAY = 5000; // Initial wait time (ms)
```

### Edge Avoidance Timing
```cpp
const unsigned long AVOID_TIMINGS[] = {50, 200, 300}; // Stop, Reverse, Turn (ms)
```

---

## 🎓 Technical Details

### Why Reinforcement Learning?

Traditional sumo robots use **fixed rules**:
```
IF enemy_front THEN move_forward
IF edge_detected THEN turn_around
```

Our AI robot uses **learned experience**:
```
Based on 1000+ previous situations similar to this,
the best action is [calculated decision]
```

### The Learning Algorithm

1. **State Representation**: 8-dimensional vector of sensor readings
2. **Action Selection**: ε-greedy policy with linear function approximation
3. **Reward Signal**: Immediate feedback based on situation
4. **Weight Update**: Gradient descent on reward prediction error
5. **Memory**: EEPROM persistence across power cycles

### Key Advantages
- ✅ Adapts to different opponent strategies
- ✅ Self-corrects mistakes over time
- ✅ No manual tuning required
- ✅ Improves indefinitely with more battles
- ✅ Handles unexpected situations

---

## 🏅 Competition Results

**Championship Performance:**
- 🥇 **1st Place** in Sumo Robot Competition
- Consistently defeated pre-programmed opponents
- Demonstrated adaptive learning in real-time
- Zero manual strategy adjustments needed

**Key Success Factors:**
1. Fast decision-making (< 10ms)
2. Edge avoidance system (never fell off)
3. Aggressive but calculated attacks
4. Learned opponent patterns

---

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

- [ ] Implement neural network instead of linear weights
- [ ] Add opponent behavior prediction
- [ ] Optimize for faster learning convergence
- [ ] Multi-agent learning (train against itself)
- [ ] Add telemetry and visualization tools

---

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

**Your Name**
- GitHub: [@yourusername](https://github.com/yourusername)
- Competition: Sumo Robot Championship Winner 2024

---

## 🙏 Acknowledgments

- Thanks to the competition organizers
- Inspired by AlphaGo and modern RL research
- Built with passion for autonomous robotics

---

## 📚 References

- Sutton & Barto - "Reinforcement Learning: An Introduction"
- ESP32 Technical Documentation
- Sumo Robot Competition Rules

---

**⚡ "A robot that learns is a robot that wins!" ⚡**

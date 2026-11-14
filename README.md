<div align="center">

#   Vertical Scrolling Shooter 2D Game - From Code to Game — SDL3

**CodeRun7 (November 2026) — Hard Task Implementation**  
*Organized by BEST Cluj-Napoca & Technical University of Cluj-Napoca (TUCN)*

![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Library](https://img.shields.io/badge/Graphics-SDL3-1E90FF?style=for-the-badge)
![Math](https://img.shields.io/badge/Math-GLM-FF6F00?style=for-the-badge)
![Difficulty](https://img.shields.io/badge/Task-Hard%20Mode-DC143C?style=for-the-badge)
![Event](https://img.shields.io/badge/Event-CodeRun7%20%40%20TUCN-2E8B57?style=for-the-badge)

</div>

---

## 📖 Overview

Inspired by classic arcade vertical scrolling shooters such as Namco's *Xevious* (1982), Jaleco's *Exerion*, and Epic MegaGames' *Tyrian 2000* (1999), this project is a 2D arcade shooter built from scratch in **C/C++** using **SDL3** and **GLM**.

Developed for the **Hard Task** track of **CodeRun7**, the game combines inertial ship physics, multi-angle enemy projectile spreads, slice-based dynamic wave spawning, and a kinetic **Repulsor** defense mechanic—all engineered with center-relative, resolution-independent geometry so the game runs smoothly and indefinitely regardless of window resizing.

---

## 🎮 Gameplay Preview

---

## ⚙️ Core Mechanics & Physics (Hard Mode)

### 🛸 1. Inertial Ship Kinematics
- **Relative Mouse Capture:** Uses `SDL_SetWindowRelativeMouseMode` with a hidden cursor to capture seamless mouse deltas (`xrel`, `yrel`).
- **Acceleration & Drag:** The player ship (`50 × 50 px`, green) accelerates proportionally to mouse movement with a thrust strength of **$400\text{ px/s}^2$**, capped at a terminal velocity of **$500\text{ px/s}$**.
- **Inertial Deceleration:** When mouse input stops, the ship smoothly decelerates at **$2\times$ acceleration ($800\text{ px/s}^2$)** while remaining strictly clamped within window boundaries.

### 👾 2. Dynamic Slice-Based Enemy Spawner
- **Spatial Partitioning:** The viewport (`700 × 800 px` default) is dynamically divided into **3 equal vertical slices**, with at most **one active enemy** per slice at any time.
- **Continuous Waves:** Enemies (`50 × 50 px`, orange) enter from the top of the screen with a randomized downward velocity between **$50$ and $150\text{ px/s}$**.
- **Respawn Timer:** Every **$3.0\text{ seconds}$**, any cleared slice spawns a new enemy at a random horizontal offset within that slice.

### 🔥 3. Weapons & Enemy AI Spread-Fire
- **Primary Blaster (Left Click):** Fires red circular projectiles ($r = 10\text{ px}$) straight upward from the ship's center at **$500\text{ px/s}$**, with an automatic fire rate of **1 shot every $0.3\text{ s}$** while held.
- **3-Way Enemy Barrage:** Active enemies begin firing **$1.0\text{ s}$** after entering the viewport at a cadence **$10\times$ slower** than the player ($3.0\text{ s}$ interval). Each volley consists of three magenta projectiles ($r = 10\text{ px}$) traveling at **$100\text{ px/s}$**:
  - **Center shot:** Locked onto the player's coordinates at the moment of firing.
  - **Flanking shots:** Vector-rotated by **$+15^\circ$** and **$-15^\circ$** relative to the primary trajectory.

### 🛡️ 4. Special Ability: The "Repulsor"
- **Kinetic Shockwave (Right Click):** Triggers once per discrete right-click (unlimited uses) to survive dense spread patterns.
- **Vector Pushback:** Reduces the velocity of all active enemy projectiles along the player-to-projectile axis by **$10\text{ px/s}$ per press**. Repeated activations can completely halt incoming projectiles or reverse their trajectory away from the ship.

### 🏆 5. Win / Loss States & Resolution Independence
- **Center-Relative Geometry:** All entities, spawn zones, and collision checks are computed relative to object centers and dynamic window dimensions (zero hardcoded `700 × 800` coordinates).
- **End-Game Feedback:**
  - **Victory** (all active enemies destroyed): Scrolling and movement halt, entities clear, and the player ship turns **Blue**.
  - **Defeat** (collision with an enemy ship or projectile): Scrolling and movement halt, entities clear, and the player ship turns **Red**.
  - **Reset:** Pressing **`R`** after a win or loss cleanly resets all timers, entities, and physics states (disabled mid-match).

---

## 🕹️ Controls

| Input | Action | Behavior |
| :--- | :--- | :--- |
| **Mouse Move** | Thruster Steering | Applies inertial acceleration ($400\text{ px/s}^2$, max $500\text{ px/s}$) |
| **Left Mouse Button (Hold)** | Primary Blaster | Fires upward projectiles ($500\text{ px/s}$) every $0.3\text{ s}$ |
| **Right Mouse Button (Click)** | **Repulsor Pulse** | Pushes all enemy projectiles away ($-10\text{ px/s}$ per click) |
| **`R` Key** | Restart Game | Resets the arena (available only after Win/Loss) |
| **`Esc` / Close Window** | Quit | Exits the application |

---

## 🛠️ Tech Stack & Dependencies

- **Language:** C++ (C++17 or newer)
- **Windowing, Input & 2D Rendering:** [SDL3](https://www.libsdl.org/)
- **Vector Math & Angle Rotations:** [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm)
- **Build Systems Supported:** CMake, Visual Studio 2022 (MSVC), CLion (MinGW / macOS), Xcode

---

## 🚀 Getting Started

### Prerequisites
Ensure you have a C++ compiler along with **SDL3** and **GLM** installed or bundled in your project dependencies directory.

### Building with CMake (Windows / Linux / macOS)
```bash
# 1. Clone the repository
git clone [https://github.com/](https://github.com/)<your-username>/<your-repo-name>.git
cd <your-repo-name>

# 2. Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 3. Run the executable
./build/CodeRun7_Shooter
Building with Visual Studio 2022 / CLion
Open the provided .sln (Visual Studio) or CMakeLists.txt (CLion).

Verify that the include/library paths for SDL3 and GLM match your local environment.

Build and run in Release or Debug mode.

🎓 Acknowledgments
Event: CodeRun7 — From Code to Game: Beginning 2D Game Programming with SDL3 (November 14, 2026)

Organizers: BEST Cluj-Napoca & Technical University of Cluj-Napoca (TUCN)

Challenge Design & Training: Senior Lecturer Adrian Sabou (Computer Graphics and Interactive Systems Group, Department of Computer Science, TUCN)

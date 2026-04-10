<div align="center">
  <h1>🏬 3D Shopping Mall Simulation</h1>
  <p><strong>A highly interactive, multi-story 3D graphics project built using OpenGL and C++.</strong></p>
</div>

<br />

## 🌟 Overview

Welcome to the **3D Shopping Mall Simulation**! This project utilizes modern OpenGL to create a fully explorable, interactive shopping mall environment. It is designed to showcase advanced computer graphics techniques including procedural generation, complex geometries, dynamic lighting, texture mapping, and physics-based interactions.

![Atrium View](LatexPic/Atrium.png)

## ✨ Technical Features

### 🏛️ Multi-Story Architecture
The mall consists of a deeply detailed environment with distinct sections on every floor, such as the prayer room with intricate Bezier curves, and various shops designed individually:
- **Ground Floor:** Essential shops and beautifully textured walkways.
- **First Floor:** Escalator accessible shops and an immersive atrium.
- **Food Court:** The lively top-floor food plaza.

<p align="center">
  <img src="LatexPic/Ground_floor_shop.png" width="32%" />
  <img src="LatexPic/First_floor_shop.png" width="32%" />
  <img src="LatexPic/FoodCourt_shop.png" width="32%" />
</p>

### 🛠 Complex Geometries & Texturing
Textures are effectively processed and applied accurately onto varying geometric objects (like spheres, cylinders, and procedural terrains) to represent real-life equivalents.
- **Bezier Curves:** Used extensively to model the beautiful Mihrab architecture in the prayer room.
- **Procedural L-Systems:** Automatically generated `Fractal Tree` providing a realistic, non-uniform natural look inside the architectural space.

<p align="center">
  <img src="LatexPic/Bezier_curve_mahrib_and_texture_mapping.png" width="48%" />
  <img src="LatexPic/FractalTree.png" width="48%" />
</p>

### 💡 Advanced Lighting Models
Dynamic illumination allows the transition between different moods and states of the scene by manipulating:
- **Ambient Lighting**: Overall base illumination toggle.
- **Directional Lighting**: Simulating direct sunlight.
- **Point Lighting**: Providing glowing light sources within individual interior elements.

| Ambient Light Off | Ambient Light On |
| :---: | :---: |
| <img src="LatexPic/ambient_light_off.png" width="400" /> | <img src="LatexPic/ambient_light_on.png" width="400" /> |

| Point Light Off | Point Light On |
| :---: | :---: |
| <img src="LatexPic/point_light_off.png" width="400" /> | <img src="LatexPic/point_light_on.png" width="400" /> |

### 🕹️ Complete Interactivity
You can dynamically explore and interact with numerous elements located throughout the structure:
- **Vertical Transportation:** Functioning `Elevators` (with state controls), automatic `Escalators`, and a standard `Staircase` leading up to the rooftop shade.
- **Navigation Perspectives:** Support for four designated view points to observe action from the top-down map style view down to a first-person dynamic view.
- **NPCs:** Distinct states like a `Character Walking` around the atrium and `Character Sitting` naturally on seating elements.

<p align="center">
  <img src="LatexPic/Elevator.png" width="32%" />
  <img src="LatexPic/Escalator.png" width="32%" />
  <img src="LatexPic/StairCase.png" width="32%" />
</p>

## 🖼️ Gallery

<details>
<summary>Click to view more screenshots!</summary>
<br>

**Parking Lot**
<img src="LatexPic/parking_lot.png" width="800" />

**Four View Points**
<img src="LatexPic/four_view_points.png" width="800" />

**NPC Interactions**
<p align="center">
  <img src="LatexPic/Character_Walking.png" width="48%" />
  <img src="LatexPic/Character_sitting.png" width="48%" />
</p>

**Interactive Environment details**
<p align="center">
  <img src="LatexPic/Door_open.png" width="32%" />
  <img src="LatexPic/Freezer_Cylinder_shape_can.png" width="32%" />
  <img src="LatexPic/Elevator_door_Open.png" width="32%" />
</p>

</details>

## 🚀 Getting Started

### Prerequisites
- Visual Studio (recommended) or an equivalent C++ IDE with C++17 support.
- OpenGL, GLFW, and GLAD libraries correctly configured.
- `stb_image.h` (Included in the source for texture loading).

### How to Run
1. Open the `.sln` or `.vcxproj` project files via your IDE.
2. Ensure your build configuration is set up properly for `x64` OpenGL contexts.
3. Build the solution and run. Make sure your working directory is correctly pointed toward the assets. 

<br/>
<div align="center">
  <i>Developed with ❤️ using C++ and OpenGL.</i>
</div>

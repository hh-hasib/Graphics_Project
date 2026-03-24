Construct a highly realistic, first-person navigable parking lot system component for a 3D Modern Shopping Mall simulation using OpenGL, GLFW, and GLM.  
The environment must use illumination and shading techniques like phong shading to achieve accurate light interaction. Navigation is exclusively from a first-person perspective, controlled via W, A, S, and D keys, like a modern 3D video game.
## Technical and Visual Guidelines

### 1. **Navigation & Camera System**
- Implement a first-person controller:
    - **W**: Move forward
    - **S**: Move backward
    - **A**: Strafe left
    - **D**: Strafe right
- Mouse movement controls camera look direction.
- Camera height and motion should reflect pedestrian scale.
- Make a simple camera that can do these things. you an not use the glm::rotate function. implement the rotation using matrix multiplication. you can use algorithms like rodriques rotation formula to rotate the camera.
### 2. **Parking Lot Environment Design**
- Layout should resemble a real underground/attached parking lot:
    - Clearly marked lanes.
    - Realistically dimensioned parking spots.
    - concrete surfaces (floor, pillars, ceiling).
    - Painted lines, directional arrows, indicator signs.
    - ceiling-mounted and wall lights (not oversized/overbright).
    - Include elements like support pillars, boom barriers, ticket machines, and possibly a few parked cars (all modeled and textured).
    - **Windows:** Integrate side windows or skylights for lighting effects.
- Avoid exaggerated proportions or stylized shapes. focus on simple shapes and realistic proportions. create primitive shapes and use them to create the objects. th objectives may be simple, and you do not ned to use textures. use colors that are distinct.

### 3. **Illumination Model and Shading**
- The environment must use advanced, physically inspired lighting via the **Phong Illumination Model**, incorporating:
    - **Ambient Lighting:** Base level illumination simulating scattered indirect light.
    - **Diffuse Lighting:** Simulates direct light contact scattering over surfaces.
    - **Specular Lighting:** Creates sharp, glossy highlights for polished concrete or painted surfaces.
    - Light sources should include:
        - Directional light (for windows/light rays)
        - Point lights (ceiling/non-directional lights)
        - Ambient component (general fill lighting).
    - **Directional light:** Simulate sunlight entering through windows. use a single cutoff angle to simulate the sun's light. it may be simple, but make th liht color sthat the directinal light can be seen distinctly.
    - Each lighting can be toggled on and off with keyboard inputs. use numbe keys for this.


### 4. **Lighting Equations & Surface Calculation**
- For each fragment/pixel, the shading must be computed using the **Phong Surface Rendering** equations. there is a phong shader present, you can modify it.

the current project files have a simple example. you can use it as a reference.


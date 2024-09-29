# Simple 2D Scene

**Author:** Daniel Torres  
**Assignment:** Simple 2D Scene  
**Due Date:** September 28, 2023, 11:59 PM  

---

## Project Description

This project is a simple 2D scene implemented using OpenGL, SDL, and GLSL shaders. It simulates a space-themed scene where multiple objects, such as Earth, a traveler, a ghost, and a ship, are animated. The background features an "animated" starry sky that cycles through multiple PNG images to create a moving space effect.

## Features

- **Earth Rotation:** The Earth rotates slowly along the z-axis, simulating planetary rotation.
- **Traveler Orbit:** A traveler object orbits around the Earth, creating a simulation of space objects in motion.
- **Ship and Ghost Animation:** The scene features a ship that moves along an elliptical path. Depending on its position, the ghost object is rendered instead of the ship. The ship scales based on its z-position to simulate depth. The ghost is scaled with respect to the ship.
- **Animated Background:** The background cycles through 8 space-themed PNG images to create the illusion of movement in space.
- **Shader Program:** GLSL shaders are used to handle texture mapping and transformations.
- **SDL-based Input Handling:** Basic input handling is implemented to manage the program's state.

## Requirements

To run this project, you will need:

- C++11 or higher
- OpenGL
- SDL2
- GLEW (for Windows)
- GLSL
- STB Image library

## Prerequisites
Ensure you have SDL2, OpenGL, GLEW (for Windows), and other required libraries installed on your system.


## License
This project is provided for educational purposes in conformance with the NYU School of Engineering Policies and Procedures on Academic Misconduct. No external collaboration was made.

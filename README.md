# 2D Game Engine

A homebrew 2D game engine built with C++17 following the Pikuma C++ 2D Game Engine Development course.

![Engine Demo](./resources/demo.gif)

## Overview

This project is a custom 2D game engine implementation created as part of learning advanced C++ game development concepts. The engine demonstrates modern C++ practices and game architecture patterns.

## Technologies Used

- **C++17** - Modern C++ standard
- **SDL2** - Simple DirectMedia Layer for graphics and input
- **GLM** - OpenGL Mathematics library for vector/matrix operations
- **ImGui** - Immediate mode GUI library for debugging tools
- **Lua** - Scripting language for game logic

## Key Concepts Learned

### Data Driven Design

Implementation of data-driven architecture allowing game content and behavior to be defined through data rather than hardcoded logic.

### Entity Component System (ECS)

A flexible game architecture pattern that separates data (Components) from behavior (Systems) and entities, enabling:

- Better code organization
- Improved performance through data-oriented design
- Easier entity management and composition

### C++ Smart Pointers

Proper use of modern C++ memory management with:

- `std::unique_ptr` for exclusive ownership
- `std::shared_ptr` for shared ownership
- `std::weak_ptr` for non-owning references

### C++ Memory Management

Understanding and implementing:

- RAII (Resource Acquisition Is Initialization)
- Manual memory management best practices
- Avoiding memory leaks and dangling pointers

## Features

- Custom Entity Component System architecture
- Lua scripting integration for game logic
- Real-time debugging interface with ImGui
- 2D rendering pipeline using SDL2
- Component-based game object system

## Purpose

This project serves as a learning exercise and demonstration of:

- Game engine architecture
- Modern C++ development practices
- Graphics programming fundamentals
- Scripting language integration

## Credits

Based on the [C++ 2D Game Engine Development](https://pikuma.com) course by Pikuma.

## License

This is an educational project created for learning purposes.

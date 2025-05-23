# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands
- `make` - Compile the project
- `cd bin && ./eschatology` - Run the compiled program

## Code Style Guidelines
- Use C99 standard
- Follow Entity Component System (ECS) architecture
- Use snake_case for functions/variables and PascalCase for types
- Add header guards with `#ifndef NAME_H`
- Include explicit memory management with malloc/free pairs
- Check return values and handle errors explicitly
- Use SC_MAP_* macros for component storage
- Keep components small and focused on a single responsibility
- Include proper documentation for public functions
- Maintain consistent indentation (4 spaces)
- Place related functions/data in appropriate component files
- Validate parameters at function entry points
- Follow raylib conventions for graphics-related code
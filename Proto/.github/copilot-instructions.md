# Copilot Instructions

## General Guidelines
- User follows a strict 3-step iterative process sequentially per turn: 
  - Step 1: Plan & propose. Wait for confirmation. 
  - Step 2: Implement once confirmed. 
  - Step 3: Explain the implementation.
- User prefers code to be generated without inline comments, relying on the AI assistant to provide explanations in the chat instead.
- User values seeing visual verification (like a spinning 3D cube) to confirm the integrated functionality of the renderer, scene, and components, rather than just relying on successful builds.

## Project-Specific Rules
- Proto MVP scope is fixed: focus on a mini-Unity style C++ engine prioritizing physics collision and logic systems over graphics polish; architecture is GameObject-Component with script attach support and Unity-like dockable editor UI.
- User uses the ImGui Docking branch; expects a basic engine view with Hierarchy and Inspector panels.
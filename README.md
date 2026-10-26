# Assignment
This is the assignment for Programming and Fundamental Algorithms course, by Hu.  

## Overall Structure
Scene instances contains objects.  
Every fixed frame, traverse the objects to update the stats.  
Every visual frame, traverse the objects to update the visual stuffs.  
At the end of every visual frame, render all objects.  
At the end of every frame, recycle the objects that's pending destroy.  

## Rendering
Multi-thread cpu rendering.  
The screen is firstly sliced into 64*64 tiles for the threads to render.  
In each tile:
1. Traverse all the objects test the depth on each pixel.
2. Draw each pixel by the visual order. If a pixel is blocked by the depth, skip it.
3. Clear all the temporary resources, to get ready for the next loop.

## BaseGameObject
This is the base of all objects.
It contains updating, texture sampling, rendering, and 2D data methods.

## BasePhysicalObject
This is the base of all physical objects.  
Using Verlet algorithm for position updating.  
Using Position Based Dynamics for constriant updating.  
Using Steering Behaviours for ai moving (TODO). (possibly not doing this)  
Still working on it. There is a test physical object.

## TODO list
1. add some legs to the TestPhysicalObject to make it more vivid.
2. build up Tilemap and Room (not newed yet), to make the world run.
3. make an input tool for input edge detection.
---
next phase: player, enemy, skills, hud...
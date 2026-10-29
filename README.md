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

## Player-Enemy ideas
All 3 sorts of player inherits the BasePlayer class (not newed yet).  
Player's tail length represents health.  
Hitting player's tail is the same as hitting the player.  
Player's health decreases by time (quicker and quicker).  
BasePlayer has a BaseAttack and BaseSkill to cast (not exist yet).  
BaseAttack can be used without any cost, with a cool down.  
BaseSkill can be used without a cool down, but would cost health.
***
All 3 sorts of enemy inhertis the BaseEnemy class (not exist).  
Hitting enemy's tail is the same as hitting the enemy.  
When killed, the enemies' head despawns, but the body nodes still exist for a while for players to eat.  
Eating red nodes would restore player's health, and grey nodes just have no effect.  


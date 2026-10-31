# Assignment
This is the assignment for Programming and Fundamental Algorithms course, by Hu.  

## Overall Structure
Scene instances contains objects.  
Every fixed frame, traverse the objects to update the stats.  
Every visual frame, traverse the objects to update the visual stuffs.  
At the end of every visual frame, render all objects.  
At the end of every frame, recycle the objects that's pending destroy.  
RoomManager: always instantiating current room and four nearby rooms, destroying others.

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

## Player-Enemy ideas [in progress]
Players: swordsman, archer, wizard.  
All 3 sorts of player inherits the BasePlayer class (not newed yet).  
Player's tail length represents health.  
Hitting player's tail is the same as hitting the player.  
Player's health decreases by time (quicker and quicker).  
BasePlayer has a BaseAttack and BaseSkill to cast (not exist yet).  
BaseAttack can be used without any cost, with a cool down.  
BaseSkill can be used without a cool down, but would cost health.
***
Enemys: fast, medium, slow.  
All 3 sorts of enemy inhertis the BaseEnemy class (not exist).  
Hitting enemy's tail is the same as hitting the enemy.  
When killed, the enemies' head despawns, but the body nodes still exist for a while for players to eat.  
Eating red nodes would restore player's health, and grey nodes just have no effect.  
Another static enemy keeps spawning slow enemy.  

## Player Attacks & Skills [in progress]
- All attacks & skills have a cool down time. Attacks have a cool down of 0.75s.
- Swordsman's attack: instantiate a physical object "sword", then keep the "sword" stays in front of the Swordsman, with an angle of (front - swingAngle / 2.f + swingAngle * (curAttackTime / maxAttackTime)), like distance constraint and  angle constraint in BaseCharacter. The "sword" should detect the collision and hurt enemies on contact. If (curAttackTime >= maxAttackTime), destroy "sword".  
- Archer's attack: instantiate a physical object "arrow", initialize the direction by player's front, then it should fly at a constant speed until hit something or reach its max lifetime. The "arrow" should detect the collision and hurt enemies on contact.
- Wizard's attack: instantiate a physical static & isTrigger object "boom area", initialize the position by player's pos + player's front + constDist, then it should disappear within 0.5s. The "boom area" should detect the collision and hurt enemies on contact. 
- All skills have a cool down of 15s.
- Swordsman's skill: for the following 10s, become invulerable. // TODO: add some visual effects when invulnerable
- Archer's skill: immediately shoot 16 "arrow" in all directions. For the following 10s, speeds up 25%.  
- Wizard's skill: immediately cast a large "boom area" at the player's pos, with a damage of 10 (eliminating all enemies).

## Reminder
I went into a problem that the player's tail is not that natural, because it looked too heavy. So I thought through for the difference between my dot-line model and the real animals. One big difference is that the mass distribution of a real animal is not even. Animals have thicker body and thiner tail. Therefore I changed the mass of the body node into a decreasing sequence. It turned out to be pretty well.    
I tried to render polyline. At first, I just used Bresenham's method to draw a pixel line. To give it some thickness, I began to try stamping - draw something on each pixel by Bresenham's method. A filled circle would be correct, but it takes a lot of time, and draw really a lot of redundant pixels. A cross is quite cheap to do so, but the thickness tend to differ by different directions, and cause false on the two ends of the line. An unfilled circle fixes the difference in different directions, but a lot of pixels inside the line are not fully covered. Finally, I decided to use the cross + unfilled circle, which will only leave some error on the two ends, but have much better performance than the filled circle. Also, the error on two ends can be covered by other objects, or simply by drawing a filled circle on each end.  

## Works still need to do:
1. 3 player attacks
1. enemy death spawn health
2. 3 player skills
3. 3 players
4. 3 enemies (done)
5. enemy spawner
5. string display
6. menu scene
7. tally screen
7. sound effects
8. save
9. more rooms
10. key and locked door 
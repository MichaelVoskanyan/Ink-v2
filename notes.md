Obj properties:

### 1. platforms/terrain:
**Can collide with stuff**
- Might want moving platforms, so maybe *some* physics?
- Can have pre-defined movement for platforms.
- **Terrain should be creative and open-ended**
Certain types of terrain that might be unlocked by player has certain properties (sand and gravel might fall, but might be more renewable.)
Can draw your own platforms. Disappearing ink as a resource can be replenished. 
If it has mass, is effected by gravity. Otherwise, not effected. 
Game loop -> draw (probably for everything), optional physics update for terrain

### 2. background
just draw that shite... (toss it in the render loop).
**NO COLLISIONS**
- potentially could implement player interaction with the background (explore later)
  - something in background could shoot at a specific zone or area, idk.

### 3. character
Definitely has physics/**collisions**/drawing
Need movement inputs, need input for drawing and selecting draw type (platform, weapon, projectile, etc.)
Movement events would happen with keyboard, draw events happen with mouse. (eg press lmb to start drawing, release lmb to stop)

### 4. Enemy
Collides with player.
Should follow a line for some enemies, other enemies would have intelligence.
Line following, optional intelligence. 
Have physics, draw, collision.

#### 4.5 Processing paths:
Cyclical, applies to (moving) terrain and enemies.
Periodic, define like a period (path should complete every 1 or 2 seconds or whatever. Use math)

### 5. Projectiles:
Should also be effected by gravity depending on object.
- Some objects can be rays, controlled based on mass?
- Could be a beam, slow-moving beam, depends.
Probably could avoid gravity for most projectiles, since it would be annoying. Per-projectile basis.
Collisions, drawing, (gravity?)
Projectile by nature wouldn't have a path. 

#### side notes/ideas:
invisible ink? 
Ink that only shows up when paper is wet, like if you unlock like a water color/brush or something
Ink that shows up in certain lighting conditions?
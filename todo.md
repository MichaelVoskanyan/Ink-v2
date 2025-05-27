# TODO:

## Level/World data structure
Should implement some kind of class/data structure which holds level/world data. This struct should hold players start location, platform types/positions/paths, any
enemies in the level, and end-states for the level.

## Level/World state/data manager
Some kind of managerial class/functions should be implemented to track and change state during gameplay. The data struct outlined above would contain actual data as
would be read from a file and loaded into the game, but this class should basically keep track of game states and state transitions, like player killed or success 
state. This would also be used to load in next levels on success state, or load in a selected level from a pause menu.

## Menu system
Need to implement something for menus. Should be able to pause game, restart level, quit to main menu, quit to desktop. Main menu should also be implemented which
could store a level selection menu, allow player to start new game, change graphics settings like render resolution, fullscreen, windowed borderless, or normal
windows, allow player to toggle VSync on or off, etc. Level selection would require some way of tracking the players progress through the game, so that might fall
under another goal of implementing player save data and game state data (like the previously mentioned graphics settings). 

## Scripting engine
This is *far* down the line, but we should begin thinking about implementing support for scripting languages like LUA or maybe even python if we want. There are 
drop-in files to implement LUA support, but rolling our own shouldn't be *too* bad, as far as I know. Alternatively, we could just hard-code everything in C++. 
The latter approach would probably be more efficient both for time-investment and actual performance, but we get a lot more flexibility by implementing support 
for scripting. Would theoretically make level design easier, allow us to rapidly implement new enemy behaviors, AI, player abilities, etc.

Just something to think about.

## Level editor
Obviously this has to be handled after the Level/World data structure is implemented, but this really shouldn't be too hard to implement and it would make design
a lot easier.
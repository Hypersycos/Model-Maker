# Model-Maker
Model Maker is my A-level computer science project, intended to facilitate the demonstration of basic mechanics taught in Physics and Maths. The result was very limited, only able to simulate friction and gravity, and there are certainly some very long if/else if chains, but I don't think the structure and presentation is too terrible.

# Usage
## Project
A project is the root file for any demonstration. Two examples can be found in /Example Files/ . A project can consist of multiple Worlds and Displays. A world contains objects to be simulated, represented by models. Models can contain polygons and circles, which are fixed representations of the model, as well as sub-models which are sub-objects. Sub-models will experience all position and rotation changes of the parent model, but will also experience forces relative to the parent model such as friction. For some reason I didn't implement this recursively, so only 2 layers are allowed. Models can also have fixed forces applied to them, stored as children of the model. A Display renders a World, while also providing widgets such as sliders which can modify or read off properties of the world or its children.
## GUI
The window consists of three sections. The centre will always contain either a dummy home screen or a render of a display. The two sidebars can be resized, and contain three subwindows: Project Tree, Properties and Connections. These subwindows can be moved around and closed using the buttons in their titlebars.
### Project Tree
The project tree allows direct manipulation of all objects in the project. The context menu can be used to copy, paste, delete and insert new objects. Objects can also be renamed by double-clicking their titles in the tree. Selecting an object allows it to be manipulated in the Properties window.
### Properties
The properties window displays every property of a selected item, and allows them to be changed. Changes are permanent while in edit mode, but will be reverted in play mode. It also does not update in real-time, instead taking a snapshot of the values when selected. Properties can also be given connections using the Input and Output tickboxes. This allows their usage in display widgets.
### Connections
The connections window just shows which properties are currently exposed, as well as their current values. Similarly to properties the values do not update in real-time, but will update when the window redraws (usually caused by a mouseover).
### Render
The central render will be blank by default, as it does not select a display on load. If a display is present, it can be selected using radio buttons at the bottom of the window. The buttons at the top can be used to play, pause and reset the model respectively. On reset the model will lose all changes made in play, although UI elements will not update.

# Code Structure
### MainWindow
MainWindow is responsible for all initial setup, as well as IO and UI layout.
### ProjectTree
ProjectTree handles the creation of the Project Tree window, as well as actions provided by it. The ProjectTreeModel handles the actual manipulation of the project, and ProjectTreeItems are used to link the Qt model with the real objects.
### Item
All members of the project, including Displays and Worlds, inherit from Item. It is the interface with which properties are manipulated and objects are serialized.
### ScreenItem
All members of a Display will inherit from ScreenItem. It initialises all properties required for on-screen widgets.
### World
All simulation is done in World::physicsUpdate().
### Display
Display is responsible for all rendering, done per frame in update().

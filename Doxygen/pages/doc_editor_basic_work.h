/**

\page doc_editor_basic_work Basic work with the editor

\section overview Overview

Editor is using project file to store list of all scenes from you game is consist. So
before start a project file needed to be created. This can be done via menu.
Menu also allows open and save projects. 

Folder where project file has stored stands as root folder. That means every scene of project
should be located inside root folder. Also all resources folder of every scene stands as root folder
for scene itself. That means all resouces like images, fonts, models, etc should be stored inside
folder of a scene. 

\section project_settings Project settings

Project settings can be accesed via activation of menu item 'Project->Settings'. Currently only available
settings is a path to export folder.

\section scene_treeview List of scenes

All scenes in project can be viewed under tab 'Project'.

![Caption text](../images/ed_project_tab.jpg "Image")

Via context menu list of scenes can be manipulated.
Folders can be added for better organisation and navigation through scenes.

![Caption text](../images/ed_project_tab_menu.jpg "Image")

\section navigation 2D & 3D navigation

Scene object can be 2D or 3D. Navigation can be manualy switched from 3D to 2D or reverse
via buttom '2D' in top panel. Also automatic switching to proper navigation is happening
when scene object is selected. For exaple when 2D object is selected then 2D navigation automaticly
became active.

In 3D mode key W, A, S, D are responding for movement. And holding middle mouse and moving a mouse cursor
with active viewport will make camera to rotate

In 2D mode holding middle mouse button and moving a mouse cursor with active viewport will make
moving throught 2D space.

\section scene_objectes_treeview Managing scene objects in a scene

After a scene was added and selected all scene objects in a scene can be
viewed under tab 'Scene'.

![Caption text](../images/ed_scene_tab.jpg "Image")

Via context menu scene objects of a scene can be manipulated.
Folder can be added for better organisation and navigation through scene objects.

![Caption text](../images/ed_scene_tab_menu.jpg "Image")

Scene object can be selected via list of scene objects or view viewport. But before
anything can be selected in vieport it should became. Just click at vieport and you will
yellow frane araound viewport. After that hold left Control nd click on at an object
viewable in viewport. If there are sewveral objects at same spot in viewport then sereval
clickck with holded left Control will make cycled selection of that objects.

\section editor_gizmo_3D 3D transform gizmo

If selected 3D scene object then it is posible to modify transform object via transform gizmo.
Buttons on top switching from moving to rotating mode. Also via buttons on top it is possible to
switch from global to local coordinate system.

![Caption text](../images/ed_gizmo_3d.jpg "Image")

\section editor_gizmo_2D 2D transform gizmo

If selected 2D scene object then it is posible to modify transform object via transform gizmo.
Dgragging gizmo area allows to drag an object. Draging small squares allows to change size.
Dragging a small circle allows change local pivot of a scene object. Holding key 'z' and dragging
mouse around gizmo area will rotate a object.

![Caption text](../images/ed_gizmo_2d.jpg "Image")

\section scene_properties Properies of scene objects

When scene object is selected all properies of them are apering in panel at the right.

![Caption text](../images/ed_scene_object_prop.jpg "Image")

Every scene objet has a name property. Properties are splited by groups for easier nabigation
through them. Properies can be as boolean, as integer, as float, as string, as color, as sprite,
as file name, as enum, as array or as refrence to a scene object.

b\section scene_run Start scene & run & test

When at least one scene is added to a object it is possible to test a scene.
Just press 'Play' button in top panel.

There are assumption that project may consist from several scenes. But only one scene can be
marked as start scene. Other scenes should be loaded via scene manager inside start scene.
Scene can be marked as start scene via context menu. Start scene marked by orange icon.

\section editor_assets Working with scene assets

Assets is very importatnt being in Atum. Most of types of scene objects is a instances of assets.
For example script object itself is an asset and to create instance of script object a scene instance is needed to be created.
List of assets sits in separate list and manageable via context menu. Scene assets can be selected only via
asset list.

When assets is created just hold left Control and drag an asset from asset list into
list of scene objects.

In some cases it is needed to change an asset for already created an asset inctance. To do
so hold down left Control and left Shift and then drag an asset from asset list into
list of scene objects.

\section editor_components Components

Components are used for extension of functionality. Some of them can be applied to scene
object another one are applied to assets. If component wascapplied to an asset then component
automaticly applaied to all instances of an asset.

Component for a scene object is managing via list on right panel. Selecting component form combo box
will automaticaly add component.

Some components can be switched in editor mode. For do so just press 'ED' button. In that mode only componenr
rlated staf is selected via Control and mouse click. Another press on 'ED' button will turn off editor mode
for component.

![Caption text](../images/ed_scene_obj_components.jpg "Image")

\section scene_includes Scene includes

Atum is heavely using work with assets. And some times it is needed to use same assets
in different scene. Do to so it is possible to include one scene in another scene.
This allows for example move main objects in base scene or to move assets of particular
belonging in separate scene. Sample project "SunnyLand" ([Editor dir]/Projects/SunnyLand)
is good example of effective usage of including of scenes.

To include scene into another just drag one scene into another scene. There are no limitation in depth of incuding.
Use context menu for deleting included scene.

![Caption text](../images/ed_scene_includes.jpg "Image")

\section scene_layers Scene layers

Some times it is needed to work with particular set of objects. For doing so
each a scene object can by assigned to a particular layer and it is possible
to set visibility state from every scene objects in layer.

List of layers are managing under tab 'Layers' 

![Caption text](../images/ed_layers.jpg "Image")

\section scene_groups Scene groups

Some times it is needed to set state for bunch of scene objects at once. To do so
each a scene object can by assigned to a particular group. To set state for whole
group call core.scene.SetStateToGroup (ScriptScene::SetStateToGroup) in script or
call core.scene_manager.SetScenesGroupsState (SceneManager::SetScenesGroupsState) in C++.

List of groups are managing under tab 'Groups'

![Caption text](../images/ed_groups.jpg "Image")

*/  

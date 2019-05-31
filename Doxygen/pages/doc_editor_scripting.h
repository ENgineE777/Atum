/**

\page doc_editor_scripting Scripting

\section scripting_overview Script system overview

Script system is based on Angels script (https://www.angelcode.com).

To write script firstly script asset needed to be created. For every script asset file containing
script itself is created. File with script has name of an asset and will be stored in same dirictory where
file of scene is located. 

There are assumption that class will be defined in script file and instances of that class will be created later.
But several classes can be defined in script file that is why there script asset has a property 'Main class'. This
property defines name of clas which should be instancieted.

It is possible to share code between scripts. To do so import key word is used.

\code
#include "NameOfAnotherScriptAsset" //allows to use classes declared in another script
\endcode

Script will be useless if it can't be tied with scene objects. Let's imagine we need to manipulate Camera2D scene
object. Firstly we needed to declare property with refrence type of Camera2D. After that we can manipulate Camera2D scene
object.

\code

class ScriptClass
{
	Camera2D@ camera; // lately this property will be tied with a scene object

	void SetCamerTarget(float x, float y)
	{
		camera.target_pos_x = x;
		camera.target_pos_y = y;
	}

	\\.......
}

\endcode

Linking property camera with scene object happing in set up of link builder of script instance.

Some scene objects can have instances. For example \ref scene_assets_2D_SpriteInst SpriteInst holds
instances of sprite asset. Such scene objects can be binded stright into an array. There are no strict
definition a type of elements of an array. If we need addtion properties that we can declare it. If
we need to control only position of intance then we declare only x and y properties.

\code

class Bullet
{
	float x, y; // this properties will be tied with a instance of a sprite

	float speed;
	float dir_x, float dir_y;
	float damage;
};

class ScriptClass
{
	array<Bullet> bullets; // latelly this property will be tied with a scene object

	void UpdateBullets(float dt)
	{
		\\logic of updating of bullets
	}

	\\.......
}

\endcode

Another cool feature that we can combine several instances in one array. For example we have a character which
rendered by using sprite of legs and sprite of torso with head and both sprites should have diferent rotation.
So we can bind two scene object into one script array and can separate properties of each insces of scene objects by
using of a prefix.

\code

class Character
{
	float x, y; // this properties will be tied with instances of a first scene object
	float angle;

	float top_angle; // this property will be ties with instances of a second object.
	                 // Not declared properties will be automaticly equal to same of a first scene object, i.e.
	                 // for example postion will same as position of instances of a first scene object

	float hp;
};

class ScriptClass
{
	array<Bullet> bullets; // latelly this property will be tied with a scene object

	\\.......
}

\endcode

Some of scene objects have script callbacks. For example Trigger2D scene object has two
callbacks OnContactStart and OnContactEnd. This callbacks can be tied with methods of main
script class.

\code

class ScriptClass
{
	void OnRespawnTrigger(int self_index, string&in name, int index) // method which will be tied with callback of scene object
    {
    }

    \\.......
}

\endcode

Some times it is requred to call method on start of a scene or call method every frame. It is possible
to define such behavior via link builder of script asset. 

\code

class
{
	void Init() // this method will be caled once on start of a scene
	{
	
	}	

	void Update(float dt) // this method will be called every frame
	{
	
	}
}

\endcode

\section scripting_asset Working with script asset

After creating and selecting script asset the link builder is showing.

![Caption text](../images/ScriptAsset.jpg "Image")

Link builders allows to define connections between scene objects and script. Link builders
working with nodes. Each node can be one of three types - property node, method node and callback node.
Properties of node are showing on right panel when selected. Nodes are managing via context menu.

![Caption text](../images/script_asset_menu.jpg "Image")

Property node defines a link beetween scene object and propery of main class. Node
has propertity with name "Name" which defines name of a property of script. If you are planing
to tie scene object wwith type of Camera2D with property camera then 'camera' should be used as name
in property of a node.

Callback node represents script callback of a scene object. Node has propertity with name "Name" but
this property just a decorator.

Method node represents method of main class. Property CallType means if method should be called
by scene object (OnCallback) or should be called on start of a scene (OnInit) or should ve called
on every frame (EveryFrame). Name property defines name of method in main class itself. Paramtype
defines additional paramter which can be passed into script method. Nonde means we will not send anything
additional. String means one additional parameter of string. Int means one addtional parametr of type integer.

To make connection between script callback of a scene object and a method of main class a callback node
and method should be linked. To do so just strat mouse drag from small arrow on callback node until
small arrow on method node. Clicking on small arrow of callback linked with method node will select link itself
and allow to edit properties in right panel. Params property on link allows set value of additional
parametr which can be passed in method depething from Paramtype property of a method node.

Manual linking of callback nodes with method nodes allows to link several callback node. Also callback
node can be linked only with one method node at same time. Selected link can be deleted via context menu. 

![Caption text](../images/script_linked_method.jpg "Image")

\section scripting_instance Working with script asset instance

Instance of script asset will create instance of main class declared in script file of an asset.
Selection of script instace will bring up link builder. It will show same nodes as in case of asset
but here it is possible to link scene objects. Nodes itself cannot be delete or added. Also links
between callback node and method node cannot be deleted or added.

![Caption text](../images/ScriptInst.jpg "Image")

To set scene object for property node or callback node just hold left control and drag scene object
from scene list to needed node.

It is possible to set needed script callback for a scene object. Just select a callback node with assigned
to them a scene object. Parameter callback_type will show up on right panel. Needed script callback can be selected
from drop down list. In case of Triger2D there are will be option to choose from OnContactStart and OnContactEnd.

![Caption text](../images/node_methods_prop.jpg "Image")

*/
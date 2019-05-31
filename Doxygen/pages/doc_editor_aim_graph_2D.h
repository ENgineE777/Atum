/**

\page doc_editor_aim_graph_2D Animation 2D graph

Anim graphh consist from nodes. Each node points to SpriteAsset. There are start node
in every graph. It is defines start animation.

![Caption text](../images/AnimGraph.jpg "Image")

Every node can have links. Links has individual name. Graph2D::ActivateLink allows to activete
link, ie make transition to another node. If sprite in node hava a looped animation
then this animation will be played endessly until link will be activated. If animation is
not looped then when animation will be finished last farme of animation will be showed.
Also it is possible to set auto transition from and node to another if animation was finished.
To do so animation need to be created and marked as default link.

Also it is possible to make transition to any node by calling Graph2D::GotoNode

Nodes are managing via context menu.

![Caption text](../images/anim_graph_menu.jpg "Image")

For assignation sprite asset to node just hold left Control and drag sprite assets from assets list to a node.

To make link to another node just hold left Control and drag line from one node to another.

Mouse click on node will bring up properties of a node in left panel.

Mouse click on link will bring up properties of a link in left panel.

![Caption text](../images/anim_link_prop.jpg "Image")

*/

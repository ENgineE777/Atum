/**

\page doc_editor_compiling_ios_android Compiling iOS & Android builds

Platrom specific project located in Project folder of source code of Atum.

To compile project on specifict platform specific project needed to be opened.

To move resorces of a project all content inside folder of you project needed
to be copied in proper plase. For ios destination is a root folder of iOS project.
For android destination is assets folder.

Also folder with shaders needed to be copyed with resorces of project (Shaders/GLES).

Name of loaded also neede to be chages. For andoid AtumLib.cpp needed to be modified (line 144)
For iOS AtumViewController.m needed to be modified (line 30)

*/

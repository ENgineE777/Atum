/**

\page doc_editor_compiling_ios_android Compiling iOS & Android builds

Platform specific projects located in Project folder inside folder with sources of Atum.
Before running Atum project resources and specific resources needed to be copied in proper folder.

To make life easer editor is supporting exporting resources of a project. To do so just activate
menu item 'Project->Export'.

Warning: do not set export folder located inside exported project. This will lead to a bug.

After resources was exported just copy content of a folder to proper folder. In case of Android
content of a folder needed to be copied to [path_to_sources]/Atum/Project/android/app/src/main/assets.
In case of iOS destination is [path_to_sources]/Atum/../ExportedData.

If resources was exported and content of folder were resources were exported will be copied to proper
directory then only needed thing is just compile and run project. Every thing should be work as a charm.

*/

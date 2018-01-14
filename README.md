
Atum engine is a newcomer in a row of game engines. Most game engines focus on render
techniques in features list. The main task of Atum is to deliver the best toolset; that’s why,
as I hope, Atum will be a good light weighted alternative to Unity for indie games. Atum already
has fully workable editor that has an ability to play test edited scene. All system code has
simple ideas behind them and focuses on easy to use functionality. That’s why code is minimized
as much as possible.

Currently the engine consists from:

- Scene Editor with ability to play test edited scene;
- Track based editor for creation animated objects
- Powerful system for binding properties into the editor;
- Render system based on DX11 but created as multi API; so, adding support of another GAPI
  is planned;
- Controls system based on aliases;
- Font system based on stb_truetype.h;
- Support of PhysX 3.0, there are samples in repo that use physics;
- Network code which allows to create server/clinet; there is some code in repo which allows
  to create a simple network game

I plan to use this engine in multiplayer game - so, I definitely will evolve the engine. Also
I plan to add support for mobile devices. And of course, the main focus is to create a toolset
that will ease games creation.
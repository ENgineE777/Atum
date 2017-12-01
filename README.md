
Recently I start work at game engine that I called Atum. Engine already have full workable editor.
Key feature of engine is that glue code between editor code and game code minimized. Code have very
simple ideas behind architecture. Also amount of code minimized as much as possible.

Currently engine consist from:

- Render system based on DX11 but created as multi API so adding support of another GAPI is planed
- Controls system based on aliases
- Powerful system for binding properties into editor
- Scene Editor with ability to run edited scene
- Font system based on stb_truetype.h
- Support of PhysX 3.0, in repo there are samples that uses physics
- Network code that alow create server/clinet, in repo there some code allow to create simple network game


I planning to use this engine in multiplayer game so I definitely will evolve engine. Also adding support
of mobile devices is planned.

Some screenshot:
https://www.gamedev.net/uploads/monthly_2017_12/Untitled-1.jpg.5aef84777d1f7583480febfced85e710.jpg

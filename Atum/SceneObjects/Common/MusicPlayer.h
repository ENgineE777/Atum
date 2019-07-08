
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_common_MusicPlayer MusicPlayer

This object plays streamed sound.

This class ::MusicPlayer is a representation on C++ side.

This class Script::MusicPlayer is a representation in a script.

Parameters
----------

Name              | Description
------------------| -------------
sound_name        | Filename of a sound file
play_on_start     | Should sound play on start

*/

/**
\ingroup gr_code_scene_objects_common
*/

/**
\brief Representation of a music player

This object plays streamed sound.

This class Script::MusicPlayer is a representation in a script.

*/

class MusicPlayer : public SceneObject
{
public:
	META_DATA_DECL(MusicPlayer)

	/**
	\brief Filename of a sound file.
	*/

	std::string sound_name;

	/**
	\brief Should sound play on start
	*/
	bool play_on_start = false;

	/**
	\brief Pointer to SoundStream
	*/

	SoundStream* stream;


#ifndef DOXYGEN_SKIP

	virtual ~MusicPlayer() = default;

	void SetState(State state) override;

	void Init() override;

	void BindClassToScript() override;

	void Work(float dt);

	bool Play() override;
	void Release() override;
#endif
};

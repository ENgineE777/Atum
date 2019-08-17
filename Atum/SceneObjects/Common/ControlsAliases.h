
#pragma once

#include "Services/Scene/SceneObject.h"

class ControlsAliases : public SceneObject
{
public:
	META_DATA_DECL(ControlsAliases)

	/**
	\brief Filename of a file with aliases.
	*/

	std::string aliases_name;

#ifndef DOXYGEN_SKIP

	virtual ~ControlsAliases() = default;

	void Init() override;

	bool Play() override;
#endif
};

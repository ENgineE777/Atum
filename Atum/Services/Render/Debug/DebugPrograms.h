
#pragma once

#include "Services/Render/Render.h"

class DebugPrograms
{
public:

	class DbgLine : public Program
	{
	public:
		virtual const char* GetVsName() { return "debug_line_vs.shd"; };
		virtual const char* GetPsName() { return "debug_line_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetDepthTest(false);
			render.GetDevice()->SetDepthWriting(false);
		};

		CLASSDECLDIF(Program, DbgLine)
	};

	class DbgLineWithDepth : public DbgLine
	{
	public:

		CLASSDECLDIF(Program, DbgLineWithDepth)
	};

	class DbgTriangle : public Program
	{
	public:
		virtual const char* GetVsName() { return "debug_triangle_vs.shd"; };
		virtual const char* GetPsName() { return "debug_triangle_ps.shd"; };

		CLASSDECLDIF(Program, DbgTriangle)
	};

	class DbgSprite : public Program
	{
	public:
		virtual const char* GetVsName() { return "debug_sprite_vs.shd"; };
		virtual const char* GetPsName() { return "debug_sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetDepthTest(false);
			render.GetDevice()->SetDepthWriting(false);
		};

		CLASSDECLDIF(Program, DbgSprite)
	};
};

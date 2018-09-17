
#pragma once

#include "Services/Render/Render.h"

class DebugPrograms
{
public:

	class DbgLineWithDepth : public Program
	{
	public:
		virtual const char* GetVsName() { return "debug_line_vs.shd"; };
		virtual const char* GetPsName() { return "debug_line_ps.shd"; };
	};

	class DbgLine : public DbgLineWithDepth
	{
	public:

		virtual void ApplyStates()
		{
			render.GetDevice()->SetDepthTest(false);
			render.GetDevice()->SetDepthWriting(false);
		};
	};

	class DbgTriangle : public Program
	{
	public:
		virtual const char* GetVsName() { return "debug_triangle_vs.shd"; };
		virtual const char* GetPsName() { return "debug_triangle_ps.shd"; };
	};

	class DbgSprite : public Program
	{
	public:
		virtual const char* GetVsName() { return "debug_sprite_vs.shd"; };
		virtual const char* GetPsName() { return "debug_sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetAlphaBlend(true);
			render.GetDevice()->SetDepthTest(false);
			render.GetDevice()->SetDepthWriting(false);
		};
	};
};

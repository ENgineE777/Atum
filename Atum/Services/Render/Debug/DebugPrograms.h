#include <windows.h>

#include "Services/Render/Render.h"

class DebugPrograms
{
public:

	class LineProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//debug_line_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//debug_line_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();

			render.GetDevice()->SetDepthTest(false);
			render.GetDevice()->SetDepthWriting(false);
		};
	};

	class LineWithDepthProgram : public LineProgram
	{
	public:

		virtual void Apply()
		{
			Program::Apply();

			render.GetDevice()->SetDepthTest(true);
			render.GetDevice()->SetDepthWriting(true);
		};
	};

	class TriangleProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//debug_triangle_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//debug_triangle_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetDepthTest(true);
			render.GetDevice()->SetDepthWriting(true);
		};
	};

	static LineProgram*          line_prg;
	static LineWithDepthProgram* line_with_depth_prg;
	static TriangleProgram*      tri_prg;

	static void Init();
};

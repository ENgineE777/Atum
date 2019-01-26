
#include "programs.h"

CLASSREGEX(Program, TriangleProgram, Programs::TriangleProgram, "TriangleProgram")
CLASSREGEX_END(Program, TriangleProgram)
CLASSREGEX(Program, ShTriangleProgram, Programs::ShTriangleProgram, "ShTriangleProgram")
CLASSREGEX_END(Program, ShTriangleProgram)
CLASSREGEX(Program, ColorProgram, Programs::ColorProgram, "ColorProgram")
CLASSREGEX_END(Program, ColorProgram)
CLASSREGEX(Program, BlurProgram, Programs::BlurProgram, "BlurProgram")
CLASSREGEX_END(Program, BlurProgram)
CLASSREGEX(Program, CombineProgram, Programs::CombineProgram, "CombineProgram")
CLASSREGEX_END(Program, CombineProgram)
CLASSREGEX(Program, QuadProgramDepth, Programs::QuadProgramDepth, "QuadProgramDepth")
CLASSREGEX_END(Program, QuadProgramDepth)
CLASSREGEX(Program, QuadProgramNoDepth, Programs::QuadProgramNoDepth, "QuadProgramNoDepth")
CLASSREGEX_END(Program, QuadProgramNoDepth)

Program* Programs::GetTranglPrg()
{
	static Program* prg = nullptr;

	if (!prg)
	{
		prg = core.render.GetProgram("TriangleProgram");
	}

	return prg;
}

Program* Programs::GetShdTranglPrg()
{
	static Program* prg = nullptr;

	if (!prg)
	{
		prg = core.render.GetProgram("ShTriangleProgram");
	}

	return prg;
}

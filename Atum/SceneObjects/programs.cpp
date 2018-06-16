
#include "programs.h"

CLASSREGEX(Program, TriangleProgram, Programs::TriangleProgram, "TriangleProgram")
CLASSREGEX(Program, ShTriangleProgram, Programs::ShTriangleProgram, "ShTriangleProgram")
CLASSREGEX(Program, ColorProgram, Programs::ColorProgram, "ColorProgram")
CLASSREGEX(Program, BlurProgram, Programs::BlurProgram, "BlurProgram")
CLASSREGEX(Program, CombineProgram, Programs::CombineProgram, "CombineProgram")
CLASSREGEX(Program, QuadProgramDepth, Programs::QuadProgramDepth, "QuadProgramDepth")
CLASSREGEX(Program, QuadProgramNoDepth, Programs::QuadProgramNoDepth, "QuadProgramNoDepth")

Program* Programs::GetTranglPrg()
{
	static Program* prg = nullptr;

	if (!prg)
	{
		prg = render.GetProgram("TriangleProgram");
	}

	return prg;
}

Program* Programs::GetShdTranglPrg()
{
	static Program* prg = nullptr;

	if (!prg)
	{
		prg = render.GetProgram("ShTriangleProgram");
	}

	return prg;
}

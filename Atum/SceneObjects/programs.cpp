
#include "programs.h"

CLASSREGEX(Program, TriangleProgram, Programs::TriangleProgram)
CLASSREGEX(Program, ShTriangleProgram, Programs::ShTriangleProgram)
CLASSREGEX(Program, ColorProgram, Programs::ColorProgram)
CLASSREGEX(Program, BlurProgram, Programs::BlurProgram)
CLASSREGEX(Program, CombineProgram, Programs::CombineProgram)
CLASSREGEX(Program, QuadProgramDepth, Programs::QuadProgramDepth)
CLASSREGEX(Program, QuadProgramNoDepth, Programs::QuadProgramNoDepth)

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


#include "MeshPrograms.h"

/*
CLASSREGEX(Program, TriangleProgram, MeshPrograms::TriangleProgram, "TriangleProgram")
CLASSREGEX_END(Program, TriangleProgram)
CLASSREGEX(Program, ShTriangleProgram, MeshPrograms::ShTriangleProgram, "ShTriangleProgram")
CLASSREGEX_END(Program, ShTriangleProgram)
CLASSREGEX(Program, ColorProgram, MeshPrograms::ColorProgram, "ColorProgram")
CLASSREGEX_END(Program, ColorProgram)
CLASSREGEX(Program, BlurProgram, MeshPrograms::BlurProgram, "BlurProgram")
CLASSREGEX_END(Program, BlurProgram)
CLASSREGEX(Program, CombineProgram, MeshPrograms::CombineProgram, "CombineProgram")
CLASSREGEX_END(Program, CombineProgram)
CLASSREGEX(Program, QuadProgramDepth, MeshPrograms::QuadProgramDepth, "QuadProgramDepth")
CLASSREGEX_END(Program, QuadProgramDepth)
CLASSREGEX(Program, QuadProgramNoDepth, MeshPrograms::QuadProgramNoDepth, "QuadProgramNoDepth")
CLASSREGEX_END(Program, QuadProgramNoDepth)*/

Program* MeshPrograms::GetTranglPrg()
{
	static Program* prg = nullptr;

	if (!prg)
	{
		prg = core.render.GetProgram("TriangleProgram");
	}

	return prg;
}

Program* MeshPrograms::GetShdTranglPrg()
{
	static Program* prg = nullptr;

	if (!prg)
	{
		prg = core.render.GetProgram("ShTriangleProgram");
	}

	return prg;
}

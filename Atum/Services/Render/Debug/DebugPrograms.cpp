
#include "DebugPrograms.h"

DebugPrograms::LineProgram*          DebugPrograms::line_prg;
DebugPrograms::LineWithDepthProgram* DebugPrograms::line_with_depth_prg;
DebugPrograms::TriangleProgram*      DebugPrograms::tri_prg;

void DebugPrograms::Init()
{
	line_prg = new LineProgram();
	line_prg->Init();

	line_with_depth_prg = new LineWithDepthProgram();
	line_with_depth_prg->Init();

	tri_prg = new TriangleProgram();
	tri_prg->Init();
}
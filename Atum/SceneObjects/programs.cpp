
#include "programs.h"

Programs::TriangleProgram*   Programs::prg;
Programs::ShTriangleProgram* Programs::shprg;
Programs::ColorProgram*      Programs::color_prg;
Programs::QuadProgram*       Programs::quad_prg;
Programs::BlurProgram*       Programs::blur_prg;
Programs::CombineProgram*    Programs::combine_prg;

void Programs::Init()
{
	prg = new TriangleProgram();
	prg->Init();

	shprg = new ShTriangleProgram();
	shprg->Init();

	color_prg = new ColorProgram();
	color_prg->Init();

	quad_prg = new QuadProgram();
	quad_prg->Init();

	blur_prg = new BlurProgram();
	blur_prg->Init();

	combine_prg = new CombineProgram();
	combine_prg->Init();
}
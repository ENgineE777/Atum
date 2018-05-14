
#include "programs.h"

CLASSDECLDECL(Programs::TriangleProgram)
CLASSDECLDECL(Programs::ShTriangleProgram)
CLASSDECLDECL(Programs::ColorProgram)
CLASSDECLDECL(Programs::BlurProgram)
CLASSDECLDECL(Programs::CombineProgram)
CLASSDECLDECL(Programs::QuadProgram)

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

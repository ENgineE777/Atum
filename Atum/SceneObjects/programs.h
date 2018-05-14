
#include "Services/Render/Render.h"

class Programs
{
public:

	class QuadProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "screen_vs.shd"; };
		virtual const char* GetPsName() { return "screen_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetAlphaBlend(true);
		};

		CLASSDECLDIF(Program, QuadProgram)
	};

	class TriangleProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "triangle_vs.shd"; };
		virtual const char* GetPsName() { return "triangle_ps.shd"; };

		CLASSDECLDIF(Program, TriangleProgram)
	};

	class ShTriangleProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "sh_triangle_vs.shd"; };
		virtual const char* GetPsName() { return "sh_triangle_ps.shd"; };

		virtual void ApplyStates()
		{
			SetMatrix(Vertex, "sh_trans", &(matrixes["sh_trans"]), 1);
			SetTexture(Pixel, "shdMap", textures["shdMap"]);
		};

		CLASSDECLDIF(Program, ShTriangleProgram)
	};

	class ColorProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "correction_vs.shd"; };
		virtual const char* GetPsName() { return "correction_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetDepthTest(false);
		};

		CLASSDECLDIF(Program, ColorProgram)
	};

	class BlurProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "blur_vs.shd"; };
		virtual const char* GetPsName() { return "blur_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetDepthTest(false);
		};

		CLASSDECLDIF(Program, BlurProgram)
	};

	class CombineProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "combine_vs.shd"; };
		virtual const char* GetPsName() { return "combine_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetDepthTest(false);
		};

		CLASSDECLDIF(Program, CombineProgram)
	};

	static Program* GetTranglPrg();
	static Program* GetShdTranglPrg();
};

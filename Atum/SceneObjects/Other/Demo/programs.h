
#include "Services/Render/Render.h"

class Programs
{
public:

	class QuadProgramDepth : public Program
	{
	public:
		virtual const char* GetVsName() { return "screen_vs.shd"; };
		virtual const char* GetPsName() { return "screen_ps.shd"; };

		virtual void ApplyStates()
		{
			render.GetDevice()->SetAlphaBlend(true);
		};
	};

	class QuadProgramNoDepth : public QuadProgramDepth
	{
	public:

		virtual void ApplyStates()
		{
			render.GetDevice()->SetAlphaBlend(true);
			render.GetDevice()->SetDepthTest(false);
		};
	};

	class TriangleProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "triangle_vs.shd"; };
		virtual const char* GetPsName() { return "triangle_ps.shd"; };
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
	};

	static Program* GetTranglPrg();
	static Program* GetShdTranglPrg();
};
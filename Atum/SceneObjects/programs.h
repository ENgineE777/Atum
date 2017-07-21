
#include "Services/Render/Render.h"

class Programs
{
public:

	class TriangleProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//triangle_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//triangle_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetAlphaBlend(false);
			render.GetDevice()->SetDepthWriting(true);
			render.GetDevice()->SetDepthTest(true);
		};
	};

	class ShTriangleProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//sh_triangle_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//sh_triangle_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();

			VS_SetMatrix("sh_trans", &(matrixes["sh_trans"]), 1);
			PS_SetTexture("shdMap", textures["shdMap"]);
			render.GetDevice()->SetAlphaBlend(false);
			render.GetDevice()->SetDepthWriting(true);
			render.GetDevice()->SetDepthTest(true);
		};
	};

	class ColorProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//correction_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//correction_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetAlphaBlend(false);
			render.GetDevice()->SetDepthWriting(false);
			render.GetDevice()->SetDepthTest(true);
		};
	};

	class BlurProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//blur_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//blur_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetAlphaBlend(false);
			render.GetDevice()->SetDepthWriting(false);
			render.GetDevice()->SetDepthTest(true);
		};
	};

	class CombineProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//combine_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//combine_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetAlphaBlend(false);
			render.GetDevice()->SetDepthWriting(false);
			render.GetDevice()->SetDepthTest(true);
		};
	};

	class QuadProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "shaders//screen_vs.shd"; };
		virtual const char* GetPsName() { return "shaders//screen_ps.shd"; };

		virtual void Apply()
		{
			Program::Apply();
			render.GetDevice()->SetAlphaBlend(true);
			render.GetDevice()->SetDepthWriting(false);
			render.GetDevice()->SetDepthTest(true);
		};
	};

	static QuadProgram*       quad_prg;
	static ColorProgram*      color_prg;
	static TriangleProgram*   prg;
	static ShTriangleProgram* shprg;
	static BlurProgram*       blur_prg;
	static CombineProgram*    combine_prg;

	static void Init();
};

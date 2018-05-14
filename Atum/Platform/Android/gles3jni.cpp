
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Services/Core/Core.h"

#include "Services/Scene/Scene.h"

class TestTriangleProgram : public Program
{
public:
	virtual const char* GetVsName() { return "vertex.txt"; };
	virtual const char* GetPsName() { return "pixel.txt"; };

	CLASSDECLDIF(Program, TestTriangleProgram)
};

CLASSDECLDECL(TestTriangleProgram)

TaskExecutor::SingleTaskPool* renderTaskPool;

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "android_fopen.h"

JNIEnv* cur_env = nullptr;
jobject cur_obj;
jclass  cur_cls;
std::vector<jvalue> cur_args;

extern "C"
{
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_Init(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_Resize(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_Update(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_SetAssetManager(JNIEnv* env, jobject obj, jobject assetManager);
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_TouchStart(JNIEnv* env, jobject obj, int index, int x, int y);
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_TouchUpdate(JNIEnv* env, jobject obj, int index, int x, int y);
	JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_TouchEnd(JNIEnv* env, jobject obj, int index);
};

void UpdateJavaEnv(JNIEnv* env, jobject obj)
{
	cur_env = env;
	cur_obj = obj;
	cur_cls = cur_env->GetObjectClass(cur_obj);
}

void CallJavaMethod(const char* function, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	std::string prototype = "(";
	int index = 0;

	bool fail = false;
	int len = strlen(format);

	cur_args.resize(0);

	while (format[index] != 0)
	{
		if (format[index] != '%' || index + 1 >= len)
		{
			fail = true;
			break;
		}
		
		jvalue cur_value;

		switch (format[index + 1])
		{
			case 'i':
			{
				prototype += "Ljava/lang/Int;";
				cur_value.i = va_arg(args, int);
				break;
			}
			case 'f':
			{
				prototype += "Ljava/lang/Float;";
				cur_value.f = (float)va_arg(args, double);
				break;
			}
			case 'd':
			{
				prototype += "Ljava/lang/Double;";
				cur_value.d = va_arg(args, double);
				break;
			}
			case 's':
			{
				prototype += "Ljava/lang/String;";
				cur_value.l = cur_env->NewStringUTF(va_arg(args, const char*));
				break;
			}
			default:
			fail = true;
		}

		cur_args.push_back(cur_value);

		if (fail)
		{
			break;
		}

		index += 2;
	}

	va_end(args);

	if (fail)
	{
		core.Log("app", "CallJavaMethod: %s - invalid format at %i", function, index);
	}
	else
	{
		prototype += ")V";

		jmethodID mid = cur_env->GetMethodID(cur_cls, function, prototype.c_str());

		if (mid == nullptr)
		{
			core.Log("app", "CallJavaMethod: Method %s not found", function);
		}
		else
		{
			cur_env->CallVoidMethodA(cur_obj, mid, &cur_args[0]);
		}
	}
}

class Renderer : public Object
{
public:

	Scene scene;
	Program * prg;
	GeometryBuffer* vbuffer;
	VertexDecl* vdecl;
	Texture* texture;
	Texture* texture_sec;
	float tri_angle = 0.0f;

	void Init()
	{
		prg = render.GetProgram("TestTriangleProgram");

		struct Vertex
		{
			Vector pos;
			uint32_t color;
		};

		texture = render.LoadTexture("bat.jpg");
		texture_sec = render.LoadTexture("folder.jpg");

		Vertex vertices[] = { { Vector(0.5f, 0.0f, 0.0f), COLOR_GREEN.Get() },
		{ Vector(0.0f, 1.0f, 0.0f), COLOR_RED.Get() },
		{ Vector(1.0f, 1.0f, 0.0f), COLOR_CYAN.Get() } };

		vbuffer = render.GetDevice()->CreateBuffer(3, sizeof(Vertex));

		Vertex* ptr = (Vertex*)vbuffer->Lock();
		memcpy(ptr, vertices, vbuffer->GetSize());
		vbuffer->Unlock();

		VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Ubyte4, VertexDecl::Color, 0 } };
		vdecl = render.GetDevice()->CreateVertexDecl(2, desc);

		scene.Init();
		scene.Load("Media/beatemup.scn");
		//scene.Load("Media/Scene.scn");
		scene.Play();
	}

	void Draw(float dt)
	{
		/*render.DebugSprite(texture, 30.0f, 200.0f);
		render.DebugSphere(0.0f, COLOR_RED, 0.25f);
		render.DebugLine2D(1.0f, COLOR_BLUE, 800.0f, COLOR_GREEN);
		render.DebugLine(Vector(-0.5f, -0.5f, 0.0f), COLOR_WHITE, Vector(0.5f, 0.5f, 0.0f), COLOR_GREEN);
		render.DebugPrintText(5.0f, COLOR_GREEN, "FPS: 999");*/

		render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		/*tri_angle += dt;

		Matrix proj;
		proj.BuildProjection(45 * RADIAN, render.GetDevice()->GetAspect(), 1.0f, 1000.0f);

		Matrix view;
		view.BuildView(Vector(0.0f, 0.0f, 5.0f), 0.0f, Vector(0.0f, 1.0f, 0.0f));

		render.SetTransform(Render::View, view);
		render.SetTransform(Render::Projection, proj);

		Matrix rot;
		rot.RotateY(tri_angle);

		Matrix mat = rot * view * proj;

		render.GetDevice()->SetProgram(prg);

		prg->SetMatrix(Program::Vertex, "mvpMatrix", &mat, 1);

		Vector4 color = 0.95f;
		prg->SetVector(Program::Pixel, "color", &color, 1);

		prg->SetTexture(Program::Pixel, "s_texture", texture);
		prg->SetTexture(Program::Pixel, "s_texture2", texture_sec);

		render.GetDevice()->SetVertexDecl(vdecl);
		render.GetDevice()->SetVertexBuffer(0, vbuffer);

		render.GetDevice()->Draw(Device::TrianglesList, 0, 1);*/

		render.ExecutePool(0, dt);
		render.ExecutePool(1000, dt);
	}
};

Renderer renderer;

JNIEXPORT void JNICALL
Java_com_atum_engine_GLES3JNILib_Init(JNIEnv* env, jobject obj)
{
	UpdateJavaEnv(env, obj);

	core.Init(nullptr);

	render.AddExecutedLevelPool(1);

	renderTaskPool = render.AddTaskPool();
	renderTaskPool->AddTask(1, &renderer, (Object::Delegate)&Renderer::Draw);

	renderer.Init();
}

JNIEXPORT void JNICALL
Java_com_atum_engine_GLES3JNILib_Resize(JNIEnv* env, jobject obj, jint width, jint height)
{
	UpdateJavaEnv(env, obj);

	render.GetDevice()->SetVideoMode(width, height, nullptr);
}

JNIEXPORT void JNICALL
Java_com_atum_engine_GLES3JNILib_Update(JNIEnv* env, jobject obj)
{
	UpdateJavaEnv(env, obj);

	core.CountDeltaTime();

	renderer.scene.Execute(core.GetDeltaTime());

	core.Update();
}

JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_SetAssetManager(JNIEnv* env, jobject obj, jobject assetManager)
{
	AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
	android_fopen_set_asset_manager(mgr);
}

JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_TouchStart(JNIEnv* env, jobject obj, int index, int x, int y)
{
	//core.Log("Touch", "Touch %i at (%i, %i) started", index, x, y);
	controls.TouchStart(index, x, y);
}

JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_TouchUpdate(JNIEnv* env, jobject obj, int index, int x, int y)
{
	//core.Log("Touch", "Touch %i updated to (%i, %i)", index, x, y);
	controls.TouchUpdate(index, x, y);
}

JNIEXPORT void JNICALL Java_com_atum_engine_GLES3JNILib_TouchEnd(JNIEnv* env, jobject obj, int index)
{
	core.Log("Touch", "Touch %i ended", index);
	controls.TouchEnd(index);
}
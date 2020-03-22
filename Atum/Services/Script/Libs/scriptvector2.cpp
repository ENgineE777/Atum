#include <assert.h>
#include <string.h> // strstr
#include <new> // new()
#include <math.h>
#include "scriptvector2.h"
#include "Support/Math/Math.h"

static void Vector2DefaultConstructor(Vector2* self)
{
	new(self) Vector2();
}

static void Vector2Init1Constructor(float f, Vector2* self)
{
	new(self) Vector2(f);
}

static void Vector2Init2Constructor(float x, float y, Vector2* self)
{
	new(self) Vector2(x, y);
}

static void Vector2CopyConstructor(const Vector2& other, Vector2* self)
{
	new(self) Vector2(other);
}

static void Vector2NegativeOperator_Generic(asIScriptGeneric *gen)
{
	Vector2* v = (Vector2*)gen->GetObject();
	static Vector2 res;
	res = -(*v);
	gen->SetReturnObject(&res);
}

static void Vector2Set1_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	thisPointer->Set(f);
}

static void Vector2Set2_Generic(asIScriptGeneric *gen)
{
	float x = gen->GetArgFloat(0);
	float y = gen->GetArgFloat(1);
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	thisPointer->Set(x, y);
}

static void Vector2Set3_Generic(asIScriptGeneric *gen)
{
	Vector2* v = (Vector2*)gen->GetArgAddress(0);
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	thisPointer->Set(v->x, v->y);
}

static void Vector2Length1_Generic(asIScriptGeneric *gen)
{
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length());
}

static void Vector2Length2_Generic(asIScriptGeneric *gen)
{
	Vector2* v = (Vector2*)gen->GetArgAddress(0);
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length(*v));
}

static void Vector2Length21_Generic(asIScriptGeneric *gen)
{
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length2());
}

static void Vector2Length22_Generic(asIScriptGeneric *gen)
{
	Vector2* v = (Vector2*)gen->GetArgAddress(0);
	Vector2* thisPointer = (Vector2*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length2(*v));
}

void RegisterScriptVector2_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("Vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("Vector2", "float x", offsetof(Vector2, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("Vector2", "float y", offsetof(Vector2, y)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(Vector2DefaultConstructor), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(Vector2Init1Constructor), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(Vector2Init2Constructor), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(const Vector2 &in)", asFUNCTION(Vector2CopyConstructor), asCALL_CDECL_OBJLAST);

	// Register the operator overloads
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opNeg()", asFUNCTION(Vector2NegativeOperator_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "bool opEquals(const Vector2&in)", asMETHODPR(Vector2, operator==, (const Vector2& v), bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAssign(float f)", asMETHODPR(Vector2, operator=, (float f), Vector2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAssign(const Vector2 &in)", asMETHODPR(Vector2, operator=, (const Vector2&), Vector2&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(float f)", asMETHODPR(Vector2, operator+=, (float f), Vector2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(const Vector2 &in)", asMETHODPR(Vector2, operator+=, (const Vector2&), Vector2&), asCALL_THISCALL); assert( r >= 0 );

	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opSubAssign(float f)", asMETHODPR(Vector2, operator-=, (float f), Vector2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opSubAssign(const Vector2 &in)", asMETHODPR(Vector2, operator-=, (const Vector2&), Vector2&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opMulAssign(float f)", asMETHODPR(Vector2, operator*=, (float f), Vector2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opMulAssign(const Vector2 &in)", asMETHODPR(Vector2, operator*=, (const Vector2&), Vector2&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opDivAssign(float f)", asMETHODPR(Vector2, operator/=, (float f), Vector2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opDivAssign(const Vector2 &in)", asMETHODPR(Vector2, operator/=, (const Vector2&), Vector2&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const", asFUNCTIONPR(operator+, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd_r(float) const", asFUNCTIONPR(operator+, (float, const Vector2&), Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const", asFUNCTIONPR(operator-, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub_r(float) const", asFUNCTIONPR(operator-, (float, const Vector2&), Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(const Vector2 &in) const", asFUNCTIONPR(operator*, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul_r(float) const", asFUNCTIONPR(operator*, (float, const Vector2&), Vector2), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("Vector2", "Vector2 opDiv(const Vector2 &in) const", asFUNCTIONPR(operator/, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opDiv_r(float) const", asFUNCTIONPR(operator/, (float, const Vector2&), Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);

	// Register the object methods
	r = engine->RegisterObjectMethod("Vector2", "void Set(float f)", asFUNCTION(Vector2Set1_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "void Set(float x, float y)", asFUNCTION(Vector2Set2_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "void Set(Vector2&in v)", asFUNCTION(Vector2Set3_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "void Normalize()", asMETHOD(Vector2, Normalize), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "void Min(Vector2&in v)", asMETHOD(Vector2, Min), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "void Max(Vector2&in v)", asMETHOD(Vector2, Max), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "float Length()", asFUNCTION(Vector2Length1_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "float Length(Vector2&in v)", asFUNCTION(Vector2Length2_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "float Length2()", asFUNCTION(Vector2Length21_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "float Length2(Vector2&in v)", asFUNCTION(Vector2Length22_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "void Lerp(Vector2&in from, Vector2&in to, float k)", asMETHOD(Vector2, Lerp), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "float Dot(Vector2&in v)", asMETHOD(Vector2, Dot), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector2", "float Cross(Vector2&in v)", asMETHOD(Vector2, Cross), asCALL_THISCALL); assert(r >= 0);
}

void RegisterScriptVector2(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
	{
		//RegisterScriptVector2_Generic(engine);
	}
	else
	{
		RegisterScriptVector2_Native(engine);
	}
}
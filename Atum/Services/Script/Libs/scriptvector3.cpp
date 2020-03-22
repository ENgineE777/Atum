#include <assert.h>
#include <string.h> // strstr
#include <new> // new()
#include <math.h>
#include "scriptvector3.h"
#include "Support/Math/Math.h"

/*Vector3()
Vector3(float f)
Vector3(float x, float y, float z)

void Set(float f)
void Set(float x, float y, float z)
void Set(Vector3 v)
Vector3 operator - () const
bool operator == (const Vector3& v)
Vector3& operator = (float f)
Vector3& operator = (const Vector3& v)
Vector3& operator += (float f)
Vector3& operator += (const Vector3& v)
Vector3& operator -= (float f)
Vector3& operator -= (const Vector3& v)
Vector3& operator *= (float f)
Vector3& operator *= (const Vector3& v)
Vector3& operator /= (float f)
Vector3& operator /= (const Vector3& v)
friend Vector3 operator + (const Vector3& v, float f)
friend Vector3 operator + (float f, const Vector3& v)
friend Vector3 operator + (const Vector3& v1, const Vector3& v2)
friend Vector3 operator - (const Vector3& v, float f)
friend Vector3 operator - (float f, const Vector3& v)
friend Vector3 operator - (const Vector3& v1, const Vector3& v2)
friend Vector3 operator * (const Vector3& v, float f)
friend Vector3 operator * (float f, const Vector3& v)
friend Vector3 operator * (const Vector3& v1, const Vector3& v2)
friend Vector3 operator / (const Vector3& v, float f)
friend Vector3 operator / (float f, const Vector3& v)
friend Vector3 operator / (const Vector3& v1, const Vector3& v2)
float Normalize()
void Min(Vector3 v)
void Max(Vector3 v)
float Length()
float Length(Vector3 v)
float Length2()
float Length2(Vector3 v)
void Lerp(Vector3 from, Vector3 to, float k)
float Dot(Vector3 v)
Vector3 Cross(Vector3 v)*/


static void Vector3DefaultConstructor(Vector3* self)
{
	new(self) Vector3();
}

static void Vector3Init1Constructor(float f, Vector3* self)
{
	new(self) Vector3(f);
}

static void Vector3Init2Constructor(float x, float y, float z,Vector3* self)
{
	new(self) Vector3(x, y, z);
}

static void Vector3CopyConstructor(const Vector3& other, Vector3* self)
{
	new(self) Vector3(other);
}

static void Vector3NegativeOperator_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetObject();
	static Vector3 res;
	res = -(*v);
	gen->SetReturnObject(&res);
}

static void Vector3Set1_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	thisPointer->Set(f);
}

static void Vector3Set2_Generic(asIScriptGeneric *gen)
{
	float x = gen->GetArgFloat(0);
	float y = gen->GetArgFloat(1);
	float z = gen->GetArgFloat(2);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	thisPointer->Set(x, y, z);
}

static void Vector3Set3_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	thisPointer->Set(v->x, v->y, v->z);
}

static void Vector3Length1_Generic(asIScriptGeneric *gen)
{
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length());
}

static void Vector3Length2_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length(*v));
}

static void Vector3Length21_Generic(asIScriptGeneric *gen)
{
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length2());
}

static void Vector3Length22_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	gen->SetReturnFloat(thisPointer->Length2(*v));
}

void RegisterScriptVector3_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("Vector3", sizeof(Vector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("Vector3", "float x", asOFFSET(Vector3, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("Vector3", "float y", asOFFSET(Vector3, y)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("Vector3", "float z", asOFFSET(Vector3, z)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vector3DefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f(float x)", asFUNCTION(Vector3Init1Constructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z)", asFUNCTION(Vector3Init2Constructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT, "void f(const Vector3 &in)", asFUNCTION(Vector3CopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("Vector3", "Vector3 opNeg()", asFUNCTION(Vector3NegativeOperator_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "bool opEquals(const Vector3&in)", asMETHODPR(Vector3, operator==, (const Vector3& v), bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opAssign(float f)", asMETHODPR(Vector3, operator=, (float f), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opAssign(const Vector3 &in)", asMETHODPR(Vector3, operator=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opAddAssign(float f)", asMETHODPR(Vector3, operator+=, (float f), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opAddAssign(const Vector3 &in)", asMETHODPR(Vector3, operator+=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opSubAssign(float f)", asMETHODPR(Vector3, operator-=, (float f), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opSubAssign(const Vector3 &in)", asMETHODPR(Vector3, operator-=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opMulAssign(float f)", asMETHODPR(Vector3, operator*=, (float f), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opMulAssign(const Vector3 &in)", asMETHODPR(Vector3, operator*=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opDivAssign(float f)", asMETHODPR(Vector3, operator/=, (float f), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 &opDivAssign(const Vector3 &in)", asMETHODPR(Vector3, operator/=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 opAdd(const Vector3 &in) const", asFUNCTIONPR(operator+, (const Vector3&, const Vector3&), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 opAdd_r(float) const", asFUNCTIONPR(operator+, (float, const Vector3&), Vector3), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 opSub(const Vector3 &in) const", asFUNCTIONPR(operator-, (const Vector3&, const Vector3&), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 opSub_r(float) const", asFUNCTIONPR(operator-, (float, const Vector3&), Vector3), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 opMul(const Vector3 &in) const", asFUNCTIONPR(operator*, (const Vector3&, const Vector3&), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 opMul_r(float) const", asFUNCTIONPR(operator*, (float, const Vector3&), Vector3), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3", "Vector3 opDiv(const Vector3 &in) const", asFUNCTIONPR(operator/, (const Vector3&, const Vector3&), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 opDiv_r(float) const", asFUNCTIONPR(operator/, (float, const Vector3&), Vector3), asCALL_CDECL_OBJLAST); assert(r >= 0);

	// Register the object methods
	r = engine->RegisterObjectMethod("Vector3", "void Set(float f)", asFUNCTION(Vector3Set1_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "void Set(float x, float y)", asFUNCTION(Vector3Set2_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "void Set(Vector3&in v)", asFUNCTION(Vector3Set3_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "void Normalize()", asMETHOD(Vector3, Normalize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "void Min(Vector3&in v)", asMETHOD(Vector3, Min), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "void Max(Vector3&in v)", asMETHOD(Vector3, Max), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "float Length()", asFUNCTION(Vector3Length1_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "float Length(Vector3&in v)", asFUNCTION(Vector3Length2_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "float Length2()", asFUNCTION(Vector3Length21_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "float Length2(Vector3&in v)", asFUNCTION(Vector3Length22_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "void Lerp(Vector3&in from, Vector3&in to, float k)", asMETHOD(Vector3, Lerp), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "float Dot(Vector3&in v)", asMETHOD(Vector3, Dot), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Vector3", "Vector3 Cross(Vector3&in v)", asMETHOD(Vector3, Cross), asCALL_THISCALL); assert(r >= 0);
}

void RegisterScriptVector3(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
	{
		//RegisterScriptVector3_Generic(engine);
	}
	else
	{
		RegisterScriptVector3_Native(engine);
	}
}

END_AS_NAMESPACE



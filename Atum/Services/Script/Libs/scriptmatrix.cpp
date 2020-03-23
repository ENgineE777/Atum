#include "scriptmatrix.h"
#include <assert.h>
#include <string.h> // strstr
#include "Support/Math/Math.h"

void MatrixDefaultConstructor(Matrix *self)
{
	new(self) Matrix();
}

static void MatrixCopyConstructor(const Matrix &other, Matrix *self)
{
	new(self) Matrix(other);
}

static void MatrixSetVx_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Matrix* thisPointer = (Matrix*)gen->GetObject();
	thisPointer->Vx() = *v;
}

static void MatrixGetVx_Generic(asIScriptGeneric *gen)
{
	Matrix* mat = (Matrix*)gen->GetObject();
	static Vector3 res;
	res = mat->Vx();
	gen->SetReturnObject(&res);
}

static void MatrixSetVy_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Matrix* thisPointer = (Matrix*)gen->GetObject();
	thisPointer->Vy() = *v;
}

static void MatrixGetVy_Generic(asIScriptGeneric *gen)
{
	Matrix* mat = (Matrix*)gen->GetObject();
	static Vector3 res;
	res = mat->Vy();
	gen->SetReturnObject(&res);
}

static void MatrixSetVz_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Matrix* thisPointer = (Matrix*)gen->GetObject();
	thisPointer->Vz() = *v;
}

static void MatrixGetVz_Generic(asIScriptGeneric *gen)
{
	Matrix* mat = (Matrix*)gen->GetObject();
	static Vector3 res;
	res = mat->Vz();
	gen->SetReturnObject(&res);
}

static void MatrixSetPos_Generic(asIScriptGeneric *gen)
{
	Vector3* v = (Vector3*)gen->GetArgAddress(0);
	Matrix* thisPointer = (Matrix*)gen->GetObject();
	thisPointer->Pos() = *v;
}

static void MatrixGetPos_Generic(asIScriptGeneric *gen)
{
	Matrix* mat = (Matrix*)gen->GetObject();
	static Vector3 res;
	res = mat->Pos();
	gen->SetReturnObject(&res);
}

//--------------------------------
// Registration
//-------------------------------------

void RegisterScriptMatrix_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("Matrix", sizeof(Matrix), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("Matrix", "float _11", offsetof(Matrix, _11)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _12", offsetof(Matrix, _12)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _13", offsetof(Matrix, _13)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _14", offsetof(Matrix, _14)); assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float _21", offsetof(Matrix, _21)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _22", offsetof(Matrix, _22)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _23", offsetof(Matrix, _23)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _24", offsetof(Matrix, _24)); assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float _31", offsetof(Matrix, _31)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _32", offsetof(Matrix, _32)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _33", offsetof(Matrix, _33)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _34", offsetof(Matrix, _34)); assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float _41", offsetof(Matrix, _41)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _42", offsetof(Matrix, _42)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _43", offsetof(Matrix, _43)); assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float _44", offsetof(Matrix, _44)); assert(r >= 0);

	// Register the constructors
	r = engine->RegisterObjectBehaviour("Matrix", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(MatrixDefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Matrix", asBEHAVE_CONSTRUCT,  "void f(const Matrix &in)", asFUNCTION(MatrixCopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("Matrix", "bool opEquals(const Matrix&in)", asMETHODPR(Matrix, operator==, (const Matrix& v), bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Matrix", "Matrix &opMulAssign(const Matrix &in)", asMETHODPR(Matrix, operator*=, (const Matrix&), Matrix&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "Matrix opMul(const Matrix &in) const", asFUNCTIONPR(operator*, (const Matrix&, const Matrix&), Matrix), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "Vector3 opMul_r(const Vector3 &in) const", asFUNCTIONPR(operator*, (const Vector3&, const Matrix&), Vector3), asCALL_CDECL_OBJLAST); assert(r >= 0);

	// Register the object methods
	r = engine->RegisterObjectMethod("Matrix", "void SetVx(Vector3 &in)", asFUNCTION(MatrixSetVx_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "Vector3 GetVx()", asFUNCTION(MatrixGetVx_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("Matrix", "void SetVy(Vector3 &in)", asFUNCTION(MatrixSetVy_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "Vector3 GetVy()", asFUNCTION(MatrixGetVy_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("Matrix", "void SetVz(Vector3 &in)", asFUNCTION(MatrixSetVz_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "Vector3 GetVz()", asFUNCTION(MatrixGetVz_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("Matrix", "void SetPos(Vector3 &in)", asFUNCTION(MatrixSetPos_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "Vector3 GetPos()", asFUNCTION(MatrixGetPos_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("Matrix", "void Identity()", asMETHOD(Matrix, Identity), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Matrix", "void SetZero()", asMETHOD(Matrix, SetZero), asCALL_THISCALL); assert( r >= 0 );

	r = engine->RegisterObjectMethod("Matrix", "void RotateX(float ang)", asMETHOD(Matrix, RotateX), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "void RotateY(float ang)", asMETHOD(Matrix, RotateY), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "void RotateZ(float ang)", asMETHOD(Matrix, RotateZ), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Matrix", "void Scale(Vector3 &in)", asMETHOD(Matrix, Scale), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "bool Inverse()", asMETHOD(Matrix, Inverse), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("Matrix", "void Transpose()", asMETHOD(Matrix, Transpose), asCALL_THISCALL); assert(r >= 0);
}

void RegisterScriptMatrix(asIScriptEngine *engine)
{
	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
	{
		//RegisterScriptVector2_Generic(engine);
	}
	else
	{
		RegisterScriptMatrix_Native(engine);
	}
}

END_AS_NAMESPACE



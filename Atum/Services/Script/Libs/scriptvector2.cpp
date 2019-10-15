#include <assert.h>
#include <string.h> // strstr
#include <new> // new()
#include <math.h>
#include "scriptvector2.h"

#ifdef __BORLANDC__
// C++Builder doesn't define a non-standard "sqrtf" function but rather an overload of "sqrt"
// for float arguments.
inline float sqrtf (float x) { return sqrt (x); }
#endif

BEGIN_AS_NAMESPACE

Vector2S::Vector2S()
{
	x = 0.0f;
	y = 0.0f;
}

Vector2S::Vector2S(const Vector2S &other)
{
	x = other.x;
	y = other.y;
}

Vector2S::Vector2S(float _x, float _y)
{
	x = _x;
	y = _y;
}

bool operator==(const Vector2S &a, const Vector2S &b)
{
	return (a.x == b.x) && (a.y == b.y);
}

bool operator!=(const Vector2S &a, const Vector2S &b)
{
	return !(a == b);
}

Vector2S &Vector2S::operator=(const Vector2S &other)
{
	x = other.x;
	y = other.y;
	return *this;
}

Vector2S &Vector2S::operator+=(const Vector2S &other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2S &Vector2S::operator-=(const Vector2S &other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2S &Vector2S::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2S &Vector2S::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

float Vector2S::Length()
{
	return sqrtf(x*x + y*y);
}

void Vector2S::Normalize()
{
	float len = Length();

	if (len>0.01f)
	{
		x /= len;
		y /= len;
	}
}

void Vector2S::Lerp(Vector2S& p1,Vector2S& p2, float k)
{
	x = p1.x + (p2.x - p1.x) * k;
	y = p1.y + (p2.y - p1.y) * k;
}

Vector2S operator+(const Vector2S &a, const Vector2S &b)
{
	Vector2S res(a.x + b.x, a.y + b.y);
	return res;
}

Vector2S operator-(const Vector2S &a, const Vector2S &b)
{
	Vector2S res(a.x - b.x, a.y - b.y);
	return res;
}

Vector2S operator*(float s, const Vector2S &v)
{
	Vector2S res(v.x * s, v.y * s);
	return res;
}

Vector2S operator*(const Vector2S &v, float s)
{
	Vector2S res(v.x * s, v.y * s);
	return res;
}

Vector2S operator/(const Vector2S &v, float s)
{
	Vector2S res(v.x / s, v.y / s);
	return res;
}

//-----------------------
// AngelScript functions
//-----------------------

static void Vector2DefaultConstructor(Vector2S *self)
{
	new(self) Vector2S();
}

static void Vector2CopyConstructor(const Vector2S &other, Vector2S *self)
{
	new(self) Vector2S(other);
}

static void Vector2InitConstructor(float x, float y, Vector2S *self)
{
	new(self) Vector2S(x,y);
}

//--------------------------------
// Registration
//-------------------------------------

void RegisterScriptVector2_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("Vector2", sizeof(Vector2S), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("Vector2", "float x", offsetof(Vector2S, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("Vector2", "float y", offsetof(Vector2S, y)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f()",                     asFUNCTION(Vector2DefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(const Vector2 &in)",    asFUNCTION(Vector2CopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(float, float)",  asFUNCTION(Vector2InitConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(const Vector2 &in)", asMETHODPR(Vector2S, operator+=, (const Vector2S &), Vector2S&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opSubAssign(const Vector2 &in)", asMETHODPR(Vector2S, operator-=, (const Vector2S &), Vector2S&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opMulAssign(float)", asMETHODPR(Vector2S, operator*=, (float), Vector2S&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opDivAssign(float)", asMETHODPR(Vector2S, operator/=, (float), Vector2S&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "bool opEquals(const Vector2 &in) const", asFUNCTIONPR(operator==, (const Vector2S&, const Vector2S&), bool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const", asFUNCTIONPR(operator+, (const Vector2S&, const Vector2S&), Vector2S), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const", asFUNCTIONPR(operator-, (const Vector2S&, const Vector2S&), Vector2S), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(float) const", asFUNCTIONPR(operator*, (const Vector2S&, float), Vector2S), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul_r(float) const", asFUNCTIONPR(operator*, (float, const Vector2S&), Vector2S), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opDiv(float) const", asFUNCTIONPR(operator/, (const Vector2S&, float), Vector2S), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("Vector2", "float Length()", asMETHOD(Vector2S,Length), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "void Normalize()", asMETHOD(Vector2S,Normalize), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "void Lerp(Vector2&in p1,Vector2&in p2, float  k)", asMETHOD(Vector2S,Lerp), asCALL_THISCALL); assert( r >= 0 );		
}

static void Vector2DefaultConstructor_Generic(asIScriptGeneric *gen)
{
	Vector2S *self = (Vector2S*)gen->GetObject();
	new(self) Vector2S();
}

static void Vector2CopyConstructor_Generic(asIScriptGeneric *gen)
{
	Vector2S *other = (Vector2S*)gen->GetArgObject(0);
	Vector2S *self = (Vector2S*)gen->GetObject();
	Vector2CopyConstructor(*other, self);
}

static void Vector2InitConstructor_Generic(asIScriptGeneric *gen)
{
	float x = gen->GetArgFloat(0);
	float y = gen->GetArgFloat(1);	
	Vector2S *self = (Vector2S*)gen->GetObject();
	Vector2InitConstructor(x,y,self);
}

static void Vector2Equal_Generic(asIScriptGeneric *gen)
{
	Vector2S *a = (Vector2S*)gen->GetObject();
	Vector2S *b = (Vector2S*)gen->GetArgAddress(0);
	bool r = *a == *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void Vector2Length_Generic(asIScriptGeneric *gen)
{
	Vector2S *s = (Vector2S*)gen->GetObject();
	gen->SetReturnFloat(s->Length());
}

static void Vector2Normalize_Generic(asIScriptGeneric *gen)
{
	Vector2S *s = (Vector2S*)gen->GetObject();
	s->Normalize();	
}

static void Vector2Lerp_Generic(asIScriptGeneric *gen)
{
	Vector2S *s = (Vector2S*)gen->GetObject();
	
	Vector2S *a = (Vector2S*)gen->GetArgAddress(0);
	Vector2S *b = (Vector2S*)gen->GetArgAddress(1);
	float k = gen->GetArgFloat(2);

	s->Lerp(*a, *b, k);
}

static void Vector2AddAssign_Generic(asIScriptGeneric *gen)
{
	Vector2S *a = (Vector2S*)gen->GetArgAddress(0);
	Vector2S *thisPointer = (Vector2S*)gen->GetObject();
	*thisPointer += *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2SubAssign_Generic(asIScriptGeneric *gen)
{
	Vector2S *a = (Vector2S*)gen->GetArgAddress(0);
	Vector2S *thisPointer = (Vector2S*)gen->GetObject();
	*thisPointer -= *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2MulAssign_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector2S *thisPointer = (Vector2S*)gen->GetObject();
	*thisPointer *= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2DivAssign_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector2S *thisPointer = (Vector2S*)gen->GetObject();
	*thisPointer /= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2Add_Generic(asIScriptGeneric *gen)
{
	Vector2S *a = (Vector2S*)gen->GetObject();
	Vector2S *b = (Vector2S*)gen->GetArgAddress(0);
	Vector2S res = *a + *b;
	gen->SetReturnObject(&res);
}

static void Vector2Sub_Generic(asIScriptGeneric *gen)
{
	Vector2S *a = (Vector2S*)gen->GetObject();
	Vector2S *b = (Vector2S*)gen->GetArgAddress(0);
	Vector2S res = *a - *b;
	gen->SetReturnObject(&res);
}

static void Vector2FloatMulVector2_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector2S *v = (Vector2S*)gen->GetObject();
	Vector2S res = s * *v;
	gen->SetReturnObject(&res);
}

static void Vector2Vector2MulFloat_Generic(asIScriptGeneric *gen)
{
	Vector2S *v = (Vector2S*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector2S res = *v * s;
	gen->SetReturnObject(&res);
}

static void Vector2Vector2DivFloat_Generic(asIScriptGeneric *gen)
{
	Vector2S *v = (Vector2S*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector2S res = *v / s;
	gen->SetReturnObject(&res);
}

void RegisterScriptVector2_Generic(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("Vector2", sizeof(Vector2S), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("Vector2", "float x", asOFFSET(Vector2S, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("Vector2", "float y", asOFFSET(Vector2S, y)); assert( r >= 0 );	

	// Register the constructors
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f()",                    asFUNCTION(Vector2DefaultConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f(const Vector2 &in)",   asFUNCTION(Vector2CopyConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT, "void f(float, float y = 0, float z = 0)", asFUNCTION(Vector2InitConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(const Vector2 &in)", asFUNCTION(Vector2AddAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opSubAssign(const Vector2 &in)", asFUNCTION(Vector2SubAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opMulAssign(float)", asFUNCTION(Vector2MulAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 &opDivAssign(float)", asFUNCTION(Vector2DivAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "bool opEquals(const Vector2 &in) const", asFUNCTION(Vector2Equal_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const", asFUNCTION(Vector2Add_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const", asFUNCTION(Vector2Sub_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul_r(float) const", asFUNCTION(Vector2FloatMulVector2_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(float) const", asFUNCTION(Vector2Vector2MulFloat_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opDiv(float) const", asFUNCTION(Vector2Vector2DivFloat_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("Vector2", "float Length() const", asFUNCTION(Vector2Length_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "void Normalize()", asFUNCTION(Vector2Normalize_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Vector2", "void Lerp(Vector2&in p1, Vector2&in p2, float k)", asFUNCTION(Vector2Lerp_Generic), asCALL_GENERIC); assert( r >= 0 );	
}

void RegisterScriptVector2(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
	{
		RegisterScriptVector2_Generic(engine);
	}
	else
	{
		RegisterScriptVector2_Native(engine);
	}
}

END_AS_NAMESPACE



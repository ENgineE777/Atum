
#pragma once

#define CLASSFACTORYDEF(baseClass) \
class ClassFactory##baseClass \
{\
public:\
	ClassFactory##baseClass()\
	{\
		if (first)\
		{\
			next = first;\
			first = this;\
		}\
		else\
		{\
			first = this;\
			next = NULL;\
		}\
	};\
	virtual ~ClassFactory##baseClass() {};\
	static ClassFactory##baseClass* First() { return first; };\
	ClassFactory##baseClass* Next() { return next; };\
	static ClassFactory##baseClass* Find(const char* name)\
	{\
		for (ClassFactory##baseClass* decl = first; decl != NULL; decl = decl->next)\
		{\
			if (StringUtils::IsEqual(decl->GetName(), name))\
			{\
				return decl;\
			}\
		}\
		return NULL;\
	}\
	virtual const char* GetName() = 0;\
	virtual baseClass* Create() = 0;\
	static baseClass* Create(const char* name)\
	{\
		ClassFactory##baseClass* decl = Find(name);\
		if (decl)\
		{\
			return decl->Create();\
		}\
		return NULL;\
	}\
protected:\
	static ClassFactory##baseClass* first;\
	ClassFactory##baseClass* next;\
};

#define CLASSFACTORYDECL(baseClass)\
ClassFactory##baseClass* ClassFactory##baseClass::first = nullptr;

#define CLASSREGEX(baseClass, shortClassName, fullClassName) \
class ClassFactory##shortClassName : public ClassFactory##baseClass \
{\
	virtual const char* GetName() { return #shortClassName; };\
	virtual baseClass* Create() { return new fullClassName(); };\
};\
ClassFactory##shortClassName classFactory##shortClassName;

#define CLASSREG(baseClass, className) \
CLASSREGEX(baseClass, className, className)
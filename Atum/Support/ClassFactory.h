
#pragma once

#define CLASSFACTORYDEF(baseClass) \
class ClassFactory##baseClass \
{\
public:\
	ClassFactory##baseClass()\
	{\
		Decls().push_back(this);\
	};\
	static void Sort()\
	{\
		for (int i=0; i<Decls().size() - 1; i++)\
		{\
			for (int j = i + 1; j < Decls().size(); j++)\
			{\
				if (!StringUtils::CompareABC(Decls()[i]->GetName(), Decls()[j]->GetName()))\
				{\
					ClassFactory##baseClass* tmp = Decls()[i];\
					Decls()[i] = Decls()[j];\
					Decls()[j] = tmp;\
				}\
			}\
		}\
	}\
	virtual ~ClassFactory##baseClass() {};\
	static ClassFactory##baseClass* Find(const char* name)\
	{\
		for (auto& decl : Decls())\
		{\
			if (StringUtils::IsEqual(decl->GetName(), name))\
			{\
				return decl;\
			}\
		}\
		return nullptr;\
	}\
	virtual const char* GetName() = 0;\
	virtual const char* GetShortName() = 0;\
	virtual baseClass* Create() = 0;\
	static baseClass* Create(const char* name)\
	{\
		ClassFactory##baseClass* decl = Find(name);\
		if (decl)\
		{\
			return decl->Create();\
		}\
		return nullptr;\
	}\
	static vector<ClassFactory##baseClass*>& Decls()\
	{\
		static vector<ClassFactory##baseClass*> decls;\
		return decls;\
	}\
};

#define CLASSREGEX(baseClass, shortClassName, fullClassName, shortName) \
class ClassFactory##shortClassName##baseClass : public ClassFactory##baseClass \
{\
	virtual const char* GetName() { return #shortClassName; };\
	virtual const char* GetShortName() { return shortName; };;\
	virtual baseClass* Create() { return new fullClassName(); };\
};\
ClassFactory##shortClassName##baseClass classFactory##shortClassName##baseClass;

#define CLASSREG(baseClass, className, shortName) \
CLASSREGEX(baseClass, className, className, shortName)
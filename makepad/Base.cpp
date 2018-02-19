#pragma once
#ifndef as_header
#define def_Base
#endif

#pragma warning( disable : 4820 ) // struct alignment warning
#pragma warning( disable : 4189 ) // local variable is initialized but not refenced
#pragma warning( disable : 4100 ) // unreferenced formal parameter

#define dec_open(BASENAME, CLASSNAME)\
	struct Vt##CLASSNAME;\
	struct CLASSNAME;\
	typedef StrongPtr<CLASSNAME, Vt##CLASSNAME, Vt##BASENAME> s##CLASSNAME;\
	typedef WeakPtr<CLASSNAME,Vt##CLASSNAME, Vt##BASENAME> w##CLASSNAME;\
	struct CLASSNAME: BASENAME{

#define dec_data(TYPENAME, DATANAME)\
	TYPENAME DATANAME;

#define dec_strong(CLASSNAME, PTRNAME)\
	s##CLASSNAME PTRNAME;

#define dec_weak(CLASSNAME, PTRNAME)\
	w##CLASSNAME PTRNAME;

#define dec_callbk(RETURN, CBNAME, ...)\
	RETURN(*CBNAME)(__VA_ARGS__); 

#define dec_table(BASENAME, CLASSNAME)\
	};\
	struct Vt##CLASSNAME: Vt##BASENAME{ 

#define dec_customMethod(CLASSNAME, RETURN, METHODNAME,...)\
	RETURN(*METHODNAME)(__VA_ARGS__); 

#define dec_method(CLASSNAME, RETURN, METHODNAME,...)\
	RETURN(*METHODNAME)(s##CLASSNAME, __VA_ARGS__); 

#define dec_close(BASENAME, CLASSNAME)\
	};\
	extern void vtable_##CLASSNAME(VtInfo *info, void *vtClass);

#define def_method(CLASSNAME, RETURN, METHODNAME,  ...)\
	const VtReflectionMethod vtreflectionmethod_##CLASSNAME##_##METHODNAME = {#RETURN, #METHODNAME, #__VA_ARGS__};\
	RETURN CLASSNAME##_##METHODNAME(s##CLASSNAME self, __VA_ARGS__)

#define def_customMethod(CLASSNAME, RETURN, METHODNAME,  ...)\
	const VtReflectionMethod vtreflectionmethod_##CLASSNAME##_##METHODNAME = {#RETURN, #METHODNAME, #__VA_ARGS__};\
	RETURN CLASSNAME##_##METHODNAME(__VA_ARGS__)

#define def_override(BASENAME, CLASSNAME, RETURN, METHODNAME,  ...)\
	const VtReflectionMethod vtreflectionmethod_##CLASSNAME##_##METHODNAME = {#RETURN, #METHODNAME, #__VA_ARGS__};\
	RETURN CLASSNAME##_##METHODNAME(s##BASENAME _self, __VA_ARGS__)

#define self_override(CLASSNAME) s##CLASSNAME self = _self.ptr();

#define def_open(BASENAME, CLASSNAME)\
	void vtable_##CLASSNAME(VtInfo* info, void* _vtClass){\
	Vt##CLASSNAME* vtClass = (Vt##CLASSNAME*)_vtClass;\
	CLASSNAME *Class = 0;\
	size_t fieldsOf = 0;\
	VtReflectionField* fields = (VtReflectionField*)(vtClass + 1);\
	if(info){\
		info->nameOf = #CLASSNAME;\
		info->baseOf = #BASENAME;\
		info->fileOf = __FILE__;\
		info->sizeOf = sizeof(CLASSNAME);\
		info->sizeVt =  sizeof(Vt##CLASSNAME);\
	}

#define def_callbk(RETURN, CBNAME, ...)\
	if(vtClass){\
		if(Class) RETURN(*CBNAME)(__VA_ARGS__) = Class->##CBNAME;\
		VtReflectionField *field = &fields[fieldsOf];\
		field->type = VT_TYPE_CALLBACK;\
		field->var = #RETURN;\
		field->name = #CBNAME;\
		field->args = #__VA_ARGS__;\
		field->offset = (size_t)&Class->##CBNAME;\
		field->size = sizeof(Class->##CBNAME);\
	}\
	fieldsOf++;

#define def_data(TYPENAME, DATANAME)\
	if(vtClass){\
		VtReflectionField *field = &fields[fieldsOf];\
		field->type = VT_TYPE_DATA;\
		field->var = #TYPENAME;\
		field->name = #DATANAME;\
		field->args = 0;\
		field->offset = (size_t)&Class->##DATANAME;\
		field->size = sizeof(Class->##DATANAME);\
	}\
	fieldsOf++;

#define def_bind(CLASSNAME, METHODNAME)\
	if(vtClass){\
		vtClass->##METHODNAME = CLASSNAME##_##METHODNAME;\
		VtReflectionField *field = &fields[fieldsOf];\
		field->type = VT_TYPE_METHOD;\
		field->var =vtreflectionmethod_##CLASSNAME##_##METHODNAME.ret;\
		field->name = vtreflectionmethod_##CLASSNAME##_##METHODNAME.name;\
		field->args = vtreflectionmethod_##CLASSNAME##_##METHODNAME.args;\
		field->offset = (size_t)( (char*)&vtClass->##METHODNAME - (char*)vtClass);\
		field->size = sizeof(vtClass->##METHODNAME);\
	}\
	fieldsOf++;

#define def_strong(TYPENAME, PTRNAME) \
	if (vtClass) {\
		VtReflectionField *field = &fields[fieldsOf]; \
		field->nextPtr = vtClass->_firstPtrField;\
		vtClass->_firstPtrField = field;\
		field->type = VT_TYPE_STRONG; \
		field->var = #TYPENAME; \
		field->name = #PTRNAME; \
		field->args = 0; \
		field->offset = (size_t)&Class->##PTRNAME; \
		field->size = sizeof(Class->##PTRNAME);\
	}\
	fieldsOf++;

#define def_weak(CLASSNAME, PTRNAME) \
	if (vtClass) { \
		VtReflectionField *field = &fields[fieldsOf]; \
		field->nextPtr = vtClass->_firstPtrField; \
		vtClass->_firstPtrField = field; \
		field->type = VT_TYPE_WEAK; \
		field->var = #PTRNAME; \
		field->name = #PTRNAME; \
		field->args = 0; \
		field->offset = (size_t)&Class->##PTRNAME; \
		field->size = sizeof(Class->##PTRNAME);\
	}\
	fieldsOf++;
#define def_close(...)\
	if(info) info->fieldsOf = fieldsOf;\
	};


// from https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
namespace typenameHelper{
	static const unsigned int FRONT_SIZE = sizeof("typenameHelper::GetTypeNameHelper<struct ") - 1u;
	static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;
	template <typename T> struct GetTypeNameHelper {
		static const char* GetTypeName(void) {
			static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
			static char typeName[size] = {};
			const char *c = __FUNCTION__ + FRONT_SIZE;
			for (int i = 0; i < size - 1u; i++, c++) typeName[i] = *c;
			return typeName;
		}
	};
};

struct Base;
struct VtBase;

template<class TSELF, class TSELFVT, class TBASEVT> class StrongPtr {
	TSELF * _ptr;
public:

	// constructors
	inline StrongPtr() {
	}

	inline StrongPtr(const Base *other) {
		this->_ptr = (TSELF*)other;
		if (this->_ptr) this->_ptr->_strongPtrs++;
	}

	inline StrongPtr(int x) {
		this->_ptr = 0;
	}

	inline StrongPtr(const StrongPtr<TSELF, TSELFVT, TBASEVT> &other) {
		this->_ptr = other._ptr;
		if (this->_ptr) this->_ptr->_strongPtrs++;
	}

	inline ~StrongPtr() {
		if (this->_ptr && --this->_ptr->_strongPtrs == 0) this->_ptr->_vt->destroyClass(this->_ptr);
	}

	// replacement 

	inline StrongPtr & operator= (const StrongPtr &other) {
		if (this->_ptr && --this->_ptr->_strongPtrs == 0) this->_ptr->_vt->destroyClass(this->_ptr);
		this->_ptr = other._ptr;
		if (this->_ptr) this->_ptr->_strongPtrs++;
		return *this;
	}

	inline StrongPtr & operator= (const Base *other) {
		if (this->_ptr && --this->_ptr->_strongPtrs == 0) this->_ptr->_vt->destroyClass(this->_ptr);
		this->_ptr = (TSELF*)other;
		if (this->_ptr) this->_ptr->_strongPtrs++;
		return *this;
	}

	inline StrongPtr & operator= (int x) {
		if (this->_ptr && --this->_ptr->_strongPtrs == 0) this->_ptr->_vt->destroyClass(this->_ptr);
		this->_ptr = 0;
		return *this;
	}

	inline void create(StrongPtr<Base, VtBase, VtBase> owner) {
		if (this->_ptr && --this->_ptr->_strongPtrs == 0) this->_ptr->_vt->destroyClass(this->_ptr);
		TSELF *newInst = (TSELF*)owner->_vt->createClass(owner.ptr(), this->name());
		newInst->_strongPtrs = 1;
		this->_ptr = newInst;
	}

	inline TBASEVT* base() const {
		if (!this - ptr) return 0;
		return (TBASEVT*)this->_ptr->_baseVt;
	}

	// casts and operators

	inline const TSELFVT* operator*() const {
		if (!this->_ptr) return 0;
		return (const TSELFVT*)this->_ptr->_vt;
	}
	operator bool() const {
		return this->_ptr != 0;
	}

	operator StrongPtr<Base, VtBase, VtBase>() const {
		return this->_ptr;
	}

	inline TSELF* operator->() const {
		return this->_ptr;
	}

	inline TSELF *ptr() const {
		return this->_ptr;
	}

	inline const char *name() const {
		return typenameHelper::GetTypeNameHelper<TSELF>::GetTypeName();
	}
};

template<class TSELF, class TSELFVT, class TBASEVT> class WeakPtr {
	TSELF * _ptr;
public:
	inline WeakPtr() {
	}

	inline WeakPtr(const Base *other) {
		this->_ptr = (TSELF*)other;
		if (this->_ptr) this->_ptr->_weakPtrs++;
	}

	inline WeakPtr(int x) {
		this->_ptr = 0;
	}

	inline WeakPtr(const StrongPtr<TSELF, TSELFVT, TBASEVT> &other) {
		this->_ptr = other.ptr();
		if (this->_ptr) this->_ptr->_weakPtrs++;
	}

	inline WeakPtr(const WeakPtr<TSELF, TSELFVT, TBASEVT> &other) {
		// lets take a weakref to the other
		this->_ptr = other._ptr;
		if (this->_ptr) this->_ptr->_weakPtrs++;
	}

	inline ~WeakPtr() {
		if (this->_ptr) {
			this->_ptr->_weakPtrs--;
			if (this->_ptr->_strongPtrs == 0 && this->_ptr->_weakPtrs == 0) this->_ptr->_vt->free(this->_ptr, __FILE__, __LINE__, this->_ptr);
		}
	}

	// replacement 
	
	inline WeakPtr & operator= (const StrongPtr<TSELF, TSELFVT, TBASEVT> &other) {
		this->~WeakPtr();
		this->_ptr = other.ptr();
		if (this->_ptr) this->_ptr->_weakPtrs++;
		return *this;
	}

	inline WeakPtr & operator= (const WeakPtr &other) {
		this->~WeakPtr();
		this->_ptr = other.lock();
		if (this->_ptr) this->_ptr->_weakPtrs++;
		return *this;
	}

	inline WeakPtr & operator= (const Base *other) {
		this->~WeakPtr();
		this->_ptr = (TSELF*)other;
		if (this->_ptr) this->_ptr->_weakPtrs++;
		return *this;
	}

	inline WeakPtr & operator= (int x) {
		this->~WeakPtr();
		this->_ptr = 0;
		return *this;
	}

	inline StrongPtr<TSELF, TSELFVT, TBASEVT> operator*() const { // this turns it into a strong pointer
		if (this->_ptr && this->_ptr->_strongPtrs == 0) {
			if (--this->_ptr->_weakPtrs == 0) this->_ptr->_vt->free(this->_ptr, __FILE__, __LINE__, this->_ptr);
			this->_ptr = 0;
		}
		return StrongPtr<TSELF, TSELFVT, TBASEVT>(this->_ptr);
	}

	inline TSELF *ptr() {
		if (this->_ptr && this->_ptr->_strongPtrs == 0) {
			if (--this->_ptr->_weakPtrs == 0) this->_ptr->_vt->free(this->_ptr, __FILE__, __LINE__, this->_ptr);
			this->_ptr = 0;
		}
		return this->_ptr;
	}

	inline const char *name() const {
		return typenameHelper::GetTypeNameHelper<TSELF>::GetTypeName();
	}
};

typedef StrongPtr<Base, VtBase, VtBase> sBase;
typedef WeakPtr<Base, VtBase, VtBase> wBase;

struct Base {
	const VtBase *_vt;
	// object ownership structures
	int _strongPtrs;
	int _weakPtrs;
};

struct VtInfo {
	const char *nameOf;
	const char *baseOf;
	const char *fileOf;
	size_t sizeOf;
	size_t sizeVt;
	size_t fieldsOf;
};

typedef void(*def_vtable_fn)(VtInfo *info, void* vtClass);
typedef void*(*def_malloc_fn)(Base *self, const char *file, int line, size_t size);
typedef void(*def_free_fn)(Base *self, const char *file, int line, void *block);
typedef void(*def_error_fn)(Base *self, const char *file, int line, const char *fmt, ...);
typedef void(*def_log_fn)(Base *self, const char *file, int line, const char *fmt, ...);

#define selfLog(fmt, ...) self->_vt->log(self.ptr(), __FILE__, __LINE__, fmt, __VA_ARGS__)
#define selfError(fmt, ...) self->_vt->error(self.ptr(), __FILE__, __LINE__, fmt, __VA_ARGS__)
#define selfMalloc(size) self->_vt->malloc(self.ptr(), __FILE__, __LINE__, size)
#define selfFree(data) self->_vt->free(self.ptr(), __FILE__, __LINE__, data)

struct VtReflectionMethod {
	const char *ret;
	const char *name;
	const char *args;
};

#define VT_TYPE_METHOD 1 
#define VT_TYPE_CALLBACK 2
#define VT_TYPE_DATA 3
#define VT_TYPE_STRONG 4
#define VT_TYPE_WEAK 5

struct VtReflectionField {
	int type;
	const char *var;
	const char *name;
	const char *args;
	size_t offset;
	size_t size;
	VtReflectionField *nextPtr;
};

struct VtBase {
	const VtBase *_baseVt; // always points to the baseclass vtable
	const char *_nameOf;
	const char *_baseOf;
	const char *_fileOf;
	size_t _sizeOf; 
	size_t _sizeVt;
	size_t _fieldsOf;

	VtReflectionField *_firstPtrField;

	// going to be optimized into tables not just a bare linked list
	const VtBase *_firstClass;
	const VtBase *_prevClass;
	const VtBase *_nextClass;

	void  (*error)(Base *self, const char *file, int line, const char *fmt, ...);
	void  (*log)(Base *self, const char *file, int line, const char *fmt, ...);
	void* (*malloc)(Base *self, const char *file, int line, size_t size);
	void  (*free)(Base *self, const char *file, int line, void *data);

	Base* (*createClass)(Base *self, const char *name);
	void (*destroyClass)(Base *self);

	void (*constructor)(sBase self);
	void (*destructor)(sBase self);
};

extern void vtable_Base(VtInfo *info, void *vtClass);
extern sBase vtable_LoadFromArray(def_vtable_fn *vtableInitArray, def_malloc_fn malloc, def_free_fn free, def_log_fn log, def_error_fn error);

#ifdef def_Base

def_method(Base, void, constructor){
}

def_method(Base, void, destructor) {
}

const VtBase *Base_findClassImpl(const VtBase *firstClass, const char *name) {
	const VtBase *iter = firstClass;
	while(iter){
		const char *a = name;
		const char *b = iter->_nameOf;
		while (*a == *b && *a && *b) a++, b++;
		if (*a == *b && *a == 0) {
			return iter;
		}
		iter = iter->_nextClass;
	}
	return 0;
}

Base *Base_createClassImpl(Base *self, const VtBase *vt, const char *name) {
	const VtBase *foundVt = Base_findClassImpl(vt->_firstClass, name);

	if (!foundVt) {
		vt->error(self, __FILE__, __LINE__, "Error finding class %s from class %s", name, vt->_nameOf);
		return 0;
	}

	// lets create a class
	size_t sizeOf = foundVt->_sizeOf;
	if (sizeOf & (sizeof(size_t) - 1)) {
		vt->error(self, __FILE__, __LINE__, "Class size not system word aligned %s %s %d", name, self->_vt->_nameOf, sizeOf);
		return 0;
	}

	// set class memory to zero in a trivial size_t loop
	size_t *buf = (size_t*)vt->malloc(self, __FILE__, __LINE__, sizeOf);
	for (size_t i = 0, len = sizeOf / sizeof(size_t); i < len; i++) buf[i] = 0;

	Base *newInstance = (Base*)buf;
	
	newInstance->_vt = foundVt;

	// call the constructors top upwards
	newInstance->_strongPtrs++;
	const VtBase *iter = newInstance->_vt;

	void(*lastConstructor)(sBase self) = iter->constructor;
	while (iter) {
		if (lastConstructor != iter->constructor) lastConstructor(newInstance);
		lastConstructor = iter->constructor;
		iter = iter->_baseVt;
	}
	lastConstructor(newInstance);
	newInstance->_strongPtrs--;
	return newInstance;
}

def_customMethod(Base, Base*, createClass, Base *self, const char *name) {
	return Base_createClassImpl(self, self->_vt, name);
}

def_customMethod(Base, void, destroyClass, Base *self) {
	if (self->_strongPtrs != 0) self->_vt->error(self,__FILE__,__LINE__,"Class refcount not 0 but still destroying");

	// grab a strong ref to self so destruct callflow doesn't recur
	self->_strongPtrs = 1;

	def_free_fn free = self->_vt->free;

	// first iterate and find call all destructors
	const VtBase *iter = self->_vt;
	void(*lastDestructor)(sBase self) = iter->destructor;
	while (iter) {
		if (lastDestructor != iter->destructor) lastDestructor(self);
		lastDestructor = iter->destructor;
		iter = iter->_baseVt;
	}
	if(lastDestructor) lastDestructor(self);

	// then iterate and clear out any references
	iter = self->_vt;
	while (iter) {
		VtReflectionField *field = iter->_firstPtrField;
		while (field) {
			// ok we have a pointer, lets clear it up
			if (field->type == VT_TYPE_STRONG) {
				// fetch the pointer ref from the memoffset
				Base *ptr = *(Base**)(((char*)self) + field->offset);
				// free it
				if (ptr && --ptr->_strongPtrs == 0) {
					ptr->_vt->destroyClass(ptr);
				};
			}
			else if (field->type == VT_TYPE_WEAK) {
				Base *ptr = *(Base**)(((char*)self) + field->offset);
				// free it
				if (ptr && --ptr->_weakPtrs == 0 && ptr->_strongPtrs == 0) {
					ptr->_vt->destroyClass(ptr);
				};
			}
			field = field->nextPtr;
		}
		iter = iter->_baseVt;
	}
	if (self->_strongPtrs != 1) self->_vt->error(self, __FILE__, __LINE__, "Class refcount not 0 but still destroying");
	self->_strongPtrs = 0;
	// only free if we have no weak references
	if(self->_weakPtrs == 0) self->_vt->free(self, __FILE__, __LINE__, self);
}

def_open(Base, Base)
def_bind(Base, constructor)
def_bind(Base, destructor)
def_bind(Base, createClass)
def_bind(Base, destroyClass)
def_close()

sBase vtable_LoadFromArray(def_vtable_fn *vtableInitArray, def_malloc_fn malloc, def_free_fn free, def_log_fn log, def_error_fn error) {
	// lets set up vtables and call our vtable init functions

	int setLen = 0;
	while(vtableInitArray[setLen]) setLen++;

	VtBase *firstClass = 0, *prevClass = 0;
	Base *self = 0;
	for (int i = 0; i < setLen; i++) {
		// call the vtable loader for into
		VtInfo info;
		vtableInitArray[i](&info, 0);
		// find baseclass
		const VtBase *vtBase = firstClass?Base_findClassImpl(firstClass, info.baseOf):0;
			
		if (firstClass && !vtBase) {
			error(0, __FILE__, __LINE__, "Error finding baseclass %s for class %s", info.baseOf, info.nameOf);
			break;
		}

		// compute total size we need
		size_t totalSize = info.sizeVt + info.fieldsOf * sizeof(VtReflectionField);

		if (totalSize & (sizeof(size_t) - 1)) {
			error(0,__FILE__,__LINE__,"Class totalSize vtable not system word aligned %s %s %d", info.nameOf, info.baseOf, vtBase->_sizeVt);
			break;
		}

		VtBase *vtClass = (VtBase*)malloc(0, __FILE__, __LINE__, totalSize);
		
		// lets zero the vtClass memory
		size_t *zero = (size_t*)vtClass;
		for (size_t c = 0, len = totalSize / sizeof(size_t); c < len; c++) zero[c] = 0;

		// copy base vtable
		if (vtBase) {
			size_t *inp = (size_t*)vtBase;
			size_t *out = (size_t*)vtClass;
			if (vtBase->_sizeVt & (sizeof(size_t) - 1)) {
				error(0, __FILE__, __LINE__,"Class vtable size not system word aligned %s %s %d", info.nameOf, info.baseOf, vtBase->_sizeVt);
				break;
			}
			for (size_t c = 0, len = vtBase->_sizeVt / sizeof(size_t); c < len; c++) out[c] = inp[c];
		}
		else { // we must be vtBase, so set up externals
			vtClass->malloc = malloc;
			vtClass->free = free;
			vtClass->log = log;
			vtClass->error = error;
		}

		// copy in info struct into vtable
		vtClass->_baseVt = vtBase;
		vtClass->_nameOf = info.nameOf;
		vtClass->_baseOf = info.baseOf;
		vtClass->_fieldsOf = info.fieldsOf;
		vtClass->_fileOf = info.fileOf;
		vtClass->_sizeOf = info.sizeOf;
		vtClass->_sizeVt = info.sizeVt;
		vtClass->_nextClass = 0;
		// set up vtable
		vtableInitArray[i](0, vtClass);
		if (!firstClass) {
			firstClass = vtClass;
		}
		vtClass->_firstClass = firstClass;
		vtClass->_prevClass = prevClass;
		if (prevClass) prevClass->_nextClass = vtClass;
		prevClass = vtClass;
	}
	return Base_createClassImpl(0, firstClass, "Base");
}

#endif
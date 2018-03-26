#ifndef GENERAL_OBJECTS_H
#define GENERAL_OBJECTS_H

#include <string>

//
// Defines the following object-related classes and functions:
//
// class Object     Reference counted object base class
// class ptr<T>     Reference counting pointer (use with Object or COM classes)
// class vector<T>  Vector-like template class for POD items
//
// SAFE_RELEASE(p)  Releases the object and sets pointer to NULL
//

/*
Base class for reference counted objects.
AddRef and Release return the new reference counts.
*/
class Object
{
    unsigned long m_references;
public:
    unsigned long AddRef()  { return ++m_references; }
    unsigned long Release() {
        unsigned long refs = --m_references;
        if (refs == 0) delete this;
        return refs;
    }

    Object() : m_references(1) {}
    virtual ~Object() {}
};

/*
A reference counting pointer template for Object or COM objects
Note: when creating or assigning from a 'raw' pointer, the reference
      count is NOT increased. Only when copying between ptr<>'s are
      the reference counts properly taken care of.
*/
template <typename T>
class ptr
{
    friend class ptr;
	T* object;  // The actual object pointer
public:
	// Equality and inequality
	bool operator==(const T* obj)     const { return object == obj; }
	bool operator==(const ptr<T> ptr) const { return object == ptr.object; }
	bool operator!=(const T* obj)	  const { return object != obj; }
	bool operator!=(const ptr<T> ptr) const { return object != ptr.object; }
	
	// Dereference and cast operator
	T* operator->() const { return object; }
    operator T*()   const { return object; }

	// Assignment
	ptr<T>& operator=(const ptr<T>& ptr) {
        if (object != ptr.object) {
		    SAFE_RELEASE(object);
		    object = ptr.object;
		    if (object != NULL) object->AddRef();
        }
		return *this;
	}

	ptr<T>& operator=(T* obj) {
        if (object != obj) {
            SAFE_RELEASE(object);
		    object = obj;
        }
		return *this;
	}

    // Dynamic cast with references
    template <typename TT>
    ptr<TT> cast() const
    {
        TT* obj = dynamic_cast<TT*>(object);
        if (obj != NULL) {
            obj->AddRef();
            return ptr<TT>(obj);
        }
        return NULL;
    }

	// Constructors
	ptr(T* obj) { object = obj; }
    template <typename TT>
	ptr(const ptr<TT>& ptr) { object = ptr.object; if (object != NULL) object->AddRef(); }
	ptr(const ptr<T>& ptr)  { object = ptr.object; if (object != NULL) object->AddRef(); }
	ptr()  { object = NULL; }
	~ptr() { SAFE_RELEASE(object); }
};

/*
A safe release template
Releases the object and sets the pointer to NULL
*/
#ifndef SAFE_RELEASE
template <typename T>
static void SAFE_RELEASE(ptr<T> &p) {
    // For ptr<>s, the assigment will release
    // the held object, if any.
	p = NULL;
}

template <typename T>
static void SAFE_RELEASE(T* &ptr) {
	if (ptr != NULL) {
        // Normal pointer, release manually
		ptr->Release();
		ptr = NULL;
	}
}
#endif

#endif
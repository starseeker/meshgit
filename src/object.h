#ifndef _OBJECT_H_
#define _OBJECT_H_

struct object {
    virtual ~object() { }
};

template<typename T>
inline bool is(object* value) { return dynamic_cast<T*>(value) != 0; }

template <typename T>
inline T* cast(object* value) { return dynamic_cast<T*>(value); }

#endif

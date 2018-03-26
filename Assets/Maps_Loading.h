#ifndef ASSETS_MAPS_LOADING_H
#define ASSETS_MAPS_LOADING_H

//
// Some useful macros and functions for reading properties in mini-chunks
//
template <typename T, typename C>
static void READ_PROPERTY(C& var, ChunkReader& reader)
{
    var = static_cast<C>(TemplateReader<T>::read(reader));
}

// Specialize booleans to avoid "forcing X to bool" warning
template <typename T>
static void READ_PROPERTY(bool& var, ChunkReader& reader)
{
    var = (TemplateReader<T>::read(reader) != 0);
}

#define BEGIN_PROPERTIES(reader)                       \
    do {                                               \
        ChunkReader& __reader(reader);                 \
        ChunkType    __type;                           \
        while ((__type = __reader.nextMini()) != -1) { \
            switch (__type) {

#define DEFINE_PROPERTY(id, type, var)                        \
        case (id): READ_PROPERTY<type>(var, __reader); break; \

#define DEFINE_PROPERTY_BYTE(id, var)    DEFINE_PROPERTY(id, char,    var)
#define DEFINE_PROPERTY_BOOLEAN(id, var) DEFINE_PROPERTY(id, char,    var)
#define DEFINE_PROPERTY_INTEGER(id, var) DEFINE_PROPERTY(id, int,     var)
#define DEFINE_PROPERTY_STRING(id, var)  DEFINE_PROPERTY(id, string,  var)
#define DEFINE_PROPERTY_WSTRING(id, var) DEFINE_PROPERTY(id, wstring, var)
#define DEFINE_PROPERTY_FLOAT(id, var)   DEFINE_PROPERTY(id, float,   var)

#define END_PROPERTIES  \
            }           \
        }               \
    } while (false);

#endif
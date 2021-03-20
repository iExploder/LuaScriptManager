#ifndef NF_MACRO_DEFINES
#define NF_MACRO_DEFINES

#include "member_checker.h"

/*
* nfmacro.h
* (C) 2021 iExploder as A part of NextFramework project.
* Platform-specific definitions & workarounds.
*/

/* C++11 Availability Check */
#ifdef _MSC_VER 
#  if _MSC_VER < 1900
#    error "C++11 Features required. Use compiler supports C++11."
#  endif
#else
#  if __cplusplus < 201103L
#    error "C++11 Features required. Use compiler supports C++11."
#  endif
#endif

/* If available, use C++ char8_t(UTF-8) distinct type & std::u8string */
#if __cpp_lib_char8_t >= 201907L
#  define NF_CHAR8_T  char8_t
#  define NF_U8STRING ::std::u8string
#else
   /* Old C++ Standard fallback */
#  define NF_CHAR8_T  char
#  define NF_U8STRING ::std::string
#endif

/* If available, use std::map<...>::try_emplace() */
#if __cpp_lib_map_try_emplace >= 201411L
#  define NF_MAP_TRYEMPLACE(m, key, ...) \
     (m.try_emplace(key, __VA_ARGS__))
#else
   /* Old C++ Standard fallback */
#  define NF_MAP_TRYEMPLACE(m, key, ...) \
     ((m.find(key) != m.end()) ? m.find(key) : m.emplace(key, __VA_ARGS__) )
#endif

/* Check & Fallback for std::map<...>::contains() (C++20~) */
CREATE_MEMBER_CHECK(contains);

#define NF_MAP_CONTAINS(m,k) \
((has_member_contains<::std::map<int,int>>::value) ? (m.contains(k)) : (m.find(k) != m.end()))

/* ASSURE Packed struct & Force Inlining  */
#ifdef _MSC_VER
#   define NF_PACKED_STRUCT(name) \
        __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#   define NF_INLINE __forceinline
#   define NF_EXPORTCFUNC extern "C" __declspec(dllexport)
#elif defined(__GNUC__)
#   define NF_PACKED_STRUCT(name) struct __attribute__((packed)) name
#   define NF_INLINE __attribute__((always_inline))
#   define NF_EXPORTCFUNC extern "C" __attribute__((visibility("default")))
#else
#   error "Not Supported Platform"
#endif

/* Path Delimiter */
#ifdef _WIN32
#   define NF_PATH_DELIMITER '\\'
#else
#   define NF_PATH_DELIMITER '/'
#endif

/*
* Avoiding minwindef.h/windows.h macro defined min/max(overwrited std::*) functions.
*/
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/*
* Convenient MACRO for Runtime Exception Inheritance
*/

#define NF_RUNTIMEEXCEPTION(name)                                 \
class name : public ::std::runtime_error                          \
{                                                                 \
public:                                                           \
    name () : ::std::runtime_error("#name") {}                    \
    name (const ::std::string& msg) : ::std::runtime_error(msg){} \
}

#endif
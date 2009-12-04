/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <cstdarg>
#include <vector>

#include "APITypes.h"

// The problem with this function is that it doesn't allow 
// implicit typecasting, which makes this function far too
// cumbersome to use.  Thus, we fall back to this unfortunate
// hack. :-/ Fortunately, the linker will optimize out any of
// these that we don't use.

//template <class T>
//std::vector<T>& vector_list( unsigned int num, ... )
//{
//    std::vector<T> out(num);
//    va_list arguments;
//
//    va_start(arguments, num);
//    for (unsigned int i = 0; i < num; i++) {
//        T newVar = va_arg (arguments, T);
//        out.push_back( newVar );
//    }
//    va_end( arguments );
//
//    return out;
//}

#define ADD_VAR_TO_VECTOR(_vect, _var) _vect.push_back(_var);

template <class T>
std::vector<T> vector_list()
{
    std::vector<T> out;
    return out;
}

template <class T>
std::vector<T> vector_list( T v1 )
{
    std::vector<T> out;
    ADD_VAR_TO_VECTOR(out, v1);
    return out;
}

template <class T>
std::vector<T> vector_list( T v1, T v2 )
{
    std::vector<T> out;
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3 )
{
    std::vector<T> out(3);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3, T v4 )
{
    std::vector<T> out(4);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    ADD_VAR_TO_VECTOR(out, v4);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3, T v4, T v5 )
{
    std::vector<T> out(5);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    ADD_VAR_TO_VECTOR(out, v4);
    ADD_VAR_TO_VECTOR(out, v5);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3, T v4, T v5, T v6 )
{
    std::vector<T> out(6);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    ADD_VAR_TO_VECTOR(out, v4);
    ADD_VAR_TO_VECTOR(out, v5);
    ADD_VAR_TO_VECTOR(out, v6);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3, T v4, T v5, T v6, T v7 )
{
    std::vector<T> out(7);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    ADD_VAR_TO_VECTOR(out, v4);
    ADD_VAR_TO_VECTOR(out, v5);
    ADD_VAR_TO_VECTOR(out, v6);
    ADD_VAR_TO_VECTOR(out, v7);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8 )
{
    std::vector<T> out(8);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    ADD_VAR_TO_VECTOR(out, v4);
    ADD_VAR_TO_VECTOR(out, v5);
    ADD_VAR_TO_VECTOR(out, v6);
    ADD_VAR_TO_VECTOR(out, v7);
    ADD_VAR_TO_VECTOR(out, v8);
    return out;
}

template <class T>
std::vector<T>& vector_list( T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9 )
{
    std::vector<T> out(9);
    ADD_VAR_TO_VECTOR(out, v1);
    ADD_VAR_TO_VECTOR(out, v2);
    ADD_VAR_TO_VECTOR(out, v3);
    ADD_VAR_TO_VECTOR(out, v4);
    ADD_VAR_TO_VECTOR(out, v5);
    ADD_VAR_TO_VECTOR(out, v6);
    ADD_VAR_TO_VECTOR(out, v7);
    ADD_VAR_TO_VECTOR(out, v8);
    ADD_VAR_TO_VECTOR(out, v9);
    return out;
}
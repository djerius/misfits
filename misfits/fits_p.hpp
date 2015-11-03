#ifndef misFITS_P_H
#define misFITS_P_H

#include <boost/preprocessor/seq.hpp>

#define misFITS_STORAGE_TYPES (double)(float)(int)(long)(short)(unsigned char)(unsigned int)(unsigned long)(unsigned short)

#define misFITS_INSTANTIATE_OVER_STORAGE_TYPES(EXPR)  BOOST_PP_SEQ_FOR_EACH(EXPR, ~, misFITS_STORAGE_TYPES )

#endif // ! misFITS_P_H

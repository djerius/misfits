#ifndef misFITS_P_H
#define misFITS_P_H

#include <boost/preprocessor/seq.hpp>

#define misFITS_STORAGE_TYPES (double)(float)(int)(unsigned int)(long)(unsigned long)(short)(unsigned short)(char)(unsigned char)(LONGLONG)

#define misFITS_INSTANTIATE_OVER_STORAGE_TYPES(EXPR)  BOOST_PP_SEQ_FOR_EACH(EXPR, ~, misFITS_STORAGE_TYPES )

namespace misFITS {

    struct resetHDU {

	FilePtr fp;
	int chdu;

	resetHDU ( FilePtr fp_, int new_hdu_num = 0) : fp( fp_), chdu ( fp_->hdu_num() ) {

	    if ( new_hdu_num )
		fp->move_to( new_hdu_num );

	}
	~resetHDU () { fp->move_to( chdu ); }
    };

}


#endif // ! misFITS_P_H

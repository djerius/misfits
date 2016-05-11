#ifndef misFITS_P_H
#define misFITS_P_H

#include <boost/preprocessor/seq.hpp>

#include "table.hpp"

#define misFITS_STORAGE_TYPES (double)(float)(int)(unsigned int)(long)(unsigned long)(short)(unsigned short)(char)(unsigned char)(LONGLONG)

#define misFITS_INSTANTIATE_OVER_STORAGE_TYPES(EXPR)  BOOST_PP_SEQ_FOR_EACH(EXPR, ~, misFITS_STORAGE_TYPES )

namespace misFITS {

    class resetHDU {

    public:
	FilePtr fp;
	int chdu;

	resetHDU ( FilePtr fp_, int new_hdu_num = 0) : fp( fp_), chdu ( fp->hdu_num() ) {

	    if ( new_hdu_num )
		fp->move_to( new_hdu_num );

	}

	resetHDU ( const HDU& hdu ) : fp( hdu.file_ ), chdu ( fp->hdu_num() ) {

	    fp->move_to( hdu.hdu_num_ );
	}

	resetHDU( const HDUPtr& hdu ) : fp( hdu->file_ ), chdu ( fp->hdu_num() ) {

	    fp->move_to( hdu->hdu_num_ );

	}

	resetHDU( const TablePtr& tbl ) : fp( tbl->file_ ), chdu ( fp->hdu_num() ) {

	    fp->move_to( tbl->hdu_num_ );

	}

	~resetHDU () { fp->move_to( chdu ); }

    private:
	resetHDU( const resetHDU& );
	resetHDU& operator=( const resetHDU& );

    };

}


#endif // ! misFITS_P_H

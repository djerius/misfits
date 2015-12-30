#include "fiducial.hpp"

#include <functional>

using namespace std;

namespace misFITS_Test {

    namespace Fiducial {

	template<>
	void
	Column< TSTRING, std::string >::write( TestFitsPtr& fp ) const {

	    // write data exactly as specified; don't let CFITSIO
	    // think it's a NULL terminated string

	    for( int row = 1 ; row <= data.size()  ; ++row ) {

		const ColumnT& str = data[row-1];

		if ( str.size() != nbytes )
		    throw misFITS::Exception::Assert( "string not equal to column width" );

		misFITS_CHECK_CFITSIO_EXPR
		    (
		     fits_write_tblbytes( fp.get(),
					  row, offset, nbytes,
					  reinterpret_cast<unsigned char*>(const_cast<char*>(str.data())),
					  &status )
		 );

	    }

	}

	static void
	normalize_str( std::string& str ) {

	    std::string::size_type nchar = str.find_first_of( '\0' );
	    if ( nchar != std::string::npos )
		str.resize( nchar );
	}


	template<>
	void
	Column< TSTRING, std::string>::normalize () {

	    for_each( data.begin(), data.end(), normalize_str );

	}


	template<>
	void
	Column< TSTRING, std::vector<std::string> >::write( TestFitsPtr& fp ) const {

	    // write data exactly as specified; don't let CFITSIO
	    // think it's a NULL terminated string. Note that
	    // this code assumes that the strings

	    for( int row = 1 ; row <= data.size()  ; ++row ) {

		const vector<ColumnT>& vs = data[row-1];

		LONGLONG toffset = offset;

		LONGLONG tnbytes = nbytes / vs.size();


		vector<ColumnT>::const_iterator str = vs.begin();
		vector<ColumnT>::const_iterator end  = vs.end();
		for ( ; str < end ; ++str, toffset += tnbytes ) {

		    if ( str->size() != tnbytes )
			throw misFITS::Exception::Assert( "sub string not equal to string width" );

		    misFITS_CHECK_CFITSIO_EXPR
			(
			 fits_write_tblbytes( fp.get(), row,
					      toffset, tnbytes,
					      reinterpret_cast<unsigned char*>(const_cast<char*>(str->data())),
					      &status )
			 );

		}
	    }
	}

	template<>
	void
	Column< TSTRING, std::vector<std::string> >::normalize( ) {

	    VectorT::iterator row( data.begin() );
	    VectorT::iterator end( data.end() );

	    for( ; row < end ; ++row ) {

		vector<ColumnT>& vs = *row;

		for_each( vs.begin(), vs.end(), normalize_str );
	    }
	}


	void
	ColumnBase::insert ( TestFitsPtr& fpp, int offset_ ) const {

	    offset = offset_;

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_get_num_cols( fpp.get() , &colnum, &status )
		 );

	    colnum++;

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_insert_col( fpp.get(), colnum,
				  const_cast<char*>( ttype.c_str() ),
				  const_cast<char*>( tform.c_str() ),
				  &status )
		 );


	    // stolen from misFITS::ColumnInfo::init

	    LONGLONG width;

#if SIZEOF_LONG == SIZEOF_LONGLONG
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_get_coltypell( fpp.get(),
				      colnum,
				      &column_type,
				      &repeat,
				      reinterpret_cast<LONGLONG*>(&width),
				      &status )
		  );
#else
	    // fits_get_coltypell is anomolous in requiring a LONGLONG
	    // width. c.f. CFITSIO internals and other routines with a width parameter
	    LONGLONG twidth;
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_get_coltypell( fpp.get(),
				      colnum,
				      &column_type,
				      &repeat,
				      &twidth,
				      &status )
		  );
	    width = twidth;
#endif // LONGLONG FUDGE

	    if ( TSTRING == column_type )
		width = 1;

	    // the column width for TBIT is reported as 1 (byte), which is
	    // not useful in calculating the true number of bytes in the
	    // column
	    if ( TBIT == column_type ) {
		nbytes = repeat / 8;
		if ( nbytes * 8  < repeat ) nbytes += 1;
		nelem = nbytes;
	    }
	    else {
		nbytes = repeat * width;
		nelem = repeat;
	    }

	}
    }

}

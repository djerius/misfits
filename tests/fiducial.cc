#include "misfits/types.hpp"

#include "fiducial.hpp"

#include <functional>

using namespace std;

namespace misFITS_Test {

    namespace Fiducial {

	// ------------------------------------------------------- //

	template<>
	void
	Column< TSTRING, std::string >::write( TestFitsPtr& fp ) const {

	    // write data exactly as specified; don't let CFITSIO
	    // think it's a NULL terminated string

	    for( data_size_type row = 1 ; row <= data.size()  ; ++row ) {

		const ColumnT& str = data[row-1];

		if ( str.size() != nbytes )
		    throw misFITS::Exception::Assert( "string not equal to column width" );

		misFITS_CHECK_CFITSIO_EXPR
		    (
		     fits_write_tblbytes( fp.get(),
					  static_cast<LONGLONG>( row ),
					  offset,
					  static_cast<LONGLONG>( nbytes ),
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

	    for( data_size_type row = 1 ; row <= data.size()  ; ++row ) {

		const vector<ColumnT>& vs = data[row-1];

		LONGLONG toffset = offset;

		std::size_t tnbytes = nbytes / vs.size();


		vector<ColumnT>::const_iterator str = vs.begin();
		vector<ColumnT>::const_iterator end  = vs.end();
		for ( ; str < end ; ++str, toffset += tnbytes ) {

		    if ( str->size() != tnbytes )
			throw misFITS::Exception::Assert( "sub string not equal to string width" );

		    misFITS_CHECK_CFITSIO_EXPR
			(
			 fits_write_tblbytes( fp.get(), static_cast<LONGLONG>(row),
					      toffset,
					      static_cast<LONGLONG>(tnbytes),
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

	// ------------------------------------------------------- //


	template<>
	void
	Column< TLOGICAL, bool >::write( TestFitsPtr& fp ) const {

	    std::vector<misFITS::NativeType<misFITS::SC_BYTE>::storage_type> buffer( nelem );

	    if ( nelem > 1 )
		throw misFITS::Exception::Assert( "currently cannot model bool arrays" );
	    for( data_size_type row = 1 ; row <= data.size()  ; ++row ) {

		buffer[0] = data[row-1];

		misFITS_CHECK_CFITSIO_EXPR
		    (
		     fits_write_col( fp.get(), TLOGICAL,
				     static_cast<int>(colnum),
				     static_cast<LONGLONG>(row),
				     1,
				     static_cast<LONGLONG>(nelem),
				     &buffer[0], &status );
		     );

	    }

	}


	template<>
	void
	Column< TLOGICAL, std::vector<bool> >::write( TestFitsPtr& fp ) const {

	    std::vector<misFITS::NativeType<misFITS::SC_BYTE>::storage_type> buffer( nelem );

	    for (Parent::data_size_type row = 1 ; row <= Parent::data.size() ; ++row ) {

		const std::vector<bool>& drow = Parent::data[row-1];

		if ( drow.size() != Parent::nelem )
		    throw misFITS::Exception::Assert( "data for vector cell has incorrect length" );

		for ( size_t idx = 0 ; idx < nelem ; idx++ )
		    buffer[idx] = drow[idx];

		misFITS_CHECK_CFITSIO_EXPR
		    (
		     fits_write_col( fp.get(), TLOGICAL, 
				     static_cast<int>(Parent::colnum),
				     static_cast<LONGLONG>(row),
				     1,
				     static_cast<LONGLONG>(Parent::nelem),
				     &buffer[0], &status )
		     );

		}

	}


	// ------------------------------------------------------- //

	void
	ColumnBase::insert ( TestFitsPtr& fpp, int offset_ ) const {

	    offset = offset_;

	    int colnum_;
	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_get_num_cols( fpp.get() , &colnum_, &status )
		 );

	    colnum = static_cast<std::size_t>(colnum_) + 1;

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_insert_col( fpp.get(), 
				  static_cast<int>(colnum),
				  const_cast<char*>( ttype.c_str() ),
				  const_cast<char*>( tform.c_str() ),
				  &status )
		 );


	    // stolen from misFITS::ColumnInfo::init

	    size_t width;

	    LONGLONG width_;
	    LONGLONG repeat_;
	    misFITS_CHECK_CFITSIO_EXPR
		( fits_get_coltypell( fpp.get(),
				      static_cast<int>(colnum),
				      &column_type,
				      &repeat_,
				      &width_,
				      &status )
		  );
	    repeat = static_cast<size_t>( repeat_ );
	    width = static_cast<size_t>(width_);

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

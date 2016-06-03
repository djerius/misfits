// --8<--8<--8<--8<--
//
// Copyright (C) 2015 Smithsonian Astrophysical Observatory
//
// This file is part of misfits
//
// misfits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -->8-->8-->8-->8--

#include <string>
#include <iostream>

#include <boost/core/null_deleter.hpp>
#include <boost/preprocessor/seq/elem.hpp>

#include <fitsio.h>

#include <misfits/hdu.hpp>

#include "fits_p.hpp"

namespace misFITS {

    // Keywords

    std::pair<int,int>
    HDU::get_hdrpos()  {

	set_as_chdu();

	int keysexist, keynum;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_hdrpos( file_->fptr(), &keysexist, &keynum, &status )
	      );

	return std::make_pair( keysexist, keynum );

    }

    // this is the least computationally burdensome thing I could come
    // up with.
    bool HDU::has_keyword( const std::string& keyname ) const {

	char card[FLEN_CARD];

	int status = 0;

	fits_read_card( file_->fptr(), keyname.c_str(), card, &status );

	switch ( status ) {

	case 0 :
	    return true;

	case VALUE_UNDEFINED:
	    return true;

	case KEY_NO_EXIST:
	    return false;


	default:
	    throw Exception::CFITSIO( status );

	}

    }



    //-----------------------------------------

    template< typename T>
    static Keyword<T>
    make_keyword( const std::string& keyname,
		  const T& value,
		  const T& default_value,
		  const std::string& comment,
		  int status ) {

	bool defined = status == 0;
	return Keyword<T>( keyname,
			   defined ? value : default_value,
			   comment,
			   defined
			   );

    }

#define RETURN_KEYWORD( T, expr )					\
    do {								\
	int status = 0;							\
	expr;								\
	if (    status							\
             && status != VALUE_UNDEFINED				\
	     && status != KEY_NO_EXIST )				\
	    throw Exception::CFITSIO (status );				\
	return make_keyword<T>( keyname, value,				\
				default_value, comment, status );	\
    } while (0)

    //-----------------------------------------

    Keyword<std::string>
    HDU::read_keyn( int keynum, const std::string& default_value ) const {

	set_as_chdu();

       char keyname[FLEN_KEYWORD+1] = { '\0' };
       char   value[FLEN_VALUE+1]   = { '\0' };
       char comment[FLEN_COMMENT+1] = { '\0' };

       RETURN_KEYWORD
           ( std::string,
             fits_read_keyn( file_->fptr(), keynum,
                             keyname, value, comment, &status )
             );

    }

    //-----------------------------------------

    template<>
    Keyword<std::string>
    HDU::get_keyword( const std::string& keyname, const std::string& default_value ) const {

	set_as_chdu();

    	std::string value;
    	char comment[FLEN_COMMENT+1] = { '\0' };

    	int status = 0;
    	int length;

	fits_get_key_strlen( file_->fptr(), keyname.c_str(), &length, &status );

    	if ( status ) {

    	    if (    status != VALUE_UNDEFINED
    		 && status != KEY_NO_EXIST )
    	    throw Exception::CFITSIO (status );

    	    return make_keyword<std::string>( keyname, value,
    					      default_value, comment, status );
    	}


    	std::vector<char> v_value( length );

    	RETURN_KEYWORD
    	    ( std::string,
    	      fits_read_string_key( file_->fptr(),
				    keyname.c_str(),
				    1, length,
				    &v_value[0],
				    NULL,
				    comment,
				    &status );
    	      value.assign( &v_value[0], length );
    	      );

    }

    template<typename T>
    Keyword<T> HDU::get_keyword( const std::string& keyname, const T& default_value ) const {

	set_as_chdu();

    	T value = default_value;
    	char comment[FLEN_COMMENT+1] = { '\0' };

    	RETURN_KEYWORD
    	    ( T,
    	      fits_read_key( file_->fptr(),
    			     StorageCode<T>::type,
    			     keyname.c_str(),
    			     &value,
    			     comment,
    			     &status )
    	      );
    }


#define GET_KEYWORD(r,d,T) \
    template Keyword<T> HDU::get_keyword<T>( const std::string& keyname, const T& default_value ) const;


    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(GET_KEYWORD)

    //-----------------------------------------

    template<>
    void HDU::set_keyword<std::string>( const Keyword<std::string>& kw ) const {

	set_as_chdu();

	if ( kw.value.size() > FLEN_VALUE ) {

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_write_key_longwarn( file_->fptr(), &status );
		 fits_update_key_longstr( file_->fptr(),
					 kw.keyname.c_str(),
					 const_cast<char*>( kw.value.c_str() ),
					 kw.comment.c_str(),
					 &status );
		 );

	}

	else {

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_update_key_str( file_->fptr(),
				      kw.keyname.c_str(),
				      const_cast<char *>(kw.value.c_str()),
				      kw.comment.c_str(),
				      &status
				      );
		 );

	}

    }

    template<typename T>
    void HDU::set_keyword( const Keyword<T>& kw ) const {

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_update_key( file_->fptr(),
			     StorageCode<T>::type,
			     kw.keyname.c_str(),
			     const_cast<T*>(&kw.value),
			     kw.comment.c_str(),
			     &status
			     )
	     );

    }


#define SET_KEYWORD(r,d,T) \
    template void HDU::set_keyword<T>( const Keyword<T>& kw ) const;


    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(SET_KEYWORD)

    //-----------------------------------------

    void HDU::delete_keyword( const std::string& keyname ) const {

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_delete_key( file_->fptr(), keyname.c_str(), &status );
	     );

    }

    //-----------------------------------------

    void HDU::add_history( const std::string& history ) const {

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_history( file_->fptr(),
				 const_cast<char*>( history.c_str() ),
				 &status
				 )
	     );

    }

    void HDU::add_comment( const std::string& comment ) const {

	set_as_chdu();

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_comment( file_->fptr(),
				 const_cast<char*>( comment.c_str() ),
				 &status
				 )
	     );

    }

    //-----------------------------------------

    HDU::HDU( WeakFilePtr& file, int hdu_num ) : hdu_num_( hdu_num ) {

	file_.set<own_or_observe::observed>( file );
	if ( 0 == hdu_num_ )
	    hdu_num_ = file_->hdu_num();

	refresh();
    }


    HDU::HDU( SharedFilePtr& file, int hdu_num ) : hdu_num_( hdu_num ) {

    	file_.set<own_or_observe::observed>( file );
    	if ( 0 == hdu_num_ )
    	    hdu_num_ = file_->hdu_num();
    	refresh();
    }

    HDU::HDU( WeakFilePtr& file, const std::string& extname, int extver ) : extname_( extname ), extver_( extver ) {

	file_.set<own_or_observe::observed>( file );

	{
	    file_->move_to( extname, extver );
	    hdu_num_ = file_->hdu_num();
	}

	refresh();
    }

    HDU::HDU( SharedFilePtr& file, const std::string& extname, int extver ) : extname_( extname ), extver_( extver ) {

	file_.set<own_or_observe::observed>( file );

	{
	    file_->move_to( extname, extver );
	    hdu_num_ = file_->hdu_num();
	}

	refresh();
    }

    HDU::HDU( ) {

	FilePtr fp( open<Entity::File,  Mode::Create>( "mem://" ) );
	file_.set<own_or_observe::owned>( fp );

	hdu_num_ = 1;
    }

    void HDU::refresh() {

	set_as_chdu();

	extname_ = get_keyword<std::string>( "EXTNAME", "" ).value;
	extver_  = get_keyword<int>( "EXTVER", 1 ).value;
    }

    void HDU::set_as_chdu () const {
	file_->move_to( hdu_num_ );
    }

    struct keyword_output {

	std::ostream& os;

	keyword_output( std::ostream& os_ ) : os( os_ ) {}

	void operator() ( const Keyword<std::string>& kw ) {
	    os << kw.keyname << " = " << kw.value << "\n";

	}
    };

    void HDU::dump_keywords( std::ostream& os, int keynum ) {

	shared_ptr<HDU> hdp =  get_shared_ptr();
	KeywordIterator start( hdp, keynum );
	KeywordIterator end( start.end() );

	for_each( start, end, keyword_output(os) );
    }


    void HDU::dump_keywords(  ) {
	dump_keywords( std::cerr, 1 );
    }



}

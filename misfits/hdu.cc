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
// includes final \0
#define FITSCARDLEN 81

#include <misfits/hdu.hpp>

#include "fits_p.hpp"

namespace misFITS {

    // Keywords

    std::pair<int,int>
    HDU::get_hdrpos()  {

	resetHDU chdu( *this );

	int keysexist, keynum;
	misFITS_CHECK_CFITSIO_EXPR
	    ( fits_get_hdrpos( file_->fptr(), &keysexist, &keynum, &status )
	      );

	return std::make_pair( keysexist, keynum );

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

    Keyword<std::string>
    HDU::read_keyword( const std::string& keyname, const std::string& default_value ) const {

	resetHDU chdu( *this );

	char value[FITSCARDLEN] = { '\0' };
	char comment[FITSCARDLEN] = { '\0' };

	RETURN_KEYWORD
	    ( std::string,
	      fits_read_keyword( file_->fptr(), keyname.c_str(),
				 value, comment, &status )
	      );
    }

    //-----------------------------------------

    Keyword<std::string>
    HDU::read_keyn( int keynum, const std::string& default_value ) const {

	resetHDU chdu( *this );

	char keyname[FITSCARDLEN] = { '\0' };
	char   value[FITSCARDLEN] = { '\0' };
	char comment[FITSCARDLEN] = { '\0' };

	RETURN_KEYWORD
	    ( std::string,
	      fits_read_keyn( file_->fptr(), keynum,
			      keyname, value, comment, &status )
	      );

    }

    //-----------------------------------------

    template<>
    Keyword<std::string>
    HDU::read_key( const std::string& keyname, const std::string& default_value ) const {

	resetHDU chdu( *this );

	char   value[FITSCARDLEN] = { '\0' };
	char comment[FITSCARDLEN] = { '\0' };

	RETURN_KEYWORD
	    ( std::string,
	      fits_read_key( file_->fptr(), SC_STRING,
			     keyname.c_str(),
			     &value, comment, &status )
	      );

    }

    template<typename T>
    Keyword<T> HDU::read_key( const std::string& keyname, const T& default_value ) const {

	resetHDU chdu( *this );

	T value = default_value;
	char comment[FITSCARDLEN] = { '\0' };

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


#define READ_KEY(r,d,T) \
    template Keyword<T> HDU::read_key<T>( const std::string& keyname, const T& default_value ) const;


    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(READ_KEY)

    //-----------------------------------------

    template<>
    void HDU::write_key<std::string>( const Keyword<std::string>& kw ) const {

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_key( file_->fptr(),
			     StorageCode<std::string>::type,
			     kw.keyname.c_str(),
			     const_cast<char *>(kw.value.c_str()),
			     kw.comment.c_str(),
			     &status
			     )
	     );
    }

    template<typename T>
    void HDU::write_key( const Keyword<T>& kw ) const {

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_key( file_->fptr(),
			     StorageCode<T>::type,
			     kw.keyname.c_str(),
			     const_cast<T*>(&kw.value),
			     kw.comment.c_str(),
			     &status
			     )
	     );

    }


#define WRITE_KEY(r,d,T) \
    template void HDU::write_key<T>( const Keyword<T>& kw ) const;


    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(WRITE_KEY)

    //-----------------------------------------

    template<>
    void HDU::update_key<std::string>( const Keyword<std::string>& kw ) const {

	resetHDU chdu( *this );

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_update_key( file_->fptr(),
			      StorageCode<std::string>::type,
			      kw.keyname.c_str(),
			      const_cast<char *>(kw.value.c_str()),
			      kw.comment.c_str(),
			      &status
			     )
	     );

    }


    template<typename T>
    void HDU::update_key( const Keyword<T>& kw ) const {

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


#define UPDATE_KEY(r,d,T) \
    template void HDU::update_key<T>( const Keyword<T>& kw ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(UPDATE_KEY)


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
	    resetHDU chdu( file_ );
	    file_->move_to( extname, extver );
	    hdu_num_ = file_->hdu_num();
	}

	refresh();
    }

    HDU::HDU( SharedFilePtr& file, const std::string& extname, int extver ) : extname_( extname ), extver_( extver ) {

	file_.set<own_or_observe::observed>( file );

	{
	    resetHDU chdu( file_ );
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

	resetHDU chdu( *this );

	extname_ = read_key<std::string>( "EXTNAME", "" ).value;
	extver_  = read_key<int>( "EXTVER", 1 ).value;
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

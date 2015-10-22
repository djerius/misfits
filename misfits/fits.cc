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

#include <cfitsio/fitsio.h>

#include <misfits/fits.hpp>
#include <misfits/table.hpp>

using namespace std;

// includes final \0
#define FITSCARDLEN 81

namespace misFITS {

    ///////////////////////
    // File constructors //
    ///////////////////////

    // destructor for shared_ptr class. should probably not throw...
    static void
    closeFitsPtr( fitsfile* fitsptr ) {
	misFITS_CHECK_CFITSIO_EXPR( fits_close_file( fitsptr, &status ) );
    }

    File::FitsPtr
    File::FitsPtr_( fitsfile* fitsptr ) {
	return File::FitsPtr( fitsptr, closeFitsPtr );
    }

    // default: open at first data HDU in read only mode
    File::File( const std::string& file ) : file( file ), fitsptr( FitsPtr_( NULL ) ) {
	fitsfile* fptr = Open::open<Entity::Data,Mode::ReadOnly>( file.c_str() );
	fitsptr = FitsPtr_( fptr  );
    }


    File::File( const std::string& file_, fitsfile* fitsfile_ ) :
	file(file_), fitsptr( FitsPtr_( fitsfile_ ) ) {};


    void
    File::reopen( const File& ofile ) {
	fitsfile *fptr;

	misFITS_CHECK_CFITSIO_EXPR( fits_reopen_file( ofile.fptr(), &fptr, &status ) );

	file = ofile.file;
	fitsptr = FitsPtr_( fptr  );

	movabs_hdu( ofile.hdu_num() );
    }

    File::File( const File& ofile ) : fitsptr( FitsPtr_(NULL) ) {

	reopen( ofile );
    }

    File&
    File::operator = ( const File& ofile ) {

	reopen( ofile );

	return *this;
    }



    //////////////////////////////////////////////////
    // Infrastructure for open and open_p factories //
    //////////////////////////////////////////////////


    // map open<Entity::XXX> to fits_open_XXX and fits_create_XXX
    namespace MapCFITSIO {

	template<>
	int open<Entity::File>( fitsfile** fptr, const std::string& file, int mode, int* status ) {
	    return fits_open_file( fptr, file.c_str(), mode, status );
	}

	template<>
	int create<Entity::File>( fitsfile** fptr, const std::string& file, int* status ) {
	    return fits_create_file( fptr, file.c_str(), status );
	}

	template<>
	int open<Entity::DiskFile>( fitsfile** fptr, const std::string& file, int mode, int* status ) {
	    return fits_open_diskfile( fptr, file.c_str(), mode, status );
	}

	template<>
	int create<Entity::DiskFile>( fitsfile** fptr, const std::string& file, int* status ) {
	    return fits_create_diskfile( fptr, file.c_str(), status );
	}

	template<>
	int open<Entity::Data>( fitsfile** fptr, const std::string& file, int mode, int* status ) {
 	    return fits_open_data( fptr, file.c_str(), mode, status );
	}

	template<>
	int open<Entity::Table>( fitsfile** fptr, const std::string& file, int mode, int* status ) {
	    return fits_open_table( fptr, file.c_str(), mode, status );
 	}

	template<>
	int open<Entity::Image>( fitsfile** fptr, const std::string& file, int mode, int* status ) {
	    return fits_open_image( fptr, file.c_str(), mode, status );
	}
    }


    // map open<Entity,Mode> to call to MapCFITSIO<Entity::open( ... M::mode() )
    namespace Open {

	template< class Entity, class Mode >
	fitsfile* open( const std::string& file ) {
	    fitsfile* fptr;
	    misFITS_CHECK_CFITSIO_EXPR( MapCFITSIO::open<Entity>( &fptr, file, Mode::mode(), &status ) );
	    return fptr;
	}

	template<>
	fitsfile* open<Entity::File,Mode::Create> ( const std::string& file ) {
	    fitsfile* fptr;
	    misFITS_CHECK_CFITSIO_EXPR( MapCFITSIO::create<Entity::File>( &fptr, file, &status ) );
	    return fptr;
	}

	template<>
	fitsfile* open<Entity::DiskFile,Mode::Create>( const std::string& file ) {
	    fitsfile* fptr;
	    misFITS_CHECK_CFITSIO_EXPR( MapCFITSIO::create<Entity::DiskFile>( &fptr, file, &status ) );
	    return fptr;
	}

	template<>
	fitsfile* open<Entity::File,Mode::CreateOverWrite>( const std::string& file ) {
	    string ofile( string("!") + file );
	    return open<Entity::File,Mode::Create>( ofile );
	}

	template<>
	fitsfile* open<Entity::DiskFile,Mode::CreateOverWrite>( const std::string& file ) {
	    string ofile( string("!") + file );
	    return open<Entity::DiskFile,Mode::Create>( ofile );
	}

    }

    //////////////////////////////////////////////////////////
    // Instantiate the factories which return File or File* //
    //////////////////////////////////////////////////////////

    template <class Entity, class Mode>
	FilePtr open( const std::string&file )  {

	return FilePtr( new File(file, Open::open<Entity,Mode>( file ) ) );
    }

    template <>  FilePtr open<Entity::Memory>( ) {
	return open<Entity::File,Mode::Create>( "mem://" );
    };


    template FilePtr open<Entity::File, Mode::ReadOnly>( const std::string& file );
    template FilePtr open<Entity::File, Mode::ReadWrite>( const std::string& file );
    template FilePtr open<Entity::File, Mode::Create>( const std::string& file );
    template FilePtr open<Entity::File, Mode::CreateOverWrite>( const std::string& file );

    template FilePtr open<Entity::DiskFile, Mode::ReadOnly>( const std::string& file );
    template FilePtr open<Entity::DiskFile, Mode::ReadWrite>( const std::string& file );
    template FilePtr open<Entity::DiskFile, Mode::Create>( const std::string& file );
    template FilePtr open<Entity::DiskFile, Mode::CreateOverWrite>( const std::string& file );

    template FilePtr open<Entity::Data, Mode::ReadOnly>( const std::string& file );
    template FilePtr open<Entity::Data, Mode::ReadWrite>( const std::string& file );

    template FilePtr open<Entity::Table, Mode::ReadOnly>( const std::string& file );
    template FilePtr open<Entity::Table, Mode::ReadWrite>( const std::string& file );

    template FilePtr open<Entity::Image, Mode::ReadOnly>( const std::string& file );
    template FilePtr open<Entity::Image, Mode::ReadWrite>( const std::string& file );
    template FilePtr open<Entity::Memory>() ;


    ///////////////////
    // Table Support //
    ///////////////////


    TablePtr File::add( const Table& in ) {

	int in_chdu = in.file()->hdu_num();

	try {
	    in.file()->movabs_hdu( in.hdu_num );

	    SharedFilePtr fptr( in.file() );
	    copy_hdu( fptr );
	} catch ( ... ) {

	    in.file()->movabs_hdu( in_chdu );

	    throw;

	}

	in.file()->movabs_hdu( in_chdu );

	return table();
    }


    TablePtr File::table( ) {

#ifdef HAVE_STD__WEAK_FROM_THIS

	WeakFilePtr wp( weak_from_this() );

#else

	WeakFilePtr wp( shared_from_this());

#endif

	return TablePtr( wp.expired() ? new Table( *this ) : new Table( wp ) );
    }


    //////////////////////////////////////
    // Wrappers around CFITSIO routines //
    //////////////////////////////////////


    HDU_Type
    File::movabs_hdu( int hdu_num ) const {
	int hdu_type;
	misFITS_CHECK_CFITSIO_EXPR( fits_movabs_hdu( fptr(), hdu_num, &hdu_type, &status ) );
	return static_cast<HDU_Type>(hdu_type);
    }

    HDU_Type
    File::movrel_hdu( int nmove ) const {
	int hdu_type;
	misFITS_CHECK_CFITSIO_EXPR( fits_movrel_hdu( fptr(), nmove, &hdu_type, &status ) );
	return static_cast<HDU_Type>(hdu_type);
    }

    void
    File::movnam_hdu( HDU_Type hdu_type, const std::string& extname, int extver) const {
	misFITS_CHECK_CFITSIO_EXPR( fits_movnam_hdu( fptr(), hdu_type, const_cast<char*>(extname.c_str()), extver, &status ) );
    }

    int
    File::get_num_hdus() const {
	int hdu_nums;
	misFITS_CHECK_CFITSIO_EXPR( fits_get_num_hdus( fptr(), &hdu_nums, &status ) );
	return hdu_nums;
    }

    int
    File::hdu_num() const {

	int hdunum;
	return fits_get_hdu_num( fptr(), &hdunum );

    }

    HDU_Type
    File::hdu_type ()const {
	int hdu_type;
	misFITS_CHECK_CFITSIO_EXPR( fits_get_hdu_type( fptr(), &hdu_type, &status ) );
	return static_cast<HDU_Type>(hdu_type);
    }

    void
    File::copy_file ( FilePtr& outfile, int previous, int current, int following ) const {
	misFITS_CHECK_CFITSIO_EXPR( fits_copy_file( fptr(), outfile->fptr(), previous, current, following, &status ) );

    }

    void
    File::copy_hdu ( FilePtr& outfile, int morekeys ) const {
	misFITS_CHECK_CFITSIO_EXPR( fits_copy_hdu( fptr(), outfile->fptr(), morekeys, &status ) );
    }

    void
    File::copy_header ( FilePtr& outfile ) const {
	misFITS_CHECK_CFITSIO_EXPR( fits_copy_header( fptr(), outfile->fptr(), &status ) );
    }

    HDU_Type
    File::delete_hdu ( ) const {
	int hdutype;
	misFITS_CHECK_CFITSIO_EXPR( fits_delete_hdu( fptr(), &hdutype, &status ) );
	return static_cast<HDU_Type>(hdutype);
    }

    Keyword<string>
    File::read_keyword( const string& keyname, const string& default_value ) const {

	char value[FITSCARDLEN] = { '\0' };
	char comment[FITSCARDLEN] = { '\0' };

	int status = 0;

	if ( fits_read_keyword( fptr(), keyname.c_str(),
				value, comment, &status )
	     && status != VALUE_UNDEFINED )
	    throw Exception::CFITSIO( status );

	return Keyword<string>( keyname,
				status == VALUE_UNDEFINED ? default_value : value,
				comment );
    }

    template<>
    Keyword<string> File::read_key( const string& keyname, const string& default_value ) const {

	char value[FITSCARDLEN] = { '\0' };
	char comment[FITSCARDLEN] = { '\0' };

	int status = 0;

	if ( fits_read_key( fptr(),
			    SC_STRING,
			    keyname.c_str(),
			    &value,
			    comment,
			    &status )
	     && status != VALUE_UNDEFINED )
	    throw Exception::CFITSIO( status );

	return Keyword<string>( keyname,
				status == VALUE_UNDEFINED ? default_value : value,
				comment );
    }

    template<typename T>
    Keyword<T> File::read_key( const std::string& keyname, const T& default_value ) const {

	T value = default_value;
	char comment[FITSCARDLEN] = { '\0' };

	int status = 0;

	if ( fits_read_key( fptr(),
			    StorageCode<T>::type,
			    keyname.c_str(),
			    &value,
			    comment,
			    &status )
	     && status != VALUE_UNDEFINED )
	    throw Exception::CFITSIO( status );

	return Keyword<T>( keyname, value, comment );
    }


}

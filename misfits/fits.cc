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
#include <misfits/fits_p.hpp>
#include <misfits/table.hpp>

#include <boost/preprocessor/seq/elem.hpp>

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
    File::File( const std::string& file ) : fitsptr( FitsPtr_( NULL ) ), file( file ) {
	fitsfile* fptr = Open::open<Entity::Data,Mode::ReadOnly>( file.c_str() );
	fitsptr = FitsPtr_( fptr  );
    }


    File::File( const std::string& file_, fitsfile* fitsfile_ ) :
	fitsptr( FitsPtr_( fitsfile_ ) ), file(file_) {}


    void
    File::reopen( const File& ofile ) {
	fitsfile *fptr;

	misFITS_CHECK_CFITSIO_EXPR( fits_reopen_file( ofile.fptr(), &fptr, &status ) );

	file = ofile.file;
	fitsptr = FitsPtr_( fptr  );

	movabs_hdu( ofile.hdu_num() );
    }

    File::File( const File& ofile ) : enable_shared_from_this<File>(), fitsptr( FitsPtr_(NULL) ) {
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

	template< Entity::Type Entity, class Mode >
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

    template <Entity::Type Entity, class Mode>
    FilePtr open( const std::string&file )  {

	return FilePtr( new File(file, Open::open<Entity,Mode>( file ) ) );
    }

    template <>  FilePtr open<Entity::Memory>( ) {
	return open<Entity::File,Mode::Create>( "mem://" );
    }


#define ENTITY_SEQ (File)(DiskFile)
#define MODE_SEQ   (ReadOnly)(ReadWrite)(Create)(CreateOverWrite)

#define OPEN_FITS(r,seq) \
    template FilePtr open<Entity::BOOST_PP_SEQ_ELEM(0,seq), Mode::BOOST_PP_SEQ_ELEM(1,seq)> ( const std::string& file );


    BOOST_PP_SEQ_FOR_EACH_PRODUCT(OPEN_FITS,(ENTITY_SEQ)(MODE_SEQ))

#undef ENTITY_SEQ
#undef MODE_SEQ

#define ENTITY_SEQ (Data)(Table)(Image)
#define MODE_SEQ (ReadOnly)(ReadWrite)


    BOOST_PP_SEQ_FOR_EACH_PRODUCT(OPEN_FITS,(ENTITY_SEQ)(MODE_SEQ))


    template FilePtr open<Entity::Memory>() ;


    ///////////////////
    // Table Support //
    ///////////////////


    TablePtr File::add( const Table& in ) {

	int in_chdu = in.file->hdu_num();

	try {
	    in.file->movabs_hdu( in.hdu_num );

	    SharedFilePtr fptr( in.file() );
	    copy_hdu( fptr );
	} catch ( ... ) {

	    in.file->movabs_hdu( in_chdu );

	    throw;

	}

	in.file->movabs_hdu( in_chdu );

	return table();
    }


    TablePtr File::table( ) {

#ifdef HAVE_STD__WEAK_FROM_THIS

        WeakFilePtr wp( weak_from_this() );

	// if object is on stack, wp won't have a vaild pointer
        return TablePtr( wp.expired() ? new Table( *this ) : new Table( wp ) );

#else

        try {
            SharedFilePtr sp( shared_from_this() );

	    // if we survived shared_from_this(), make sure it has a
	    // valid pointer (e.g. it's on the heap)
            return TablePtr( sp.get() ? new Table( *this ) : new Table( WeakFilePtr(sp) ) );

        } catch ( const std::bad_weak_ptr& e  ) {

	    // shared_from_this didn't like things, probably because
	    // this object is on the stack
	    return TablePtr( new Table( *this )	 );
        }

#endif

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

    //-----------------------------------------

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


#define READ_KEY(r,d,T) \
    template Keyword<T> File::read_key<T>( const std::string& keyname, const T& default_value ) const;


    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(READ_KEY)

    //-----------------------------------------
    template<typename T>
    void File::read_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, T* data ) const {

    	misFITS_CHECK_CFITSIO_EXPR
    	    (
    	     fits_read_col( fptr(),
    			    StorageCode<T>::type,
    			    colnum,
    			    firstrow, firstelem,
    			    nelem,
    			    NULL,
    			    data, NULL, &status)
    	     );

    }

#define READ_COL(r,d,T) \
    template void File::read_col<T>( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(READ_COL)

    //-----------------------------------------

    template<typename T>
    void File::write_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, T* data ) const {

	misFITS_CHECK_CFITSIO_EXPR
	    (
	     fits_write_col( fptr(),
			     StorageCode<T>::type,
			     colnum,
			     firstrow, firstelem,
			     nelem,
			     data, &status)
	     );

    }

#define WRITE_COL(r,d,T) \
    template void File::write_col<T>( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem, T* data ) const;

    misFITS_INSTANTIATE_OVER_STORAGE_TYPES(WRITE_COL)

}

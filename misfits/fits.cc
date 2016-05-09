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

#include <fitsio.h>

#include <misfits/fits.hpp>
#include <misfits/fits_p.hpp>
#include <misfits/table.hpp>

using namespace std;

namespace misFITS {

    //////////////////////////////////////
    // File constructors and Destructor //
    //////////////////////////////////////

    // destructor for shared_ptr class. should probably not throw...
    static void
    closeFitsPtr( fitsfile* fitsptr ) {
	if ( fitsptr )
	    misFITS_CHECK_CFITSIO_EXPR( fits_close_file( fitsptr, &status ) );
    }

    File::FitsPtr
    File::FitsPtr_( fitsfile* fitsptr ) {
	return File::FitsPtr( fitsptr, closeFitsPtr );
    }

    File::File( const std::string& file_, fitsfile* fitsfile_, OpenMode mode_ ) :
	fitsptr( FitsPtr_( fitsfile_ ) ), mode( mode_ ), file(file_) {}


    File::~File () {

	// CFITSIO::fits_close_file may return an error.  Since
	// destructors shouldn't throw exceptions, handle it by
	// logging the error to stderr.  Normally the client would
	// call the close method and catch the exception.  This is in
	// case that doesn't happen.

	try {
	    close();
	} catch ( Exception& e ) {
	    std::cerr << "Client error: misFITS::File::close invoked by misFITS::File::~File.  Error closing file " << file << ": " << e.what() << std::endl;
	}


    }

    FilePtr
    File::reopen( ) {

	fitsfile *nfptr;

	misFITS_CHECK_CFITSIO_EXPR( fits_reopen_file( fptr(), &nfptr, &status ) );

	FilePtr fp = FilePtr( new File( file, nfptr, mode ) );
	fp->move_to( hdu_num() );

	return fp;
    }


    //////////////////////////////////////////////////
    // Infrastructure for open and open_p factories //
    //////////////////////////////////////////////////


    // map open<Entity::XXX> to fits_open_XXX and fits_create_XXX
    namespace MapCFITSIO {

	template<>
	int open<Entity::File>( fitsfile** fptr, const std::string& file, OpenMode mode, int* status ) {
	    return fits_open_file( fptr, file.c_str(), boost::underlying_cast<int>(mode), status );
	}

	template<>
	int create<Entity::File>( fitsfile** fptr, const std::string& file, int* status ) {
	    return fits_create_file( fptr, file.c_str(), status );
	}

	template<>
	int open<Entity::DiskFile>( fitsfile** fptr, const std::string& file, OpenMode mode, int* status ) {
	    return fits_open_diskfile( fptr, file.c_str(), boost::underlying_cast<int>(mode), status );
	}

	template<>
	int create<Entity::DiskFile>( fitsfile** fptr, const std::string& file, int* status ) {
	    return fits_create_diskfile( fptr, file.c_str(), status );
	}

	template<>
	int open<Entity::Data>( fitsfile** fptr, const std::string& file, OpenMode mode, int* status ) {
 	    return fits_open_data( fptr, file.c_str(), boost::underlying_cast<int>(mode), status );
	}

	template<>
	int open<Entity::Table>( fitsfile** fptr, const std::string& file, OpenMode mode, int* status ) {
	    return fits_open_table( fptr, file.c_str(), boost::underlying_cast<int>(mode), status );
 	}

	template<>
	int open<Entity::Image>( fitsfile** fptr, const std::string& file, OpenMode mode, int* status ) {
	    return fits_open_image( fptr, file.c_str(), boost::underlying_cast<int>(mode), status );
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

	return FilePtr( new File(file, Open::open<Entity,Mode>( file ), Mode::mode() ) );
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


    ///////////////////
    // Table Support //
    ///////////////////

    TablePtr File::add( const Table& in ) {

	FilePtr fp = get_shared_ptr();
	return in.copy( fp, TableCopy::HDU );

    }

    TablePtr File::add( const TablePtr& in ) {
	return File::add( *in.get() );
    }


    HDUPtr File::hdu( int hdu_num ) {

	FilePtr fp = get_shared_ptr();
	return HDUPtr( new HDU( fp, hdu_num ) );

    }

    HDUPtr File::hdu( const std::string& extname, int extver ) {

	FilePtr fp = get_shared_ptr();
	return HDUPtr( new HDU( fp, extname, extver  ) );

    }

    TablePtr File::table( int hdu_num_ ) {

	FilePtr fp = get_shared_ptr();
	return TablePtr( new Table( fp, hdu_num_ ) );
    }

    TablePtr File::table( const std::string& extname, int extver ) {

	FilePtr fp = get_shared_ptr();
	return TablePtr( new Table( fp, extname, extver ) );
    }


    //////////////////////////////////////
    // Wrappers around CFITSIO routines //
    //////////////////////////////////////

    void
    File::close(  ) {
	if ( fitsptr )
	    misFITS_CHECK_CFITSIO_EXPR( fits_close_file( fitsptr.release(), &status ) );
    }

    void File::flush ( const FlushMode& mode ) const {

	switch( boost::native_value( mode ) ) {

	case FlushMode::File :
	    misFITS_CHECK_CFITSIO_EXPR( fits_flush_file( fptr(), &status ) );
	    break;

	case FlushMode::Buffer :
	    misFITS_CHECK_CFITSIO_EXPR( fits_flush_buffer( fptr(), 0, &status ) );
	    break;
	}
    }


    HDU_Type
    File::move_by( int nmove ) const {
	int hdu_type;
	misFITS_CHECK_CFITSIO_EXPR( fits_movrel_hdu( fptr(), nmove, &hdu_type, &status ) );
	return static_cast<HDU_Type>(hdu_type);
    }

    HDU_Type
    File::move_to( int hdu_num ) const {
	int hdu_type;
	misFITS_CHECK_CFITSIO_EXPR( fits_movabs_hdu( fptr(), hdu_num, &hdu_type, &status ) );
	return static_cast<HDU_Type>(hdu_type);
    }

    void
    File::move_to( const std::string& extname, int extver, HDU_Type hdu_type ) const {
	misFITS_CHECK_CFITSIO_EXPR( fits_movnam_hdu( fptr(),
						     boost::underlying_cast<int>(hdu_type),
						     const_cast<char*>(extname.c_str()),
						     extver, &status ) );
    }

    int
    File::num_hdus() const {
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
    File::copy ( FilePtr& outfile, FileCopy::Flag what, int morekeys ) const {

	// the various copy routines don't seem to check for readonly status;
	// they wait until cfitsio syncs state with the file.

	if ( OpenMode::ReadOnly == outfile->mode )
	    throw Exception::CFITSIO( READONLY_FILE );

	if ( what  == FileCopy::CurrentHeader ) {
	    misFITS_CHECK_CFITSIO_EXPR( fits_copy_header( fptr(), outfile->fptr(), &status ) );
	}
	else if ( what == FileCopy::CurrentHDU ) {
	    misFITS_CHECK_CFITSIO_EXPR( fits_copy_hdu( fptr(), outfile->fptr(), morekeys, &status ) );
	}
	else {

	    misFITS_CHECK_CFITSIO_EXPR
		(
		 fits_copy_file( fptr(), outfile->fptr(),
				 what & FileCopy::PreviousHDUs,
				 what & FileCopy::CurrentHDU,
				 what & FileCopy::FollowingHDUs,
				 &status ) );
	}

    }


    HDU_Type
    File::delete_hdu ( ) const {
	int hdutype;
	misFITS_CHECK_CFITSIO_EXPR( fits_delete_hdu( fptr(), &hdutype, &status ) );
	return static_cast<HDU_Type>(hdutype);
    }


}

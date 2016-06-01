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

#ifndef misFITS_H
#define misFITS_H

#include <boost/core/null_deleter.hpp>

#include <string>
#include <iostream>
#include <utility>
#include <vector>

#include <misfits/config.hpp>

// forward declarations

namespace misFITS {

    class File;
    typedef shared_ptr<File> SharedFilePtr;
    typedef weak_ptr<File> WeakFilePtr;
    typedef SharedFilePtr FilePtr;

    class HDU;
    typedef shared_ptr<HDU> SharedHDUPtr;
    typedef weak_ptr<HDU> WeakHDUPtr;
    typedef SharedHDUPtr HDUPtr;

    class Table;
    typedef shared_ptr<Table> SharedTablePtr;
    typedef weak_ptr<Table> WeakTablePtr;
    typedef SharedTablePtr TablePtr;

    // there must be a beter way to do this
    class ColumnInfo;
    typedef std::vector<ColumnInfo> TableColumnsType;

    template< class T >
    class Shared : public enable_shared_from_this<T> {

    protected:

	shared_ptr<T>
	get_shared_ptr() {

	    shared_ptr<T> sp;

	    try {
		sp = enable_shared_from_this<T>::shared_from_this();

	    } catch ( const bad_weak_ptr& e  ) {

		// shared_from_this didn't like things, probably because
		// this object is on the stack
		sp.reset(); // just in case

	    } catch ( ... ) {

		throw;
	    }


	    // if we survived, make sure sp has a valid pointer (e.g. it's
	    // on the heap)
	    return sp.get() ? sp : shared_ptr<T>( static_cast<T*>(this), boost::null_deleter() ) ;

	}

    };
}

#include <misfits/types.hpp>
#include <misfits/keyword.hpp>
#include <misfits/exception.hpp>
#include <misfits/bitset.hpp>

namespace misFITS {

#define misFITS_CHECK_CFITSIO_EXPR( expr )	\
    do {					\
	int status = 0;				\
	expr;					\
	if ( status )				\
	    throw misFITS::Exception::CFITSIO (status );	\
    } while (0)


    // forward declarations for befriending
    namespace Entry {
	template<class T> class Column;
    }
    class Row;


    // declare some enums so can be used as template arguments
    namespace Entity {
	enum Type { File, Memory, DiskFile, Data, Table, Image };
    }


    // ditto, but some should have actual values, so make 'em a struct.
    namespace Mode {
	struct ReadOnly  { static OpenMode mode() { return OpenMode::ReadOnly  ; } };
	struct ReadWrite { static OpenMode mode() { return OpenMode::ReadWrite ; } };
	struct Create  : public ReadWrite  { };
	struct CreateOverWrite : public ReadWrite  {};
    }

    // map open<Entity::XXX> to fits_open_XXX and fits_create_XXX
    namespace MapCFITSIO {

	// no generic template

	template<Entity::Type Entity> int open( fitsfile** fptr, const std::string& file, OpenMode mode, int* status );
	template<Entity::Type Entity> int create( fitsfile** fptr, const std::string& file, int* status );

    }


    // map open<Entity,Mode> to call to MapCFITSIO<Entity::open( ... M::mode() )
    namespace Open {

	template< Entity::Type Entity, class Mode >
	fitsfile* open( const std::string& file );

    }


    class File;
    typedef shared_ptr<File> SharedFilePtr;
    typedef weak_ptr<File> WeakFilePtr;
    typedef SharedFilePtr FilePtr;

    class Table;
    typedef shared_ptr<Table> SharedTablePtr;
    typedef weak_ptr<Table> WeakTablePtr;
    typedef SharedTablePtr TablePtr;


    // predeclare "factory" open function
    template<Entity::Type Entity> FilePtr open( );
    template<Entity::Type Entity, class Mode> FilePtr open( const std::string& file );

    class File : public Shared<File> {

    private:


	///////////////////
        // members	 //
        ///////////////////

	typedef unique_ptr<fitsfile,void(*)(fitsfile*)> FitsPtr;
	FitsPtr fitsptr;
	OpenMode mode;

	/////////////////////////
        // constructors	       //
        /////////////////////////

	// null declaration; not defined.
	File( const File& );
	File& operator=( const File& );

	////////////////////
        // methods	  //
        ////////////////////

	// constructors which accept a fitsfile*
	//
	// don't make public; can't trust that if a user passes us a
	// fitsfile* they won't go ahead and free it.
	//
	// these will only be available to File::open

	File( const std::string& file_, fitsfile* fitsfile_, OpenMode mode );

	static FitsPtr FitsPtr_( fitsfile* fitsptr );

	friend FilePtr open<Entity::File, Mode::ReadOnly>( const std::string&file );
	friend FilePtr open<Entity::File, Mode::ReadWrite>( const std::string&file );
	friend FilePtr open<Entity::File, Mode::Create>( const std::string&file );
	friend FilePtr open<Entity::File, Mode::CreateOverWrite>( const std::string&file );

	friend FilePtr open<Entity::DiskFile, Mode::ReadOnly>( const std::string&file );
	friend FilePtr open<Entity::DiskFile, Mode::ReadWrite>( const std::string&file );
	friend FilePtr open<Entity::DiskFile, Mode::Create>( const std::string&file );
	friend FilePtr open<Entity::DiskFile, Mode::CreateOverWrite>( const std::string&file );

	friend FilePtr open<Entity::Data, Mode::ReadOnly>( const std::string&file );
	friend FilePtr open<Entity::Data, Mode::ReadWrite>( const std::string&file );

	friend FilePtr open<Entity::Table, Mode::ReadOnly>( const std::string&file );
	friend FilePtr open<Entity::Table, Mode::ReadWrite>( const std::string&file );

	friend FilePtr open<Entity::Image, Mode::ReadOnly>( const std::string&file );
	friend FilePtr open<Entity::Image, Mode::ReadWrite>( const std::string&file );

	friend FilePtr open<Entity::Memory>(  );

	friend class HDU;
	friend class Table;
	friend class ColumnInfo;
	template< typename T> friend class Entry::Column;

	inline fitsfile* fptr() const {
	    return fitsptr.get();
	}

    public:

	~File();

	////////////////////
        // members	  //
        ////////////////////

	std::string file;


        /////////////////////////
        // Generic HDU support //
        /////////////////////////

	HDUPtr hdu( int hdu_num = 0);
	HDUPtr hdu( const std::string& extname, int extver = 1 );

	//////////////////////////
        // Table support        //
        //////////////////////////
	TablePtr table( int hdu_num = 0);
	TablePtr table( const std::string& extname, int extver = 1);
	TablePtr add( const Table& table );
	TablePtr add( const TablePtr& table );


	/////////////////////////////
        // CFITSIO wrappers	   //
        /////////////////////////////

	void close ();
	FilePtr reopen( );

	void move_by( int nmove ) const;

	void move_to( int hdu_num ) const;
	void move_to( const std::string&extname, int extver = 1, HDU_Type hdu_type = HDU_Type::Any ) const;

	int num_hdus() const;
	int hdu_num() const;
	HDU_Type hdu_type() const;
	void copy ( FilePtr& outfile, FileCopy::Flag what, int morekeys = 0 ) const;

	HDU_Type delete_hdu ( ) const;

	void flush ( const FlushMode& mode = FlushMode::File ) const;

    };

    //////////////////////////
    // open command //
    //////////////////////////
    template <Entity::Type Entity, class Mode> FilePtr open( const std::string&file );
    template <Entity::Type Entity>  FilePtr open( );

}

#endif // !misFITS_H

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

#include <string>
#include <iostream>
#include <utility>

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
	struct ReadOnly  { static int mode() { return OpenMode::ReadOnly ; } };
	struct ReadWrite { static int mode() { return OpenMode::ReadWrite ; } };
	struct Create  : public ReadWrite  { };
	struct CreateOverWrite : public ReadWrite  {};
    }

    // map open<Entity::XXX> to fits_open_XXX and fits_create_XXX
    namespace MapCFITSIO {

	// no generic template

	template<Entity::Type Entity> int open( fitsfile** fptr, const std::string& file, int mode, int* status );
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

    class File : public enable_shared_from_this<File> {

    private:


	///////////////////
        // members	 //
        ///////////////////

	typedef unique_ptr<fitsfile,void(*)(fitsfile*)> FitsPtr;
	FitsPtr fitsptr;
	int mode;

	/////////////////////////
        // constructors	       //
        /////////////////////////

	// null declaration; not defined.
	File( const File& );
	File& operator=( const File& );

	// return a shared ptr to the current object
	FilePtr fileptr();

	////////////////////
        // methods	  //
        ////////////////////

	// constructors which accept a fitsfile*
	//
	// don't make public; can't trust that if a user passes us a
	// fitsfile* they won't go ahead and free it.
	//
	// these will only be available to File::open

	File( const std::string& file_, fitsfile* fitsfile_, int mode );

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

	HDU_Type move_by( int nmove ) const;

	HDU_Type move_to( int hdu_num ) const;
	void move_to( const std::string&extname, int extver = 1, HDU_Type hdu_type = HDU_Type::Any ) const;

	int num_hdus() const;
	int hdu_num() const;
	HDU_Type hdu_type() const;
	void copy ( FilePtr& outfile, FileCopy::Flag what, int morekeys = 0 ) const;

	HDU_Type delete_hdu ( ) const;

	template< typename T>
	void read_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;
	template< typename T>
	void write_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, const T* data ) const;


	std::pair<int,int> get_hdrpos( );

	Keyword<std::string> read_keyword( const std::string& keyname,
					   const std::string& default_value = "") const;

	Keyword<std::string> read_keyn( int keynum, const std::string& default_value = "" ) const;

	template<typename T>
	Keyword<T> read_key( const std::string& keyname,
			     const T& default_value = StorageCode<T>::default_value() ) const;

	template<typename T>
	void write_key( const Keyword<T>& kw) const;

	template<typename T>
	void update_key( const Keyword<T>& kw) const;


    };

    //////////////////////////
    // open command //
    //////////////////////////
    template <Entity::Type Entity, class Mode> FilePtr open( const std::string&file );
    template <Entity::Type Entity>  FilePtr open( );

}

#endif // !misFITS_H

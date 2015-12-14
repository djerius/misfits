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
#include <stdexcept>
#include <utility>

#include <boost/dynamic_bitset.hpp>
#include <misfits/config.hpp>

// forward declarations

namespace misFITS {

    class File;
    typedef shared_ptr<File> SharedFilePtr;
    typedef weak_ptr<File> WeakFilePtr;
    typedef SharedFilePtr FilePtr;

    class Table;
    typedef shared_ptr<Table> SharedTablePtr;
    typedef weak_ptr<Table> WeakTablePtr;
    typedef SharedTablePtr TablePtr;


}

#include <misfits/types.hpp>
#include <misfits/keyword.hpp>

namespace misFITS {


    class Exception : public std::exception {

    public:
	class CFITSIO;
	class Assert : public std::runtime_error {
	public:
	    Assert( const char* what_arg ) : runtime_error( what_arg ) { }
	};

    };

    class Exception::CFITSIO : public Exception {

    public:
	CFITSIO( int status);
	const char* what() const  throw () ;

	int status () { return status_; }

    private:
	char error[26*82];
	int status_;
    };


#define misFITS_CHECK_CFITSIO_EXPR( expr )	\
    do {					\
	int status = 0;				\
	expr;					\
	if ( status )				\
	    throw Exception::CFITSIO (status );	\
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
	struct ReadOnly  { static int mode() { return READONLY ; } };
	struct ReadWrite { static int mode() { return READWRITE ; } };
	struct Create    {};
	struct CreateOverWrite    {};
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

    //
    typedef uint8_t byte_t;
    typedef boost::dynamic_bitset<byte_t> BitSet;


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

	/////////////////////////
        // constructors	       //
        /////////////////////////

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

	File( const std::string& file_, fitsfile* fitsfile_ );

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

	inline fitsfile* fptr() const {
	    return fitsptr.get();
	}

	// helper for reopening files
	void reopen( const File& );

    public:

	////////////////////
        // members	  //
        ////////////////////

	std::string file;


	/////////////////////////
        // constructors	       //
        /////////////////////////

	// default: open at first data HDU in read only mode
	File( const std::string& file );

	// reopen
	File( const File& );

	//////////////////////////
        // Table support        //
        //////////////////////////
	TablePtr table();
	TablePtr add( const Table& table );


	/////////////////////////////
        // CFITSIO wrappers	   //
        /////////////////////////////

	HDU_Type movabs_hdu( int hdu_num ) const;
	HDU_Type movrel_hdu( int nmove ) const;
	void movnam_hdu( HDU_Type hdu_type, const std::string&extname, int extver = 1) const;
	int get_num_hdus() const;
	int hdu_num() const;
	HDU_Type hdu_type() const;
	void copy_file ( FilePtr& outfile, int previous, int current, int following ) const;
	void copy_hdu ( FilePtr& outfile, int morekeys = 0 ) const;
	void copy_header ( FilePtr& outfile ) const;
	HDU_Type delete_hdu ( ) const;

	template< typename T>
	void read_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;
	template< typename T>
	void write_col( int colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;


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

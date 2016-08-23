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

#ifndef misFITS_ROW_ENTRY_HPP
#define misFITS_ROW_ENTRY_HPP

#include <vector>

#include <misfits/row_entry_fwd.hpp>

#include <misfits/types.hpp>
#include <misfits/table.hpp>

namespace misFITS {

    class Row;

    namespace RowEntry {

	////////////////////
        // Columns	  //
        ////////////////////

	class ColumnBase {

	    friend class misFITS::Row;

	private:
	    virtual void read( const Table& table, LONGLONG firstrow ) = 0;
	    virtual void write( const Table& table, LONGLONG firstrow ) = 0;

	protected:
	    ColumnBase( const ColumnInfo& info )
		: colnum_( info.colnum ),
		  nelem_( info.nelem() ),
		  natomic_( nelem_ ),
		  id_( info.column_type->id() )
	    {}

	    virtual ~ColumnBase() {}

	    Table::Columns::size_type colnum_;
	    // number of FITS elements
	    LONGLONG nelem_;

	    // number of atomic elements to read/write. may differ from nelem_ if it's a bitfield.
	    LONGLONG natomic_;

	    ColumnType::ID::type id_;
	};

	//-----------------------------------------

	template< typename T >
	class ColumnInit : public ColumnBase {

	protected:
	    typedef T Base;

	public:
	    ColumnInit( const ColumnInfo& info )
		: ColumnBase( info )
	    {
		ColumnInit<T>::init();
	    }

	    virtual ~ColumnInit() { };

	    void init(){};

	};


	// special handling for byte_t when used with bits
	template<> void ColumnInit<byte_t>::init();


	//-----------------------------------------
	// this is the generic public interface to columns
	// it gets specialized for various things.

	template< typename T >
	class Column : public ColumnInit<T> {

	    typedef ColumnInit<T> Parent;

	public:
	    Column( const ColumnInfo& info, T* base ) : ColumnInit<T>( info ), base_( base ) {
		Column<T>::init();
	    }

	    void read( const Table& table, LONGLONG firstrow ) {
		table.read_col( Parent::colnum_, firstrow, 1, Parent::natomic_, base_ );
	    }
	    void write( const Table& table, LONGLONG firstrow ) {
		table.write_col( Parent::colnum_, firstrow, 1, Parent::natomic_, base_ );
	    }

	    void init() {};

	private:
	    T* base_;

	};



	//-----------------------------------------

	template< typename T, typename VT >
	class ColumnVector : public ColumnInit<T> {

	    typedef ColumnInit<T> Parent;
	    typedef VT Base;

	public:
	    ColumnVector( const ColumnInfo& info, Base* base )
		: ColumnInit<T>( info ), base_( base )
	    {
		ColumnVector<T,VT>::init();
		base_->resize( Parent::natomic_ );
	    }

	    void read( const Table& table, LONGLONG firstrow ) {
		table.read_col<T>( Parent::colnum_, firstrow, 1,
				   static_cast<LONGLONG>(Parent::natomic_), &((*base_)[0]) );
	    }
	    void write( const Table& table, LONGLONG firstrow ) {
		table.write_col<T>( Parent::colnum_, firstrow, 1,
				    static_cast<LONGLONG>(Parent::natomic_), &((*base_)[0]) );
	    }

	protected:
	    Base* base_;
	    void init() {}
	};



	template< typename T >
	struct Column< std::vector<T> > : public ColumnVector< T, std::vector<T> > {

	    typedef std::vector<T> Base;

	public:
	    Column( const ColumnInfo& info, Base* base )
		: ColumnVector< T, std::vector<T> >( info, base ) {}
	};


	template< typename T>
	struct Column< boost::container::vector<T> > : public ColumnVector< T, boost::container::vector<T> > {

	    typedef boost::container::vector<T> Base;

	public:
	    Column( const ColumnInfo& info, Base* base )
		: ColumnVector< T, Base >( info, base ) {}
	};

	//-----------------------------------------

	// we can't simply write into a bool as we don't know how the
	// bool implements true or false. we have to rely upon
	// conversion, so first read into a buffer, then assign to the
	// element in the bool storage.  this works for both the
	// specialized monster that is std::vector<bool> and
	// boost::container::vector<bool>

	template< typename T, typename VT >
	class BoolColumnVector : public ColumnInit<T> {

	    typedef std::vector<NativeType<SC_BYTE>::storage_type> Buffer;
	    typedef ColumnInit<T> Parent;
	    typedef VT Base;

	public:
	    BoolColumnVector( const ColumnInfo& info, Base* base )
		: ColumnInit<T>( info ), base_( base )
	    {
		BoolColumnVector<T,VT>::init();
		base_->resize( Parent::natomic_ );
		buffer.resize( Parent::natomic_ );
	    }

	    void read( const Table& table, LONGLONG firstrow ) {

		table.read_col<ColumnType::ID::Logical>( Parent::colnum_, firstrow, 1, static_cast<LONGLONG>( Parent::natomic_ ), &buffer[0] );

		for ( Buffer::size_type idx = 0 ; idx < Parent::natomic_ ; idx++ )
		    (*base_)[idx] = buffer[idx];

	    }
	    void write( const Table& table, LONGLONG firstrow ) {

		for ( Buffer::size_type idx = 0 ; idx < Parent::natomic_ ; idx++ )
		    buffer[idx] = (*base_)[idx];

		table.write_col<ColumnType::ID::Logical>( Parent::colnum_, firstrow, 1, static_cast<LONGLONG>( Parent::natomic_ ), &buffer[0] );
	    }

	protected:
	    Base* base_;
	    Buffer buffer;
	    void init() {}
	};



	template<>
	struct Column< std::vector<bool> > : public BoolColumnVector< bool, std::vector<bool> > {

	    typedef std::vector<bool> Base;

	public:
	    Column( const ColumnInfo& info, Base* base )
		: BoolColumnVector< bool, Base >( info, base ) {}
	};


	template<>
	struct Column< boost::container::vector<bool> > : public BoolColumnVector< bool, boost::container::vector<bool> > {

	    typedef boost::container::vector<bool> Base;

	public:
	    Column( const ColumnInfo& info, Base* base )
		: BoolColumnVector< bool, Base >( info, base ) {}
	};

	//-----------------------------------------

	template<>
	class Column< BitSet > : public ColumnBase {

	    typedef BitSet Base;
	    typedef Base::size_type size_type;
	    typedef std::vector<BitSet::block_type> Buffer;

	public:

	    Column( const ColumnInfo& info, BitSet* base );
	    virtual ~Column() { };

	    void read(  const Table& table, LONGLONG firstrow );
	    void write( const Table& table, LONGLONG firstrow );


	private:
	    Base* base_;
	    size_type max_bits_;

	    // number of bytes required to store the bits. cached for
	    // speed
	    Buffer::size_type nbytes_;

	    Buffer buffer;
	};

	//-----------------------------------------

	template<>
	class Column< bool > : public ColumnBase {

	    typedef bool Base;
	    typedef std::vector<NativeType<SC_BYTE>::storage_type> Buffer;

	public:
	    Column( const ColumnInfo& info, bool* base );
	    virtual ~Column() { };

	    virtual void read( const Table& table, LONGLONG firstrow );
	    virtual void write( const Table& table, LONGLONG firstrow );

	protected:
	    Base* base_;
	    Buffer buffer;
	};


	//-----------------------------------------

	template<>
	class Column<std::string>: public ColumnBase {

	    typedef std::string Base;
	    typedef std::vector<char> Buffer;

	public:

	    Column( const ColumnInfo& info, std::string* base );
	    virtual ~Column() { };

	    void read(  const Table& table, LONGLONG firstrow );
	    void write( const Table& table, LONGLONG firstrow );

	private:

	    // for compatibility with C++ < 11, use intermediate
	    // buffer.  in C++11, consecutive characters in
	    // std::string are defined to be contiguous, and we can
	    // read directly into the string. see stackoverflow.com/questions/25169915
	    Buffer buffer;

	    Base* base_;
	    LONGLONG offset;

	};

	//-----------------------------------------

	template<typename VT>
	class StringColumnVector: public ColumnBase {

	    typedef VT Base;
	    typedef std::vector<char> Buffer;

	public:

	    StringColumnVector( const ColumnInfo& info, Base* base )
		: ColumnBase( info ), base_( base ),
		  offset( info.offset ),
		  width( static_cast<Buffer::size_type>(info.extent[0] ) ) {

		if ( ColumnType::ID::String != info.column_type->id() )
		    throw Exception::Assert( "a vector<std::string> destination can only be used with a FITS 'A' column type" );

		natomic_ = info.nbytes;
		buffer.resize( natomic_ );
		base_->resize( nelem_ / width );
		for_each( base_->begin(), base_->end(), bind2nd(mem_fun_ref( &std::string::reserve ), width ) );
	    }


	    void read( const Table& table, LONGLONG firstrow ) {

		table.read_bytes( firstrow, offset,
				  static_cast<LONGLONG>( natomic_ ),
				  reinterpret_cast<unsigned char*>(&buffer[0])
				  );

		char* start = &buffer[0];

		typename Base::iterator str = base_->begin();
		typename Base::iterator end = base_->end();
		for ( ; str < end ; ++str ) {
		    str->assign( start, width );
		    start += width;

		    // FITS standard allows null terminated strings See 7.3.3.1 of the FITS
		    // paper at
		    // http://fits.gsfc.nasa.gov/standard30/fits_standard30aa.pdf
		    // however, what happens when the cell has a multi-dimensional
		    // character array?  May each array be null terminated, or does
		    // the data get truncated after the first NULL prior to dividing it into
		    // the separate arrays?

		    // CFITSIO has the second behavior.  This code follows the first behavior.

		    Buffer::size_type nchar = str->find_first_of( '\0' );
		    if ( nchar != std::string::npos )
			str->resize( nchar );
		}

	    }

	    void write( const Table& table, LONGLONG firstrow ) {

		buffer.assign( natomic_, ' ' );

		char* start = &buffer[0];

		typename Base::iterator str = base_->begin();
		typename Base::iterator end = base_->end();
		for ( ; str < end ; ++str, start += width ) {
		    std::string::size_type maxc = str->length();
		    if ( width > maxc ) {
			str->copy( start, maxc );
			start[maxc] = '\0';
		    }
		    else {
			str->copy( start, width );
		    }
		}

		table.write_bytes( firstrow, offset,
				   static_cast<LONGLONG>(natomic_),
				   reinterpret_cast<unsigned char*>(&buffer[0])
				   );
	    }

	    virtual ~StringColumnVector() { };

	private:
	    Base* base_;

	    // for compatibility with C++ < 11, use intermediate
	    // buffer.  in C++11, consecutive characters in
	    // std::string are defined to be contiguous, and we can
	    // read directly into the string. see stackoverflow.com/questions/25169915

	    Buffer buffer;
	    LONGLONG offset;
	    Buffer::size_type width;

	};

	template<>
	struct Column< std::vector<std::string> > : public StringColumnVector< std::vector<std::string> > {

	    typedef std::vector<std::string> Base;

	public:
	    Column( const ColumnInfo& info, Base* base )
		: StringColumnVector< Base >( info, base ) {}
	};


	template<>
	struct Column< boost::container::vector<std::string> > : public StringColumnVector< boost::container::vector<std::string> > {

	    typedef boost::container::vector<std::string> Base;

	public:
	    Column( const ColumnInfo& info, Base* base )
		: StringColumnVector< Base >( info, base ) {}
	};

	//-----------------------------------------


    }

}

#endif // ! misFITS_ROW_ENTRY_HPP

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

#ifndef misFITS_ROW_H
#define misFITS_ROW_H

#include <algorithm>
#include <vector>

#include <own_or_observe_ptr.hpp>
#include <misfits/types.hpp>

namespace misFITS {

    namespace Entry {


	////////////////////////////////
        // Types of addressing	      //
        ////////////////////////////////


	class Offset {

	    friend class Group;

	protected:

	    Offset( size_t offset ) : offset_( offset ) {}
	    virtual ~Offset(){}

	    virtual void read( const File& file, LONGLONG firstrow, void* base ) = 0;
	    virtual void write( const File& file, LONGLONG firstrow, void* base  ) = 0;
	    void reset( size_t offset ) { offset_ = offset; }

	    size_t offset_;
	};

	class Absolute {

	    friend class misFITS::Row;

	protected:
	    Absolute( void* base ) : base_( base ) {}
	    virtual ~Absolute(){}

	    virtual void read( const File& file, LONGLONG firstrow ) = 0;
	    virtual void write( const File& file, LONGLONG firstrow ) = 0;
	    void reset( void* base ) { base_ = base; }

	    void* base_;
	};

	////////////////////
        // Columns	  //
        ////////////////////

	template< class T >
	class Column {

	protected:
	    Column( int colnum, LONGLONG nelem ) :
		colnum_( colnum ), nelem_( nelem ) {}
	    virtual ~Column() { };

	    void read( const File& file, LONGLONG firstrow, T* data ) {
		file.read_col( colnum_, firstrow, 1, nelem_, data );
	    }

	    void write( const File& file, LONGLONG firstrow, T* data ) {
		file.write_col( colnum_, firstrow, 1, nelem_, data );
	    }

	private:
	    int colnum_;
	    LONGLONG nelem_;
	};


	template< class T>
	class AbsoluteColumn : public Column<T>, public Absolute  {

	    friend class misFITS::Row;

	public:
	    AbsoluteColumn( int colnum, LONGLONG nelem, T* base )
		: Column<T>( colnum, nelem  ),
		  Absolute( base )
	    {}

	private:

	    void read( const File& file, LONGLONG firstrow )  { Column<T>::read( file, firstrow, static_cast<T*>(base_) ); }
	    void write( const File& file, LONGLONG firstrow ) { Column<T>::write( file, firstrow, static_cast<T*>(base_) ); }
	    ;
	};

	template< class T>
	class OffsetColumn : public Column<T>, public Offset  {

	public:

	    OffsetColumn( int colnum, LONGLONG nelem, size_t offset )
		: Column<T>( colnum, nelem ),
		  Offset( offset )
	    {}

	    void read( const File& file, LONGLONG firstrow, void *base ) {
		Column<T>::read( file, firstrow, reinterpret_cast<T*>( static_cast<char*>(base) + offset_  ));
	    }

	    void write( const File& file, LONGLONG firstrow, void *base ) {
		Column<T>::write( file, firstrow, reinterpret_cast<T*>( static_cast<char*>(base) + offset_ ) );
	    }

	};


	///////////////////
        // Groups	 //
        ///////////////////

	class Group  {

	public:
	    virtual Group* group( size_t offset ) = 0;

	    template< class T>
	    void column( const std::string& column_name, size_t offset );

	    const misFITS::ColumnInfo& column_info( const std::string& column_name );

	    void
	    push_back( std::shared_ptr<Offset> offset ) {
		entries.push_back( offset );
	    }


	protected:

	    Group( misFITS::Row* row ) : row_( row ) {}

	    misFITS::Row* row_;
	    std::vector< std::shared_ptr<Offset> > entries;
	    void read( const File& file, LONGLONG firstrow, void* base );
	    void write( const File& file, LONGLONG firstrow, void* base );

	};

	class GroupOffset : public Group, public Offset {

	    friend class GroupAbsolute;


	public:
	    GroupOffset( misFITS::Row* row, size_t offset ) : Group( row ), Offset( offset ) { }

	private:

	    GroupOffset* group( size_t offset );

	    void read( const File& file, LONGLONG firstrow, void* base );
	    void write( const File& file, LONGLONG firstrow, void* base );

	};


	class GroupAbsolute : public Group, public Absolute {

	    friend class misFITS::Row;

	public:
	    GroupAbsolute( misFITS::Row *row, void* base ) : Group( row ), Absolute( base ) { }

	private:
	    GroupOffset* group( size_t offset );

	    void read( const File& file, LONGLONG firstrow );
	    void write( const File& file, LONGLONG firstrow );

	};

	template<class ReturnClass>
	class GroupDSL {

	public:
	    GroupDSL( ReturnClass* caller, Group* group ) : caller_( caller ), group_( group ) {}

	    template< class T >
	    GroupDSL& column( const std::string& column_name, size_t offset ) {
		const misFITS::ColumnInfo& ci ( group_->column_info( column_name ) );
		std::shared_ptr< Entry::OffsetColumn<T> > column( std::make_shared< Entry::OffsetColumn<T> >( ci.colnum,
													      ci.nelem(),
													      offset )
								  );
		group_->push_back( column );
		return *this;
	    }

	    ReturnClass& endgroup() { return *caller_; }

	    GroupDSL<GroupDSL> group( size_t offset ) {
		return GroupDSL<GroupDSL>( this, group_->group( offset ) );
	    }


	private:

	    ReturnClass* caller_;
	    Group* group_;

	};


    }

    /////////////
    // The Row //
    /////////////


    class Row {

	friend class Entry::Group;
    public:

	bool read();

	void write();

	template< class T >
	Row& column( const std::string& column_name, T* base ) {

	    const misFITS::ColumnInfo& ci = table->column( column_name );
	    entries.push_back( std::make_shared< Entry::AbsoluteColumn<T> >( ci.colnum,
									     ci.nelem(),
									     base ) );
	    return *this;
	}

	Entry::GroupDSL<Row> group( void* base );

	/////////////////////////
        // Constructors	       //
        /////////////////////////

	Row( own_or_observe::rptr<Table>* base );
	Row( Table& table );
	Row( TablePtr& table );
	Row( FilePtr& file );
	Row( File& file );

	LONGLONG idx() const { return idx_ ; }
	LONGLONG idx( LONGLONG idx ) {
	    idx_ = idx ;
	    return idx_;
	}
	LONGLONG advance( LONGLONG nrows = 1 ) {
	    idx_ += nrows;
	    return idx_;
	}

	bool auto_advance() const { return auto_advance_ ; }
	bool auto_advance( bool flag ) {
	    auto_advance_ = flag;
	    return auto_advance_;
	}

    private :
	own_or_observe::ptr<Table> table;

	void init();

	LONGLONG idx_;
	bool auto_advance_;
	// if the row object is copied, don't want two objects
	// managing the same column entries
	std::vector< std::shared_ptr<Entry::Absolute> > entries;

    };


}

#endif // ! misFITS_ROW_H

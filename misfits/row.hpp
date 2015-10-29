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
	    virtual void write( const File& file, LONGLONG firstrow, void *base  ) = 0;
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

	class Column {

	protected:
	    Column( int colnum, LONGLONG nelem, misFITS::StorageType storage_type ) :
		colnum_( colnum ), nelem_( nelem ), storage_type_( storage_type ) {}
	    virtual ~Column() { };

	    void read( const File& file, LONGLONG firstrow, void* data );
	    void write( const File& file, LONGLONG firstrow, void* data );

	private:
	    int colnum_;
	    LONGLONG nelem_;
	    misFITS::StorageType storage_type_;
	};


	class AbsoluteColumn : public Column, public Absolute  {

	    friend class misFITS::Row;

	private:

	    AbsoluteColumn( int colnum, LONGLONG nelem, misFITS::StorageType storage_type, void* base )
		: Column( colnum, nelem, storage_type ),
		  Absolute( base )
	    {}

	    void read( const File& file, LONGLONG firstrow );
	    void write( const File& file, LONGLONG firstrow );
	};

	class OffsetColumn : public Column, public Offset  {

	public:

	    OffsetColumn( int colnum, LONGLONG nelem, misFITS::StorageType storage_type, size_t offset )
		: Column( colnum, nelem, storage_type ),
		  Offset( offset )
	    {}

	    void read( const File& file, LONGLONG firstrow, void *base );
	    void write( const File& file, LONGLONG firstrow, void *base );

	};


	///////////////////
        // Groups	 //
        ///////////////////

	class Group  {

	public:
	    virtual Group* group( size_t offset ) = 0;

	    void column( const std::string& column_name, misFITS::StorageType type, size_t offset );
	    void column( const char* column_name, misFITS::StorageType type, size_t offset );

	protected:

	    Group( misFITS::Row* row ) : row_( row ) {}
	    virtual ~Group() {
		for_each( entries.begin(), entries.end(), delete_entry );
	    }

	    misFITS::Row* row_;
	    std::vector<Offset*> entries;
	    const misFITS::ColumnInfo& column_info( const std::string& column_name );
	    void read( const File& file, LONGLONG firstrow, void* base );
	    void write( const File& file, LONGLONG firstrow, void* base );

	private:
	    static void delete_entry (Offset* entry ) {

		delete entry;

	    }

	};

	class GroupOffset : public Group, public Offset {

	    friend class GroupAbsolute;

	private:

	    GroupOffset( misFITS::Row* row, size_t offset ) : Group( row ), Offset( offset ) { }

	    GroupOffset* group( size_t offset );

	    void read( const File& file, LONGLONG firstrow, void* base );
	    void write( const File& file, LONGLONG firstrow, void* base );

	};


	class GroupAbsolute : public Group, public Absolute {

	    friend class misFITS::Row;

	private:

	    GroupAbsolute( misFITS::Row *row, void* base ) : Group( row ), Absolute( base ) { }

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
		group_->column( column_name, misFITS::StorageCode<T>::type, offset );
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

	    entries.push_back( new Entry::AbsoluteColumn( ci.colnum,
							  ci.nelem(),
							  misFITS::StorageCode<T>::type,
							  base ) );
	    return *this;
	}

	Entry::GroupDSL<Row> group( void* base );

	Row( Table* table ) : table( table ) {}
	Row( Table& table ) : table( &table ) {}
	virtual ~Row() {

	    for_each( entries.begin(), entries.end(), delete_entry );

	}

	LONGLONG idx () { return idx_ ; }

    private :
	Table* table;

	static void
	delete_entry( Entry::Absolute* entry ) {

	    delete entry;

	}

	LONGLONG idx_;
	std::vector<Entry::Absolute*> entries;

    };


}

#endif // ! misFITS_ROW_H

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

#ifndef misFITS_TABLE_H
#define misFITS_TABLE_H

#include <string>
#include <vector>

#include <boost/core/scoped_enum.hpp>

#include <misfits/fits.hpp>
#include <misfits/hdu.hpp>
#include <misfits/extent.hpp>

#include <misfits/columninfo.hpp>

namespace misFITS {

    class Row;

    namespace RowEntry {
	template<typename T> class Column;

    }

    BOOST_SCOPED_ENUM_DECLARE_BEGIN( TableCopy )
    {
	HDU, Header
    }
    BOOST_SCOPED_ENUM_DECLARE_END( TableCopy )

    struct ColumnCopy {
	enum Flag {
	    NoDuplicates       = 0x01,
	    Replace            = 0x02,
	    OverWrite          = 0x03,
	    ExtendTable        = 0x10,
	    ExtendTableIfEmpty = 0x20,
	    FlagMask           = 0x0f
	};
    };

    class Table : public HDU {

	template<typename T> friend class RowEntry::Column;


    public:

	typedef TableColumnsType Columns;

	Table( const std::string&, int extver = 1 );
	Table( WeakFilePtr file, int hdu_num = 0 );
	Table( WeakFilePtr file, const std::string& extname, int extver = 1 );

	const ColumnInfo& colinfo( Columns::size_type colnum ) const;
	const ColumnInfo& colinfo( const std::string& name ) const;

	// copy table to other file
	TablePtr copy( FilePtr& file, const TableCopy& what, int morekeys = 0 ) const;

	Table& add( const ColumnInfo& );

	Table& add( const std::string& ttype,
		    ColumnType typecode,
		    const std::string& tunit = "",
		    const Extent& extent = Extent(1),
		    Columns::size_type colnum = 0);

	Table& add( const std::string& ttype,
		    ColumnType typecode,
		    const Extent& extent,
		    Columns::size_type colnum = 0) {

	    return add( ttype, typecode, "", extent, colnum );
	}

	void resize( Columns::size_type colnum, const Extent& extent );
	void resize( const std::string& name, const Extent& extent );

	Columns::size_type num_columns() const;
	LONGLONG num_rows() const;

	void flush ( const FlushMode& mode = FlushMode::File ) const {
	    file_->flush( mode );
	};

	bool has_column( const std::string& templt )  const;
	bool exists_column( const std::string& templt )  const {
	    std::cerr << "misFITS::Table::exists_column is deprecated; use misFITS::Table::has_column\n";
	    return has_column( templt );
	};
	void delete_column( Columns::size_type );
	void delete_column( const std::string& name );

	void copy_column( Table& dest, const std::string& name,
			  ColumnCopy::Flag how = static_cast<ColumnCopy::Flag>(ColumnCopy::NoDuplicates | ColumnCopy::ExtendTable) ) const;

	void copy_columns( Table& dest, const std::vector< std::string >& name,
			  ColumnCopy::Flag how = static_cast<ColumnCopy::Flag>(ColumnCopy::NoDuplicates | ColumnCopy::ExtendTable) ) const;

	void copy_columns( Table& dest, ColumnCopy::Flag how = static_cast<ColumnCopy::Flag>(ColumnCopy::NoDuplicates | ColumnCopy::ExtendTable) ) const;

	misFITS::Row row();


    private:

	template< typename T>
	void read_col( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, T* data ) const;
	template< typename T>
	void write_col( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, const T* data ) const;

	// in rare cases (i.e. TLOGICAL), can't uniquely map CFITSIO storage type to required datatype
	template< BOOST_SCOPED_ENUM_NATIVE(ColumnType) T>
	void read_col( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, NativeType<SC_BYTE>::storage_type* data ) const;
	template< BOOST_SCOPED_ENUM_NATIVE(ColumnType) T>
	void write_col( Columns::size_type colnum, LONGLONG firstrow, LONGLONG firstelem, LONGLONG nelem_, const NativeType<SC_BYTE>::storage_type* data ) const;

	void read_bytes( LONGLONG firstrow, LONGLONG offset, LONGLONG nbytes, unsigned char* data ) const;
	void write_bytes( LONGLONG firstrow, LONGLONG offset, LONGLONG nbytes, unsigned char* data ) const;

    protected:

	// disable default copy constructors
	Table( const Table& );
	Table& operator= (const Table&);
	void refresh ();
	LONGLONG row_idx_;
	Columns columns;

    };


}


#endif // ! misFITS_TABLE_H

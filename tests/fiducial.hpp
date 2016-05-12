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

#ifndef TEST_misFITS_FIDUCIAL_HPP
#define TEST_misFITS_FIDUCIAL_HPP

#include <string>
#include <vector>

#include <misfits/fits.hpp>
#include "util.hpp"

namespace misFITS_Test {

    namespace Fiducial {

	struct ColumnBase {

	    int storage_type;
	    mutable int column_type;
	    std::string ttype;
	    std::string tform;
	    std::string tdim;
	    std::string tunit;
	    mutable std::size_t colnum;
	    mutable std::size_t nbytes;
	    mutable int offset;
	    mutable std::size_t repeat;
	    mutable std::size_t nelem;

	    ColumnBase(
		       int storage_type_,
		       const std::string& type,
		       const std::string& form,
		       const std::string& dim
		       )
		: storage_type( storage_type_ ),
		  ttype(type),
		  tform(form),
		  tdim(dim),
		  tunit( type + "_unit" ),
		  offset(1)
	    {}

	    void insert ( TestFitsPtr& fpp, int offset ) const;
	    virtual void write( TestFitsPtr& fp ) const = 0;
	    virtual void normalize( ) = 0;
	};

	template<typename T>
	struct ColumnBaseT : public ColumnBase  {

	    typedef std::vector<T> VectorT;
	    typedef typename VectorT::size_type data_size_type;

	    VectorT data;

	    void push_back( T datum ) {
		data.push_back( datum );
	    }

	    ColumnBaseT(
			int storage_type,
			const std::string& type,
			const std::string& form,
			const std::string& dim
			)
		: ColumnBase( storage_type, type, form, dim ) {}


	};

	template<int storage_type, typename T>
	struct Column : public ColumnBaseT<T> {

	    typedef T ColumnT;

	    typedef ColumnBaseT<T> Parent;

	    Column(
		   const std::string& type,
		   const std::string& form,
		   const std::string& dim = ""
		   ) : ColumnBaseT<T>( storage_type, type, form, dim ) {};


	    void write ( TestFitsPtr& fpp ) const {

		misFITS_CHECK_CFITSIO_EXPR
		    ( fits_write_col( fpp.get(), storage_type,
				      static_cast<int>(Parent::colnum),
				      1, 1,
				      static_cast<LONGLONG>(Parent::data.size()),
				      const_cast<T*>(&Parent::data[0]), &status )
		      );

	    }

	    virtual void normalize( ) {}

	};

	template<int storage_type, typename T >
	struct Column< storage_type, std::vector<T> > : public ColumnBaseT< std::vector<T> > {

	    typedef T ColumnT;
	    typedef ColumnBaseT< std::vector<T> > Parent;

	    Column(
		   const std::string& type,
		   const std::string& form,
		   const std::string& dim = ""
		   ) : ColumnBaseT< std::vector<T> >( storage_type, type, form, dim ) {};


	    void write( TestFitsPtr& fpp ) const {

		for (typename Parent::data_size_type row = 1 ; row <= Parent::data.size() ; ++row ) {
		    const std::vector<T>& drow = Parent::data[row-1];

		    if ( drow.size() != Parent::nelem )
			throw misFITS::Exception::Assert( "data for vector cell has incorrect length" );

		    misFITS_CHECK_CFITSIO_EXPR
			(
			 fits_write_col( fpp.get(), storage_type,
					 static_cast<int>(Parent::colnum),
					 static_cast<LONGLONG>(row),
					 1,
					 static_cast<LONGLONG>(Parent::nelem),
					 const_cast<T*>(&drow[0]), &status )
			 );

		}

	    }

	    virtual void normalize( ) {}


	};

	template<> void Column< TSTRING, std::string >::write( TestFitsPtr& fp ) const;
	template<> void	Column< TSTRING, std::vector<std::string> >::write( TestFitsPtr& fp ) const;

	template<> void Column< TSTRING, std::string>::normalize ();
	template<> void Column< TSTRING, std::vector<std::string> >::normalize( );

	template<> void Column< TLOGICAL, bool >::write( TestFitsPtr& fp ) const;
	template<> void	Column< TLOGICAL, std::vector<bool> >::write( TestFitsPtr& fp ) const;



    }
}

#endif // ! TEST_misFITS_FIDUCIAL_HPP

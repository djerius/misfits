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

#ifndef TEST_misFITS_FIDUCIAL_DATA_H
#define TEST_misFITS_FIDUCIAL_DATA_H

#include <string>
#include <vector>

#include "fiducial.hpp"

namespace misFITS_Test {


  namespace Fiducial {

      struct Data {

	  static const std::size_t nrows;
	  static const std::size_t nbits;
	  static std::size_t nbytes;

	  Column<TSHORT , short>		i1;
	  Column<TSHORT , std::vector<short> >  iv1;

	  Column<TINT   , int>			j1;
	  Column<TINT   , std::vector<int> >    jv1;

	  Column<TFLOAT , float>		e1;
	  Column<TFLOAT , std::vector<float> >	ev1;

	  Column<TDOUBLE, double>		d1;
	  Column<TDOUBLE, std::vector<double> > dv1;

	  Column<TLOGICAL, bool>		l1;
	  Column<TLOGICAL, std::vector<bool> >  lv1;


	  // we're writing bits as a string of bytes
	  Column<TBYTE  , std::vector<misFITS::byte_t> >	x1;

	  // this doesn't get written; it's just there for comparison
	  Column<TBIT   , misFITS::BitSet>      x2;

	  Column<TSTRING, std::string>		a1;
	  Column<TSTRING, std::vector<std::string> >	a2;
	  Column<TSTRING, std::vector<std::string> >	a3;

	  Column<TSTRING, std::string>		a4;
	  Column<TSTRING, std::vector<std::string> >	a5;
	  Column<TSTRING, std::vector<std::string> >	a6;


	  typedef std::vector<ColumnBase*> Cols;
	  Cols columns;
	  Cols::size_type num_columns () { return columns.size() ; }

	  Data();

	  void  insert_columns( TestFitsPtr& );
	  void  write( TestFitsPtr& ) const;
	  void  normalize_data( );

	  static std::string Xform();
      };

  }

}

#endif // ! TEST_misFITS_FIDUCIAL_DATA_H

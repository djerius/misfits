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

#ifndef TEST_misFITS_COLUMNS_H
#define TEST_misFITS_COLUMNS_H

#include <string>
#include <vector>

#include "util.hpp"

namespace misFITS_Test {

  struct Columns {

    std::vector< std::string > ttype;
    std::vector< std::string > tform;
    std::vector< std::string > tunit;
    std::vector< std::string > tdim;


    void add( const char* type,
              const char* form,
              const char *unit = "",
              const char* tdim = ""
              );

    TestFitsPtr create( std::string filename = "" );
    int ncols() { return ttype.size() ; }
  };

  struct ColInfo {

    char tdim[80];
    char ttype[80];
    char tunit[80];
    char typechar[80];
    char tdisp[80];
    LONGLONG repeat;
    double scale;
    double zero;
    LONGLONG nulval;

    int typecode;
    LONGLONG width;

    LONGLONG naxes[10];
    int naxis;

    ColInfo( const TestFitsPtr& fp, int colnum );

  };



}


#endif // ! TEST_misFITS_COLUMNS_H

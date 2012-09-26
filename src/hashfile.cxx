/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2012
 
  This file is part of ticclopstools

  ticclopstools is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  ticclopstools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      
  or send mail to:
      
*/

#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <cstdlib>
#include "ticclops/Hasher.h"

using namespace std;

void usage(){
  cerr << "usage: hasfile input [output]" << endl;
  exit( EXIT_SUCCESS );
}

int main( const int argc, const char* argv[] ){
  if ( argc < 2 || argc > 3 )
    usage();
  ifstream is( argv[1] );
  if ( !is ){
    cerr << "couldn't open inputfile: " << argv[1] << endl;
    exit( EXIT_FAILURE );
  }
  ostream *os = 0;
  if ( argc == 3 ){
    os = new ofstream( argv[2] );
    if ( !*os ){
      cerr << "couldn't open " << argv[2] << endl;
      exit( EXIT_FAILURE );
    }
  }
  else
    os = &cout;
  Hasher hash;
  hash.hash_file( is );
  hash.output( *os );
}

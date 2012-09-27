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

#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "ticcutils/StringOps.h"
#include "timbl/TimblAPI.h"
#include "ticclops/Hasher.h"

using namespace std;

BigInt Hasher::hash( const string& word ){
  BigInt hash = 0;
  if ( word.empty() )
    return 0;
  for( string::size_type i=0; i < word.length(); ++i ){
    int n = word[i];
    hash += n * n * n * n * n;
  }
  map<BigInt,string>::iterator it = hash_map.find( hash );
  if ( it == hash_map.end() ){
    hash_map[hash] = word;
  }
  else {
    hash_map[hash] += "#" + word;
  }
}

bool Hasher::fill( istream& is ){
  string line;
  while ( getline( is, line ) ) {
    vector<string> v;
    int num = TiCC::split_at( line, v , "~" );
    if ( num < 2 ){
      cerr << "encountered a very strange line '" << line << "'" << endl;
      return false;
    }
    if ( num > 2 ){
      for ( size_t i=2; i < num; ++i ){
	v[1] += "~" + v[i];
      }
    }
    BigInt hash;
    if ( !Timbl::stringTo( v[0], hash ) ){
      cerr << "illegal hash " << v[0] << "in " << line << endl;
      return false;
    }
    hash_map[hash] = v[1];
  }
  return true;
}


void Hasher::hash_file( istream& is ){
  string word;
  while ( getline( is, word ) ){
    hash( word );
  }
}

void Hasher::output( ostream& os ){
  map<BigInt,string>::iterator it = hash_map.begin();
  while ( it != hash_map.end() ){
    os << it->first << "~" << it->second << endl;
    ++it;
  }
}

string Hasher::lookup( BigInt hash ) const {
  map<BigInt, string>::const_iterator it = hash_map.find( hash );
  if ( it == hash_map.end() )
    return "#";
  else
    return it->second;
}

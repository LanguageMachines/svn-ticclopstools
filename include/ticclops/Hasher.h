/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2014
 
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

#ifndef TIICLOPS_HASHER_H
#define TIICLOPS_HASHER_H

typedef unsigned long long BigInt;

class Hasher {
 public:
  BigInt hash( const std::string& );
  void hash_file( std::istream& );
  void output( std::ostream& );
  bool fill( std::istream& is );
  std::string lookup( BigInt h ) const;
 private:
  std::map<BigInt,std::string> hash_map;
};

#endif

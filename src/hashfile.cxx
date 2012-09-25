#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

using namespace std;

void usage(){
  cerr << "usage: hasfile input [output]" << endl;
  exit( EXIT_SUCCESS );
}

typedef unsigned long int BigInt;

map<BigInt, string> hash_map;

void hash( istream& is ){
  string line;
  while ( getline( is, line ) ){
    if ( line.empty() )
      continue;
    BigInt hash = 0;
    for( string::size_type i=0; i < line.length(); ++i ){
      int n = line[i];
      hash += n * n * n * n * n;
    }
    map<BigInt,string>::iterator it = hash_map.find( hash );
    if ( it == hash_map.end() ){
      hash_map[hash] = line;
    }
    else {
      hash_map[hash] += "#" + line;
    }
  }
}


void output( ostream& os ){
  map<BigInt,string>::iterator it = hash_map.begin();
  while ( it != hash_map.end() ){
    os << it->first << "~" << it->second << endl;
    ++it;
  }
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
  hash( is );
  output( *os );
}

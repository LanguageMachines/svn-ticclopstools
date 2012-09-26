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

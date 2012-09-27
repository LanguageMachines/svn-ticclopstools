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

#include <csignal>
#include <cerrno>
#include <string>
#include <map>
#include "config.h"
#include "timblserver/TimblServerAPI.h"
#include "timblserver/FdStream.h"
#include "ticclops/AnaServer.h"

using namespace std;
using namespace TiCC;

#define SLOG (*Log(cur_log))
#define SDBG (*Dbg(cur_log))

#define LOG cerr

inline void usage(){
  cerr << "usage:  anaserv -S port [-f hashfile]" << endl;
  cerr << " or     anaserv -S port [-l lexiconfile]" << endl;
  exit( EXIT_SUCCESS );
}

AnaServerClass::AnaServerClass( Timbl::TimblOpts& opts ):
  cur_log("AnaServ", StampMessage ){
  string val;
  bool mood;
  if ( opts.Find( 'D', val, mood ) ){
    if ( val == "LogNormal" )
      cur_log.setlevel( LogNormal );
    else if ( val == "LogDebug" )
      cur_log.setlevel( LogDebug );
    else if ( val == "LogHeavy" )
      cur_log.setlevel( LogHeavy );
    else if ( val == "LogExtreme" )
      cur_log.setlevel( LogExtreme );
    else {
      cerr << "Unknown Debug mode! (-D " << val << ")" << endl;
    }
    opts.Delete( 'D' );
  }
  if ( opts.Find( 'S', val, mood ) ){
    if ( !Timbl::stringTo( val, serverPort ) ){
      cerr << "illegal value for option -S" << endl;
      usage();
    }
  }
  else {
    cerr << "Missing value for option -S" << endl;
  }
  if ( !opts.Find( 'm', val, mood ) ){
    if ( !Timbl::stringTo( val, maxConn ) ){
      cerr << "illegal value for option -m" << endl;
      usage();
    }
    else
      maxConn = 20;
  }
  if ( opts.Find( "f", val, mood ) ){
    ifstream is( val.c_str() );
    if ( !is ){
      cerr << "couldn't open hashes file: " << val << endl;
      exit( EXIT_FAILURE );
    }
    hash.fill( is );    
  }
  else if ( opts.Find( "l", val, mood ) ){
    ifstream is( val.c_str() );
    if ( !is ){
      cerr << "couldn't open lexicon file: " << val << endl;
      exit( EXIT_FAILURE );
    }
    hash.hash_file( is );
  }
  else {
    cerr << "Missing option -l or -f " << endl;
    usage();
  }
  cerr << "Anagram Server " << VERSION << endl;
  cerr << "based on " << TimblServer::VersionName() << endl;
  doDaemon = true;
  dbLevel = LogNormal;
  if ( opts.Find( "pidfile", val ) ) {
    pidFile = val;
    opts.Delete( "pidfile" );
  }
  if ( opts.Find( "logfile", val ) ) {
    logFile = val;
    opts.Delete( "logfile" );
  }
  if ( opts.Find( "daemonize", val ) ) {
    doDaemon = ( val != "no" && val != "NO" && val != "false" && val != "FALSE" );
    opts.Delete( "daemonize" );
  }
  RunServer();
}

AnaServerClass::~AnaServerClass(){
}

struct childArgs{
  AnaServerClass *Mother;
  Sockets::ServerSocket *socket;
  int maxC;
};

void StopServerFun( int Signal ){
  if ( Signal == SIGINT ){
    exit(EXIT_FAILURE);
  }
  signal( SIGINT, StopServerFun );
}  

void BrokenPipeChildFun( int Signal ){
  if ( Signal == SIGPIPE ){
    signal( SIGPIPE, BrokenPipeChildFun );
  }
}

void AfterDaemonFun( int Signal ){
  if ( Signal == SIGCHLD ){
    exit(1);
  }
}

void AnaServerClass::exec( const string& line, ostream& os ){
  BigInt hash;
  if ( !Timbl::stringTo( line, hash ) ){
    os << "ERROR: invalid hash. Must be numeric!" << endl;
  }
  else {
    os << lookup( hash ) << endl;
  }
}

// ***** This is the routine that is executed from a new thread **********
void *runChild( void *arg ){
  childArgs *args = (childArgs *)arg;
  AnaServerClass *theServer = args->Mother;
  LogStream *cur_log = &theServer->cur_log;
  Sockets::Socket *Sock = args->socket;
  static int service_count = 0;
  static pthread_mutex_t my_lock = PTHREAD_MUTEX_INITIALIZER;
  //
  // use a mutex to update the global service counter
  //
  pthread_mutex_lock( &my_lock );
  service_count++;
  int nw = 0;
  if ( service_count > args->maxC ){
    Sock->write( "Maximum connections exceeded\n" );
    Sock->write( "try again later...\n" );
    pthread_mutex_unlock( &my_lock );
    SLOG << "Thread " << pthread_self() << " refused " << endl;
  }
  else {
    // Greeting message for the client
    //
    pthread_mutex_unlock( &my_lock );
    time_t timebefore, timeafter;
    time( &timebefore );
    // report connection to the server terminal
    //
    SLOG << "Thread " << pthread_self() << ", Socket number = "
	 << Sock->getSockId() << ", started at: " 
	 << asctime( localtime( &timebefore) );
    signal( SIGPIPE, BrokenPipeChildFun );
    fdistream is( Sock->getSockId() );
    fdostream os( Sock->getSockId() );
    os << "Welcome to the Anagram server." << endl;
    string line;
    while ( getline( is, line ) ){
      line = TiCC::trim( line );
      SDBG << "Line='" << line << "'" << endl;
      theServer->exec( line, os );
    }
    time( &timeafter );
    SLOG << "Thread " << pthread_self() << ", terminated at: " 
	 << asctime( localtime( &timeafter ) );
    SLOG << "Total time used in this thread: " << timeafter - timebefore 
	 << " sec, " << nw << " words processed " << endl;
  }
  // exit this thread
  //
  pthread_mutex_lock( &my_lock );
  service_count--;
  SLOG << "Socket total = " << service_count << endl;
  pthread_mutex_unlock( &my_lock );
  delete Sock;
  return 0;
}

bool AnaServerClass::RunServer(){
  cerr << "trying to start a Server on port: " << serverPort << endl
       << "maximum # of simultaneous connections: " << maxConn
       << endl;
  if ( !logFile.empty() ){
    ostream *tmp = new ofstream( logFile.c_str() );
    if ( tmp && tmp->good() ){
      cerr << "switching logging to file " << logFile << endl;
      cur_log.associate( *tmp );
      cur_log.message( "T-Scan:" );
      SLOG << "Started logging " << endl;	
    }
    else {
      cerr << "unable to create logfile: " << logFile << endl;
      cerr << "not started" << endl;
      exit(EXIT_FAILURE);
    }
  }
  int start = 0;
  if ( doDaemon ){
    signal( SIGCHLD, AfterDaemonFun );
    if ( logFile.empty() )
      start = TimblServer::daemonize( 1, 1 );
    else
      start = TimblServer::daemonize( 0, 0 );
  }
  if ( start < 0 ){
    LOG << "Failed to daemonize error= " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  };
  if ( !pidFile.empty() ){
    // we have a liftoff!
    // signal it to the world
    remove( pidFile.c_str() ) ;
    ofstream pid_file( pidFile.c_str() ) ;
    if ( !pid_file ){
      LOG << "Unable to create pidfile:"<< pidFile << endl;
      LOG << "TimblServer NOT Started" << endl;
      exit(EXIT_FAILURE);
    }
    else {
      pid_t pid = getpid();
      pid_file << pid << endl;
    }
  }
  
  // set the attributes
  pthread_attr_t attr;
  if ( pthread_attr_init(&attr) ||
       pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED ) ){
    LOG << "Threads: couldn't set attributes" << endl;
    exit(EXIT_FAILURE);
  }
  //
  // setup Signal handling to abort the server.
  signal( SIGINT, StopServerFun );
  
  pthread_t chld_thr;
  
  // start up server
  // 
  LOG << "Started Server on port: " << serverPort << endl
      << "Maximum # of simultaneous connections: " << maxConn
      << endl;
  
  Sockets::ServerSocket server;
  string portString = Timbl::toString<int>(serverPort);
  if ( !server.connect( portString ) ){
    LOG << "Failed to start Server: " << server.getMessage() << endl;
    exit(EXIT_FAILURE);
  }
  if ( !server.listen( maxConn ) < 0 ){
    LOG << "Server: listen failed " << strerror( errno ) << endl;
    exit(EXIT_FAILURE);
  };
  
  while(true){ // waiting for connections loop
    Sockets::ServerSocket *newSock = new Sockets::ServerSocket();
    if ( !server.accept( *newSock ) ){
      if( errno == EINTR )
	continue;
      else {
	LOG << "Server: Accept Error: " << server.getMessage() << endl;
	exit(EXIT_FAILURE);
      }
    }
    LOG << "Accepting Connection " << newSock->getSockId()
	<< " from remote host: " << newSock->getClientName() << endl;
    
    // create a new thread to process the incoming request 
    // (The thread will terminate itself when done processing
    // and release its socket handle)
    //
    childArgs *args = new childArgs();
    args->Mother = this;
    args->maxC = maxConn;
    args->socket = newSock;
    pthread_create( &chld_thr, &attr, runChild, (void *)args );
    // the server is now free to accept another socket request 
  }
}
  
int main(int argc, char *argv[]) {
  Timbl::TimblOpts opts( argc, argv );
  string val;
  bool mood;
  if ( opts.Find( "h", val, mood ) ||
       opts.Find( "help", val, mood ) ){
    usage();
  }
  if ( opts.Find( "V", val, mood ) ||
       opts.Find( "version", val, mood ) ){
    exit( EXIT_SUCCESS );
  }
  AnaServerClass server( opts );
  exit(EXIT_SUCCESS);
}


#ifndef ANASERVER_SERVER_H
#define ANASERVER_SERVER_H
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

#include "ticcutils/LogStream.h"
#include "ticclops/Hasher.h"

class AnaServerClass {
 public:
  AnaServerClass( Timbl::TimblOpts& opts );
  ~AnaServerClass();
  void Start( Timbl::TimblOpts& Opts );
  void exec( const std::string&, std::ostream& );
  TiCC::LogStream cur_log;
  std::string lookup( BigInt h ) const { return hash.lookup( h ); };
 private:
  bool RunServer();
  std::string configFile;
  std::string pidFile;
  std::string logFile;
  bool doDaemon;
  int serverPort;
  int maxConn;
  LogLevel dbLevel;
  Hasher hash;
};

#endif

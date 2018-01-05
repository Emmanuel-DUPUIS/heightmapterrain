//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include <ostream>

// Activates the auto-validation code
#define __CheckCodeValidity

#define __CheckGLErrors UxError::displayGLErrors(__FILE__,__LINE__)
#define __Assert(message) UxError::assertion(__FILE__,__LINE__,true , message)
#define __AssertIfNot(cond,message) UxError::assertion(__FILE__,__LINE__,cond, message)

class UxError
{
private:
  
  static char* _Cwd;
  static const char* getCwd();
  static std::string removeCwd(const char* iFilenName);

public:

  __DeclareDeletedCtor(UxError);

  static std::ostream& error(const char *file, int line);
  static std::ostream& warning(const char *file, int line);
  static void displayGLErrors(const char *file, int line);
  static void assertion(const char *file, int line, bool iConditionToFulfill, const std::string& iReason);
  static void UxError::exit(int32_t iCode);
};


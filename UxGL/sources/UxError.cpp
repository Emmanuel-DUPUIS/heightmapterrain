//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxError.h"

#include <direct.h>
#include <iostream>
#include <gl/glew.h>
#include <vector>

#define __error(e) {e, #e}

char* UxError::_Cwd = nullptr;

const char* UxError::getCwd()
{
  if (!_Cwd)
  {
    _Cwd = new char[2048];
    _getcwd(_Cwd, 2048);
    char *p = _Cwd;
    while (*p)
    {
      if (*p > 64 && *p < 91)
        *p += 32;
      p++;
    }
  }
  return _Cwd;
}


std::string UxError::removeCwd(const char* iFileName)
{
  const char* cwd = getCwd();

  const char *pCwd = cwd;
  const char *pFileName = iFileName;
  while (*pCwd != '\0' && pFileName != '\0' && *pCwd == *pFileName)
  {
    pCwd++;
    pFileName++;
  }

  if (*pCwd == '\0')
    return std::string(pFileName);

  std::string path;
  while (*pCwd != '\0')
  {
    if (*pCwd == '\\' || *pCwd == '/')
      path += ".." + *pCwd;
    pCwd++;
  }
  path += "..\\";
  return path + pFileName;
}

std::ostream& UxError::error(const char *file, int line)
{
  return std::cerr << "ERROR[" << removeCwd(file).c_str() << ":" << line << "]: ";
}

std::ostream& UxError::warning(const char *file, int line)
{
  return std::cerr << "WARNING[" << removeCwd(file).c_str() << ":" << line << "]: ";
}

void UxError::displayGLErrors(const char *file, int line)
{
  static std::vector<std::pair<GLenum, const char*>> errors = { __error(GL_INVALID_ENUM), __error(GL_INVALID_VALUE), __error(GL_INVALID_OPERATION), __error(GL_STACK_OVERFLOW), __error(GL_STACK_UNDERFLOW), __error(GL_OUT_OF_MEMORY), __error(GL_INVALID_FRAMEBUFFER_OPERATION), __error(GL_CONTEXT_LOST), __error(GL_TABLE_TOO_LARGE) };

  GLenum errorNumber = glGetError();

  while (errorNumber != GL_NO_ERROR)
  {
    const char *errorLabel = "Unknown type";
    auto it = errors.begin();
    for (; it != errors.end(); it++)
    {
      if (errorNumber == it->first)
      {
        errorLabel = it->second;
        break;
      }
    }

    error(file, line) << errorLabel << "' (" << (it!= errors.end()?it->first:0) << ")\n";
    errorNumber = glGetError();
  }
}

void UxError::assertion(const char *file, int line, bool iConditionToFulfill, const std::string& iReason)
{
  if (!iConditionToFulfill)
  {
    std::cerr << "WARNING[" << removeCwd(file).c_str() << ":" << line << "]: " << iReason.c_str() << "\n";
    UxError::exit(-33);
  }
}

void UxError::exit(int32_t iCode)
{
  ::exit(iCode);
}
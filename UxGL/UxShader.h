//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"

#include <gl/glew.h>
#include <string>
#include <vector>

class UxReportBase;


//========================================================================
//  Shader encapsulation:
//    Manages shader loading, report integration.
//========================================================================

class UxShader
{
private:
  GLenum                     _Type;
  GLuint                     _GLName;
  std::vector<UxReportBase*> _Reports;

public:
  
  UxShader(GLenum iType, const std::vector<std::string>& iFiles);
  UxShader(UxShader&& source);
  UxShader& operator =(UxShader&& source);
  ~UxShader();
  __DeclareDeletedRefCtorsAndAssignments(UxShader)  
  
  GLuint getGLName() const { return _GLName; }

  std::vector<UxReportBase*>::const_iterator beginReport() const { return _Reports.begin(); }
  std::vector<UxReportBase*>::const_iterator endReport() const { return _Reports.end(); }


protected:

  void load(std::vector<std::string> iFileNames, bool iCheckErrors = true);
  void displayBuffer(char *pBuffer, std::vector<uint32_t> iNbLines, std::vector<std::string> iFileNames);
};

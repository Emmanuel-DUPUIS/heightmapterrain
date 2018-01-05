//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxShaderStorageBase.h"

//========================================================================
//  GPU report:
//    Manages data report of GPU data browsable in CPU (debug session).
//========================================================================

class UxReportBase
{
private:
  std::string                          _ReportName;
  std::string                          _StructureName;
  uint32_t                             _BufferSize;
  std::string                          _ReportStructureFilePath;
  std::shared_ptr<UxShaderStorageBase> _ShaderStorage;

protected:
  uint32_t                             _MaxRecordNumber;

public:

  UxReportBase(const std::string& iReportName, const std::string& iStructureName, size_t iBufferSize, const std::string& iGLSLFilePath, uint32_t iMaxRecordNumber);
  virtual ~UxReportBase();
  __DeclareDeletedCtorsAndAssignments(UxReportBase)

  const std::string& getName() const { return _ReportName; }
  std::string        getSSBOName() const { return "UxReport_" + _ReportName; }
  std::string        getGLSLFileName() const { return _ReportStructureFilePath + ".glsl"; }
  uint32_t           getMaxRecordNumber() const { return _MaxRecordNumber; }
  
  void bindToProgram(const UxProgram* iProgram);

protected:
  UxShaderStorageBase* getShaderStorage();
  void map();
  void unmap();

  void generateGLSL() const;
};

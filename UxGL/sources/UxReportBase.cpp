//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxReportBase.h"

#include "UxReportManager.h"
#include "UxUtils.h"
#include "UxError.h"

#include <cassert>
#include "UxUtils.h"


UxReportBase::UxReportBase(const std::string& iReportName, const std::string& iStructureName, size_t iBufferSize, const std::string& iGLSLFilePath, uint32_t iMaxRecordNumber)
{
  _ReportName    = iReportName;
  _StructureName = iStructureName;
  _BufferSize    = iBufferSize;

  _ReportStructureFilePath = iGLSLFilePath;
  _MaxRecordNumber         = iMaxRecordNumber;

  UxReportManager::addReport(this);

  generateGLSL();
}

UxReportBase::~UxReportBase()
{
  UxReportManager::removeReport(_ReportName);
}


UxShaderStorageBase* UxReportBase::getShaderStorage()
{
  if (!_ShaderStorage)
    _ShaderStorage = std::make_shared<UxShaderStorageBase>(_ReportName, GL_DYNAMIC_READ, -1, _StructureName, _BufferSize);

  return _ShaderStorage.get();
}


void UxReportBase::bindToProgram(const UxProgram* iProgram)
{
  std::string name = "Struct_" + getSSBOName();
  getShaderStorage()->bindToProgram(iProgram, name);
}

void UxReportBase::map()
{
  getShaderStorage()->map();
}

void UxReportBase::unmap()
{
  getShaderStorage()->unmap();
}

void UxReportBase::generateGLSL() const
{
  FILE* fpW = nullptr;
  if (fopen_s(&fpW, getGLSLFileName().c_str(), "w"))
  {
    UxError::error(__FILE__, __LINE__) << " Can't open file " << getGLSLFileName() << " in 'w' mode.\n";
    UxError::UxError::exit(-1);
  }

  FILE* fpR = nullptr;
  if (fopen_s(&fpR, _ReportStructureFilePath.c_str(), "r"))
  {
    UxError::error(__FILE__, __LINE__) << " Can't open file " << _GLOBAL_USING << " in 'r' mode.\n";
    UxError::UxError::exit(-1);
  }

  fputs((std::string("\nlayout(std430) writeonly buffer Struct_") + getSSBOName()).c_str(), fpW);
  fputs("\n{\n  struct {\n", fpW);

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), fpR) != nullptr)
    fputs(std::string("    " + UxUtils::GLSLTypeToCPlusPlus(buffer)).c_str(), fpW);
  fclose(fpR);

  fputs("\n  } data[", fpW);
  fprintf(fpW, "%d", _MaxRecordNumber);
  fputs("];\n  uint counter;", fpW);
  fputs((std::string("\n} ") + getSSBOName() + ";\n").c_str(), fpW);

  fclose(fpW);
}


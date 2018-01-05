//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxShaderStorageBase.h"


//========================================================================
//  Shader Storage Buffer encapsulation:
//    Manages shader storage with structure specification as a template
//    class.
//========================================================================

template<typename tpStorageStructure>
class UxShaderStorage: public UxShaderStorageBase
{
public:
  UxShaderStorage(const std::string& iName, GLenum iUsage, uint32_t iArraySize = 1, GLint iBinding = -1);
  ~UxShaderStorage() {};
  __DeclareDeletedCtorsAndAssignments(UxShaderStorage)

  template<typename tpStorageStructure>
  friend class UxShaderStorageDataAccessor;

protected:
  tpStorageStructure* map(GLbitfield iAccess = GL_MAP_WRITE_BIT);
};

template<typename tpStorageStructure>
UxShaderStorage<tpStorageStructure>::UxShaderStorage(const std::string& iName, GLenum iUsage, uint32_t iArraySize, GLint iBinding)
: UxShaderStorageBase(iName, iBinding, typeid(tpStorageStructure).name(), iArraySize * sizeof(tpStorageStructure), iUsage)
{
}

template<typename tpStorageStructure>
tpStorageStructure* UxShaderStorage<tpStorageStructure>::map(GLbitfield iAccess)
{
  UxShaderStorageBase::map();
  tpStorageStructure* pMappedData = (tpStorageStructure*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _BufferSize, iAccess);
  __CheckGLErrors;
  return pMappedData;
}

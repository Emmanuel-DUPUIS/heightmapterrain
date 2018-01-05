//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxError.h"
#include "UxProgram.h"

#include <cassert>
#include "gl/glew.h"


//========================================================================
//  Shader Storage Buffer Object encapsulation:
//    Manages automatic allocation resource (binding). To be derived with
//    class template to specify the data structure of the storage.
//========================================================================

class UxShaderStorageBase
{
  typedef class UxResourceAllocator<UxShaderStorageBase, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS> allocator;

private:
  std::string _Name;
  GLuint      _Buffer;
  GLuint      _Binding;
  std::string _StructureName;
  size_t      _BufferSize;

protected:
  bool        _IsMapped;

public:
  UxShaderStorageBase(const std::string& iName, GLenum iUsage, int32_t iBinding, const std::string& iStructureName, size_t iBufferSize);
  virtual ~UxShaderStorageBase();
  __DeclareDeletedCtorsAndAssignments(UxShaderStorageBase)

  uint32_t            getLocation() const { return _Binding; }
  const std::string&  getName() const { return _Name; }

  void bindToProgram(const UxProgram* iProgram, const std::string& iStorageName);

  friend class UxReportBase;

protected:
  void map();
  void unmap();
};

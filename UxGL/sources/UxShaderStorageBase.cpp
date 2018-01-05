//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxShaderStorageBase.h"

UxShaderStorageBase::UxShaderStorageBase(const std::string& iName, GLenum iUsage, int32_t iBinding, const std::string& iStructureName, size_t iBufferSize)
{
  _Name          = iName;
  _Buffer        = 0;
  _Binding       = allocator::allocate(this, _Name, iBinding);
  _StructureName = iStructureName;
  _BufferSize    = iBufferSize;
  _IsMapped      = false;

  glGenBuffers(1, &_Buffer);
  __CheckGLErrors;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, _Buffer);
  __CheckGLErrors;
  glBufferData(GL_SHADER_STORAGE_BUFFER, _BufferSize, nullptr, iUsage);
  __CheckGLErrors;
  glBindBufferRange(GL_UNIFORM_BUFFER, _Buffer, _Binding, 0, _BufferSize);
  __CheckGLErrors;
}

UxShaderStorageBase::~UxShaderStorageBase()
{
  glDeleteBuffers(1, &_Buffer);
  _Buffer = 0;
}

void UxShaderStorageBase::bindToProgram(const UxProgram* iProgram, const std::string& iStorageName)
{
  GLuint index = glGetProgramResourceIndex(iProgram->id(), GL_SHADER_STORAGE_BLOCK, iStorageName.c_str());
  __CheckGLErrors;

    if (index == -1)
    {
      UxError::error(__FILE__, __LINE__) << "No index for the shader storage resource \"" << iStorageName << "\".\n";
      UxError::exit(-1);
    }

  // Verifies that the buffer size identical to the storage block
  GLenum property = GL_BUFFER_DATA_SIZE;
  GLint size;
  GLsizei length;
  glGetProgramResourceiv(iProgram->id(), GL_SHADER_STORAGE_BLOCK, index, 1, &property, 1, &length, &size);
  if (_BufferSize != size)
  {
    UxError::error(__FILE__, __LINE__) << "Inconsistent definition between structure associated to the SSBO \"" << _StructureName
      << "\" (size=" << _BufferSize << ") and the shader storage \"" << iStorageName << "\" in program \"" << iProgram->getName()
      << "\" (size=" << size << ").\n";
    UxError::exit(-1);
  }

  glShaderStorageBlockBinding(iProgram->id(), index, _Binding);
  __CheckGLErrors;
}

void UxShaderStorageBase::map()
{
  __AssertIfNot(_Buffer && !_IsMapped, "Invalid map call of Shader Storage.");
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _Binding, _Buffer);
  __CheckGLErrors;
  _IsMapped = true;
}

void UxShaderStorageBase::unmap()
{
  __AssertIfNot(_Buffer && _IsMapped, "Invalid unmap call of Shader Storage.");
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _Binding, _Buffer);
  __CheckGLErrors;
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  __CheckGLErrors;
  _IsMapped = false;
}

//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxUniformBlock.h"
#include "UxShaderStorage.h"

#include <vector>
#include <memory>

class UxVertexInputAttribute;

//========================================================================
//  OpenGL Context Object Manager:
//    Manages list of OpenGL context objects.
//========================================================================

class UxGLObjects
{
private:
  static bool _Startup;
  static void Startup();
  static std::vector<std::shared_ptr<UxUniformBlockBase>>*      _UniformBlocks;
  static std::vector<std::shared_ptr<UxVertexInputAttribute>>*  _InputAttributes;
  static std::vector<std::shared_ptr<UxShaderStorageBase>>*     _ShaderStorages;

public:

  __DeclareDeletedCtor(UxGLObjects)

  static void addInputAttribute(const std::string& iAttributeName);
  static std::shared_ptr<UxVertexInputAttribute> getInputAttribute(const std::string& iAttributeName);

  template<typename tpUBOStruct>
  static void addUniformBlock(const std::string& iUniformName, GLenum iUsage = GL_DYNAMIC_DRAW);

  template<typename tpSSBOStruct>
  static void addStorageBlock(const std::string& iStorageName, GLenum iUsage);

  static std::shared_ptr<UxUniformBlockBase> getUniformBlock(const std::string& iUniformName);

  template<typename tpUBOStruct>
  static std::shared_ptr<UxUniformBlock<tpUBOStruct>> getUniformBlock(const std::string& iUniformName);

  static std::shared_ptr<UxShaderStorageBase> getShaderStorage(const std::string& iStorageName);

  template<typename tpSSBOStruct>
  static std::shared_ptr<UxShaderStorage<tpSSBOStruct>> getShaderStorage(const std::string& iStorageName);
};

template<typename tpUBOStruct>
void UxGLObjects::addUniformBlock(const std::string& iUniformName, GLenum iUsage)
{
  Startup();

  UxUniformBlock<tpUBOStruct>* pBlock = new UxUniformBlock<tpUBOStruct>(iUniformName, iUsage);
  _UniformBlocks->push_back(std::shared_ptr<UxUniformBlockBase>(pBlock));
}

template<typename tpUBOStruct>
std::shared_ptr<UxUniformBlock<tpUBOStruct>> UxGLObjects::getUniformBlock(const std::string& iBlockName)
{
  Startup();

  for (auto it = _UniformBlocks->cbegin(); it != _UniformBlocks->cend(); it++)
  {
    if ((*it)->getName() == iBlockName)
      return std::dynamic_pointer_cast<UxUniformBlock<tpUBOStruct>>(*it);
  }

  __Assert(std::string("Uniform block \"") + iBlockName + "\" not (yet) registered.");

    return nullptr;
}

template<typename tpSSBOStruct>
void UxGLObjects::addShaderStorage(const std::string& iStorageName, GLenum iUsage)
{
  Startup();

  UxShaderStorage<tpSSBOStruct>* pStorage = new UxShaderStorage<tpSSBOStruct>(iStorageName, iUsage);
  _ShaderStorages->push_back(std::shared_ptr<UxShaderStorageBase>(pStorage));
}

template<typename tpSSBOStruct>
std::shared_ptr<UxShaderStorage<tpSSBOStruct>> UxGLObjects::getShaderStorage(const std::string& iStorageName)
{
  Startup();

  for (auto it = _ShaderStorages->cbegin(); it != _ShaderStorages->cend(); it++)
  {
    if ((*it)->getName() == iStorageName)
      return std::dynamic_pointer_cast<UxShaderStorage<tpSSBOStruct>>(*it);
  }

  __Assert(std::string("Shader storage \"") + iStorageName + "\" not (yet) registered.");

  return nullptr;
}

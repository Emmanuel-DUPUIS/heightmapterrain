//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxGLObjects.h"

#include "UxVertexInputAttribute.h"
#include "UxUniformBlockBase.h"

bool UxGLObjects::_Startup = false;
std::vector<std::shared_ptr<UxUniformBlockBase>>*      UxGLObjects::_UniformBlocks   = nullptr;
std::vector<std::shared_ptr<UxVertexInputAttribute>>*  UxGLObjects::_InputAttributes = nullptr;

void UxGLObjects::Startup()
{
  if (!_Startup)
  {
    _UniformBlocks   = new std::vector<std::shared_ptr<UxUniformBlockBase>>();
    _InputAttributes = new std::vector<std::shared_ptr<UxVertexInputAttribute>>();

    _Startup = true;
  }
}

void UxGLObjects::addInputAttribute(const std::string& iAttributeName)
{
  Startup();

  UxVertexInputAttribute* pAttribute = new UxVertexInputAttribute(iAttributeName);
  _InputAttributes->push_back(std::shared_ptr<UxVertexInputAttribute>(pAttribute));
}

std::shared_ptr<UxVertexInputAttribute> UxGLObjects::getInputAttribute(const std::string& iAttributeName)
{
  Startup();

  for (auto it = _InputAttributes->cbegin(); it != _InputAttributes->cend(); it++)
  {
    if ((*it)->getName() == iAttributeName)
      return *it;
  }

  __Assert(std::string("Vertex Input Attribute \"") + iAttributeName + "\" not (yet) registered.");
  return std::shared_ptr<UxVertexInputAttribute>();
}

std::shared_ptr<UxUniformBlockBase> UxGLObjects::getUniformBlock(const std::string& iBlockName)
{
  Startup();

  for (auto it = _UniformBlocks->cbegin(); it != _UniformBlocks->cend(); it++)
  {
    if ((*it)->getName() == iBlockName)
      return *it;
  }

  __Assert(std::string("Uniform Block \"") + iBlockName + "\" not (yet) registered.");
  return nullptr;
}

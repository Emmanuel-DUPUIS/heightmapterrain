//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxVertexInputAttribute.h"
#include "UxError.h"

#include <cassert>
#include <gl/glew.h>


UxVertexInputAttribute::UxVertexInputAttribute(const std::string& iName, int32_t iLocation)
{
  _Name     = iName;
  _Location = allocator::allocate(this, iName, iLocation);
}

UxVertexInputAttribute::~UxVertexInputAttribute()
{
  allocator::remove(this);
}

std::shared_ptr<UxVertexInputAttribute> UxVertexInputAttribute::getAttribute(const std::string& iName)
{
  return allocator::getResource(iName);
}

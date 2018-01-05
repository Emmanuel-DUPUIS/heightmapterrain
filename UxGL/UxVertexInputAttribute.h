//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxResourceAllocator.h"

#include <gl/glew.h>
#include <memory>
#include <vector>

//========================================================================
//  Vertex Input Attribute encapsulation:
//    Manages automatic resource (binding) allocation.
//========================================================================

class UxVertexInputAttribute
{
  // Resource allocator
  typedef class UxResourceAllocator<UxVertexInputAttribute, GL_MAX_VERTEX_ATTRIBS> allocator;
 
private:
  uint32_t     _Location;  // From zero to _MaxAllocationNumber
  std::string  _Name;      // Generic name for the application, may differ from GLSL names used in vertex shaders 

public:
  UxVertexInputAttribute(const std::string& iName, int32_t iLocation = -1);
  ~UxVertexInputAttribute();
  __DeclareDeletedCtorsAndAssignments(UxVertexInputAttribute)

  uint32_t            getLocation() const { return _Location; }
  const std::string&  getName() const { return _Name; }

  // Retrieves attribute from its name in the allocated table
  static std::shared_ptr<UxVertexInputAttribute> getAttribute(const std::string& iName);
};

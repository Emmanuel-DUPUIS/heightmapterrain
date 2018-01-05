//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#define __DeclareDeletedCtor(__className) __className() = delete; 

#define __DeclareDeletedRefCtorsAndAssignments(__className) \
__className(const __className&) = delete; \
__className& operator= (const __className&) = delete; \
   

#define __DeclareDeletedMoveCtorsAndAssignments(__className) \
__className(__className&&) = delete; \
__className& operator= (__className&&) = delete; \


#define __DeclareDeletedCtorsAndAssignments(__className) __DeclareDeletedRefCtorsAndAssignments(__className) __DeclareDeletedMoveCtorsAndAssignments(__className)

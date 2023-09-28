/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2010, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#pragma once

#include "TMVerDefs.h"		//Target specific version defines.
#include "CopyrightDefs.h"

#define _STRINGIFY(X) #X

#define STRINGIFY(X) _STRINGIFY(X)

#define __SN_TODO__ __FILE__ "("STRINGIFY(__LINE__)"): " __FUNCTION__ ": TODO: "
#define __SN_TODO2__ __FILE__ "("STRINGIFY(__LINE__)"): TODO: "

//////////////////////////////////////////////////////////////////////////
//
// This is the main VERSION of Target Manager and it's associated 
// components.
//
//////////////////////////////////////////////////////////////////////////

#define TM_BUILD_DESCRIPTION PLATFORM " TM v" STRINGIFY(TM_VERSION_DOTS) BRANCH_HEAD

#define TM_VERSION TM_MAJOR_VERSION,TM_FEATURE_VERSION,TM_BUILD_NUMBER,TM_INTERNAL_NUMBER

#define STR_TM_VERSION STRINGIFY(TM_VERSION)

#define TM_VERSION_DOTS TM_MAJOR_VERSION.TM_FEATURE_VERSION.TM_BUILD_NUMBER.TM_INTERNAL_NUMBER

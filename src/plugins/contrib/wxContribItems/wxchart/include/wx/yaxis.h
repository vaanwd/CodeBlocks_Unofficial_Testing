/////////////////////////////////////////////////////////////////////////////
// Name:        yaxis.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXYAXIS_H__ )
#define __WXYAXIS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "yaxis.h"
#endif

#include "wx/axis.h"

//+++-S-cd-------------------------------------------------------------------
//	NAME:		CWxXAxis
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
/* C::B begin */
class WXDLLIMPEXP_CHART wxYAxis : public wxAxis
/* C::B end */
{

    public:
        wxYAxis(ChartValue max = 0, ChartValue min = 0);
        ~wxYAxis() {};

        // Draw xaxis area
        //-----------------
        void Draw(CHART_HPAINT hp, CHART_HRECT hr);
};

#endif // __WXXAXIS_H__

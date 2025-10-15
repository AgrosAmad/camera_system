/****************************************************************************
 *
 * Active Silicon
 *
 * Project     : SDK
 * Description : C++ wrapper class for PHX library calls
 * Updated     : 30-May-2017
 *
 * Copyright (c) 2019 Active Silicon
 ****************************************************************************
 * Comments:
 * --------
 * Header file of the C++ wrapper class.
 *
 ****************************************************************************
 */

#ifndef _PHX_CORE_H
#define _PHX_CORE_H

#if defined _PHX_MACOSX
#import <ASLPhoenix/phx_api.h>
#else
#if defined _TMG_WINDOWS
#include "tmg2_api.h"
#endif
#include "phx_api.h"
#endif

#include <vector>
#include <stdexcept>

class CPhxCore
{
public:
   CPhxCore();
   ~CPhxCore();

   typedef void (PHX_C_CALL *tFnErrorHandlerFn) (const char*, etStat, const char*);
   void     Open(const char *pszConfigFileName = NULL, ui32 dwBoardNumber = 1, ui32 dwChannelNumber = 1, etParamValue eConfigMode = PHX_CONFIG_NORMAL, ui32 dwBufferCount = 2, tFnErrorHandlerFn pFnErrorHandler = PHX_ErrHandlerDefault);
   void     Close();

   bool     IsOpen() const;
   tHandle  GetHandle() const;

   void     AcquisitionStart() const;
   void     AcquisitionStop () const;

   void     CxpCameraWrite(ui32 dwCxpRegAddress, ui32  dwCxpRegValue) const;
   void     CxpCameraRead (ui32 dwCxpRegAddress, ui32 &dwCxpRegValue) const;

   ui8 *    BufferGet() const;
   void     BufferRelease() const;

   void     PhxParameterSet(etParam eParameter, void *pvValue) const;
   void     PhxParameterGet(etParam eParameter, void *pvValue) const;

   /*
   PhxParameterSet()
    * Set a control feature of the frame grabber.
    */
   template<typename T>
   void PhxParameterSet(etParam eParameter, T Value) const
   {
      if (!m_hPhxChannel) {
         throw std::runtime_error("Function \"open()\" has not yet been called.\n");
      } else {
         if (PHX_OK != PHX_ParameterSet(m_hPhxChannel, eParameter, &Value)) {
            throw std::runtime_error("Error setting parameter to frame grabber.\n");
         }
      }
   }

   typedef void (PHX_C_CALL *tInterruptCallbackFn) (ui32 InterruptMask, void *pvContext);
   void SetInterruptCallbackFunction(tInterruptCallbackFn pFnInterruptCallback, void *pvContext);

private:
   tInterruptCallbackFn m_pFnInterruptCallback;
   tHandle              m_hPhxChannel;
   void                *m_pvContext;

   static void PHX_C_CALL PhxCoreCallback(tHandle hChannel, ui32 dwInterruptMask, void *pvParams);
};

#endif

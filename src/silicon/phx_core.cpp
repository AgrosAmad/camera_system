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
 * Definitions of the C++ wrapper class.
 *
 ****************************************************************************
 */

#include "phx_core.h"

#if !defined htonl
#define htonl(A)  ((((ui32)(A) & 0xff000000) >> 24) | \
                   (((ui32)(A) & 0x00ff0000) >>  8) | \
                   (((ui32)(A) & 0x0000ff00) <<  8) | \
                   (((ui32)(A) & 0x000000ff) << 24))
#endif

CPhxCore::CPhxCore()
: m_hPhxChannel(0)
, m_pFnInterruptCallback(0)
, m_pvContext(0)
{
}

CPhxCore::~CPhxCore()
{
   Close();
}

/*
Open()
 * Open the specified PHX channel and set the number of acquisition buffers and its configuration file (PCF).
 */
void CPhxCore::Open(
   const char       *pszConfigFileName,   /* = NULL                     PCF file name */
   ui32              dwBoardNumber,       /* = 1                        Board number */
   ui32              dwChannelNumber,     /* = 1                        Channel number */
   etParamValue      eConfigMode,         /* = PHX_CONFIG_NORMAL        Configuration mode */
   ui32              dwBufferCount,       /* = 2                        Acquisition buffer count */
   tFnErrorHandlerFn pFnErrorHandler      /* = PHX_ErrHandlerDefault    Pointer to the error handler function to be used by the PHX library */
)
{
   /* Before opening, make sure all resources previously allocated are freed. */
   Close();

   /* Create handle to channel */
   if (PHX_OK != PHX_Create(&m_hPhxChannel, pFnErrorHandler)) {
      throw std::runtime_error("Failed to create PHX handle.\n");
   }

   /* Select board */
   etParamValue eBoard;
   switch (dwBoardNumber) {
      case 1:  eBoard = PHX_BOARD_NUMBER_1; break;
      case 2:  eBoard = PHX_BOARD_NUMBER_2; break;
      case 3:  eBoard = PHX_BOARD_NUMBER_3; break;
      case 4:  eBoard = PHX_BOARD_NUMBER_4; break;
      case 5:  eBoard = PHX_BOARD_NUMBER_5; break;
      case 6:  eBoard = PHX_BOARD_NUMBER_6; break;
      case 7:  eBoard = PHX_BOARD_NUMBER_7; break;
      default: throw std::runtime_error("Invalid board number.\n");
   }

   PhxParameterSet(PHX_BOARD_NUMBER, eBoard);

   /* Select channel */
   etParamValue eChannel;
   switch (dwChannelNumber) {
      case 1:  eChannel = PHX_CHANNEL_NUMBER_1; break;
      case 2:  eChannel = PHX_CHANNEL_NUMBER_2; break;
      case 3:  eChannel = PHX_CHANNEL_NUMBER_3; break;
      case 4:  eChannel = PHX_CHANNEL_NUMBER_4; break;
      case 5:  eChannel = PHX_CHANNEL_NUMBER_5; break;
      case 6:  eChannel = PHX_CHANNEL_NUMBER_6; break;
      case 7:  eChannel = PHX_CHANNEL_NUMBER_7; break;
      case 8:  eChannel = PHX_CHANNEL_NUMBER_8; break;
      default: throw std::runtime_error("Invalid channel number.\n");
   }

   PhxParameterSet(PHX_CHANNEL_NUMBER, eChannel);

   /* Set configuration file */
   if (pszConfigFileName) {
      FILE * pConfigFile = NULL;
      if ((pConfigFile = fopen(pszConfigFileName, "rb"))) {
         fclose(pConfigFile);
         PhxParameterSet(PHX_CONFIG_FILE, pszConfigFileName);
      } else {
         throw std::runtime_error("Failed to read configuration file.\n");
      }
   }

   /* Set configuration mode */
   PhxParameterSet(PHX_CONFIG_MODE, eConfigMode);

   /* Open the channel */
   if (PHX_OK != PHX_Open(m_hPhxChannel)) {
      throw std::runtime_error ("Failed to open channel.\n");
   }

   /* Set the number of acquisition buffers */
   PhxParameterSet(PHX_ACQ_NUM_BUFFERS, dwBufferCount);
}

/*
Close()
 * Close and destroy the PHX handle to the open channel.
 */
void CPhxCore::Close()
{
   if (m_hPhxChannel) {
      PHX_Destroy(&m_hPhxChannel);
      m_hPhxChannel = 0;
   }
}

bool CPhxCore::IsOpen() const
{
   /* If the handle is non NULL, the channel is open */
   return (m_hPhxChannel != 0);
}

tHandle CPhxCore::GetHandle() const
{
   return m_hPhxChannel;
}

/*
AcquisitionStart()
 * Start the acquisition engine of the frame grabber.
 */
void CPhxCore::AcquisitionStart() const
{
   /* First stop acquisition */
   AcquisitionStop();

   /* Set context used in callback function and start acquisition */
   PhxParameterSet(PHX_EVENT_CONTEXT, (void *)this);
   if (PHX_OK != PHX_StreamRead(m_hPhxChannel, PHX_START, reinterpret_cast<void *>(PhxCoreCallback))) {
      throw std::runtime_error("Error starting Frame Grabber acquisition.\n");
   }
}

/*
AcquisitionStop()
 * Stop the acquisition engine of the frame grabber.
 */
void CPhxCore::AcquisitionStop() const
{
   if (PHX_OK != PHX_StreamRead(m_hPhxChannel, PHX_ABORT, NULL)) {
      throw std::runtime_error("Error stopping Frame Grabber acquisition.\n");
   }
}

/*
CxpCameraWrite()
 * CXP camera register write.
 */
void CPhxCore::CxpCameraWrite(
   ui32 dwCxpRegAddress,
   ui32 dwCxpRegValue
) const
{
   ui32 dwTempValue  = htonl(dwCxpRegValue);
   ui32 dwSize       = sizeof(dwCxpRegValue);

   if (PHX_OK != PHX_ControlWrite(m_hPhxChannel, PHX_REGISTER_DEVICE, &dwCxpRegAddress, (ui8 *)&dwTempValue, (ui32 *)&dwSize, 800)) {
      throw std::runtime_error("Error writing to Camera CXP register.\n");
   }

   if (dwSize != sizeof(dwCxpRegValue)) {
      throw std::runtime_error("Error writing to Camera CXP register: size written does not match size requested.\n");
   }
}

/*
CxpCameraRead()
 * CXP camera register read.
 */
void CPhxCore::CxpCameraRead(
   ui32  dwCxpRegAddress,
   ui32 &dwCxpRegValue
) const
{
   ui32 dwSize = sizeof(dwCxpRegValue);
   ui32 dwTempValue;

   if (PHX_ControlRead(m_hPhxChannel, PHX_REGISTER_DEVICE, &dwCxpRegAddress, (ui8 *)&dwTempValue, (ui32 *)&dwSize, 800)) {
      throw std::runtime_error("Error reading Camera CXP register.\n");
   }

   if (dwSize != sizeof(dwCxpRegValue)) {
      throw std::runtime_error("Error reading Camera CXP register: size read does not match size requested.\n");
   }

   dwCxpRegValue = htonl(dwTempValue);
}

/*
BufferGet()
 * Retrieve pointer to acquired buffer.
 */
ui8 * CPhxCore::BufferGet() const
{
   ui8 *pbBuffer = 0;

   if (m_hPhxChannel) {
      stImageBuff sBuffer;
      etStat      ePhxStat;
      ePhxStat = PHX_StreamRead(m_hPhxChannel, PHX_BUFFER_GET, &sBuffer);
      if (PHX_OK != ePhxStat) {
         throw std::runtime_error("Error getting buffer.\n");
      }
      pbBuffer = reinterpret_cast<ui8 *>(sBuffer.pvAddress);
   }

   return pbBuffer;
}

/*
BufferRelease()
 * Release buffer (make it available to the hardware).
 */
void CPhxCore::BufferRelease() const
{
   if (m_hPhxChannel) {
      if (PHX_OK != PHX_StreamRead(m_hPhxChannel, PHX_BUFFER_RELEASE, NULL)) {
         throw std::runtime_error("Error releasing buffer.\n");
      }
   }
}

/*
PhxParameterSet()
 * Overload of the template PhxParameterSet() function to allow use of a void * parameter.
 */
void CPhxCore::PhxParameterSet(
   etParam  eParameter,
   void    *pvValue
) const
{
   if (!m_hPhxChannel) {
      throw std::runtime_error("Function \"open()\" has not yet been called.\n");
   } else {
      if (PHX_OK != PHX_ParameterSet(m_hPhxChannel, eParameter, pvValue)) {
         throw std::runtime_error("Error setting parameter to frame grabber.\n");
      }
   }
}

/*
PhxParameterGet()
 * Get a control feature of the frame grabber.
 */
void CPhxCore::PhxParameterGet(
   etParam  eParameter,
   void    *pvValue
) const
{
   if (!m_hPhxChannel) {
      throw std::runtime_error("Function \"open()\" has not yet been called.\n");
   } else {
      if (PHX_OK != PHX_ParameterGet(m_hPhxChannel, eParameter, pvValue)) {
         throw std::runtime_error("Error getting parameter from frame grabber.\n");
      }
   }
}

/*
SetInterruptCallbackFunction()
 * Set the function pointer and context that will be called from the PHX callback function.
 */
void CPhxCore::SetInterruptCallbackFunction(
   tInterruptCallbackFn pFnInterruptCallback,
   void                *pvContext
)
{
   m_pFnInterruptCallback  = pFnInterruptCallback;
   m_pvContext             = pvContext;
}

/*
PhxCoreCallback()
 * PHX callback routine. Called every time an event occurs during acquisition.
 */
void PHX_C_CALL CPhxCore::PhxCoreCallback(
   tHandle  hChannel,
   ui32     dwInterruptMask,
   void    *pvParams
)
{
   if (!pvParams) {
#if defined _PHX_WIN32
      MessageBoxA(NULL, "Error: null context pointer in callback.", "Callback function error", MB_OK);
#else
      printf("Error in  callback function: null context pointer.\n");
#endif
      return;
   }

   CPhxCore *pThis = reinterpret_cast<CPhxCore *>(pvParams);

   if (pThis->m_pFnInterruptCallback) {
      pThis->m_pFnInterruptCallback(dwInterruptMask, pThis->m_pvContext);
   }
}

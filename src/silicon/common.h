/****************************************************************************
 *
 * Active Silicon
 *
 * Project     : SDK
 * Description : Common routines used by the example suite
 * Updated     : 15-Jun-2017
 *
 * Copyright (c) 2019 Active Silicon
 ****************************************************************************
 * Comments:
 * --------
 * This file contains the definitions for the "common" module.
 *
 ****************************************************************************
 */

#ifndef _PHX_COMMON
#define _PHX_COMMON

#ifdef __cplusplus
extern "C" {
#endif

#if defined _MSC_VER
/* Disable compiler warning C4152: nonstandard extension, function/data pointer conversion in expression */
#pragma warning( disable : 4152 )
#endif

/* Mac OS X uses frameworks for the library header files
 */
#if defined _PHX_MACOSX
#import <ASLPhoenix/phx_api.h>

/* For all other OSs include the Phoenix header files
 */
#else
#include <phx_api.h>    /* Main Phoenix library    */

#if !defined _PHX_VXWORKS
#if defined _PHX_DISPLAY
#include <pbl_api.h>    /* Phoenix Buffer library */
#include <pdl_api.h>    /* Phoenix Display library */
#include <pil_api.h>    /* Phoenix Image library */
#endif
#endif

#endif

/* Include system header files for keyboard handling, under Posix
 */
#ifdef _PHX_POSIX
#include <termios.h>
#endif


/* A structure containing various user defined parameters.
 * These are initialised by the PhxCommonParseCmd function,
 * before being passed to the appropriate subroutine.
 */
#define PHX_MAX_FILE_LENGTH 128
typedef struct {
   ui32              dwBoardNumber;
   etParamValue      eBoardNumber;
   ui32              dwChannelNumber;
   etParamValue      eChannelNumber;
   char              bConfigFileName[PHX_MAX_FILE_LENGTH];
   char              bOutputFileName[PHX_MAX_FILE_LENGTH];
   char             *pszConfigFileName;
   char             *pszOutputFileName;
   ui32              dwRecordingNumBuffers;
   ui32              dwRecordingFrameRate;
   ui32              dwRecordingQualityFactor;
   char              bRecordingFormat[PHX_MAX_FILE_LENGTH];
   char             *pszRecordingFormat;
   ui32              dwCxpBitRate;
   etParamValue      eCxpBitRate;
   ui32              dwCxpLinks;
   etParamValue      eCxpLinks;
   ui32              dwConfigMode;
   etParamValue      eConfigMode;
   tFlag             fWaitForKill;
} tPhxCmd;

typedef struct {
   ui32 dwAcqStartAddress;
   ui32 dwAcqStartValue;
   ui32 dwAcqStopAddress;
   ui32 dwAcqStopValue;
   ui32 dwPixelFormatAddress;
} tCxpRegisters;

#define PHX_MAX_SZ_LENGTH 256 /* Maximum string length */

/* Function prototypes
 */
#if defined __cplusplus
extern "C" {
#endif
etStat   PhxCommonParseCmd                   ( int argc, char *argv[], tPhxCmd *ptPhxCmd );
etStat   PhxCommonGetBitsPerPixel            ( etParamValue eFormat, ui32 *pdwBitsPerPixel );
void     PhxCommonKbClose                    ( void );
void     PhxCommonKbInit                     ( void );
int      PhxCommonKbRead                     ( void );
int      PhxCommonKbHit                      ( void );
etStat   PhxCommonParseCxpRegs               ( const char *pszConfigFileName, tCxpRegisters *sCameraRegisters );
etStat   PhxCommonUpdateCxpRegs              ( tHandle hCamera, tCxpRegisters *sCameraRegisters );
etStat   PhxCommonIsCxp                      ( tHandle hCamera, tFlag *pfIsCxp );
etStat   PhxCommonWriteCxpReg                ( tHandle hCamera, ui32 dwAddress, ui32 dwValue, ui32 dwTimeout );
etStat   PhxCommonReadCxpReg                 ( tHandle hCamera, ui32 dwAddress, ui32 *pdwValue, ui32 dwTimeout );
etStat   PhxCommonGetCxpDiscoveryStatus      ( tHandle hCamera, ui32 dwTimeoutSec, tFlag *pfIsDiscovered );
etStat   PhxCommonGetBusFormat               ( etParamValue ePhxCamSrcCol, ui32 dwPhxCamSrcDepth, etParamValue ePhxBusFormatIn, etParamValue *pePhxBusFormatOut );
void     PhxCommonSaveImageToFile            ( const char *pszFileName, void *pvData, ui32 dwSizeInBytes );
void PHX_C_CALL PhxCommonDisplayErrorHandler ( const char *pszFnName, etStat eErrCode, const char *pszDescString );
#if defined __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif   /* _PHX_COMMON */

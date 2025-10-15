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
 * This file provides cross platform support services for the PHX SDK examples.
 * Whilst the API is fixed for all platforms, the implementation is OS specific.
 *
 ****************************************************************************
 */

#include "common.h"

#ifndef _PHX_WIN32
#include "fcntl.h"
#endif

#ifdef _PHX_POSIX
static int              peek_character = -1;
static struct termios   initial_settings, new_settings;
#endif
#ifdef _PHX_VXWORKS
volatile int gnKbHitCount;
volatile int gnKbHitCountMax = 100;
#endif
#ifdef _PHX_MACOSX
#include <libgen.h>
#endif

#ifndef htonl
#define htonl( A ) ( ( ( (ui32)( A ) & 0xff000000 ) >> 24 ) | \
                     ( ( (ui32)( A ) & 0x00ff0000 ) >>  8 ) | \
                     ( ( (ui32)( A ) & 0x0000ff00 ) <<  8 ) | \
                     ( ( (ui32)( A ) & 0x000000ff ) << 24 ) )
#endif

/*
PhxCommonParseCmd()
 * Parse the command line parameters, and place results in a common structure.
 * The command line parameters take the following form:
 * AppName -b <BoardNumber> -c <ConfigFileName> -o <OutputFileName>
 * -b <BoardNumber>              is an optional parameter specifying which board to use.
 *                               The default value is board 1.
 * -n <ChannelNumber>            is an optional parameter specifying which channel to use.
 *                               The default value is channel 1.
 * -c <ConfigFileName>           is an optional parameter specifying the Phoenix Configuration File.
 *                               The default value is no configuration file, ie use the library default values.
 * -o <OutputFileName>           is an optional parameter specifying the root name of an output file.
 *                               The default setting is "as_file".
 * -f <RecordingFormat>          is an optional parameter specifying the recording file format.
 *                               The default value is "avi".
 * -s <RecordingFrameRate>       is an optional parameter specifying the playback frame rate of
 *                               a recorded sequence.
 *                               The default value is 25 (fps).
 * -q <RecordingQualityFactor>   is an optional parameter specifying the JPEG quality
 *                               of a recorded AVI sequence.
 *                               The default value is 30.
 * -r <RecordingBufferCount>     is an optional parameter specifying the number of frames to be recorded.
 *                               The default value is 100.
 * -l <CXPDiscovery>             is an optional parameter specifying the number of CXP links.
 *                               The default value is 0 (AUTO).
 * -g <CXPBitRate>               is an optional parameter specifying the CXP bit rate (Gbps) the camera
 *                               has to be set to.
 *                               The default value is 0 (AUTO).
 * -m <ConfigurationMode>        is an optional parameter specifying the PHX configuration mode used to
 *                               open a channel:
 *                               0 (PHX_CONFIG_NORMAL), 1 (PHX_CONFIG_ACQ_ONLY) or 2 (PHX_CONFIG_COMMS_ONLY).
 *                               The default value is 0 (PHX_CONFIG_NORMAL).
 * -k                            if enabled (ON) the application will loop until the app is Killed,
 *                               ie <CTRL> C, kill -9, slay, etc
 *                               The default value is OFF.
 * Whilst all parameters may be specified, each example application only uses appropriate
 * parameters, for example "OutputFileName" will be ignored by the phx_info example.
 */
etStat PhxCommonParseCmd(
   int      argc,       /* Command line parameter count */
   char    *argv[],     /* Command line parameters */
   tPhxCmd *ptPhxCmd    /* Structure containing parsed information */
)
{
   char    *pszFunctionName   = NULL;
   char     szDisplayName[PHX_MAX_FILE_LENGTH];
   size_t   nSize             = 0;
   etStat   eStat             = PHX_OK;
   int      i                 = 0;

   /* Initialise the PhxCmd structure with default values */
   ptPhxCmd->dwBoardNumber             = 1;
   ptPhxCmd->dwChannelNumber           = 1;
   ptPhxCmd->pszConfigFileName         = NULL;
   ptPhxCmd->pszOutputFileName         = (char *) "as_file";
   ptPhxCmd->dwRecordingNumBuffers     = 100;
   ptPhxCmd->pszRecordingFormat        = (char *) "avi";
   ptPhxCmd->dwRecordingFrameRate      = 25;
   ptPhxCmd->dwRecordingQualityFactor  = 30;
   ptPhxCmd->dwCxpBitRate              = 0;
   ptPhxCmd->dwCxpLinks                = 0;
   ptPhxCmd->dwConfigMode              = 0;
   ptPhxCmd->fWaitForKill              = FALSE;

   /* The first argument is always the function name itself */
#if defined _PHX_WIN32
   pszFunctionName = strrchr( argv[0], '\\' ); /* Find last occurence of '\' */
#else
   pszFunctionName = strrchr( argv[0], '/' );  /* Find last occurence of '/' */
#endif
   if ( pszFunctionName ) {
      /* Don't print '\' ('/') and file extension */
      char * pszTemp = strrchr( pszFunctionName, '.' );  /* Find last occurence of '.' */
      if ( pszTemp ) {
         nSize = pszTemp - pszFunctionName - 1;
      } else {
         nSize = strlen( pszFunctionName ) - 1;
      }
      strncpy( szDisplayName, pszFunctionName+1, nSize );
      if ( nSize < PHX_MAX_FILE_LENGTH) {    /* NULL terminate the string */
         szDisplayName[nSize] = 0;
      }
      printf( "*** Active Silicon SDK Example %s ***\n\n", szDisplayName );
   } else {
      printf( "*** Active Silicon SDK Example ***\n\n" );
   }
   i++;

   /* Parse the command options */
   while ( i < argc ) {
      /* Board number */
      if ( !strcmp( argv[i], "-b" ) || !strcmp( argv[i], "-B" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwBoardNumber = atoi( argv[i] );
         }
      }
      /* chaNnel number */
      else if ( !strcmp( argv[i], "-n" ) || !strcmp( argv[i], "-N" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwChannelNumber = atoi( argv[i] );
         }
      }
      /* Config File */
      else if ( !strcmp( argv[i], "-c" ) || !strcmp( argv[i], "-C" ) ) {
         if ( ++i < argc ) {
            if ( argv[i][0] != '-' ) {
               strcpy( ptPhxCmd->bConfigFileName, argv[i] );
               ptPhxCmd->pszConfigFileName = ptPhxCmd->bConfigFileName;
            }
         }
      }
      /* Output File */
      else if ( !strcmp( argv[i], "-o" ) || !strcmp( argv[i], "-O" ) ) {
         if ( ++i < argc ) {
            strcpy( ptPhxCmd->bOutputFileName, argv[i] );
            ptPhxCmd->pszOutputFileName = ptPhxCmd->bOutputFileName;
         }
      }
      /* recording Format */
      else if ( !strcmp( argv[i], "-f" ) || !strcmp( argv[i], "-F" ) ) {
         if ( ++i < argc ) {
            strcpy( ptPhxCmd->bRecordingFormat, argv[i] );
            ptPhxCmd->pszRecordingFormat = ptPhxCmd->bRecordingFormat;
         }
      }
      /* recording frame rate (Speed) */
      else if ( !strcmp( argv[i], "-s" ) || !strcmp( argv[i], "-S" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwRecordingFrameRate = atoi( argv[i] );
         }
      }
      /* recording Quality factor */
      else if ( !strcmp( argv[i], "-q" ) || !strcmp( argv[i], "-Q" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwRecordingQualityFactor = atoi( argv[i] );
         }
      }
      /* Recording buffer count */
      else if ( !strcmp( argv[i], "-r" ) || !strcmp( argv[i], "-R" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwRecordingNumBuffers = atoi( argv[i] );
         }
      }
      /* CXP bit rate */
      else if ( !strcmp( argv[i], "-g" ) || !strcmp( argv[i], "-G" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwCxpBitRate = atoi( argv[i] );
         }
      }
      /* CXP number of links */
      else if ( !strcmp( argv[i], "-l" ) || !strcmp( argv[i], "-L" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwCxpLinks = atoi( argv[i] );
         }
      }
      /* Configuration mode */
      else if ( !strcmp( argv[i], "-m" ) || !strcmp( argv[i], "-M" ) ) {
         if ( ++i < argc ) {
            ptPhxCmd->dwConfigMode = atoi( argv[i] );
         }
      }
      /* quit on Kill */
      else if ( !strcmp( argv[i], "-k" ) || !strcmp( argv[i], "-K" ) ) {
         ptPhxCmd->fWaitForKill = TRUE;
      }
      else {
         printf( "Unrecognised parameter %s - Ignoring\n", argv[i] );
      }
      i++;
   }

   printf( "Using Board Number   = %d\n",  ptPhxCmd->dwBoardNumber );
   printf( "      Channel Number = %d\n",  ptPhxCmd->dwChannelNumber );
   printf( "      Config File    = " );
   if ( NULL == ptPhxCmd->pszConfigFileName ) {
      printf( "<None>\n" );
   } else {
      printf( "%s\n", ptPhxCmd->pszConfigFileName );
   }
   printf( "\n" );

   switch( ptPhxCmd->dwBoardNumber ) {
      case 0: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_AUTO; break;
      case 1: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_1;    break;
      case 2: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_2;    break;
      case 3: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_3;    break;
      case 4: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_4;    break;
      case 5: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_5;    break;
      case 6: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_6;    break;
      case 7: ptPhxCmd->eBoardNumber = PHX_BOARD_NUMBER_7;    break;
      default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Error; break;
   }

   switch( ptPhxCmd->dwChannelNumber ) {
      case 0: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_AUTO; break;
      case 1: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_1;    break;
      case 2: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_2;    break;
      case 3: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_3;    break;
      case 4: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_4;    break;
      case 5: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_5;    break;
      case 6: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_6;    break;
      case 7: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_7;    break;
      case 8: ptPhxCmd->eChannelNumber = PHX_CHANNEL_NUMBER_8;    break;
      default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Error;     break;
   }

   switch( ptPhxCmd->dwCxpBitRate ) {
      case  0: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_AUTO;  break;
      case  1: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP1;  break;
      case  2: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP2;  break;
      case  3: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP3;  break;
      case  5: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP5;  break;
      case  6: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP6;  break;
      case 10: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP10; break;
      case 12: ptPhxCmd->eCxpBitRate = PHX_CXP_BITRATE_MODE_CXP12; break;
      default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Error;      break;
   }

   switch( ptPhxCmd->dwCxpLinks ) {
      case 0: ptPhxCmd->eCxpLinks = PHX_CXP_DISCOVERY_MODE_AUTO;  break;
      case 1: ptPhxCmd->eCxpLinks = PHX_CXP_DISCOVERY_MODE_1X;    break;
      case 2: ptPhxCmd->eCxpLinks = PHX_CXP_DISCOVERY_MODE_2X;    break;
      case 4: ptPhxCmd->eCxpLinks = PHX_CXP_DISCOVERY_MODE_4X;    break;
      default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Error;     break;
   }
   
   switch( ptPhxCmd->dwConfigMode ) {
      case 0: ptPhxCmd->eConfigMode = PHX_CONFIG_NORMAL;       break;
      case 1: ptPhxCmd->eConfigMode = PHX_CONFIG_ACQ_ONLY;     break;
      case 2: ptPhxCmd->eConfigMode = PHX_CONFIG_COMMS_ONLY;   break;
      default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Error;  break;
   }

Error:
   return eStat;
}

/*
PhxCommonGetBitsPerPixel()
 * Return the number of bits per pixel for a given pixel format
 */
etStat PhxCommonGetBitsPerPixel(
   etParamValue  eFormat,
   ui32         *pdwBitsPerPixel
) {
   etStat eStat = PHX_OK;

   switch ( eFormat ) {
      case PHX_BUS_FORMAT_MONO8:
      case PHX_BUS_FORMAT_BAYER_GR8:   case PHX_BUS_FORMAT_BAYER_RG8:
      case PHX_BUS_FORMAT_BAYER_GB8:   case PHX_BUS_FORMAT_BAYER_BG8:
         *pdwBitsPerPixel = 8;
         break;

      case PHX_BUS_FORMAT_MONO10P:
         *pdwBitsPerPixel = 10;
         break;

      case PHX_BUS_FORMAT_MONO12P:
         *pdwBitsPerPixel = 12;
         break;

      case PHX_BUS_FORMAT_MONO14P:
         *pdwBitsPerPixel = 14;
         break;

      case PHX_BUS_FORMAT_MONO10:      case PHX_BUS_FORMAT_MONO12:
      case PHX_BUS_FORMAT_MONO14:      case PHX_BUS_FORMAT_MONO16:
      case PHX_BUS_FORMAT_BAYER_GR10:  case PHX_BUS_FORMAT_BAYER_RG10:
      case PHX_BUS_FORMAT_BAYER_GB10:  case PHX_BUS_FORMAT_BAYER_BG10:
      case PHX_BUS_FORMAT_BAYER_GR12:  case PHX_BUS_FORMAT_BAYER_RG12:
      case PHX_BUS_FORMAT_BAYER_GB12:  case PHX_BUS_FORMAT_BAYER_BG12:
      case PHX_BUS_FORMAT_BAYER_GR14:  case PHX_BUS_FORMAT_BAYER_RG14:
      case PHX_BUS_FORMAT_BAYER_GB14:  case PHX_BUS_FORMAT_BAYER_BG14:
      case PHX_BUS_FORMAT_BAYER_GR16:  case PHX_BUS_FORMAT_BAYER_RG16:
      case PHX_BUS_FORMAT_BAYER_GB16:  case PHX_BUS_FORMAT_BAYER_BG16:
      case PHX_BUS_FORMAT_BGR5:        case PHX_BUS_FORMAT_RGB5:
      case PHX_BUS_FORMAT_BGR565:      case PHX_BUS_FORMAT_RGB565:
      case PHX_BUS_FORMAT_YUV422_8:
         *pdwBitsPerPixel = 16;
         break;

      case PHX_BUS_FORMAT_BGR8:        case PHX_BUS_FORMAT_RGB8:
         *pdwBitsPerPixel = 24;
         break;

      case PHX_BUS_FORMAT_MONO32:      case PHX_BUS_FORMAT_RGBA8:
      case PHX_BUS_FORMAT_BGR10:       case PHX_BUS_FORMAT_RGB10:
      case PHX_BUS_FORMAT_XBGR8:       case PHX_BUS_FORMAT_BGRX8:
      case PHX_BUS_FORMAT_XRGB8:       case PHX_BUS_FORMAT_RGBX8:
      case PHX_BUS_FORMAT_BGR101210:   case PHX_BUS_FORMAT_RGB101210:
         *pdwBitsPerPixel = 32;
         break;
         
      case PHX_BUS_FORMAT_MONO36:      case PHX_BUS_FORMAT_RGBA10:
         *pdwBitsPerPixel = 40;
         break;

      case PHX_BUS_FORMAT_RGBA12:
      case PHX_BUS_FORMAT_BGR12:       case PHX_BUS_FORMAT_RGB12:
      case PHX_BUS_FORMAT_BGR14:       case PHX_BUS_FORMAT_RGB14:
      case PHX_BUS_FORMAT_BGR16:       case PHX_BUS_FORMAT_RGB16:
         *pdwBitsPerPixel = 48;
         break;

      case PHX_BUS_FORMAT_RGBA14:
         *pdwBitsPerPixel = 56;
         break;

      case PHX_BUS_FORMAT_RGBA16:
         *pdwBitsPerPixel = 64;
         break;

      default:
         *pdwBitsPerPixel = 0;
         printf( "Unrecognised buffer depth\n" );
         eStat = PHX_ERROR_BAD_PARAM;
         break;
   }

   return eStat;
}

/*
PhxCommonKbHit()
 * Cross platform implementation of a keyboard input routine to terminate application.
 */
int PhxCommonKbHit( void )
{
#if defined _PHX_POSIX
struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
#elif defined _PHX_VXWORKS
   return ( gnKbHitCount++ > gnKbHitCountMax );
#elif defined _MSC_VER
   return _kbhit();
#else
   return kbhit();
#endif
}


/*
PhxCommonKbClose()
 * Cross platform implementation of a keyboard restore routine.
 */
void PhxCommonKbClose( void )
{
#ifdef _PHX_POSIX
   tcsetattr( 0, TCSANOW, &initial_settings );
#elif defined _PHX_VXWORKS
   /* TODO */
#else
   /* Nothing to do */
#endif
}


/*
PhxCommonKbInit()
 * Cross platform implementation of a keyboard initialisation routine.
 */
void PhxCommonKbInit( void )
{
#ifdef _PHX_POSIX
   tcgetattr( 0, &initial_settings );
   new_settings = initial_settings;
   new_settings.c_lflag    &= ~ICANON;
   new_settings.c_lflag    &= ~ECHO;
   new_settings.c_lflag    &= ~ISIG;
   new_settings.c_cc[VMIN]  = 1;
   new_settings.c_cc[VTIME] = 0;
   tcsetattr( 0, TCSANOW, &new_settings );
#elif defined _PHX_VXWORKS
   gnKbHitCount = 0;
#else
   /* Nothing to do */
#endif
}


/*
PhxCommonKbRead()
 * Cross platform implementation of a keyboard character read routine.
 */
int PhxCommonKbRead( void )
{
#ifdef _PHX_POSIX
   char ch;

   if ( peek_character != -1 ) {
      ch = peek_character;
      peek_character = -1;
      return ch;
   }
   read( 0, &ch, 1 );
   return ch;

#elif defined _PHX_VXWORKS
   /* TODO */
#elif defined _MSC_VER
   return _getch();
#else
   return getch();
#endif
}

/*
PhxCommonParseCxpRegs()
 * Parse the PCF file for CXP camera registers.
 */
etStat PhxCommonParseCxpRegs(
   const char *pszConfigFileName,      /* Name of the Phoenix Configuration File */
   tCxpRegisters *ptCameraRegisters    /* Structure containing a description of the CXP registers */
)
{
   etStat   eStat          = PHX_OK;
   FILE    *pConfigFile    = NULL;
   char    *pszToken       = 0;
   char     szLine[256];
   ui32     dwLineCount;

   if ( !pszConfigFileName || !ptCameraRegisters ) {
      eStat = PHX_ERROR_BAD_PARAM;
      goto Exit;
   }

   ptCameraRegisters->dwAcqStartAddress      = 0;
   ptCameraRegisters->dwAcqStartValue        = 0;
   ptCameraRegisters->dwAcqStopAddress       = 0;
   ptCameraRegisters->dwAcqStopValue         = 0;
   ptCameraRegisters->dwPixelFormatAddress   = 0;

   /* Parse the configuration file */
   /* Only check for lines starting with '#!' */
   if ( NULL == ( pConfigFile = fopen( pszConfigFileName, "r" ) ) ) {
      printf( "Failed to parse PCF file for CXP registers:\nerror reading the file.\n" );
      eStat = PHX_ERROR_FILE_OPEN_FAILED;
      goto Exit;
   }

   /* Read and parse each line. */
   dwLineCount = 0;
   while ( fgets( szLine, 256, pConfigFile ) != NULL ) {
      dwLineCount++;
      pszToken = strtok( szLine, " ,\t\n\r" );
      if ( pszToken ) {
         if ( pszToken[0] != '#' || pszToken[1] != '!' ) {
            continue;
         } else {
            /* If the first token of a line is '#!' then it could be a line describing a CXP register. */
            pszToken = strtok( NULL, " ,\t\n\r" );
         }
      } else {
         continue;
      }

      if ( 0 == strcmp( pszToken, "CXP_REG_ACQ_START_ADDRESS" ) ) {
         pszToken = strtok( NULL, " ,\t\n\r" );
         ptCameraRegisters->dwAcqStartAddress = (ui32) strtoul( pszToken, NULL, 16 );
      }
      else if ( 0 == strcmp( pszToken, "CXP_REG_ACQ_START_VALUE" ) ) {
         pszToken = strtok( NULL, " ,\t\n\r" );
         ptCameraRegisters->dwAcqStartValue = (ui32) strtoul( pszToken, NULL, 0 );
      }
      else if ( 0 == strcmp( pszToken, "CXP_REG_ACQ_STOP_ADDRESS" ) ) {
         pszToken = strtok( NULL, " ,\t\n\r" );
         ptCameraRegisters->dwAcqStopAddress = (ui32) strtoul( pszToken, NULL, 16 );
      }
      else if ( 0 == strcmp( pszToken, "CXP_REG_ACQ_STOP_VALUE" ) ) {
         pszToken = strtok( NULL, " ,\t\n\r" );
         ptCameraRegisters->dwAcqStopValue = (ui32) strtoul( pszToken, NULL, 0 );
      }
      else if ( 0 == strcmp( pszToken, "CXP_REG_PIXEL_FORMAT_ADDRESS" ) ) {
         pszToken = strtok( NULL, " ,\t\n\r" );
         ptCameraRegisters->dwPixelFormatAddress = (ui32) strtoul( pszToken, NULL, 16 );
      }
      else {
         continue;
      }
   }

   fclose( pConfigFile );

   if ( 0 == dwLineCount ) {
      printf( "Failed to parse PCF file for CXP registers:\nzero lines in configuration file.\n" );
      eStat = PHX_ERROR_FILE_INVALID;
      goto Exit;
   }

Exit:
   return eStat;
}

/*
PhxCommonUpdateCxpRegs()
 * Check for unset CXP camera registers.
 * Read them from the camera if >= CXP v1.1
 */
etStat PhxCommonUpdateCxpRegs(
   tHandle        hCamera,          /* PHX handle */
   tCxpRegisters *ptCameraRegisters /* Structure containing a description of the CXP registers */
)
{
   etStat eStat                   = PHX_OK;
   ui32   dwBootstrapRevisionAddr = 0x0004;
   ui32   dwBootstrapAcqStartAddr = 0x300C;
   ui32   dwBootstrapAcqStopAddr  = 0x3010;
   ui32   dwRevision = 0;

   if ( !ptCameraRegisters ) {
      eStat = PHX_ERROR_BAD_PARAM;
      goto Exit;
   }

   /* Read the CXP Revision */
   eStat = PhxCommonReadCxpReg( hCamera, dwBootstrapRevisionAddr, &dwRevision, 800 );
   if ( PHX_OK != eStat )
      goto Exit;

   if( dwRevision < 0x00010001 ) { /* e.g. v1.0 */
      /* Check for unset values */
      if( !ptCameraRegisters->dwAcqStartAddress || !ptCameraRegisters->dwAcqStopAddress ) {
         printf( "\nCXP camera start/stop address not found in .PCF\n\n" );
         eStat = PHX_ERROR_CXP_INVALID_ADDRESS;
         goto Exit;
      }
   }
   else { /* e.g. v1.1 */
      /* Read the address of the AcquisitionStart register from the camera */
      eStat = PhxCommonReadCxpReg(hCamera, dwBootstrapAcqStartAddr, &ptCameraRegisters->dwAcqStartAddress, 800);
      if ( PHX_OK != eStat )
         goto Exit;

	  /* Read the address of the AcquisitionStop  register from the camera */
      eStat = PhxCommonReadCxpReg(hCamera, dwBootstrapAcqStopAddr,  &ptCameraRegisters->dwAcqStopAddress,  800);
      if ( PHX_OK != eStat )
         goto Exit;

      printf( "\nUsing CXP camera start address 0x%x, stop address 0x%x.\n\n", ptCameraRegisters->dwAcqStartAddress, ptCameraRegisters->dwAcqStopAddress );
      ptCameraRegisters->dwAcqStartValue = 1;
      ptCameraRegisters->dwAcqStopValue  = 1;
   }

Exit:
   return eStat;
}

/*
PhxCommonIsCxp()
 * Return true if board type is CXP and false otherwise.
 */
etStat PhxCommonIsCxp(
   tHandle  hCamera, /* PHX handle */
   tFlag   *pfIsCxp  /* Flag indicating whether the board type is CXP or not */
)
{
   etStat   eStat          = PHX_OK;
   ui8     *pbProperties   = NULL;

   if ( NULL == pfIsCxp ) {
      eStat =  PHX_ERROR_BAD_PARAM;
      goto Exit;
   }

   *pfIsCxp = FALSE;

   /* Get the board properties */
   eStat = PHX_ParameterGet( hCamera, PHX_BOARD_PROPERTIES, &pbProperties );

   if ( PHX_OK == eStat ) {
      char    *pbTmpProperties   = NULL;
      char    *pbToken           = NULL;
      char     szSeparators[]    = "=\n";
      size_t   nSize             = 0;

      if ( NULL == pbProperties ) {
         eStat = PHX_ERROR_INTERNAL_ERROR;
         goto Exit;
      }

      /* Take a copy of the property string since strtok is destructive */
      nSize = strlen( (char *)pbProperties );
      pbTmpProperties = (char *)malloc( nSize );
      memcpy( pbTmpProperties, pbProperties, nSize );

      /* Get the first token */
      pbToken = strtok( pbTmpProperties, szSeparators );

      /* Step through each of the tokens, and find the "Camera Interface" property */
      while ( NULL != pbToken ) {
         if ( 0 == strcmp( pbToken, "Camera Interface" ) ) {
            pbToken = strtok( NULL, szSeparators );
            if ( ( 0 != pbToken ) && ( 0 == strcmp( pbToken, "CoaXPress" ) ) ) {
               *pfIsCxp = TRUE;
               break;
            }
         }

         /* Drop the property value and get the next property */
         pbToken = strtok( NULL, szSeparators );
         pbToken = strtok( NULL, szSeparators );
      }

      free( pbTmpProperties );
   }

Exit:
   return eStat;
}

/*
PhxCommonWriteCxpReg()
 * Write to a 32 bit CXP register in camera.
 */
etStat PhxCommonWriteCxpReg(
   tHandle  hCamera,    /* PHX handle */
   ui32     dwAddress,  /* Address of the camera register */
   ui32     dwValue,    /* Value to write */
   ui32     dwTimeout   /* Timeout for operation */
)
{
   etStat eStat = PHX_OK;
   ui32 dwBigEndian = htonl( dwValue );
   ui32 dwSize = sizeof( dwValue );
   eStat = PHX_ControlWrite( hCamera, PHX_REGISTER_DEVICE, &dwAddress, (ui8 *)&dwBigEndian, &dwSize, dwTimeout );
   return eStat;
}

/*
PhxCommonReadCxpReg()
 * Read from a 32 bit CXP register in camera.
 */
etStat PhxCommonReadCxpReg(
   tHandle  hCamera,    /* PHX handle */
   ui32     dwAddress,  /* Address of the camera register */
   ui32    *pdwValue,   /* Value to write */
   ui32     dwTimeout   /* Timeout for operation */
)
{
   etStat eStat = PHX_OK;
   ui32 dwSize = sizeof( ui32 );
   eStat = PHX_ControlRead( hCamera, PHX_REGISTER_DEVICE, &dwAddress, (ui8 *)pdwValue, &dwSize, dwTimeout );
   if ( eStat == PHX_OK ) {
      if ( pdwValue ) {
         *pdwValue = htonl( *pdwValue );
      } else {
          eStat = PHX_ERROR_INVALID_POINTER;
      }
   }
   return eStat;
}

/*
PhxCommonGetCxpDiscoveryStatus()
 * Checks if CXP camera is discovered.
 */
etStat PhxCommonGetCxpDiscoveryStatus(
   tHandle  hCamera,
   ui32     dwTimeoutSec,
   tFlag   *pfIsDiscovered
)
{
   etStat   eStat = PHX_OK;   
   ui32     i     = 0;

   printf( "Checking for CXP camera discovery\n..." );

   do {  /* Sleep between two attempts is only 500 ms */      
      etParamValue eDiscoveryStatus;
      eStat = PHX_ParameterGet( hCamera, PHX_CXP_INFO, &eDiscoveryStatus );
      if ( PHX_OK != eStat ) goto Exit;
      if ( PHX_CXP_CAMERA_DISCOVERED & eDiscoveryStatus ) {
         *pfIsDiscovered = TRUE;
         break;
      }

      printf( "." );

      _PHX_SleepMs(500);
   } while ( ++i < dwTimeoutSec * 2 );

   if ( *pfIsDiscovered ) {
      printf( "\nCXP camera is discovered.\n\n" );
   }
   else {
      printf( "\nCXP camera was not discovered.\n\n" );
   }

Exit:
   return eStat;
}

/*
PhxCommonGetBusFormat()
 * Determine the PHX_BUS_FORMAT based on PHX_CAM_SRC_COL and PHX_CAM_SRC_DEPTH.
 */
etStat PhxCommonGetBusFormat(
   etParamValue   ePhxCamSrcCol,       /* PHX_CAM_SRC_COL value */
   ui32           dwPhxCamSrcDepth,    /* PHX_CAM_SRC_DEPTH value */
   etParamValue   ePhxBusFormatIn,     /* Current PHX_BUS_FORMAT value */
   etParamValue  *pePhxBusFormatOut    /* PHX_BUS_FORMAT return value */
)
{
   etStat   eStat       = PHX_OK;
   int      nBayerDepth = 0;

   if ( !pePhxBusFormatOut ) {
      eStat = PHX_ERROR_INVALID_POINTER;
      goto Exit;
   }

   /* Check if legacy PHX_DST_FORMAT_BAY value is being used, and replace it with new PHX_BUS_FORMAT_BAYER value if needed */
   if ( PHX_DST_FORMAT_BAY8  == ePhxBusFormatIn ) {
         switch ( ePhxCamSrcCol ) {
            case PHX_CAM_SRC_BAYER_RG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_RG8; break;
            case PHX_CAM_SRC_BAYER_GR: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GR8; break;
            case PHX_CAM_SRC_BAYER_GB: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GB8; break;
            case PHX_CAM_SRC_BAYER_BG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_BG8; break;
            default: break;
         }
   }
   else if ( PHX_DST_FORMAT_BAY10 == ePhxBusFormatIn ) {
         switch ( ePhxCamSrcCol ) {
            case PHX_CAM_SRC_BAYER_RG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_RG10; break;
            case PHX_CAM_SRC_BAYER_GR: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GR10; break;
            case PHX_CAM_SRC_BAYER_GB: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GB10; break;
            case PHX_CAM_SRC_BAYER_BG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_BG10; break;
            default: break;
         }
   }
   else if ( PHX_DST_FORMAT_BAY12 == ePhxBusFormatIn ) {
         switch ( ePhxCamSrcCol ) {
            case PHX_CAM_SRC_BAYER_RG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_RG12; break;
            case PHX_CAM_SRC_BAYER_GR: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GR12; break;
            case PHX_CAM_SRC_BAYER_GB: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GB12; break;
            case PHX_CAM_SRC_BAYER_BG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_BG12; break;
            default: break;
         }
   }
   else if ( PHX_DST_FORMAT_BAY14 == ePhxBusFormatIn ) {
         switch ( ePhxCamSrcCol ) {
            case PHX_CAM_SRC_BAYER_RG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_RG14; break;
            case PHX_CAM_SRC_BAYER_GR: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GR14; break;
            case PHX_CAM_SRC_BAYER_GB: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GB14; break;
            case PHX_CAM_SRC_BAYER_BG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_BG14; break;
            default: break;
         }
   }
   else if ( PHX_DST_FORMAT_BAY16 == ePhxBusFormatIn ) {
         switch ( ePhxCamSrcCol ) {
            case PHX_CAM_SRC_BAYER_RG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_RG16; break;
            case PHX_CAM_SRC_BAYER_GR: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GR16; break;
            case PHX_CAM_SRC_BAYER_GB: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_GB16; break;
            case PHX_CAM_SRC_BAYER_BG: ePhxBusFormatIn = PHX_BUS_FORMAT_BAYER_BG16; break;
            default: break;
         }
   }

   /* Check if Bayer format (used when determining the PHX_BUS_FORMAT for a Mono souce) */
        if (   PHX_BUS_FORMAT_BAYER_GR8 <= ePhxBusFormatIn
            && PHX_BUS_FORMAT_BAYER_BG8 >= ePhxBusFormatIn ) {
      nBayerDepth = 8;
   }
   else if (   PHX_BUS_FORMAT_BAYER_GR10 <= ePhxBusFormatIn
            && PHX_BUS_FORMAT_BAYER_BG10 >= ePhxBusFormatIn ) {
      nBayerDepth = 10;
   }
   else if (   PHX_BUS_FORMAT_BAYER_GR12 <= ePhxBusFormatIn
            && PHX_BUS_FORMAT_BAYER_BG12 >= ePhxBusFormatIn ) {
      nBayerDepth = 12;
   }
   else if (   PHX_BUS_FORMAT_BAYER_GR14 <= ePhxBusFormatIn
            && PHX_BUS_FORMAT_BAYER_BG14 >= ePhxBusFormatIn ) {
      nBayerDepth = 14;
   }
   else if (   PHX_BUS_FORMAT_BAYER_GR16 <= ePhxBusFormatIn
            && PHX_BUS_FORMAT_BAYER_BG16 >= ePhxBusFormatIn ) {
      nBayerDepth = 16;
   }

   /* Set the return value to the input PHX_BUS_FORMAT value */
   *pePhxBusFormatOut = ePhxBusFormatIn;

   /* Determine PHX_BUS_FORMAT output value */
   switch ( ePhxCamSrcCol ) {
      case PHX_CAM_SRC_MONO:
         switch ( dwPhxCamSrcDepth ) {
            case 8:
               if ( PHX_BUS_FORMAT_MONO8 != ePhxBusFormatIn && 8 != nBayerDepth ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_MONO8;
               }
               break;

            case 10:
               if ( PHX_BUS_FORMAT_MONO10 != ePhxBusFormatIn && 10 != nBayerDepth ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_MONO10;
               }
               break;

            case 12:
               if ( PHX_BUS_FORMAT_MONO12 != ePhxBusFormatIn && 12 != nBayerDepth ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_MONO12;
               }
               break;

            case 14:
               if ( PHX_BUS_FORMAT_MONO14 != ePhxBusFormatIn && 14 != nBayerDepth ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_MONO14;
               }
               break;

            case 16:
               if ( PHX_BUS_FORMAT_MONO16 != ePhxBusFormatIn && 16 != nBayerDepth ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_MONO16;
               }
               break;

            default:
               eStat = PHX_ERROR_BAD_PARAM_VALUE;
               goto Exit;
               break;
         }
         break;

      case PHX_CAM_SRC_RGB:
         switch ( dwPhxCamSrcDepth ) {
            case 8:
               if ( PHX_BUS_FORMAT_RGB8 != ePhxBusFormatIn && PHX_BUS_FORMAT_BGR8 != ePhxBusFormatIn ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_RGB8;
               }
               break;

            case 10:
               if ( PHX_BUS_FORMAT_RGB10 != ePhxBusFormatIn && PHX_BUS_FORMAT_BGR10 != ePhxBusFormatIn ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_RGB10;
               }
               break;

            case 12:
               if ( PHX_BUS_FORMAT_RGB12 != ePhxBusFormatIn && PHX_BUS_FORMAT_BGR12 != ePhxBusFormatIn ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_RGB12;
               }
               break;

            case 14:
               if ( PHX_BUS_FORMAT_RGB14 != ePhxBusFormatIn && PHX_BUS_FORMAT_BGR14 != ePhxBusFormatIn ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_RGB14;
               }
               break;

            case 16:
               if ( PHX_BUS_FORMAT_RGB16 != ePhxBusFormatIn && PHX_BUS_FORMAT_BGR16 != ePhxBusFormatIn ) {
                  *pePhxBusFormatOut = PHX_BUS_FORMAT_RGB16;
               }
               break;

            default:
               eStat = PHX_ERROR_BAD_PARAM_VALUE;
               goto Exit;
               break;
         }
         break;

      case PHX_CAM_SRC_BAYER_RG:
         switch ( dwPhxCamSrcDepth ) {
            case 8:  *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_RG8;  break;
            case 10: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_RG10; break;
            case 12: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_RG12; break;
            case 14: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_RG14; break;
            case 16: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_RG16; break;
            default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Exit;   break;
         }
         break;

      case PHX_CAM_SRC_BAYER_GR:
         switch ( dwPhxCamSrcDepth ) {
            case 8:  *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GR8;  break;
            case 10: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GR10; break;
            case 12: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GR12; break;
            case 14: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GR14; break;
            case 16: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GR16; break;
            default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Exit;   break;
         }
         break;

      case PHX_CAM_SRC_BAYER_GB:
         switch ( dwPhxCamSrcDepth ) {
            case 8:  *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GB8;  break;
            case 10: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GB10; break;
            case 12: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GB12; break;
            case 14: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GB14; break;
            case 16: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_GB16; break;
            default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Exit;   break;
         }
         break;

      case PHX_CAM_SRC_BAYER_BG:
         switch ( dwPhxCamSrcDepth ) {
            case 8:  *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_BG8;  break;
            case 10: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_BG10; break;
            case 12: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_BG12; break;
            case 14: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_BG14; break;
            case 16: *pePhxBusFormatOut = PHX_BUS_FORMAT_BAYER_BG16; break;
            default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Exit;   break;
         }
         break;

      case PHX_CAM_SRC_YUV422:
         switch ( dwPhxCamSrcDepth ) {
            case 8:
            case 16: *pePhxBusFormatOut = PHX_BUS_FORMAT_YUV422_8;   break;
            default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Exit;   break;
         }
         break;

      case PHX_CAM_SRC_RGBA:
         switch ( dwPhxCamSrcDepth ) {
            case 8:  *pePhxBusFormatOut = PHX_BUS_FORMAT_RGBA8;      break;
            case 10: *pePhxBusFormatOut = PHX_BUS_FORMAT_RGBA10;     break;
            case 12: *pePhxBusFormatOut = PHX_BUS_FORMAT_RGBA12;     break;
            case 14: *pePhxBusFormatOut = PHX_BUS_FORMAT_RGBA14;     break;
            case 16: *pePhxBusFormatOut = PHX_BUS_FORMAT_RGBA16;     break;
            default: eStat = PHX_ERROR_BAD_PARAM_VALUE; goto Exit;   break;
         }
         break;

      default:
         eStat = PHX_ERROR_BAD_PARAM_VALUE;
         goto Exit;
         break;
   }

Exit:
   return eStat;
}

void PhxCommonSaveImageToFile(
   const char *pszFileName,   /* File name */
   void       *pvData,        /* Data to save */
   ui32        dwSizeInBytes  /* Size in bytes of the data */
)
{
#if defined _PHX_WIN32
   FILE *pf = NULL;

   fopen_s( &pf, pszFileName, "wb" );
   if ( pf ) {
      fwrite( pvData, sizeof( ui8 ), dwSizeInBytes, pf );
      fclose( pf );
   } else {
      printf( "Failed to open image file.\n" );
   }
#else
   int fd = -1;

   fd = open( pszFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
      if ( fd == -1 ) {
         printf( "Failed to open image file\n" );
      } else {
         write( fd, pvData, dwSizeInBytes );
         close( fd );
      }
#endif
}

void PHX_C_CALL PhxCommonDisplayErrorHandler(
   const char *pszFnName,        /* Function name */
   etStat eErrCode,              /* Error code */
   const char *pszDescString     /* Error description */
)
{
   char szErrorMessage[PHX_MAX_SZ_LENGTH];
   sprintf( szErrorMessage, "%s failed with error code 0x%08x.\n", pszFnName, eErrCode );
   if ( NULL != pszDescString ) {
      strcat( szErrorMessage, pszDescString );
   }
   printf( "%s\n", szErrorMessage );
}


#if defined _PHX_VXWORKS
/* Simple function to map from a board number to the relevant etCamConfigLoad value.
 */
int PhxGeneric(
   int nBoardNumber
)
{
   return ( PHX_DIGITAL | nBoardNumber );
}

int PhxD24(
   int nBoardNumber )
{
   return ( PHX_D24CL_PCI32 | nBoardNumber );
}

/* Function used to run our example code, but initialise the keyboard handling before and faster.
 * Note: this relies on the fact that the spawn function accepts 10 parameters, although all 10
 * do not have to be specified on the command line.
 */
int PhxRun ( int (*pFn)(int, int, int, int, int, int ,int, int, int, int),
             int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10
)
{   
   int nStatus;

   PhxCommonKbInit();
   nStatus = pFn(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
   PhxCommonKbClose();
}
#endif

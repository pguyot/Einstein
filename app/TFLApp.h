// ==============================
// File:			TFLApp.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#ifndef _TFLAPP_H
#define _TFLAPP_H

#if TARGET_OS_WIN32
#include <winsock2.h>
#endif
#if TARGET_OS_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <FL/x.H>
#include <K/Defines/KDefinitions.h>
#include <FL/Fl_Widget.H>

#include "Version.h"

class TROMImage;
class TEmulator;
class TSoundManager;
class TScreenManager;
class TLog;
class TPlatformManager;
class TNetworkManager;
class TSerialPortManager;
class TMonitor;
class TSymbolList;

class Fl_Widget;
class TFLSettings;

///
/// Classe pour le programme einstein en ligne de commande.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 113 $
///
/// \test	aucun test d�fini.
///
class TFLApp
{
#if TARGET_OS_WIN32
  class TFLAppPipeServer {
  public:
    TFLAppPipeServer(TFLApp*);
    ~TFLAppPipeServer();
    int open();
    void close();
  private:
    static void thread_(void *);
    static void awake_(void *);
    static const int BUFSIZE = 4096;
    TFLApp *app_;
    OVERLAPPED over_;
    HANDLE hPipeInst;
    HANDLE hPipe;
    TCHAR chRequest[BUFSIZE];
    DWORD cbRead;
    TCHAR chReply[BUFSIZE];

  };
#endif
public:
	///
	/// Constructeur par d�faut.
	///
	TFLApp( void );

	///
	/// Destructeur.
	///
	~TFLApp( void );

	///
	/// Point d'entr�e.
	///
	void Run( int argc, char* argv[] );

	///
	/// run a callback from any of the menu items in our pulldown
	///
	void do_callback(Fl_Callback *cb, void *user=0L);

	///
	/// user requested simulating the power button
	///
	void menuPower();

	///
	/// user requested simulating holding the power button longer
	///
	void menuBacklight();

	///
	/// user requested to install a package
	///
	void menuInstallPackage();

	///
	/// show the "About..." screen
	///
	void menuAbout();

	///
	/// user wants to see the Settings dialog box
	///
	void menuShowSettings();

	///
	/// Open the dialog to download the ROM via TCP/IP
	///
	void menuDownloadROM();

  TPlatformManager *getPlatformManager() { return mPlatformManager; }
private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TFLApp( const TFLApp& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TFLApp& operator = ( const TFLApp& inCopy );

	///
	/// Affiche un message d'erreur sur la syntaxe et sort.
	///
	void SyntaxError( void );

	///
	/// Affiche un message d'erreur sur la syntaxe (une option en particulier)
	/// et sort.
	///
	/// \param inBadOption	bad option
	///
	void SyntaxError( const char* inBadOption );

	///
	/// Affiche l'aide et sort.
	///
	void Help( void );

	///
	/// Affiche la version et sort.
	///
	void Version( void );

	///
	/// Cr�e le gestionnaire de son.
	///
    void CreateSoundManager( void );
	void CreateSoundManager( const char* inClass );

	///
	/// Cr�e le gestionnaire d'�cran.
	///
	void CreateScreenManager(
				const char* inClass,
				int inPortraitWidth,
				int inPortraitHeight,
				Boolean inFullScreen,
                Boolean hideMouse,
                int yOffset = 0);

	///
	/// Cr�e le log.
	///
	void CreateLog( const char* inPath );

	///
	/// Point d'entr�e du processus l�ger.
	///
	static void* SThreadEntry( void* inUserData )
		{
			((TFLApp*) inUserData)->ThreadEntry();
			return NULL;
		}

	///
	/// Point d'entr�e du processus l�ger.
	///
	void ThreadEntry( void );

	///
	/// Boucle du menu.
	///
	void MenuLoop( void );

	///
	/// Boucle du menu (moniteur)
	///
	void MonitorMenuLoop( void );

	///
	/// Boucle du menu (app)
	///
	void AppMenuLoop( void );

	///
	/// Execute a command.
	///
	/// \return true if the command was known.
	///
	Boolean ExecuteCommand( const char* inCommand );

	///
	/// Affiche l'aide (du menu)
	///
	void PrintHelp( void );

	///
	/// Affiche une ligne (dans stdout ou via le moniteur)
	///
	void PrintLine( const char* inLine );

	void static quit_cb(Fl_Widget *w, void *p);

	/// \name Variables
	const char*			mProgramName;		///< Nom du programme.
	TROMImage*			mROMImage;			///< Image ROM.
	TEmulator*			mEmulator;			///< Emulateur.
	TSoundManager*		mSoundManager;		///< Gestionnaire de son.
	TScreenManager*		mScreenManager;		///< Gestionnaire d'�cran.
	TPlatformManager*	mPlatformManager;	///< Reference to the platform manager.
    TNetworkManager*    mNetworkManager;    ///< Network manager.
    TSerialPortManager* mSerialPortManager; ///< Serial port manager.
	TLog*				mLog;				///< Log.
	TMonitor*			mMonitor;			///< Monitor.
	TSymbolList*		mSymbolList;		///< List of symbols.
	Boolean				mQuit;				///< If we should quit.
	TFLSettings			*flSettings;		///< settings dialog box
#if TARGET_OS_WIN32
    TFLAppPipeServer mPipeServer;
#endif
};

#endif
		// _TFLAPP_H

// ============================================================================ //
// Mac Airways:                                                                 //
// The cashiers, flight attendants and pilots all look the same, feel the same  //
// and act the same. When asked questions about the flight, they reply that you //
// don't want to know, don't need to know and would you please return to your   //
// seat and watch the movie.                                                    //
// ============================================================================ //

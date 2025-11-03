//
// Shareaza.h
//
// Copyright (c) Shareaza Development Team, 2002-2015.
// This file is part of SHAREAZA (shareaza.sourceforge.net)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "Resource.h"
#include "ComObject.h"

class CBuffer;
class CDatabase;
class CFontManager;
class CMainWnd;
class CPacketWnd;
class CShareazaFile;
class CSplashDlg;


class __declspec(novtable) CLogMessage
{
public:
	CLogMessage(WORD nType, const CString& strLog) : m_strLog( strLog ), m_nType( nType ), m_Time( CTime::GetCurrentTime() ) {}
	CString m_strLog;
	WORD	m_nType;
	CTime	m_Time;
};

typedef CList< CLogMessage* > CLogMessageList;


class CShareazaCommandLineInfo : public CCommandLineInfo
{
public:
	CShareazaCommandLineInfo();

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	BOOL	m_bTray;
	BOOL	m_bNoSplash;
	BOOL	m_bNoAlphaWarning;
	INT		m_nGUIMode;
	BOOL	m_bHelp;
	CString	m_sTask;
	BOOL	m_bWait;

private:
	CShareazaCommandLineInfo(const CShareazaCommandLineInfo&);
	CShareazaCommandLineInfo& operator=(const CShareazaCommandLineInfo&);
};


/**
 * @brief Represents the main Shareaza application class.
 *
 * This class is responsible for initializing and running the Shareaza application.
 * It handles command-line parsing, message logging, and provides access to
 * global application resources.
 */
class CShareazaApp : public CWinApp
{
	DECLARE_DYNAMIC(CShareazaApp)

public:
	/**
	 * @brief Constructs a new CShareazaApp object.
	 */
	CShareazaApp();

	/**
	 * @brief Destroys the CShareazaApp object.
	 */
	virtual ~CShareazaApp();

	HANDLE				m_pMutex;
	CMutex				m_pSection;
	WORD				m_nVersion[4];
	BYTE				m_pBTVersion[4];			// SZxx
	CString				m_sVersion;					// x.x.x.x
	CString				m_sVersionLong;				// x.x.x.x Release/Debug 32-bit/64-bit (rXXXX date)
	CString				m_sSmartAgent;				// Shareaza x.x.x.x
	CString				m_sBuildDate;
	CString				m_strBinaryPath;			// Shareaza.exe path
	BYTE				m_nFontQuality;
	CFont				m_gdiFont;
	CFont				m_gdiFontBold;
	CFont				m_gdiFontLine;
	CWnd*				m_pSafeWnd;
	volatile LONG		m_bBusy;					// Shareaza is busy
	volatile bool		m_bInteractive;				// Shareaza begins initialization
	volatile bool		m_bLive;					// Shareaza fully initialized
	volatile bool		m_bClosing;					// Shareaza begins closing
	bool				m_bIsServer;				// Is OS a Server version
	bool				m_bIsWin2000;				// Is OS Windows 2000
	bool				m_bIsVistaOrNewer;			// Is OS Vista or newer
	bool				m_bIs7OrNewer;				// Is OS 7 or newer
	bool				m_bLimitedConnections;		// Networking is limited (XP SP2)
	BOOL				m_bMenuWasVisible;			// For the menus in media player window
	DWORD				m_nLastInput;				// Time of last input event (in secs)
	HHOOK				m_hHookKbd;
	HHOOK				m_hHookMouse;
	CPacketWnd*			m_pPacketWnd;				// Packet Window (NULL - not opened)
	CShareazaCommandLineInfo m_cmdInfo;				// Command-line options

	// Cryptography Context handle
	HCRYPTPROV			m_hCryptProv;

	// Kernel functions
	HRESULT		(WINAPI *m_pRegisterApplicationRestart)( __in_opt PCWSTR pwzCommandline, __in DWORD dwFlags );

	// For themes functions
	HINSTANCE			m_hTheme;
	HRESULT		(WINAPI *m_pfnSetWindowTheme)(HWND, LPCWSTR, LPCWSTR);
	BOOL		(WINAPI *m_pfnIsThemeActive)(VOID);
	HANDLE		(WINAPI *m_pfnOpenThemeData)(HWND, LPCWSTR);
	HRESULT		(WINAPI *m_pfnCloseThemeData)(HANDLE);
	HRESULT		(WINAPI *m_pfnDrawThemeBackground)(HANDLE, HDC, int, int, const RECT*, const RECT*);
	HRESULT		(WINAPI *m_pfnGetThemeSysFont)(HTHEME, int, __out LOGFONTW* );

	// Shell functions
	HINSTANCE			m_hShlWapi;
	BOOL		(WINAPI *m_pfnAssocIsDangerous)(LPCWSTR);

	HINSTANCE			m_hShell32;
	HRESULT		(WINAPI *m_pfnSHGetFolderPathW)(__reserved HWND hwnd, __in int csidl, __in_opt HANDLE hToken, __in DWORD dwFlags, __out_ecount(MAX_PATH) LPWSTR pszPath);
	HRESULT		(WINAPI *m_pfnSHGetKnownFolderPath)(__in REFKNOWNFOLDERID rfid, __in DWORD /* KNOWN_FOLDER_FLAG */ dwFlags, __in_opt HANDLE hToken, __deref_out PWSTR *ppszPath);
	HRESULT		(WINAPI *m_pfnSHCreateItemFromParsingName)(__in PCWSTR pszPath, __in_opt IBindCtx *pbc, __in REFIID riid, __deref_out void **ppv);
	HRESULT		(WINAPI *m_pfnSHGetPropertyStoreFromParsingName)(__in PCWSTR pszPath, __in_opt IBindCtx *pbc, __in GETPROPERTYSTOREFLAGS flags, __in REFIID riid, __deref_out void **ppv);
	HRESULT		(WINAPI *m_pfnSetCurrentProcessExplicitAppUserModelID)(__in PCWSTR pszAppID);
	HRESULT		(WINAPI *m_pfnSHGetImageList)(__in int iImageList, __in REFIID riid, __out void **ppv);

	HINSTANCE			m_hUser32;
	BOOL		(WINAPI *m_pfnChangeWindowMessageFilter)(UINT message, DWORD dwFlag);
	BOOL		(WINAPI *m_pfnShutdownBlockReasonCreate)(_In_ HWND hWnd, _In_ LPCWSTR pwszReason);
	BOOL		(WINAPI *m_pfnShutdownBlockReasonDestroy)(_In_ HWND hWnd);

	BOOL GetPropertyStoreFromParsingName( LPCWSTR pszPath, IPropertyStore**ppv );

	// GeoIP - IP to Country lookup
	HINSTANCE			m_hGeoIP;
	GeoIP*				m_pGeoIP;
	typedef GeoIP* (*GeoIP_newFunc)(int);
	typedef int (*GeoIP_cleanupFunc)(void);
	typedef void (*GeoIP_deleteFunc)(GeoIP* gi);
	typedef const char * (*GeoIP_country_code_by_ipnumFunc) (GeoIP* gi, unsigned long ipnum);
	typedef const char * (*GeoIP_country_name_by_ipnumFunc) (GeoIP* gi, unsigned long ipnum);
	typedef const char * (*GeoIP_country_code_by_ipnumFunc_v6) (GeoIP* gi, geoipv6_t ipnum);
	typedef const char * (*GeoIP_country_name_by_ipnumFunc_v6) (GeoIP* gi, geoipv6_t ipnum);
	GeoIP_cleanupFunc				m_pfnGeoIP_cleanup;
	GeoIP_deleteFunc				m_pfnGeoIP_delete;
	GeoIP_country_code_by_ipnumFunc	m_pfnGeoIP_country_code_by_ipnum;
	GeoIP_country_name_by_ipnumFunc	m_pfnGeoIP_country_name_by_ipnum;
	GeoIP_country_code_by_ipnumFunc_v6	m_pfnGeoIP_country_code_by_ipnum_v6;
	GeoIP_country_name_by_ipnumFunc_v6	m_pfnGeoIP_country_name_by_ipnum_v6;

	HINSTANCE			m_hLibGFL;

	/**
	 * @brief Loads a library and returns a handle to it.
	 * @param pszPath The path to the library to load.
	 * @return A handle to the loaded library, or NULL if the library could not be loaded.
	 */
	HINSTANCE			CustomLoadLibrary(LPCTSTR);

	/**
	 * @brief Gets a pointer to the main window.
	 * @return A pointer to the main window, or NULL if the main window has not been created yet.
	 */
	CMainWnd*			SafeMainWnd() const;

	/**
	 * @brief Handles an internal URI.
	 * @param pszURI The URI to handle.
	 * @return TRUE if the URI was handled successfully, FALSE otherwise.
	 */
	BOOL				InternalURI(LPCTSTR pszURI);

	// Logging functions
	CLogMessageList		m_oMessages;	// Log temporary storage
	CCriticalSection	m_csMessage;	// m_oMessages guard
	/**
	 * @brief Checks if a log message type is disabled.
	 * @param nType The type of the log message.
	 * @return true if the log message type is disabled, false otherwise.
	 */
	bool				IsLogDisabled(WORD nType) const;

	/**
	 * @brief Shows the startup text.
	 */
	void				ShowStartupText();

	/**
	 * @brief Logs a message.
	 * @param nType The type of the message.
	 * @param nID The resource ID of the message string.
	 * @param ... Additional arguments to be formatted into the message string.
	 */
	void				Message(WORD nType, UINT nID, ...);

	/**
	 * @brief Logs a message.
	 * @param nType The type of the message.
	 * @param pszFormat The format string for the message.
	 * @param ... Additional arguments to be formatted into the message string.
	 */
	void				Message(WORD nType, LPCTSTR pszFormat, ...);

	/**
	 * @brief Prints a message to the log file and the system window.
	 * @param nType The type of the message.
	 * @param strLog The message to print.
	 */
	void				PrintMessage(WORD nType, const CString& strLog);

	/**
	 * @brief Updates the splash screen.
	 * @param pszMessage The message to display on the splash screen.
	 * @param nMax The maximum value for the progress bar.
	 * @param bClosing true if the application is closing, false otherwise.
	 */
	void				SplashStep(LPCTSTR pszMessage = NULL, int nMax = 0, bool bClosing = false);

	/**
	 * @brief Aborts the splash screen.
	 */
	void				SplashAbort();

	/**
	 * @brief Gets the country code for an IPv4 address.
	 * @param pAddress The IPv4 address.
	 * @return The country code for the address.
	 */
	CString				GetCountryCode(IN_ADDR pAddress) const;

	/**
	 * @brief Gets the country name for an IPv4 address.
	 * @param pAddress The IPv4 address.
	 * @return The country name for the address.
	 */
	CString				GetCountryName(IN_ADDR pAddress) const;

	/**
	 * @brief Gets the country code for an IPv6 address.
	 * @param pAddress The IPv6 address.
	 * @return The country code for the address.
	 */
	CString				GetCountryCode(IN6_ADDR pAddress) const;

	/**
	 * @brief Gets the country name for an IPv6 address.
	 * @param pAddress The IPv6 address.
	 * @return The country name for the address.
	 */
	CString				GetCountryName(IN6_ADDR pAddress) const;

	/**
	 * @brief Opens a file or URL.
	 * @param lpszFileName The name of the file or URL to open.
	 * @return A pointer to the document that was opened, or NULL if the file or URL could not be opened.
	 */
	virtual CDocument*	OpenDocumentFile(LPCTSTR lpszFileName);

	/**
	 * @brief Opens a file or URL.
	 * @param lpszFileName The name of the file or URL to open.
	 * @param bDoIt If TRUE, the file or URL will be opened. If FALSE, the function will only check if the file or URL can be opened.
	 * @param bDispay If TRUE, the file will be displayed in the library.
	 * @return TRUE if the file or URL was opened successfully, FALSE otherwise.
	 */
	BOOL				Open(LPCTSTR lpszFileName, BOOL bDoIt, BOOL bDispay = FALSE);

	/**
	 * @brief Shows a file in the library.
	 * @param lpszFileName The name of the file to show.
	 * @param bDoIt If TRUE, the file will be shown. If FALSE, the function will only check if the file can be shown.
	 * @return TRUE if the file was shown successfully, FALSE otherwise.
	 */
	BOOL				DisplayFile(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Opens a host list file.
	 * @param lpszFileName The name of the host list file to open.
	 * @param bDoIt If TRUE, the file will be opened. If FALSE, the function will only check if the file can be opened.
	 * @return TRUE if the file was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenImport(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Opens a shell shortcut file.
	 * @param lpszFileName The name of the shell shortcut file to open.
	 * @param bDoIt If TRUE, the file will be opened. If FALSE, the function will only check if the file can be opened.
	 * @return TRUE if the file was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenShellShortcut(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Opens an internet shortcut file.
	 * @param lpszFileName The name of the internet shortcut file to open.
	 * @param bDoIt If TRUE, the file will be opened. If FALSE, the function will only check if the file can be opened.
	 * @return TRUE if the file was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenInternetShortcut(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Opens a torrent file.
	 * @param lpszFileName The name of the torrent file to open.
	 * @param bDoIt If TRUE, the file will be opened. If FALSE, the function will only check if the file can be opened.
	 * @return TRUE if the file was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenTorrent(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Opens a collection file.
	 * @param lpszFileName The name of the collection file to open.
	 * @param bDoIt If TRUE, the file will be opened. If FALSE, the function will only check if the file can be opened.
	 * @return TRUE if the file was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenCollection(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Opens a URL.
	 * @param lpszFileName The URL to open.
	 * @param bDoIt If TRUE, the URL will be opened. If FALSE, the function will only check if the URL can be opened.
	 * @param bSilent If TRUE, the URL will be opened silently.
	 * @return TRUE if the URL was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenURL(LPCTSTR lpszFileName, BOOL bDoIt, BOOL bSilent = FALSE);

	/**
	 * @brief Opens a Shareaza download file.
	 * @param lpszFileName The name of the download file to open.
	 * @param bDoIt If TRUE, the file will be opened. If FALSE, the function will only check if the file can be opened.
	 * @return TRUE if the file was opened successfully, FALSE otherwise.
	 */
	BOOL				OpenDownload(LPCTSTR lpszFileName, BOOL bDoIt);

	/**
	 * @brief Gets the path to the Windows folder.
	 * @return The path to the Windows folder.
	 */
	CString				GetWindowsFolder() const;

	/**
	 * @brief Gets the path to the 64-bit Program Files folder.
	 * @return The path to the 64-bit Program Files folder.
	 */
	CString				GetProgramFilesFolder64() const;

	/**
	 * @brief Gets the path to the Program Files folder.
	 * @return The path to the Program Files folder.
	 */
	CString				GetProgramFilesFolder() const;

	/**
	 * @brief Gets the path to the Documents folder.
	 * @return The path to the Documents folder.
	 */
	CString				GetDocumentsFolder() const;

	/**
	 * @brief Gets the path to the Downloads folder.
	 * @return The path to the Downloads folder.
	 */
	CString				GetDownloadsFolder() const;

	/**
	 * @brief Gets the path to the AppData folder.
	 * @return The path to the AppData folder.
	 */
	CString				GetAppDataFolder() const;

	/**
	 * @brief Gets the path to the Local AppData folder.
	 * @return The path to the Local AppData folder.
	 */
	CString				GetLocalAppDataFolder() const;

	/**
	 * @brief Renames, deletes, or releases a file.
	 * @param strSource The path to the file to rename, delete, or release.
	 * @param pszTarget The new path for the file. If this parameter is 0, the file will be deleted. If this parameter is 1, the file will be released.
	 */
	void				OnRename(LPCTSTR strSource, LPCTSTR pszTarget = (LPCTSTR)1);

	/**
	 * @brief Gets a pointer to the database.
	 * @return A pointer to the database. The caller is responsible for deleting the pointer.
	 */
	CDatabase*			GetDatabase() const;

	/**
	 * @brief Copies text to the clipboard.
	 * @param strText The text to copy to the clipboard.
	 * @return TRUE if the text was copied successfully, FALSE otherwise.
	 */
	BOOL SetClipboardText(const CString& strText);

protected:
	CSplashDlg*			m_dlgSplash;		// Splash dialog

	virtual BOOL		InitInstance();
	virtual int			ExitInstance();
	virtual void		WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL		Register();
	virtual BOOL		Unregister();
	virtual void		AddToRecentFileList(LPCTSTR lpszPathName);

	void				InitResources();	// Initialize Shareaza version, system info, load DLLs, etc.
	void				InitFonts();		// Create default fonts
	BOOL				ParseCommandLine();	// Parse and execute command-line

	void				LoadCountry();		// Load the GeoIP library for mapping IPs to countries
	void				FreeCountry();		// Free GeoIP resources

	DECLARE_MESSAGE_MAP()
	MiniDmpSender* mpSender;

private:
	CShareazaApp(const CShareazaApp&);
	CShareazaApp& operator=(const CShareazaApp&);
	void RegisterBugReporting();
};

extern CShareazaApp			theApp;						// Shareaza Application
extern SYSTEM_INFO			System;						// System Information


class CProgressDialog : public CComPtr< IProgressDialog >
{
public:
	CProgressDialog(LPCTSTR szTitle, DWORD dwFlags = PROGDLG_NOCANCEL | PROGDLG_AUTOTIME);
	virtual ~CProgressDialog();

	void Progress(LPCTSTR szText, QWORD nCompleted = 0, QWORD nTotal = 0);
};

//
// Utility Functions
//

// Detect Administrative privileges
BOOL IsRunAsAdmin();

// Post message to main window in safe way
BOOL PostMainWndMessage(UINT Msg, WPARAM wParam = NULL, LPARAM lParam = NULL);

CRuntimeClass* AfxClassForName(LPCTSTR pszClass);

BOOL	LoadString(CString& str, UINT nID);
CString	LoadString(UINT nID);
BOOL	LoadSourcesString(CString& str, DWORD num, bool bFraction=false);

DWORD	TimeFromString(LPCTSTR psz);
CString	TimeToString(time_t tVal);
BOOL	TimeFromString(LPCTSTR psz, FILETIME* pTime);
CString	TimeToString(FILETIME* pTime);

void	RecalcDropWidth(CComboBox* pWnd, int nMargin = 0);
// Load 16x16, 32x32, 48x48 icons from .ico, .exe, .dll files
BOOL LoadIcon(LPCTSTR szFilename, HICON* phSmallIcon, HICON* phLargeIcon, HICON* phHugeIcon, int nIcon = 0);
// Load 16x16 icon from module pointed by its CLSID
HICON LoadCLSIDIcon(LPCTSTR szCLSID);
// Load and add icon to CImageList, mirrored if needed
int		AddIcon(UINT nIcon, CImageList& gdiImageList);
// Add icon to CImageList, mirrored if needed
int		AddIcon(HICON hIcon, CImageList& gdiImageList);
// Create mirrored icon. Returns:
// mirrored icon (original destroyed if needed) if succeed or original icon otherwise
HICON	CreateMirroredIcon(HICON hIconOrig, BOOL bDestroyOriginal = TRUE);
HBITMAP	CreateMirroredBitmap(HBITMAP hbmOrig);

LRESULT CALLBACK KbdHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam);

// Generate safe file name for file system (bPath == true - allow path i.e. "\" symbol)
CString SafeFilename(CString strName, bool bPath = false);

// Create directory. If one or more of the intermediate folders do not exist, they are created as well.
BOOL CreateDirectory(LPCTSTR szPath);

// Delete file(s) with user confirmation
void DeleteFiles(CStringList& pList);

// Delete file in many ways
BOOL DeleteFileEx(LPCTSTR szFileName, BOOL bShared, BOOL bToRecycleBin, BOOL bEnableDelayed);

// Delete postponed file
void PurgeDeletes();

// Loads RT_HTML or RT_GZIP resource as string
CString LoadHTML(HINSTANCE hInstance, UINT nResourceID);
CString LoadRichHTML(UINT nResourceID, CString& strResponse, CShareazaFile* pFile = NULL);

// Save icon in .ico-format to buffer
BOOL SaveIcon(HICON hIcon, CBuffer& oBuffer, int colors = -1);

// Loads well-known resource for HTTP-uploading
bool ResourceRequest(const CString& strPath, CBuffer& pResponse, CString& sHeader);

// Mark file as downloaded from Internet (using NTFS stream)
bool MarkFileAsDownload(const CString& sFilename);

// Load GUID from NTFS stream of file
bool LoadGUID(const CString& sFilename, Hashes::Guid& oGUID);

// Save GUID to NTFS stream of file
bool SaveGUID(const CString& sFilename, const Hashes::Guid& oGUID);

// Resolve shell shortcut (.lnk file)
CString ResolveShortcut(LPCTSTR lpszFileName);

// Get Win32 API error description
CString GetErrorString(DWORD dwError = GetLastError());

// Show message box using GetErrorString() message
void ReportError(DWORD dwError = GetLastError());

// Displays a dialog box enabling the user to select a Shell folder
CString BrowseForFolder(UINT nTitle, LPCTSTR szInitialPath = NULL, HWND hWnd = NULL);
CString BrowseForFolder(LPCTSTR szTitle, LPCTSTR szInitialPath = NULL, HWND hWnd = NULL);

// Do message loop
void SafeMessageLoop();

// Detect full screen application
BOOL IsUserUsingFullscreen();

// Start Windows service
BOOL AreServiceHealthy(LPCTSTR szService);

// Creates shell link
IShellLink* CreateShellLink(LPCWSTR szTargetExecutablePath, LPCWSTR szCommandLineArgs, LPCWSTR szTitle, LPCWSTR szIconPath, int nIconIndex, LPCWSTR szDescription);

// Select existing string of ComboBox, or add and select a new one
void AddAndSelect(CComboBox& wndBox, const CString& sText);

struct CompareNums
{
	bool operator()(WORD lhs, WORD rhs) const
	{
		return lhs > rhs;
	}
};

// Use with whole numbers only
template <typename T>
inline T GetRandomNum(const T& min, const T& max)
{
	if ( theApp.m_hCryptProv != 0 )
	{
		T nRandom = 0;
		if ( CryptGenRandom( theApp.m_hCryptProv, sizeof( T ), (BYTE*)&nRandom ) )
			return static_cast< T >( (double)nRandom  * ( (double)max - (double)min + 1 ) / ( (double)static_cast< T >( -1 ) + 1 ) + min );
	}

	// Fallback to non-secure method
	return static_cast< T >( (double)rand() * ( max - min + 1 ) / ( (double)RAND_MAX + 1 ) + min );
}

template <>
__int8 GetRandomNum<__int8>(const __int8& min, const __int8& max);

template <>
__int16 GetRandomNum<__int16>(const __int16& min, const __int16& max);

template <>
__int32 GetRandomNum<__int32>(const __int32& min, const __int32& max);

template <>
__int64 GetRandomNum<__int64>(const __int64& min, const __int64& max);

#define WM_WINSOCK			(WM_APP+101)	// Winsock messages proxy to Network object ( used by WSAAsyncGetHostByName() function )
#define WM_VERSIONCHECK		(WM_APP+102)	// Version check ( WAPARM: VERSION_CHECK nCode, LPARAM: unused )
#define WM_OPENCHAT			(WM_APP+103)	// Open chat window ( WAPARM: CChatSession* pChat, LPARAM: unused )
#define WM_TRAY				(WM_APP+104)	// Tray icon notification ( WPARAM: unused, LPARAM: uMouseMessage )
#define WM_URL				(WM_APP+105)	// Open URL ( WPARAM: CShareazaURL* pURL, LPARAM: unused )
#define WM_SKINCHANGED		(WM_APP+106)	// Skin change ( WPARAM: unused, LPARAM: unused )
#define WM_COLLECTION		(WM_APP+107)	// Open collection file ( WPARAM: LPTSTR szFilename, LPARAM: unused )
#define WM_OPENSEARCH		(WM_APP+108)	// Open new search ( WPARAM: CQuerySearch* pSearch, LPARAM: unused )
#define WM_LIBRARYSEARCH	(WM_APP+110)	// Start file library search ( WPARAM: LPTSTR pszSearch, LPARAM: unused )
#define WM_PLAYFILE			(WM_APP+111)	// Play file by media system ( WPARAM: TRISTATE bForcePlay - TRI_TRUE - force play, TRI_FALSE - force enqueue, LPARAM: CString* pFilename )
#define WM_SETALPHA			(WM_APP+113)	// Increase/decrease main window transparency ( WPARAM: 0 - to decrease or 1 - to increase, LPARAM: unused )
#define WM_METADATA			(WM_APP+114)	// Set/clear library meatapanel data and status message ( WPARAM: CMetaPanel* pPanelData, LPARAM: LPCTSTR pszMessage )
#define WM_SANITY_CHECK		(WM_APP+115)	// Run allsystem check against banned hosts ( WPARAM: unused, LPARAM: unused )
#define WM_NOWUPLOADING		(WM_APP+117)	// New upload notification ( WPARAM: unused, LPARAM: CString* pFilename )
#define WM_TORRENT			(WM_APP+118)	// Open torrent file ( WPARAM: LPTSTR szFilename, LPARAM: unused )
#define WM_IMPORT			(WM_APP+119)	// Import hub list file ( WPARAM: LPTSTR szFilename, LPARAM: unused )

// WM_COPYDATA types
#define COPYDATA_SCHEDULER	0				// Scheduler task ( lpData: LPCTSTR szTaskData - encoded string )
#define COPYDATA_OPEN		1				// Open file ( lpData: LPCTSTR szFilename - file name or URL )

#define ID_PLUGIN_FIRST	27000
#define ID_PLUGIN_LAST	27999

#define WM_COPYGLOBALDATA	0x0049			// Undocumented way for drag-n-drop

#define PANEL_WIDTH			200				// Left panel default size in pixels (Home, Search, IRC tabs)
#define THUMB_STORE_SIZE	128				// Thumbnail dimensions (128x128 px)

#define HTTP_HEADER_MAX_LINE	(256 * 1024)// Maximum allowed size of single HTTP-header line (256 Kb)


// Client's name
#define CLIENT_NAME			"Shareaza"
#define CLIENT_NAME_T		_T( CLIENT_NAME )

#define MOD_CLIENT_NAME_T	_T("i") CLIENT_NAME_T

// Client's main window class name
#define CLIENT_HWND			CLIENT_NAME_T _T("MainWnd")

// Client's settings key
#define REGISTRY_KEY		_T("Software\\") CLIENT_NAME_T _T("\\") CLIENT_NAME_T

// Network ID stuff

// 4 Character vendor code (used on G1, G2)
// BEAR, LIME, RAZA, RAZB, etc
#define VENDOR_CODE			"RAZI"

// ed2k client ID number.
// 0 = eMule, 1 = cDonkey, 4 = old Shareaza alpha/beta/mod/fork, 0x28 (40) = Shareaza, 0xcb (203) = ShareazaPlus with RazaCB core, etc
#define ED2K_CLIENT_ID		41

// 2 Character BT peer-id code
// SZ = Shareaza, S~ = old Shareaza alpha/beta , CB = ShareazaPlus with RazaCB core, AZ = Azureus, etc
#define BT_ID1				'i'
#define BT_ID2				'S'

#define WEB_SITE			"https://github.com/ansani/Shareaza/releases"
#define WEB_SITE_T			_T( WEB_SITE )
#define URI_UPDATE			_T("https://api.github.com/repos/ansani/Shareaza/releases/latest")
#define CONNECTION_SITE_T	_T("https://connectiontest.kirurusec.com/")

// URLs used by Shareaza
// -----------------------------------------------------------------------
// ID					URL								Notes
// -----------------------------------------------------------------------
//
// Help
//
// ID_HELP_FAQ		help/?faq
// ID_HELP_TEST		help/test/?port=x&lang=x&Version=x.x.x.x
//
// Help/Guides
//
// ID_HELP_GUIDE	help/?guide
// ID_HELP_ROUTER	help/?router
// ID_HELP_SECURITY	help/?security
// ID_HELP_CODEC	help/?codec
//
// Help/Websites
//
// ID_HELP_HOMEPAGE	?Version=x.x.x.x
// ID_HELP_FORUMS	help/?forum
// ID_HELP_UPDATE	help/update/?Version=x.x.x.x			unused
// ID_HELP_WEB_1	help/external/?link1					unused
// ID_HELP_WEB_2	help/external/?link1					unused
// ID_HELP_WEB_3	help/external/?link1					unused
// ID_HELP_WEB_4	help/external/?link1					unused
// ID_HELP_WEB_5	help/external/?link1					unused
// ID_HELP_WEB_6	help/external/?link1					Skin Guide
//
// ID_HELP_TORRENT	help/?torrentencoding					Torrent Encoding Help
// -				?id=support								BugTrap
// -				version/?Version=x.x.x.x&&Platform=x	CVersionChecker


// Drag-n-drop stuff

#define MAX_DRAG_SIZE		256
#define MAX_DRAG_SIZE_2		(MAX_DRAG_SIZE/2)
#define DRAG_COLOR_KEY		(RGB(250,255,250))	// Light-green
#define DRAG_HOVER_TIME		1000				// Dragging mouse press button after X ms

extern const LPCTSTR RT_BMP;
extern const LPCTSTR RT_JPEG;
extern const LPCTSTR RT_PNG;
extern const LPCTSTR RT_GZIP;

//
// Library.h
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

#include "ComObject.h"
#include "ThreadImpl.h"
#include "SharedFile.h"

class CQueryHit;
class CQuerySearch;
class CLibraryFolder;
class CAlbumFolder;

#define LIBRARY_SER_VERSION	29
// History:
// 27 - Changed CLibraryFile metadata saving order (ryo-oh-ki)
// 28 - Added CLibraryMaps m_pIndexMap, m_pNameMap and m_pPathMap counts (ryo-oh-ki)
// 29 - Added CLibraryDictionary serialize (ryo-oh-ki)

/**
 * @brief Manages the user's library of shared files.
 *
 * This class is responsible for loading, saving, and managing the user's library
 * of shared files. It also provides methods for searching the library and
 * handling query hits.
 */
class CLibrary :
	public CComObject,
	public CThreadImpl
{
// Construction
public:
	/**
	 * @brief Constructs a new CLibrary object.
	 */
	CLibrary();

	/**
	 * @brief Destroys the CLibrary object.
	 */
	virtual ~CLibrary();

	DECLARE_DYNAMIC(CLibrary)

// Attributes
public:
	mutable CMutexEx	m_pSection;

protected:
	volatile LONG	m_nUpdateCookie;		// Library cookie (ms)
	volatile LONG	m_nScanCookie;			// Used by CLibraryFolder::ThreadScan()
	volatile DWORD	m_nScanCount;			// Library scan counter
	volatile DWORD	m_nScanTime;			// Last library scan time (ms)
	volatile LONG	m_nForcedUpdate;		// Forced update request
	volatile LONG	m_nSaveCookie;			// Library last save cookie (ms)
	volatile DWORD	m_nSaveTime;			// Library last save time (ms)
	int				m_nFileSwitch;			// Library next save slot number

// Sync Operations
public:
	inline DWORD GetCookie() const
	{
		return m_nUpdateCookie;
	}

	inline DWORD GetScanCookie()
	{
		return (DWORD)InterlockedIncrement( &m_nScanCookie );
	}

	inline DWORD GetScanCount() const
	{
		return m_nScanCount;
	}

	// Mark library as modified:
	// bForce = false - Library has internal changes so it must be saved;
	// bForce = true - Library has also disk changes so it must be rescanned.
	inline void Update(bool bForce = false)
	{
		InterlockedExchange( &m_nUpdateCookie, (LONG)GetTickCount() );

		if ( bForce )
			InterlockedExchange( &m_nForcedUpdate, TRUE );
	}

// File and Folder Operations
public:
	/**
	 * @brief Looks up a file in the library by its index.
	 * @param nIndex The index of the file to look up.
	 * @param bSharedOnly If TRUE, only shared files will be returned.
	 * @param bAvailableOnly If TRUE, only available files will be returned.
	 * @return A pointer to the file, or NULL if the file could not be found.
	 */
	CLibraryFile*	LookupFile(DWORD nIndex, BOOL bSharedOnly = FALSE, BOOL bAvailableOnly = FALSE) const;

	/**
	 * @brief Gets the root album folder.
	 * @return A pointer to the root album folder.
	 */
	CAlbumFolder*	GetAlbumRoot();

	/**
	 * @brief Adds a file to the library.
	 * @param pFile A pointer to the file to add.
	 */
	void			AddFile(CLibraryFile* pFile);

	/**
	 * @brief Removes a file from the library.
	 * @param pFile A pointer to the file to remove.
	 */
	void			RemoveFile(CLibraryFile* pFile);

// General Operations
public:
	/**
	 * @brief Updates the alternate sources for library files.
	 * @param pHits A pointer to the query hits to process.
	 * @return true if the alternate sources were updated successfully, false otherwise.
	 */
	bool			OnQueryHits(const CQueryHit* pHits);

	/**
	 * @brief Searches the library for files.
	 * @param pSearch A pointer to the search query.
	 * @param nMaximum The maximum number of results to return.
	 * @param bLocal If TRUE, only the local library will be searched.
	 * @param bAvailableOnly If TRUE, only available files will be returned.
	 * @return A pointer to a list of matching files. The caller is responsible for deleting the list.
	 */
	CFileList*		Search(const CQuerySearch* pSearch, int nMaximum = 0, bool bLocal = false, bool bAvailableOnly = false);

	/**
	 * @brief Clears the library.
	 */
	void			Clear();

	/**
	 * @brief Loads the library from disk.
	 * @return TRUE if the library was loaded successfully, FALSE otherwise.
	 */
	BOOL			Load();

	/**
	 * @brief Saves the library to disk.
	 * @return TRUE if the library was saved successfully, FALSE otherwise.
	 */
	BOOL			Save();

	/**
	 * @brief Stops the library thread.
	 */
	void			StopThread();

	/**
	 * @brief Checks if a file is a bad file.
	 * @param szFilenameOnly The name of the file to check.
	 * @param szPathOnly The path to the file to check.
	 * @param dwFileAttributes The attributes of the file to check.
	 * @return TRUE if the file is a bad file, FALSE otherwise.
	 */
	static BOOL		IsBadFile(LPCTSTR szFilenameOnly, LPCTSTR szPathOnly = NULL, DWORD dwFileAttributes = 0);

protected:
	void			OnRun();
	void			Serialize(CArchive& ar);
	BOOL			ThreadScan();
	BOOL			SafeReadTime(CFile& pFile, FILETIME* pFileTime) throw();
	BOOL			SafeSerialize(CArchive& ar) throw();

// Automation
protected:
	BEGIN_INTERFACE_PART(Library, ILibrary)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get_Folders)(ILibraryFolders FAR* FAR* ppFolders);
		STDMETHOD(get_Albums)(IUnknown FAR* FAR* ppAlbums);
		STDMETHOD(get_Files)(ILibraryFiles FAR* FAR* ppFiles);
		STDMETHOD(FindByName)(BSTR sName, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(FindByPath)(BSTR sPath, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(FindByURN)(BSTR sURN, ILibraryFile FAR* FAR* ppFile);
		STDMETHOD(FindByIndex)(LONG nIndex, ILibraryFile FAR* FAR* ppFile);
	END_INTERFACE_PART(Library)

	DECLARE_INTERFACE_MAP()
};

extern CLibrary Library;

#include "LibraryList.h"
#include "LibraryMaps.h"

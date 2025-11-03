//
// Downloads.h
//
// Copyright (c) Shareaza Development Team, 2002-2014.
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

class CBuffer;
class CConnection;
class CDownload;
class CDownloadSource;
class CEDClient;
class CLibraryFile;
class CMatchFile;
class CQueryHit;
class CShareazaURL;


/**
 * @brief Manages the application's downloads.
 *
 * This class is responsible for adding, removing, and managing downloads.
 * It also provides methods for loading and saving the download list to disk.
 */
class CDownloads
{
public:
	enum { dlPathNull, dlPathComplete, dlPathIncomplete };

// Construction
public:
	/**
	 * @brief Constructs a new CDownloads object.
	 */
	CDownloads();

	/**
	 * @brief Destroys the CDownloads object.
	 */
	virtual ~CDownloads();

// Attributes
public:
	DWORD			m_tBandwidthAtMax;			// The last time download bandwidth was all in use
	DWORD			m_tBandwidthAtMaxED2K;		// The last time all ed2k bandwidth was used
	DWORD			m_nTransfers;
	DWORD			m_nBandwidth;
	DWORD			m_nTryingCount;
	DWORD			m_nTryingNoSourcesCount;
	bool			m_bClosing;
	QWORD			m_nComplete;				// The last complete size of incomplete downloads
	QWORD			m_nTotal;					// The last total size of incomplete downloads
	DWORD			m_tDateAdded;				// The time file was added in the downloads

private:
	CList< CDownload* >	m_pList;
	CMap< ULONG, ULONG, DWORD, DWORD > m_pHostLimits;
	int				m_nRunCookie;
	DWORD			m_tBandwidthLastCalc;		// The last time the bandwidth was calculated
	DWORD			m_nLimitGeneric;
	DWORD			m_nLimitDonkey;
	bool			m_bAllowMoreDownloads;
	bool			m_bAllowMoreTransfers;

// Operations
public:
	/**
	 * @brief Adds a new download to the list.
	 * @param bAddToHead If TRUE, the download will be added to the head of the list.
	 * @return A pointer to the new download.
	 */
	CDownload*	Add(BOOL bAddToHead = FALSE);

	/**
	 * @brief Adds a new download to the list from a query hit.
	 * @param pHit A pointer to the query hit.
	 * @param bAddToHead If TRUE, the download will be added to the head of the list.
	 * @return A pointer to the new download.
	 */
	CDownload*	Add(CQueryHit* pHit, BOOL bAddToHead = FALSE);

	/**
	 * @brief Adds a new download to the list from a match file.
	 * @param pFile A pointer to the match file.
	 * @param bAddToHead If TRUE, the download will be added to the head of the list.
	 * @return A pointer to the new download.
	 */
	CDownload*	Add(CMatchFile* pFile, BOOL bAddToHead = FALSE);

	/**
	 * @brief Adds a new download to the list from a URL.
	 * @param oURL The URL.
	 * @param bAddToHead If TRUE, the download will be added to the head of the list.
	 * @return A pointer to the new download.
	 */
	CDownload*	Add(const CShareazaURL& oURL, BOOL bAddToHead = FALSE);

	/**
	 * @brief Pauses all downloads.
	 */
	void		PauseAll();

	/**
	 * @brief Clears all completed downloads from the list.
	 */
	void		ClearCompleted();

	/**
	 * @brief Clears all paused downloads from the list.
	 */
	void		ClearPaused();

	/**
	 * @brief Clears all downloads from the list.
	 * @param bClosing If TRUE, the application is closing.
	 */
	void		Clear(bool bClosing = false);

	/**
	 * @brief Closes all active transfers.
	 */
	void		CloseTransfers();

	/**
	 * @brief Gets the number of seeding downloads.
	 * @return The number of seeding downloads.
	 */
	int			GetSeedCount() const;

	/**
	 * @brief Gets the number of downloads in the list.
	 * @param bActiveOnly If TRUE, only active downloads will be counted.
	 * @return The number of downloads in the list.
	 */
	INT_PTR		GetCount(BOOL bActiveOnly = FALSE) const;
	/**
	 * @brief Gets the number of downloads that are currently trying to connect.
	 * @param bTorrentsOnly If TRUE, only torrent downloads will be counted.
	 * @param nLimit The maximum number of downloads to count.
	 * @return The number of downloads that are currently trying to connect.
	 */
	DWORD		GetTryingCount(bool bTorrentsOnly = false, DWORD nLimit = 0) const;

	/**
	 * @brief Gets the number of connecting transfers.
	 * @param nLimit The maximum number of transfers to count.
	 * @return The number of connecting transfers.
	 */
	DWORD		GetConnectingTransferCount(DWORD nLimit = 0) const;

	/**
	 * @brief Checks if a download source is valid.
	 * @param pSource A pointer to the download source.
	 * @return TRUE if the download source is valid, FALSE otherwise.
	 */
	BOOL		Check(CDownloadSource* pSource) const;

	/**
	 * @brief Checks if a download is active.
	 * @param pDownload A pointer to the download.
	 * @param nScope The scope of the check.
	 * @return true if the download is active, false otherwise.
	 */
	bool		CheckActive(CDownload* pDownload, int nScope) const;
	/**
	 * @brief Moves a download in the list.
	 * @param pDownload A pointer to the download to move.
	 * @param nDelta The number of positions to move the download.
	 * @return TRUE if the download was moved successfully, FALSE otherwise.
	 */
	BOOL		Move(CDownload* pDownload, int nDelta);

	/**
	 * @brief Reorders a download in the list.
	 * @param pDownload A pointer to the download to reorder.
	 * @param pBefore A pointer to the download to place the download before.
	 * @return TRUE if the download was reordered successfully, FALSE otherwise.
	 */
	BOOL		Reorder(CDownload* pDownload, CDownload* pBefore);
	QWORD		GetAmountDownloadedFrom(IN_ADDR* pAddress);

	/**
	 * @brief Finds a download by its .sd-file name.
	 * @param sSDName The name of the .sd-file.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindBySDName(const CString& sSDName) const;
	/**
	 * @brief Finds a download by its path.
	 * @param sPath The path of the file.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindByPath(const CString& sPath) const;
	/**
	 * @brief Finds a download by its URN.
	 * @param pszURN The URN of the file.
	 * @param bSharedOnly If TRUE, only shared files will be searched.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindByURN(LPCTSTR pszURN, BOOL bSharedOnly = FALSE) const;
	/**
	 * @brief Finds a download by its SHA1 hash.
	 * @param oSHA1 The SHA1 hash of the file.
	 * @param bSharedOnly If TRUE, only shared files will be searched.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindBySHA1(const Hashes::Sha1Hash& oSHA1, BOOL bSharedOnly = FALSE) const;
	/**
	 * @brief Finds a download by its Tiger hash.
	 * @param oTiger The Tiger hash of the file.
	 * @param bSharedOnly If TRUE, only shared files will be searched.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindByTiger(const Hashes::TigerHash& oTiger, BOOL bSharedOnly = FALSE) const;
	/**
	 * @brief Finds a download by its eD2k hash.
	 * @param oED2K The eD2k hash of the file.
	 * @param bSharedOnly If TRUE, only shared files will be searched.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindByED2K(const Hashes::Ed2kHash& oED2K, BOOL bSharedOnly = FALSE) const;
	/**
	 * @brief Finds a download by its BitTorrent hash.
	 * @param oBTH The BitTorrent hash of the file.
	 * @param bSharedOnly If TRUE, only shared files will be searched.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindByBTH(const Hashes::BtHash& oBTH, BOOL bSharedOnly = FALSE) const;
	/**
	 * @brief Finds a download by its MD5 hash.
	 * @param oMD5 The MD5 hash of the file.
	 * @param bSharedOnly If TRUE, only shared files will be searched.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindByMD5(const Hashes::Md5Hash& oMD5, BOOL bSharedOnly = FALSE) const;
	/**
	 * @brief Finds a download by its serialization ID.
	 * @param nSerID The serialization ID of the file.
	 * @return A pointer to the download, or NULL if not found.
	 */
	CDownload*	FindBySID(DWORD nSerID) const;
	/**
	 * @brief Gets a free serialization ID.
	 * @return A free serialization ID.
	 */
	DWORD		GetFreeSID();

	/**
	 * @brief Loads all available .sd-files from the Incomplete folder.
	 */
	void		Load();

	/**
	 * @brief Loads a specified .sd-file.
	 * @param strPath The path to the .sd-file.
	 * @return A pointer to the loaded download.
	 */
	CDownload*	Load(const CString& strPath);

	/**
	 * @brief Saves the download list to disk.
	 * @param bForce If TRUE, the list will be saved even if it has not changed.
	 */
	void		Save(BOOL bForce = TRUE);

	/**
	 * @brief Called periodically to update the download list.
	 */
	void		OnRun();

	/**
	 * @brief Called when a push request is received.
	 * @param oGUID The GUID of the file being pushed.
	 * @param pConnection A pointer to the connection.
	 * @return TRUE if the push was handled, FALSE otherwise.
	 */
	BOOL		OnPush(const Hashes::Guid& oGUID, CConnection* pConnection);

	/**
	 * @brief Called when query hits are received.
	 * @param pHits A pointer to the query hits.
	 * @return true if the hits were handled, false otherwise.
	 */
	bool		OnQueryHits(const CQueryHit* pHits);

	/**
	 * @brief Called when a file has been hashed and verified in the library.
	 * @param pFile A pointer to the library file.
	 * @param bVerified The verification status of the file.
	 */
	void		OnVerify(const CLibraryFile* pFile, TRISTATE bVerified);

	/**
	 * @brief Renames, deletes, or releases a downloading file.
	 * @param pszSource The original path of the file.
	 * @param pszTarget The new path of the file. If NULL, the file is deleted. If 1, the file is released.
	 */
	void		OnRename(LPCTSTR pszSource, LPCTSTR pszTarget);

	/**
	 * @brief Sets the per-host transfer limit.
	 * @param pAddress The IP address of the host.
	 * @param nLimit The new limit.
	 */
	void		SetPerHostLimit(IN_ADDR* pAddress, DWORD nLimit);
	/**
	 * @brief Checks if there is enough space available to download a file.
	 * @param nVolume The size of the file.
	 * @param nPath The path to check for free space.
	 * @return TRUE if there is enough space available, FALSE otherwise.
	 */
	BOOL		IsSpaceAvailable(QWORD nVolume, int nPath = dlPathNull);

	void		UpdateAllows();
	/**
	 * @brief Checks if more downloads are allowed.
	 * @return true if more downloads are allowed, false otherwise.
	 */
	bool		AllowMoreDownloads() const;

	/**
	 * @brief Checks if more transfers are allowed.
	 * @return true if more transfers are allowed, false otherwise.
	 */
	bool		AllowMoreTransfers() const;

	/**
	 * @brief Checks if more transfers are allowed from a specific host.
	 * @param pAddress The IP address of the host.
	 * @param bFirstAttempt If TRUE, this is the first attempt to connect to this host.
	 * @return true if more transfers are allowed, false otherwise.
	 */
	bool		AllowMoreTransfers(IN_ADDR* pAdress, bool bFirstAttempt = false) const;
	/**
	 * @brief Removes a download from the list.
	 * @param pDownload A pointer to the download to remove.
	 */
	void		Remove(CDownload* pDownload);

	/**
	 * @brief Gets an iterator for the download list.
	 * @return A position value that can be used for iteration.
	 */
	POSITION	GetIterator() const;

	/**
	 * @brief Gets a reverse iterator for the download list.
	 * @return A position value that can be used for iteration.
	 */
	POSITION	GetReverseIterator() const;

	/**
	 * @brief Gets the next download in the list.
	 * @param pos The position of the current download.
	 * @return A pointer to the next download.
	 */
	CDownload*	GetNext(POSITION& pos) const;

	/**
	 * @brief Gets the previous download in the list.
	 * @param pos The position of the current download.
	 * @return A pointer to the previous download.
	 */
	CDownload*	GetPrevious(POSITION& pos) const;

	/**
	 * @brief Checks if a download is in the list.
	 * @param pDownload A pointer to the download.
	 * @return TRUE if the download is in the list, FALSE otherwise.
	 */
	BOOL		Check(CDownload* pDownload) const;
private:
	int			GetActiveTorrentCount() const;
//	DWORD		GetTransferCount() const;
	BOOL		Swap(CDownload* p1, CDownload* p2);
	DWORD		GetBandwidth() const;
	BOOL		OnDonkeyCallback(CEDClient* pClient, CDownloadSource* pExcept = NULL);
	void		LoadFromCompoundFiles();
	BOOL		LoadFromCompoundFile(LPCTSTR pszFile);
	BOOL		LoadFromTimePair();
	void		SerializeCompound(CArchive& ar);
	void		PurgePreviews();

	CDownloads(const CDownloads&);
	CDownloads& operator=(const CDownloads&);
};

extern CDownloads Downloads;

//
// Uploads.h
//
// Copyright (c) Shareaza Development Team, 2002-2012.
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

class CShareazaFile;
class CBuffer;
class CConnection;
class CUploadTransfer;


/**
 * @brief Manages the application's uploads.
 *
 * This class is responsible for adding, removing, and managing uploads.
 * It also provides methods for controlling bandwidth usage and handling
 * upload-related events.
 */
class CUploads
{
// Construction
public:
	/**
	 * @brief Constructs a new CUploads object.
	 */
	CUploads();

	/**
	 * @brief Destroys the CUploads object.
	 */
	~CUploads();

// Attributes
public:
	DWORD		m_nCount;			// Active count
	DWORD		m_nBandwidth;		// Total speed
	DWORD		m_nTorrentSpeed;	// BitTorrent clamp
public:
	BOOL		m_bStable;			// Stable flag
	DWORD		m_nBestSpeed;		// Best speed
protected:
	CList< CUploadTransfer* >	m_pList;

// Operations
public:
	/**
	 * @brief Clears all uploads from the list.
	 * @param bMessage If TRUE, a message will be displayed.
	 */
	void		Clear(BOOL bMessage = TRUE);

	/**
	 * @brief Gets the number of uploads in a specific state.
	 * @param pExcept A pointer to an upload to exclude from the count.
	 * @param nState The state to count.
	 * @return The number of uploads in the specified state.
	 */
	DWORD		GetCount(CUploadTransfer* pExcept, int nState = -1) const;

	/**
	 * @brief Gets the number of BitTorrent uploads in a specific state.
	 * @param nState The state to count.
	 * @return The number of BitTorrent uploads in the specified state.
	 */
	DWORD		GetTorrentCount(int nState) const;
public:
	/**
	 * @brief Checks if more uploads are allowed to the specified address.
	 * @param pAddress A pointer to the address to check.
	 * @return TRUE if more uploads are allowed, FALSE otherwise.
	 */
	BOOL		AllowMoreTo(const IN_ADDR* pAddress) const;

	/**
	 * @brief Checks if more uploads are allowed to the specified address.
	 * @param pAddress A pointer to the address to check.
	 * @return TRUE if more uploads are allowed, FALSE otherwise.
	 */
	BOOL		AllowMoreTo(const IN6_ADDR* pAddress) const;

	/**
	 * @brief Checks if a file can be uploaded to the specified address.
	 * @param pAddress A pointer to the address to check.
	 * @param pFile A pointer to the file to check.
	 * @return TRUE if the file can be uploaded, FALSE otherwise.
	 */
	BOOL		CanUploadFileTo(const IN_ADDR* pAddress, const CShareazaFile* pFile) const;

	/**
	 * @brief Checks if a file can be uploaded to the specified address.
	 * @param pAddress A pointer to the address to check.
	 * @param pFile A pointer to the file to check.
	 * @return TRUE if the file can be uploaded, FALSE otherwise.
	 */
	BOOL		CanUploadFileTo(const IN6_ADDR* pAddress, const CShareazaFile* pFile) const;

	/**
	 * @brief Enforces the per-host upload limit.
	 * @param pUpload A pointer to the upload to check.
	 * @param bRequest If TRUE, this is a request for a new upload.
	 * @return TRUE if the upload is allowed, FALSE otherwise.
	 */
	BOOL		EnforcePerHostLimit(CUploadTransfer* pUpload, BOOL bRequest = FALSE);
public:
	/**
	 * @brief Sets the stable flag for the upload manager.
	 * @param nSpeed The current upload speed.
	 */
	void		SetStable(DWORD nSpeed);
	/**
	 * @brief Gets the current total upload bandwidth usage.
	 * @return The current total upload bandwidth usage in bytes per second.
	 */
	DWORD		GetBandwidth() const;

	/**
	 * @brief Calculates the upload limit in bytes per second.
	 * @return The upload limit in bytes per second.
	 */
	DWORD		GetBandwidthLimit() const;

	/**
	 * @brief Called periodically to update the upload list.
	 */
	void		OnRun();

	/**
	 * @brief Called when a new connection is accepted.
	 * @param pConnection A pointer to the connection.
	 * @return TRUE if the connection was handled, FALSE otherwise.
	 */
	BOOL		OnAccept(CConnection* pConnection);

	/**
	 * @brief Renames, deletes, or releases an uploading file.
	 * @param pszSource The original path of the file.
	 * @param pszTarget The new path of the file. If NULL, the file is deleted. If 1, the file is released.
	 */
	void		OnRename(LPCTSTR pszSource, LPCTSTR pszTarget);
public:
	/**
	 * @brief Adds an upload to the list.
	 * @param pUpload A pointer to the upload to add.
	 */
	void		Add(CUploadTransfer* pUpload);

	/**
	 * @brief Removes an upload from the list.
	 * @param pUpload A pointer to the upload to remove.
	 */
	void		Remove(CUploadTransfer* pUpload);

// List Access
public:
	inline POSITION GetIterator() const
	{
		return m_pList.GetHeadPosition();
	}

	inline CUploadTransfer* GetNext(POSITION& pos) const
	{
		return m_pList.GetNext( pos );
	}

	inline BOOL Check(CUploadTransfer* pUpload) const
	{
		return m_pList.Find( pUpload ) != NULL;
	}

	inline INT_PTR GetTransferCount() const
	{
		return GetCount( NULL, -2 );
	}

	inline DWORD GetTorrentTransferCount() const
	{
		return GetTorrentCount( -2 );
	}

	inline DWORD GetTorrentUploadCount() const
	{
		return GetTorrentCount( -3 );
	}

};

extern CUploads Uploads;

//
// Transfer.h
//
// Copyright (c) Shareaza Development Team, 2002-2011.
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

#include "Connection.h"

class CBuffer;


/**
 * @brief Represents an abstract file transfer.
 *
 * This class provides a base for file transfers, handling the common
 * functionality such as connecting, closing, and managing headers.
 */
class CTransfer abstract : public CConnection
{
public:
	/**
	 * @brief Constructs a new CTransfer object.
	 * @param nProtocol The protocol to use for the transfer.
	 */
	CTransfer(PROTOCOLID nProtocol = PROTOCOL_ANY);

	/**
	 * @brief Destroys the CTransfer object.
	 */
	virtual ~CTransfer();

	SOCKADDR_IN			m_pServer;			// Reference server (ED2K, DC++)
	CString				m_sRemoteNick;		// Remote user nick
	DWORD				m_nRunCookie;
	CList< CString >	m_pSourcesSent;
	CArray< CString >	m_pHeaderName;
	CArray< CString >	m_pHeaderValue;
	int					m_nState;			// Common state code
	DWORD				m_nBandwidth;		// Bandwidth allocated
	QWORD				m_nOffset;			// Fragment offset
	QWORD				m_nLength;			// Fragment length
	QWORD				m_nPosition;		// Fragment position
	DWORD				m_tRequest;			// The time a request was sent

	/**
	 * @brief Connects to the specified IPv4 address and port.
	 * @param pAddress A pointer to the IPv4 address to connect to.
	 * @param nPort The port to connect to.
	 * @return TRUE if the connection is successful, FALSE otherwise.
	 */
	virtual BOOL	ConnectTo(const IN_ADDR* pAddress, WORD nPort);

	/**
	 * @brief Connects to the specified IPv6 address and port.
	 * @param pAddress A pointer to the IPv6 address to connect to.
	 * @param nPort The port to connect to.
	 * @return TRUE if the connection is successful, FALSE otherwise.
	 */
	virtual BOOL	ConnectToIPv6(const IN6_ADDR* pAddress, WORD nPort);

	/**
	 * @brief Connects to the specified IPv4 address and port using SSL.
	 * @param pAddress A pointer to the IPv4 address to connect to.
	 * @param nPort The port to connect to.
	 * @return TRUE if the connection is successful, FALSE otherwise.
	 */
	virtual BOOL	SSLConnectTo(const IN_ADDR* pAddress, WORD nPort);

	/**
	 * @brief Connects to the specified IPv6 address and port using SSL.
	 * @param pAddress A pointer to the IPv6 address to connect to.
	 * @param nPort The port to connect to.
	 * @return TRUE if the connection is successful, FALSE otherwise.
	 */
	virtual BOOL	SSLConnectToIPv6(const IN6_ADDR* pAddress, WORD nPort);

	/**
	 * @brief Attaches this transfer to an existing connection.
	 * @param pConnection A pointer to the connection to attach to.
	 */
	virtual void	AttachTo(CConnection* pConnection);

	/**
	 * @brief Closes the transfer.
	 * @param nError The error code to associate with the closure.
	 */
	virtual void	Close(UINT nError = 0);

protected:
	void			ClearHeaders();
	virtual BOOL	OnHeaderLine(CString& strHeader, CString& strValue);
};

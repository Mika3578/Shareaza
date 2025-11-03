//
// Network.h
//
// Copyright (c) Shareaza Development Team, 2002-2017.
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

#include "ThreadImpl.h"
#include "Settings.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

class CBuffer;
class CConnection;
class CFirewall;
class CG2Packet;
class CLocalSearch;
class CNeighbour;
class CPacket;
class CQueryHit;
class CQueryKeys;
class CQuerySearch;
class CRouteCache;
class CUPnP;


/**
 * @brief Enumeration of checks to be used with CNetwork::IsFirewalled.
 */
enum
{
	CHECK_BOTH,    /**< Check both TCP and UDP. */
	CHECK_TCP,     /**< Check TCP only. */
	CHECK_UDP,     /**< Check UDP only. */
	CHECK_IP,      /**< Check IP only. */
	CHECK_TCP6,    /**< Check TCP6 only. */
	CHECK_UDP6,    /**< Check UDP6 only. */
	CHECK_IP6      /**< Check IP6 only. */
};

/**
 * @brief Enumeration of commands for the asynchronous resolver.
 */
enum
{
	RESOLVE_ONLY,               /**< Resolve and update host cache. */
	RESOLVE_CONNECT_ULTRAPEER,  /**< Resolve, update host cache and connect as ultrapeer. */
	RESOLVE_CONNECT,            /**< Resolve, update host cache and connect. */
	RESOLVE_DISCOVERY           /**< Resolve and update discovery services. */
};

/**
 * @brief Manages the network connectivity and communication for the application.
 *
 * This class is responsible for initializing and shutting down the network,
 * managing connections, resolving hostnames, and handling network events.
 */
class CNetwork : public CThreadImpl
{
public:
	CNetwork();
	~CNetwork();

// Attributes
public:
	CAutoPtr< CRouteCache >	NodeRoute;
	CAutoPtr< CRouteCache >	QueryRoute;
	CAutoPtr< CQueryKeys >	QueryKeys;
	CAutoPtr< CUPnP >		UPnPFinder;			// UPnP
	CAutoPtr< CFirewall >	Firewall;			// Windows Firewall

	CMutexEx		m_pSection;
	SOCKADDR_IN		m_pHost;					// Structure (Windows Sockets) which holds address of the local machine
	SOCKADDR_IN6	m_pHostIPv6;				// Structure (Windows Sockets) which holds address of the local machine
	
	BOOL			m_bAutoConnect;
	volatile bool	m_bConnected;				// Network has finished initializing and is connected
	DWORD			m_tStartedConnecting;		// The time Shareaza started trying to connect
	TRISTATE		m_bUPnPPortsForwarded;		// UPnP values are assigned when the discovery is complete

protected:
	CStringA		m_sHostName;
	mutable CCriticalSection	m_pHASection;
	CList< ULONG >	m_pHostAddresses;

	struct ipv6_compare {
		inline bool operator()(const IN6_ADDR& _Left, const IN6_ADDR& _Right) const throw()
		{
			return ( memcmp( &_Left, &_Right, sizeof( IN6_ADDR ) ) > 0 );
		}
	};

	typedef std::set< IN6_ADDR, ipv6_compare > ipv6_set;
	ipv6_set m_pHostAddressesIPv6;

	DWORD			m_nUPnPTier;				// UPnP tier number (0..UPNP_MAX)
	DWORD			m_tUPnPMap;					// Time of last UPnP port mapping
	BOOL			m_bHomeNetworkNAT;				// Home network NAT> LAN NAT> Internet

	typedef struct
	{
		CString		m_sAddress;
		PROTOCOLID	m_nProtocol;
		WORD		m_nPort;
		BYTE		m_nCommand;
		union
		{
			char	m_pBuffer[ MAXGETHOSTSTRUCT ];
			HOSTENT	m_pHost;
		};
	} ResolveStruct;

	typedef CMap< HANDLE, HANDLE, ResolveStruct*, ResolveStruct* > CResolveMap;

	CResolveMap					m_pLookups;
	mutable CCriticalSection	m_pLookupsSection;

	class CJob
	{
	public:
		enum JobType { Null, Hit, Search };

		CJob(JobType nType = Null, void* pData = NULL, int nStage = 0)
			: m_nType( nType )
			, m_pData( pData )
			, m_nStage( nStage )
		{
		}

		CJob(const CJob& oJob)
			: m_nType( oJob.m_nType )
			, m_pData( oJob.m_pData )
			, m_nStage( oJob.m_nStage )
		{
		}

		CJob& operator=(const CJob& oJob)
		{
			m_nType = oJob.m_nType;
			m_pData = oJob.m_pData;
			m_nStage = oJob.m_nStage;
			return *this;
		}

		void Next()
		{
			++ m_nStage;
		}

		JobType GetType() const
		{
			return m_nType;
		}

		void* GetData() const
		{
			return m_pData;
		}

		int GetStage() const
		{
			return m_nStage;
		}

	protected:
		JobType	m_nType;
		void*	m_pData;
		int		m_nStage;
	};
	CCriticalSection	m_pJobSection;	// m_oJobs synchronization
	CList< CJob >		m_oJobs;

	// Process asynchronous jobs (hits, searches, etc.)
	void		RunJobs();
	void		ClearJobs();

	// Handle and destroy query searches
	bool		ProcessQuerySearch(CNetwork::CJob& oJob);

	// Handle and destroy query hits
	bool		ProcessQueryHits(CNetwork::CJob& oJob);

	// Get asynchronously resolved host
	ResolveStruct* GetResolve(HANDLE hAsync);

	// Clear asynchronous resolver queue
	void		ClearResolve();

	// Restore WinINet connection to Internet
	bool		InternetConnect();

	bool		PreRun();
	void		OnRun();
	void		PostRun();

// Operations
public:
	/**
	 * @brief Initialize network: Windows Sockets, Windows Firewall, UPnP NAT.
	 * @return TRUE if initialization is successful, FALSE otherwise.
	 */
	BOOL		Init();

	/**
	 * @brief Shutdown network.
	 */
	void		Clear();

	/**
	 * @brief Checks if the given IPv4 address is the local machine's address.
	 * @param nAddress The IPv4 address to check.
	 * @return TRUE if the address is the local machine's address, FALSE otherwise.
	 */
	BOOL		IsSelfIP(const IN_ADDR& nAddress) const;

	/**
	 * @brief Checks if the given IPv6 address is the local machine's address.
	 * @param nAddress The IPv6 address to check.
	 * @return TRUE if the address is the local machine's address, FALSE otherwise.
	 */
	BOOL		IsSelfIP(const IN6_ADDR& nAddress) const;

	/**
	 * @brief Checks if the network is available.
	 * @return true if the network is available, false otherwise.
	 */
	bool		IsAvailable() const;

	/**
	 * @brief Checks if the network is connected.
	 * @return true if the network is connected, false otherwise.
	 */
	bool		IsConnected() const;

	/**
	 * @brief Checks if the network is listening for incoming connections.
	 * @return true if the network is listening, false otherwise.
	 */
	bool		IsListening() const;

	/**
	 * @brief Checks if the network is listening for incoming IPv6 connections.
	 * @return true if the network is listening, false otherwise.
	 */
	bool		IsListeningIPv6() const;

	/**
	 * @brief Checks if the network is well connected.
	 * @return true if the network is well connected, false otherwise.
	 */
	bool		IsWellConnected() const;

	/**
	 * @brief Checks if the network is stable.
	 * @return true if the network is stable, false otherwise.
	 */
	bool		IsStable() const;

	/**
	 * @brief Checks if the IPv6 network is stable.
	 * @return true if the IPv6 network is stable, false otherwise.
	 */
	bool		IsStableIPv6() const;

	/**
	 * @brief Checks if the network is firewalled.
	 * @param nCheck The type of check to perform (e.g., CHECK_TCP, CHECK_UDP).
	 * @return TRUE if the network is firewalled, FALSE otherwise.
	 */
	BOOL		IsFirewalled(int nCheck = CHECK_UDP) const;

	/**
	 * @brief Gets the time when the network became stable.
	 * @return The time when the network became stable, in milliseconds.
	 */
	DWORD		GetStableTime() const;

	/**
	 * @brief Checks if there is a connection to the given IPv4 address.
	 * @param pAddress The IPv4 address to check.
	 * @return TRUE if there is a connection, FALSE otherwise.
	 */
	BOOL		IsConnectedTo(const IN_ADDR* pAddress) const;

	/**
	 * @brief Checks if there is a connection to the given IPv6 address.
	 * @param pAddress The IPv6 address to check.
	 * @return TRUE if there is a connection, FALSE otherwise.
	 */
	BOOL		IsConnectedTo(const IN6_ADDR* pAddress) const;

	/**
	 * @brief Checks if the network is ready to start downloading.
	 * @param tNow The current time, in milliseconds.
	 * @return TRUE if the network is ready to start downloading, FALSE otherwise.
	 */
	BOOL		ReadyToTransfer(DWORD tNow) const;

	/**
	 * @brief Connects to the network.
	 * @param bAutoConnect If TRUE, the connection will be established automatically.
	 * @return TRUE if the connection is successful, FALSE otherwise.
	 */
	BOOL		Connect(BOOL bAutoConnect = FALSE);

	/**
	 * @brief Disconnects from the network.
	 */
	void		Disconnect();

	/**
	 * @brief Connects to a specific address.
	 * @param pszAddress The address to connect to.
	 * @param nPort The port to connect to.
	 * @param nProtocol The protocol to use.
	 * @param bNoUltraPeer If TRUE, the connection will not be established to an ultrapeer.
	 * @return TRUE if the connection is successful, FALSE otherwise.
	 */
	BOOL		ConnectTo(LPCTSTR pszAddress, int nPort = 0, PROTOCOLID nProtocol = PROTOCOL_NULL, BOOL bNoUltraPeer = FALSE);

	/**
	 * @brief Acquires the local address.
	 * @param hSocket The socket to use.
	 * @param bPort If TRUE, the port will be acquired.
	 * @param pFromAddress The address to acquire from.
	 * @param pFromIPv6Address The IPv6 address to acquire from.
	 * @return TRUE if the address is acquired successfully, FALSE otherwise.
	 */
	BOOL		AcquireLocalAddress(SOCKET hSocket, bool bPort = false, const IN_ADDR* pFromAddress = NULL, const IN6_ADDR* pFromIPv6Address = NULL);

	/**
	 * @brief Acquires the local address.
	 * @param pszHeader The header to use.
	 * @param nPort The port to use.
	 * @param pFromAddress The address to acquire from.
	 * @param pFromIPv6Address The IPv6 address to acquire from.
	 * @return TRUE if the address is acquired successfully, FALSE otherwise.
	 */
	BOOL		AcquireLocalAddress(LPCTSTR pszHeader, WORD nPort = 0, const IN_ADDR* pFromAddress = NULL, const IN6_ADDR* pFromIPv6Address = NULL);

	/**
	 * @brief Acquires the local address.
	 * @param pAddress The address to acquire.
	 * @param nPort The port to use.
	 * @param pFromAddress The address to acquire from.
	 * @return TRUE if the address is acquired successfully, FALSE otherwise.
	 */
	BOOL		AcquireLocalAddress(const IN_ADDR& pAddress, WORD nPort = 0, const IN_ADDR* pFromAddress = NULL);

	/**
	 * @brief Acquires the local address.
	 * @param pAddress The IPv6 address to acquire.
	 * @param nPort The port to use.
	 * @param pFromAddress The address to acquire from.
	 * @return TRUE if the address is acquired successfully, FALSE otherwise.
	 */
	BOOL		AcquireLocalAddress(const IN6_ADDR& pAddress, WORD nPort = 0, const IN6_ADDR* pFromAddress = NULL);

	/**
	 * @brief Resolves a hostname to an IP address.
	 * @param pszHost The hostname to resolve.
	 * @param nPort The port to use.
	 * @param pHost A pointer to a SOCKADDR_IN structure that receives the resolved address.
	 * @param bNames If TRUE, the name will be resolved.
	 * @return TRUE if the hostname is resolved successfully, FALSE otherwise.
	 */
	static BOOL	Resolve(LPCTSTR pszHost, int nPort, SOCKADDR_IN* pHost, BOOL bNames = TRUE);

	/**
	 * @brief Asynchronously resolves a hostname to an IP address.
	 * @param pszAddress The hostname to resolve.
	 * @param nPort The port to use.
	 * @param nProtocol The protocol to use.
	 * @param nCommand The command to execute after the hostname is resolved.
	 * @return TRUE if the asynchronous resolution is started successfully, FALSE otherwise.
	 */
	BOOL		AsyncResolve(LPCTSTR pszAddress, WORD nPort, PROTOCOLID nProtocol, BYTE nCommand);

	/**
	 * @brief Gets the number of pending network name resolves.
	 * @return The number of pending network name resolves.
	 */
	UINT		GetResolveCount() const;

	/**
	 * @brief Checks if the given IPv4 address is reserved.
	 * @param pAddress The IPv4 address to check.
	 * @return TRUE if the address is reserved, FALSE otherwise.
	 */
	BOOL		IsReserved(const IN_ADDR* pAddress) const;

	/**
	 * @brief Checks if the given IPv6 address is reserved.
	 * @param pAddress The IPv6 address to check.
	 * @return TRUE if the address is reserved, FALSE otherwise.
	 */
	BOOL		IsReserved(const IN6_ADDR* pAddress) const;

	/**
	 * @brief Gets a random port number.
	 * @return A random port number.
	 */
	WORD		RandomPort() const;

	/**
	 * @brief Creates a new GUID.
	 * @param oID A reference to a Hashes::Guid object that receives the new GUID.
	 */
	void		CreateID(Hashes::Guid& oID);

	/**
	 * @brief Checks if the given IPv4 address is firewalled.
	 * @param pAddress The IPv4 address to check.
	 * @param bIncludeSelf If TRUE, the local machine's address will be included in the check.
	 * @param bIgnoreLocalIP If TRUE, local IP addresses will be ignored.
	 * @return TRUE if the address is firewalled, FALSE otherwise.
	 */
	BOOL		IsFirewalledAddress(const IN_ADDR* pAddress, BOOL bIncludeSelf = FALSE, BOOL bIgnoreLocalIP = Settings.Connection.IgnoreLocalIP) const;

	/**
	 * @brief Checks if the given IPv6 address is firewalled.
	 * @param pAddress The IPv6 address to check.
	 * @param bIncludeSelf If TRUE, the local machine's address will be included in the check.
	 * @param bIgnoreLocalIP If TRUE, local IP addresses will be ignored.
	 * @return TRUE if the address is firewalled, FALSE otherwise.
	 */
	BOOL		IsFirewalledAddress(const IN6_ADDR* pAddress, BOOL bIncludeSelf = FALSE, BOOL bIgnoreLocalIP = Settings.Connection.IgnoreLocalIP) const;

	/**
	 * @brief Checks if the given IPv4 address is valid for the specified address.
	 * @param pForAddress The address for which the address is being checked.
	 * @param pAddress The address to check.
	 * @return TRUE if the address is valid, FALSE otherwise.
	 */
	BOOL		IsValidAddressFor(const IN_ADDR* pForAddress, const IN_ADDR* pAddress) const;

	/**
	 * @brief Checks if the given IPv6 address is valid for the specified address.
	 * @param pForAddress The address for which the address is being checked.
	 * @param pAddress The address to check.
	 * @return TRUE if the address is valid, FALSE otherwise.
	 */
	BOOL		IsValidAddressFor(const IN6_ADDR* pForAddress, const IN6_ADDR* pAddress) const;

	/**
	 * @brief Checks if the given IPv4 address is on the home network.
	 * @param pAddress The IPv4 address to check.
	 * @return TRUE if the address is on the home network, FALSE otherwise.
	 */
	BOOL		IsHomeNetwork(const IN_ADDR* pAddress) const;

	/**
	 * @brief Checks if the given IPv4 address is on the local area network.
	 * @param pAddress The IPv4 address to check.
	 * @return TRUE if the address is on the local area network, FALSE otherwise.
	 */
	BOOL		IsLocalAreaNetwork(const IN_ADDR* pAddress) const;

	/**
	 * @brief Gets the network level of the given IPv4 address.
	 * @param pAddress The IPv4 address to check.
	 * @return The network level of the address.
	 */
	int			GetNetworkLevel(const IN_ADDR* pAddress) const;

	/**
	 * @brief Gets the network level of the given IPv6 address.
	 * @param pAddress The IPv6 address to check.
	 * @return The network level of the address.
	 */
	int			GetNetworkLevel(const IN6_ADDR* pAddress) const;

	/**
	 * @brief Gets the local machine's IPv4 address for the specified address.
	 * @param pAddress The address for which to get the local machine's address.
	 * @return The local machine's IPv4 address.
	 */
	IN_ADDR		GetMyAddressFor(const IN_ADDR* pAddress) const;

	/**
	 * @brief Gets the local machine's IPv6 address for the specified address.
	 * @param pAddress The address for which to get the local machine's address.
	 * @return The local machine's IPv6 address.
	 */
	IN6_ADDR	GetMyAddressFor(const IN6_ADDR* pAddress) const;

	/**
	 * @brief Gets the port number used by the network.
	 * @return The port number.
	 */
	WORD		GetPort() const;

	/**
	 * @brief Gets the node route for a given GUID.
	 * @param oGUID The GUID to get the route for.
	 * @param ppNeighbour A pointer to a CNeighbour pointer that will receive the neighbour.
	 * @param pEndpoint A pointer to a SOCKADDR_IN structure that will receive the endpoint.
	 * @return TRUE if the route was found, FALSE otherwise.
	 */
	BOOL		GetNodeRoute(const Hashes::Guid& oGUID, CNeighbour** ppNeighbour, SOCKADDR_IN* pEndpoint);

	/**
	 * @brief Routes a G2 packet.
	 * @param pPacket The packet to route.
	 * @return TRUE if the packet was routed successfully, FALSE otherwise.
	 */
	BOOL		RoutePacket(CG2Packet* pPacket);

	/**
	 * @brief Sends a push message.
	 * @param oGUID The GUID of the file to push.
	 * @param nIndex The index of the file to push.
	 * @return TRUE if the push was sent successfully, FALSE otherwise.
	 */
	BOOL		SendPush(const Hashes::Guid& oGUID, DWORD nIndex = 0);

	/**
	 * @brief Routes query hits.
	 * @param pHits The query hits to route.
	 * @param pPacket The packet to route the hits in.
	 * @return TRUE if the hits were routed successfully, FALSE otherwise.
	 */
	BOOL		RouteHits(CQueryHit* pHits, CPacket* pPacket);

	/**
	 * @brief Called when a Winsock message is received.
	 * @param wParam The message-specific parameter.
	 * @param lParam The message-specific parameter.
	 */
	void		OnWinsock(WPARAM wParam, LPARAM lParam);

	/**
	 * @brief Handle push for downloads, chats and browsers.
	 * @param oGUID The GUID of the push.
	 * @param pConnection The connection that received the push.
	 * @return TRUE if the push is handled successfully, FALSE otherwise.
	 */
	BOOL		OnPush(const Hashes::Guid& oGUID, CConnection* pConnection);

	/**
	 * @brief Add query search to queue.
	 * @param pSearch The query search to add.
	 */
	void		OnQuerySearch(CLocalSearch* pSearch);

	/**
	 * @brief Add query hit to queue.
	 * @param pHits The query hit to add.
	 */
	void		OnQueryHits(CQueryHit* pHits);

	/**
	 * @brief Safe way to accept socket.
	 * @param hSocket The socket to accept.
	 * @param addr A pointer to a SOCKADDR_IN structure that receives the address of the connecting entity.
	 * @param lpfnCondition A pointer to an application-supplied function that is used to determine if a connection should be accepted.
	 * @param dwCallbackData Application-supplied data to be passed to the condition function.
	 * @return A new socket descriptor if the connection is accepted, otherwise INVALID_SOCKET.
	 */
	static SOCKET AcceptSocket(SOCKET hSocket, SOCKADDR_IN* addr, LPCONDITIONPROC lpfnCondition, DWORD_PTR dwCallbackData = 0);

	/**
	 * @brief Safe way to accept socket.
	 * @param hSocket The socket to accept.
	 * @param addr A pointer to a SOCKADDR_IN6 structure that receives the address of the connecting entity.
	 * @param lpfnCondition A pointer to an application-supplied function that is used to determine if a connection should be accepted.
	 * @param dwCallbackData Application-supplied data to be passed to the condition function.
	 * @return A new socket descriptor if the connection is accepted, otherwise INVALID_SOCKET.
	 */
	static SOCKET AcceptSocket(SOCKET hSocket, SOCKADDR_IN6* addr, LPCONDITIONPROC lpfnCondition, DWORD_PTR dwCallbackData = 0);

	/**
	 * @brief Safe way to close socket.
	 * @param hSocket The socket to close.
	 * @param bForce If TRUE, the socket will be closed forcefully.
	 */
	static void	CloseSocket(SOCKET& hSocket, const bool bForce);

	/**
	 * @brief Safe way to send TCP data.
	 * @param s The socket to send data on.
	 * @param buf The buffer containing the data to send.
	 * @param len The length of the data to send.
	 * @return The number of bytes sent, or SOCKET_ERROR if an error occurred.
	 */
	static int Send(SOCKET s, const char* buf, int len);

	/**
	 * @brief Safe way to send TCP data over SSL.
	 * @param s The SSL object to send data on.
	 * @param buf The buffer containing the data to send.
	 * @param len The length of the data to send.
	 * @return The number of bytes sent, or a value less than or equal to 0 if an error occurred.
	 */
	static int SSLSend(SSL* s, const char* buf, int len);

	/**
	 * @brief Safe way to send UDP data.
	 * @param s The socket to send data on.
	 * @param buf The buffer containing the data to send.
	 * @param len The length of the data to send.
	 * @param pTo A pointer to a SOCKADDR_IN structure that contains the address of the destination.
	 * @return The number of bytes sent, or SOCKET_ERROR if an error occurred.
	 */
	static int SendTo(SOCKET s, const char* buf, int len, const SOCKADDR_IN* pTo);

	/**
	 * @brief Safe way to send UDP data.
	 * @param s The socket to send data on.
	 * @param buf The buffer containing the data to send.
	 * @param len The length of the data to send.
	 * @param pTo A pointer to a SOCKADDR_IN6 structure that contains the address of the destination.
	 * @return The number of bytes sent, or SOCKET_ERROR if an error occurred.
	 */
	static int SendTo(SOCKET s, const char* buf, int len, const SOCKADDR_IN6* pTo);

	/**
	 * @brief Safe way to receive TCP data.
	 * @param s The socket to receive data from.
	 * @param buf The buffer to receive the data.
	 * @param len The length of the buffer.
	 * @return The number of bytes received, or SOCKET_ERROR if an error occurred.
	 */
	static int Recv(SOCKET s, char* buf, int len);

	/**
	 * @brief Safe way to receive TCP data over SSL.
	 * @param s The SSL object to receive data from.
	 * @param buf The buffer to receive the data.
	 * @param len The length of the buffer.
	 * @return The number of bytes received, or a value less than or equal to 0 if an error occurred.
	 */
	static int SSLRecv(SSL* s, char* buf, int len);

	/**
	 * @brief Safe way to receive UDP data.
	 * @param s The socket to receive data from.
	 * @param buf The buffer to receive the data.
	 * @param len The length of the buffer.
	 * @param pFrom A pointer to a SOCKADDR_IN structure that receives the address of the sender.
	 * @return The number of bytes received, or SOCKET_ERROR if an error occurred.
	 */
	static int RecvFrom(SOCKET s, char* buf, int len, SOCKADDR_IN* pFrom);

	/**
	 * @brief Safe way to receive UDP data.
	 * @param s The socket to receive data from.
	 * @param buf The buffer to receive the data.
	 * @param len The length of the buffer.
	 * @param pFrom A pointer to a SOCKADDR_IN6 structure that receives the address of the sender.
	 * @return The number of bytes received, or SOCKET_ERROR if an error occurred.
	 */
	static int RecvFrom(SOCKET s, char* buf, int len, SOCKADDR_IN6* pFrom);

	/**
	 * @brief Safe way to call InternetOpen.
	 * @return A handle to the Internet session, or NULL if an error occurred.
	 */
	static HINTERNET InternetOpen();

	/**
	 * @brief Safe way to call InternetOpenUrl.
	 * @param hInternet The handle to the Internet session.
	 * @param lpszUrl The URL to open.
	 * @param lpszHeaders A pointer to a string that contains the headers to be sent to the server.
	 * @param dwHeadersLength The length of the headers.
	 * @param dwFlags The flags to use.
	 * @return A handle to the URL, or NULL if an error occurred.
	 */
	static HINTERNET InternetOpenUrl(HINTERNET hInternet, LPCWSTR lpszUrl, LPCWSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags);

	/**
	 * @brief Safe way to call WSACleanup.
	 */
	static void Cleanup();

	/**
	 * @brief Create TCP and UDP port mappings.
	 */
	void MapPorts();

	/**
	 * @brief Remove TCP and UDP port mappings.
	 */
	void DeletePorts();

	/**
	 * @brief UPnP success (called by UPnP-services).
	 */
	void OnMapSuccess();

	/**
	 * @brief UPnP error (called by UPnP-services).
	 */
	void OnMapFailed();

	friend class CHandshakes;
	friend class CNeighbours;
};

extern CNetwork Network;
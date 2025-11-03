//
// Application.h
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


/**
 * @brief Represents the main Shareaza application object.
 *
 * This class provides access to the core components of the Shareaza application,
 * such as the user interface, settings, and library. It also exposes methods
 * for interacting with the application, such as creating new windows and menus.
 */
class CApplication : public CComObject
{
	DECLARE_DYNAMIC(CApplication)

public:
	/**
	 * @brief Constructs a new CApplication object.
	 */
	CApplication();

	/**
	 * @brief Destroys the CApplication object.
	 */
	virtual ~CApplication();

	/**
	 * @brief Gets a pointer to the main application object.
	 * @param ppIApplication A pointer to a variable that receives the IApplication interface pointer.
	 * @return HRESULT S_OK if successful, or an error code otherwise.
	 */
	static HRESULT GetApp(IApplication** ppIApplication) throw();

	/**
	 * @brief Gets a pointer to the user interface object.
	 * @param ppIUserInterface A pointer to a variable that receives the IUserInterface interface pointer.
	 * @return HRESULT S_OK if successful, or an error code otherwise.
	 */
	static HRESULT GetUI(IUserInterface** ppIUserInterface) throw();

	/**
	 * @brief Gets a pointer to the settings object.
	 * @param ppISettings A pointer to a variable that receives the ISettings interface pointer.
	 * @return HRESULT S_OK if successful, or an error code otherwise.
	 */
	static HRESULT GetSettings(ISettings** ppISettings) throw();

// IApplication
protected:
	BEGIN_INTERFACE_PART(Application, IApplication)
		DECLARE_DISPATCH()
		/**
		 * @brief Gets a pointer to the main application object.
		 * @param ppApplication A pointer to a variable that receives the IApplication interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);

		/**
		 * @brief Gets the version of the application.
		 * @param psVersion A pointer to a BSTR that receives the version string.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_Version)(BSTR FAR* psVersion);

		/**
		 * @brief Checks if the given version string is compatible with the current application version.
		 * @param sVersion The version string to check.
		 * @return HRESULT S_OK if the version is compatible, or an error code otherwise.
		 */
		STDMETHOD(CheckVersion)(BSTR sVersion);

		/**
		 * @brief Creates a new XML element.
		 * @param ppXML A pointer to a variable that receives the ISXMLElement interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(CreateXML)(ISXMLElement FAR* FAR* ppXML);

		/**
		 * @brief Gets a pointer to the user interface object.
		 * @param ppUserInterface A pointer to a variable that receives the IUserInterface interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_UserInterface)(IUserInterface FAR* FAR* ppUserInterface);

		/**
		 * @brief Gets a pointer to the library object.
		 * @param ppLibrary A pointer to a variable that receives the ILibrary interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);

		/**
		 * @brief Gets a pointer to the settings object.
		 * @param ppSettings A pointer to a variable that receives the ISettings interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_Settings)(ISettings FAR* FAR* ppSettings);

		/**
		 * @brief Gets a pointer to the image service object.
		 * @param ppImageService A pointer to a variable that receives the IImageServicePlugin interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_ImageService)(IImageServicePlugin FAR* FAR* ppImageService);

		/**
		 * @brief Gets the smart agent string.
		 * @param psSmartAgent A pointer to a BSTR that receives the smart agent string.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_SmartAgent)(BSTR FAR* psSmartAgent);

		/**
		 * @brief Displays a message to the user.
		 * @param nType The type of the message.
		 * @param bsMessage The message to display.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(Message)(WORD nType, BSTR bsMessage);
	END_INTERFACE_PART(Application)

	BEGIN_INTERFACE_PART(UserInterface, IUserInterface)
		DECLARE_DISPATCH()
		/**
		 * @brief Gets a pointer to the main application object.
		 * @param ppApplication A pointer to a variable that receives the IApplication interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);

		/**
		 * @brief Gets a pointer to the user interface object.
		 * @param ppUserInterface A pointer to a variable that receives the IUserInterface interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_UserInterface)(IUserInterface FAR* FAR* ppUserInterface);

		/**
		 * @brief Creates a new window.
		 * @param bsName The name of the window.
		 * @param pOwner A pointer to the owner of the window.
		 * @param ppWindow A pointer to a variable that receives the IPluginWindow interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(NewWindow)(BSTR bsName, IPluginWindowOwner FAR* pOwner, IPluginWindow FAR* FAR* ppWindow);

		/**
		 * @brief Gets the handle of the main window.
		 * @param phWnd A pointer to a variable that receives the handle of the main window.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_MainWindowHwnd)(HWND FAR* phWnd);

		/**
		 * @brief Gets a pointer to the active view.
		 * @param ppView A pointer to a variable that receives the IGenericView interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(get_ActiveView)(IGenericView FAR* FAR* ppView);

		/**
		 * @brief Registers a new command.
		 * @param bsName The name of the command.
		 * @param hIcon The icon of the command.
		 * @param pnCommandID A pointer to a variable that receives the ID of the command.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(RegisterCommand)(BSTR bsName, HICON hIcon, UINT* pnCommandID);

		/**
		 * @brief Adds a menu or toolbar from an XML string.
		 * @param sXML The XML string to add.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(AddFromString)(BSTR sXML);

		/**
		 * @brief Adds a menu or toolbar from a resource.
		 * @param hInstance The handle to the instance of the module whose executable file contains the resource.
		 * @param nID The identifier of the resource.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(AddFromResource)(HINSTANCE hInstance, UINT nID);

		/**
		 * @brief Adds a menu or toolbar from an XML element.
		 * @param pXML The XML element to add.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(AddFromXML)(ISXMLElement FAR* pXML);

		/**
		 * @brief Gets a pointer to a menu.
		 * @param bsName The name of the menu.
		 * @param bCreate A boolean value that indicates whether to create the menu if it does not exist.
		 * @param ppMenu A pointer to a variable that receives the ISMenu interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(GetMenu)(BSTR bsName, VARIANT_BOOL bCreate, ISMenu FAR* FAR* ppMenu);

		/**
		 * @brief Gets a pointer to a toolbar.
		 * @param bsName The name of the toolbar.
		 * @param bCreate A boolean value that indicates whether to create the toolbar if it does not exist.
		 * @param ppToolbar A pointer to a variable that receives the ISToolbar interface pointer.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(GetToolbar)(BSTR bsName, VARIANT_BOOL bCreate, ISToolbar FAR* FAR* ppToolbar);

		/**
		 * @brief Gets the ID of a command.
		 * @param bsName The name of the command.
		 * @param pnCommandID A pointer to a variable that receives the ID of the command.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(NameToID)(BSTR bsName, UINT* pnCommandID);

		/**
		 * @brief Adds a string to the string table.
		 * @param nStringID The ID of the string.
		 * @param sText The text of the string.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(AddString)(UINT nStringID, BSTR sText);

		/**
		 * @brief Loads a string from the string table.
		 * @param nStringID The ID of the string.
		 * @param psText A pointer to a BSTR that receives the text of the string.
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(LoadString)(UINT nStringID, BSTR* psText);
	END_INTERFACE_PART(UserInterface)

	BEGIN_INTERFACE_PART(Settings, ISettings)
		DECLARE_DISPATCH()
		/**
		 * @brief Gets a setting value.
		 * @param value A pointer to a VARIANT that receives the value of the setting. The `bstrVal` member of the VARIANT should contain the path to the setting (e.g. "Gnutella2.EnableAlways").
		 * @return HRESULT S_OK if successful, or an error code otherwise.
		 */
		STDMETHOD(GetValue)(VARIANT* value);	// Pass as BSTR path (e.g. Gnutella2.EnableAlways, get back the actual value
	END_INTERFACE_PART(Settings)

	DECLARE_OLECREATE(CApplication)
	DECLARE_INTERFACE_MAP()
};

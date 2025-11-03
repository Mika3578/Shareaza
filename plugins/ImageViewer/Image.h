//
// Image.h
//
// This software is released into the public domain. You are free to
// redistribute and modify without any restrictions.
// This file is part of SHAREAZA (shareaza.sourceforge.net), original author Michael Stokes. 
//

#pragma once


/**
 * @brief Represents an image.
 *
 * This class provides methods for loading, clearing, and manipulating images.
 */
class CImage  
{
public:
	CImage();
	virtual ~CImage();
	
	BYTE*	m_pImage;		// Pointer to image data
	int		m_nWidth;		// Width
	int		m_nHeight;		// Height
	int		m_nComponents;	// Components (1=mono, 3=RGB, 4=RGBA)
	BOOL	m_bPartial;		// Is it partially loaded?
	
	/**
	 * @brief Loads an image from a file.
	 * @param pszFile The path to the image file.
	 * @return TRUE if the image was loaded successfully, FALSE otherwise.
	 */
	BOOL	Load(LPCTSTR pszFile);

	/**
	 * @brief Clears the image.
	 */
	void	Clear();

	/**
	 * @brief Ensures that the image is in RGB format.
	 * @param crFill The color to fill the background with if the image has an alpha channel.
	 * @return TRUE if the image is in RGB format, FALSE otherwise.
	 */
	BOOL	EnsureRGB(COLORREF crFill = 0xFFFFFFFF);

	/**
	 * @brief Converts a monochrome image to RGB.
	 * @return TRUE if the conversion was successful, FALSE otherwise.
	 */
	BOOL	MonoToRGB();

	/**
	 * @brief Converts an image with an alpha channel to RGB.
	 * @param crFill The color to use for the background.
	 * @return TRUE if the conversion was successful, FALSE otherwise.
	 */
	BOOL	AlphaToRGB(COLORREF crFill);

	/**
	 * @brief Resamples the image to a new width and height.
	 * @param nWidth The new width.
	 * @param nHeight The new height.
	 * @return A handle to the resampled bitmap.
	 */
	HBITMAP	Resample(int nWidth, int nHeight);
	
// Internal Helpers
protected:
	IImageServicePlugin*	LoadService(LPCTSTR pszFile);
};

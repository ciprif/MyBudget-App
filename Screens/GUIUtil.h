/* Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/**
 * \file GUIUtil.h
 * \author Ciprian Filipas
 * \date Sep 19, 2012
 */

#ifndef GUIUTIL_H_
#define GUIUTIL_H_

#include "../Model/ModelUtil.h"

namespace GUI
{
	// Options box related variables
	const MAUtil::WString HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[] = { L"Add income", L"Add expense" };
	const int HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH = 2;

	const MAUtil::WString LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[] = { L"Add income", L"Add expense",
																	   L"Sort by date", L"Sort by type",
																	   L"Sort by amount" };
	const int LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH = 5;

	const MAUtil::WString SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[] = { L"Save", L"Restore" };
	const int SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH = 2;

	// UI related constants
	const int GRAY = 0xC0C0C0;
	const int GREEN = 0x00FF00;
	const int YELLOW = 0xFFFF00;
	const int RED = 0xFF0000;
	const int DIALOG_FONT_SIZE_SCREEN_LARGE = 25;
	const int DIALOG_FONT_SIZE_SMALL_SCREEN_LARGE = 18;
	const int DIALOG_FONT_SIZE_SCREEN_MEDIUM = 18;
	const int DIALOG_FONT_SIZE_SMALL_SCREEN_MEDIUM = 13;
	const int DIALOG_FONT_SIZE_SCREEN_SMALL = 13;
	const int DIALOG_FONT_SIZE_SMALL_SCREEN_SMALL = 10;
	const int DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE = 200;
	const int DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_MEDIUM = 150;
	const int DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_SMALL = 100;
	const int DESCRIPTION_EDIT_BOX_LINES = 10;
	const int DIALOG_HEIGHT = 750;
	const int DIALOG_BUTTON_WIDTH_SCREEN_LARGE = 140;
	const int DIALOG_BUTTON_WIDTH_SCREEN_MEDIUM = 100;
	const int DIALOG_BUTTON_WIDTH_SCREEN_SMALL = 80;
	const int DIALOG_BOTTOM_LINE_HEIGHT = 20;
	const int DIALOG_COLOR_WHITE = 0xFFFFFF;
	const int MAX_VALUE = 9999;
	const char DEFAULT_COIN[] = "EUR";
	const int IMAGE_BUTTON_WIDTH_LARGE = 100;
	const int IMAGE_BUTTON_WIDTH_MEDIUM = 70;
	const int IMAGE_BUTTON_WIDTH_SMALL = 40;
	const int IMAGE_BUTTON_HEIGHT_LARGE = 100;
	const int IMAGE_BUTTON_HEIGHT_MEDIUM = 70;
	const int IMAGE_BUTTON_HEIGHT_SMALL = 40;

	const int SMALL_SCREEN = 0;
	const int MEDIUM_SCREEN = 0;
	const int LARGE_SCREEN = 0;

	// Pointer to the active screen
	static MAUtil::CustomEventListener* _activeScreen;

	// Static variables set by calling the SetSizeRelatedVariables() function; This are used for scaling the UI in order to make it
	// work and look properly on as many screen sizes as posible
	static int _dialogFontSize, _dialogSmallFontSize, _descriptionBoxHeight, _dialogButtonWidth, _imageButtonWidth, _imageButtonHeight;
	static int _screenType;

	//Platform variables; only one can be true during the application lifetime
	static bool _WindowsPhone7, _IPhoneOS, _Android;

	/**
	 * \brief Static function used for determining the screen size
	 * @param screenHeight const int& the screen physical height
	 * @param screenWidth const int& the screen physical width
	 */
	static void DetermineScreenSize(const int& screenHeight, const int& screenWidth)
	{
		if(480 > screenHeight && 320 > screenWidth) _screenType = 0;
		else if(480 < screenHeight && 640 > screenHeight && 320 < screenWidth && 480 > screenWidth) _screenType = 1;
		else _screenType = 2;
	}

	/**
	 * \brief This function sets the _WindowsPhone7, _IPhoneOS and _Android bool values
	 */
	static void DeterminePlatform()
	{
		_Android = false;
		_WindowsPhone7 = false;
		_IPhoneOS = false;

		char buffer[Model::BUFF_SIZE];
		maGetSystemProperty("mosync.device.OS", buffer, Model::BUFF_SIZE);

		if(strcmp(buffer, "iPhone OS") == 0)
		{
			_IPhoneOS = true;
		}
		else if(strcmp(buffer, "Android") == 0)
		{
			_Android = true;
		}
		else
		{
			_WindowsPhone7 = true;
		}
	}

	/**
	 * \brief Static funtion used for setting the UI variables related to the screen size
	 */
	static void SetSizeRelatedVariables()
	{
		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		DetermineScreenSize(screenHeight, screenWidth);
		DeterminePlatform();

		switch(_screenType)
		{
		case 0:
			_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_SMALL;
			_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_SMALL;
			_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_SMALL;
			_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_SMALL;
			_imageButtonWidth = IMAGE_BUTTON_WIDTH_SMALL;
			_imageButtonHeight = IMAGE_BUTTON_HEIGHT_SMALL;
			break;
		case 1:
			_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_MEDIUM;
			_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_MEDIUM;
			_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_MEDIUM;
			_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_MEDIUM;
			_imageButtonWidth = IMAGE_BUTTON_WIDTH_MEDIUM;
			_imageButtonHeight = IMAGE_BUTTON_HEIGHT_MEDIUM;
			break;
		case 2:
			_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE;
			if(_IPhoneOS)
			{
				_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_LARGE - 10;
				_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_LARGE - 10;
				_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_LARGE;
				_imageButtonWidth = IMAGE_BUTTON_WIDTH_LARGE;
				_imageButtonHeight = IMAGE_BUTTON_HEIGHT_LARGE;
			}
			else
			{
				_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_LARGE;
				_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_LARGE;
				_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE;
				_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_LARGE;
				_imageButtonWidth = IMAGE_BUTTON_WIDTH_LARGE;
				_imageButtonHeight = IMAGE_BUTTON_HEIGHT_LARGE;
			}
			break;
		}
	}
}


#endif /* GUIUTIL_H_ */

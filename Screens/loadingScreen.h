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
 * \file loadingScreen.h
 * \author Ciprian Filiapas
 * \date Aug 2, 2012
 */

#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <NativeUI/Screen.h>

namespace NativeUI
{
	class VerticalLayout;
	class ActivityIndicator;
	class Label;
}

namespace GUI
{
	/**
	 * \brief Class for the screen showed at loading
	 */
	class LoadingScreen : public NativeUI::Screen
	{
	public:
		/**
		 * \brief No parameter constructor
		 */
		LoadingScreen();

		/**
		 * \brief Destructor
		 */
		~LoadingScreen();
	private:
		/**
		 * \brief This function calls the UI creation functions
		 */
		void _createUI();

		/**
		 * \brief This function sets _isWP7 bool value
		 */
		void _setPlatform();

		// _isWP7 true if the platform is Windows Phone 7, false otherwise; used for platform specific UI sizes.
		bool _isWP7;
	};
}
#endif /* LOADINGSCREEN_H_ */

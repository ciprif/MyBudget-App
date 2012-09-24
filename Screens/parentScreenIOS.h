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
 * \file parentScreenIOS.h
 * \author Ciprian Filiapas
 * \date Sep 24, 2012
 */

#ifndef PARENTSCREENIOS_H_
#define PARENTSCREENIOS_H_

#include <NativeUI/TabScreen.h>
#include <NativeUI/TabScreenListener.h>
#include <MAUtil/Environment.h>

namespace GUI
{

	class HomeScreen;
	class ListScreen;
	class SettingsScreen;

	class ParentScreenIOS : public NativeUI::TabScreen, public NativeUI::TabScreenListener
	{
	public:
		/**
		 * \brief Constructor with parameters
		 * @param hs HomeScreen* pointer to the home screen
		 * @param ls ListScreen* pointer to the list screen
		 * @param ss SettingsScreen* pointer to the settings screen
		 */
		ParentScreenIOS(HomeScreen* hs, ListScreen* ls, SettingsScreen* ss);

		/**
		 * \brief Destructor
		 */
		~ParentScreenIOS();

		/**
		 * This method is called when a tab screen has changed to a new tab. Is used for changing the custom event listener
		 * @param tabScreen NativeUI::TabScreen* The tab screen object that generated the event.
		 * @param tabScreenIndex int The index of the new tab.
		 */
		void tabScreenTabChanged(NativeUI::TabScreen* tabScreen, int tabScreenIndex);

		/**
		 * \brief This function is used for getting the current active screen (custom event listener)
		 * @return MAUtil::CustomEventListener* pointer to the active screen
		 *
		 * Note: This is used on IOS only for making the options box event handling easier
		 */
		MAUtil::CustomEventListener* getActiveScreen() const;
	private:
		/**
		 * \brief Constructor
		 */
		ParentScreenIOS() {}

		MAUtil::CustomEventListener* _activeScreen;
		GUI::HomeScreen* _homeScreenRef;
		GUI::ListScreen* _listScreenRef;
		GUI::SettingsScreen* _settingsScreenRef;
	};
}
#endif /* PARENTSCREENIOS_H_ */

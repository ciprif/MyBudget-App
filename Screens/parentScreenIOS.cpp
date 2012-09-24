/*
 * parentScreenIOS.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: Cipri
 */
#include "parentScreenIOS.h"
#include "homeScreen.h"
#include "listScreen.h"
#include "settingsScreen.h"

namespace GUI
{
	/**
	 * \brief Constructor
	 */
	ParentScreenIOS::ParentScreenIOS(HomeScreen* hs, ListScreen* ls, SettingsScreen* ss) : _homeScreenRef(NULL), _listScreenRef(NULL), _settingsScreenRef(NULL)
	{
		_homeScreenRef = hs;
		_listScreenRef = ls;
		_settingsScreenRef = ss;

		this->addTab(hs);
		this->addTab(ls);
		this->addTab(ss);

		_activeScreen = hs;

		this->addTabScreenListener(this);
	}

	/**
	 * \brief Destructor
	 */
	ParentScreenIOS::~ParentScreenIOS()
	{
		this->removeTabScreenListener(this);
	}

	/**
	 * This method is called when a tab screen has changed to a new tab. Is used for changing the custom event listener
	 * @param tabScreen NativeUI::TabScreen* The tab screen object that generated the event.
	 * @param tabScreenIndex int The index of the new tab.
	 */
	void ParentScreenIOS::tabScreenTabChanged(NativeUI::TabScreen* tabScreen, int tabScreenIndex)
	{
		if(tabScreen == this && NULL != _homeScreenRef && NULL != _listScreenRef && NULL != _settingsScreenRef)
		{
			MAUtil::Environment::getEnvironment().removeCustomEventListener(_activeScreen);
			switch(tabScreenIndex)
			{
			case 0:
				_activeScreen = _homeScreenRef;
				MAUtil::Environment::getEnvironment().addCustomEventListener(_homeScreenRef);
				break;
			case 1:
				_activeScreen = _listScreenRef;
				MAUtil::Environment::getEnvironment().addCustomEventListener(_listScreenRef);
				break;
			case 2:
				_activeScreen = _settingsScreenRef;
				MAUtil::Environment::getEnvironment().addCustomEventListener(_settingsScreenRef);
				break;
			}
		}
	}

	/**
	 * \brief This function is used for getting the current active screen (custom event listener)
	 * @return MAUtil::CustomEventListener* pointer to the active screen
	 *
	 * Note: This is used on IOS only for making the options box event handling easier
	 */
	MAUtil::CustomEventListener* ParentScreenIOS::getActiveScreen() const
	{
		return _activeScreen;
	}
}

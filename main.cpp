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
 * \file main.cpp
 * \author Ciprian Filipas
 * \brief This file contains the entry point for the application
 */

#include <ma.h>
#include <mavsprintf.h>
#include <MAUtil/Moblet.h>
#include <NativeUI/TabScreen.h>
#include <NativeUI/PanoramaView.h>
#include <MAUtil/Environment.h>
#include <NativeUI/TabScreenListener.h>
#include <conprint.h>

#include "Screens/homeScreen.h"
#include "Screens/addExpenseDialog.h"
#include "Screens/addIncomeDialog.h"
#include "Screens/listScreen.h"
#include "Screens/settingsScreen.h"
#include "Screens/loadingScreen.h"
#include "Logical/observer.h"
#include "Logical/settingsManager.h"
#include "Screens/GUIUtil.h"

using namespace MAUtil;

/**
 * Moblet to be used as a template for a Native UI application.
 */
class NativeUIMoblet : public Moblet, public NativeUI::TabScreenListener
{
public:
	/**
	 * The constructor creates the user interface.
	 */
	NativeUIMoblet()
	{
		// create a new observer object.
		_observer = new Logical::Observer();

		// create the GUI.
		createUI();
	}

	/**
	 * Destructor.
	 */
	virtual ~NativeUIMoblet()
	{
		delete _homeScreen;
		delete _parentScreen;
		delete _observer;
	}

	/**
	 * Create the user interface.
	 */
	void createUI()
	{
		GUI::LoadingScreen loadingScreen;
		loadingScreen.show();
		// the parent container is a panoramaView for WP7 and a TabScreen for Android and iOS

		GUI::DeterminePlatform();

		_listScreen = new GUI::ListScreen();
		_listScreen->setTitle("Transactions");
		_homeScreen = new GUI::HomeScreen();
		_homeScreen->setTitle("Home");
		_settingsScreen = new GUI::SettingsScreen();
		_settingsScreen->setTitle("Settings");

		_observer->setHomeScreenRef(_homeScreen);
		_observer->setListScreenRef(_listScreen);
		_observer->setSettingsScreenRef(_settingsScreen);

		if(GUI::_IPhoneOS || GUI::_Android)
		{
			_parentScreen = new NativeUI::TabScreen();
			_parentScreen->setTitle("MyBudget");

			((NativeUI::TabScreen*)_parentScreen)->addTab(_homeScreen);
			((NativeUI::TabScreen*)_parentScreen)->addTab(_listScreen);
			((NativeUI::TabScreen*)_parentScreen)->addTab(_settingsScreen);
			((NativeUI::TabScreen*)_parentScreen)->addTabScreenListener(this);
			GUI::_activeScreen = _homeScreen;
			 MAUtil::Environment::getEnvironment().addCustomEventListener(GUI::_activeScreen);
		}
		else
		{
			_parentScreen = new NativeUI::PanoramaView();
			_parentScreen->setTitle("MyBudget");

			((NativeUI::PanoramaView*) _parentScreen)->addScreen(_homeScreen);
			((NativeUI::PanoramaView*) _parentScreen)->addScreen(_listScreen);
			((NativeUI::PanoramaView*) _parentScreen)->addScreen(_settingsScreen);
		}

		//Show the screen
		_parentScreen->show();

		_incomesDialog = new GUI::AddIncomeDialog();
		_expensesDialog = new GUI::AddExpenseDialog();

		_homeScreen->setAddExpensesDialogReference(_expensesDialog);
		_homeScreen->setAddIncomesDialogReference(_incomesDialog);
		_listScreen->setAddExpensesDialogReference(_expensesDialog);
		_listScreen->setAddIncomeDialogReference(_incomesDialog);
	}

	/**
	 * Called when a key is pressed.
	 */
	void keyPressEvent(int keyCode, int nativeCode)
	{
		if (MAK_BACK == keyCode || MAK_0 == keyCode)
		{
			// Call close to exit the application.
			close();
		}
	}

	/**
     * This method is called when a tab screen has changed to a new tab. Is used for changing the custom event listener
     * @param tabScreen The tab screen object that generated the event.
     * @param tabScreenIndex The index of the new tab.
     */
	void tabScreenTabChanged(NativeUI::TabScreen* tabScreen, int tabScreenIndex)
	{
		if(tabScreen == _parentScreen)
		{
			MAUtil::Environment::getEnvironment().removeCustomEventListener(GUI::_activeScreen);
			switch(tabScreenIndex)
			{
			case 0:
				GUI::_activeScreen = _homeScreen;
				MAUtil::Environment::getEnvironment().addCustomEventListener(_homeScreen);
				break;
			case 1:
				GUI::_activeScreen = _listScreen;
				MAUtil::Environment::getEnvironment().addCustomEventListener(_listScreen);
				break;
			case 2:
				GUI::_activeScreen = _settingsScreen;
				MAUtil::Environment::getEnvironment().addCustomEventListener(_settingsScreen);
				break;
			}
		}
	}

private:
	NativeUI::Screen* _parentScreen;
    GUI::HomeScreen* _homeScreen;
    GUI::ListScreen* _listScreen;
    GUI::SettingsScreen* _settingsScreen;
    GUI::AddExpenseDialog* _expensesDialog;
    GUI::AddIncomeDialog* _incomesDialog;

    Logical::Observer* _observer;
};

/**
 * Main function that is called when the program starts.
 */
extern "C" int MAMain()
{
	Moblet::run(new NativeUIMoblet());
	return 0;
}

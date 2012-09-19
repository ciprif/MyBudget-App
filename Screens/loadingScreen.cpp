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
 * \file loadingScreen.cpp
 * \author Ciprian Filipas
 * \date Aug 2, 2012
 */


#include <NativeUI/Screen.h>
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/ActivityIndicator.h>
#include <NativeUI/Label.h>

#include "loadingScreen.h"
#include "../Model/ModelUtil.h"
#include "GUIUtil.h"

namespace GUI
{
	/**
	 * \brief No parameter constructor
	 */
	LoadingScreen::LoadingScreen()
	{
		DeterminePlatform();
		_createUI();
	}

	/**
	 * \brief Destructor
	 */
	LoadingScreen::~LoadingScreen() {}

	/**
	 * \brief This function calls the UI creation functions
	 */
	void LoadingScreen::_createUI()
	{
		NativeUI::VerticalLayout* mainLayout = new NativeUI::VerticalLayout();
		NativeUI::ActivityIndicator* activityIndicator = new NativeUI::ActivityIndicator();
		NativeUI::Label* loading;

		NativeUI::VerticalLayout* spacer1;
		NativeUI::VerticalLayout* spacer2;
		NativeUI::VerticalLayout* spacer3;

		if(_WindowsPhone7)
		{
			loading = new NativeUI::Label("Loading...");
			loading->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);
			loading->fillSpaceHorizontally();
			spacer3 = new NativeUI::VerticalLayout();
			spacer3->setHeight(15);
			spacer3->fillSpaceHorizontally();
			activityIndicator->fillSpaceHorizontally();
		}

		spacer1 = new NativeUI::VerticalLayout();
		spacer1->fillSpaceVertically();
		spacer2 = new NativeUI::VerticalLayout();
		spacer2->fillSpaceVertically();

		mainLayout->addChild(spacer1);
		mainLayout->addChild(activityIndicator);

		if(_WindowsPhone7)
		{
			mainLayout->addChild(loading);
			mainLayout->addChild(spacer3);
		}
		else
		{
			mainLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		}

		mainLayout->addChild(spacer2);
		activityIndicator->show();

		this->setMainWidget(mainLayout);
	}
}

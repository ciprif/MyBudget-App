/*
 * loadingScreen.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: Cipri
 */


#include <NativeUI/Screen.h>
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/ActivityIndicator.h>
#include <NativeUI/Label.h>

#include "loadingScreen.h"
#include "../Model/util.h"

namespace GUI
{
	LoadingScreen::LoadingScreen()
	{
		_setPlatform();

		NativeUI::VerticalLayout* mainLayout = new NativeUI::VerticalLayout();
		NativeUI::ActivityIndicator* activityIndicator = new NativeUI::ActivityIndicator();
		NativeUI::Label* loading;

		NativeUI::VerticalLayout* spacer1;
		NativeUI::VerticalLayout* spacer2;
		NativeUI::VerticalLayout* spacer3;

		if(_isWP7)
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

		if(_isWP7)
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

	void LoadingScreen::_setPlatform()
	{
		char buffer[Model::BUFF_SIZE];
		maGetSystemProperty("mosync.device.OS", buffer, Model::BUFF_SIZE);

		if(strcmp(buffer, "iOS") == 0 || strcmp(buffer, "Android") == 0)
		{
			_isWP7 = false;
		}
		else
		{
			_isWP7 = true;
		}
	}

	LoadingScreen::~LoadingScreen() {}
}

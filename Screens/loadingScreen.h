/*
 * loadingScreen.h
 *
 *  Created on: Aug 2, 2012
 *      Author: Cipri
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

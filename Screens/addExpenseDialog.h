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
 *
 * \file addExpenseDialog.h
 * \author Ciprian Filipas
 * \date Jun 19, 2012
 */

#ifndef ADDEXPENSEDIALOG_H_
#define ADDEXPENSEDIALOG_H_

#include <NativeUI/Dialog.h>
#include <NativeUI/CheckBoxListener.h>
#include <NativeUI/ButtonListener.h>
#include <NativeUI/ToggleButtonListener.h>
#include <NativeUI/EditBoxListener.h>
#include <MAUtil/String.h>
#include <MAUtil/Vector.h>
#include "../Model/ModelUtil.h"

// Forward declarations
namespace NativeUI
{
	class HorizontalLayout;
	class VerticalLayout;
	class Label;
	class CheckBox;
	class EditBox;
	class ImageButton;
	class Button;
	class ToggleButton;
	class DatePicker;
	class TimePicker;
}

namespace Logical
{
	class Observer;
}

namespace GUI
{
	class HomeScreen;
	class ListScreen;

	/**
	 * \brief Class for the dialog shown when the user requires an expense addition
	 */
	class AddExpenseDialog : public NativeUI::Dialog, public NativeUI::CheckBoxListener,
							 public NativeUI::ButtonListener, public NativeUI::ToggleButtonListener,
							 public NativeUI::EditBoxListener
	{
	public:
		/**
		 * \brief Constructor
		 */
		AddExpenseDialog();

		/**
		 * \brief Constructor with 3 parameters
		 * @param availableBudget const Double& the availabale budget
		 * @param posibleDeptValue const Double& the posible debt value
		 * @param coin const MAUtil::String& the coin
		 */
		AddExpenseDialog(const double& availableBudget, const double& posibleDeptValue, const MAUtil::String& coin);

		/**
		 * \brief Destructor
		 */
		~AddExpenseDialog();

		 /**
		 * This method is called if the touch-up event was inside the
		 * bounds of the button.
		 * Platform: iOS, Android, Windows Phone.
		 * @param button The button object that generated the event.
		 */
		void buttonClicked(Widget* button);

		/**
		 * This method is called when the state of the check box was changed
		 * by the user.
		 * @param checkBox The check box object that generated the event.
		 * @param state True if the check box is checked, false otherwise.
		 */
		void checkBoxStateChanged(NativeUI::CheckBox* checkBox, bool state);

		/**
		 * \brief This function is used for setting the HomeScreen reference
		 * @param homeScreenRef HomeScreen* pointer to the HomeScreen screen
		 */
		void setHomeScreenRef(HomeScreen* homeScreenRef);

		/**
		 * \brief This function is used for setting the ListScreen reference
		 * @param homeScreenRef ListScreen* pointer to the HomeScreen screen
		 */
		void setListScreenRef(ListScreen* listScreenRef);

		/**
		 * This method is called when the state of the toggle button was changed
		 * by the user.
		 * @param toggleButton The toggle button object that generated the event.
		 * @param state True if the toggle button is checked, false otherwise.
		 */
		void toggleButtonStateChanged(NativeUI::ToggleButton* toggleButton, bool state);

		/**
		 * \brief This function is used for showing the UI. Inherited from the NativeUI::Dialog class
		 */
		void show();

		/**
		 * \brief This function is used for setting the observer reference
		 * @param obs Logical::Observer* pointer to the application wide observer
		 */
		void setObserver(Logical::Observer* obs);

		/**
		 * \brief This function is used for setting the _coin value
		 * @param coin const MAUtil::String& the new coin value
		 */
		void setCoin(const MAUtil::String& coin);

		/**
		 * \brief This function is used for setting the available budget
		 * @param availableBudget const double& the new value
		 */
		void setAvailableBudget(const double& availableBudget);

		/**
		 * \brief This function is used for setting the posible debt value
		 * @param value const double& the new value
		 */
		void setAcceptedDebtValue(const double& value);

		/**
		 * \brief This function is used for updating the placeholder value of the amount editBox
		 */
		void updateAmountValue();

		/**
		 * \brief This function is used for setting the _launcedFromHomeScreen boolean
		 * @param value bool true if the this dialog was launched from the home screen, false otherwise
		 */
		void setLaunchedFromHomeScreen(bool value);

		/**
		 * \brief This function handles the return press button event from the querty keyboard;
		 * 		  this function is inherited from the NativeUI::EditBoxListener class.
		 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
		 */
		void editBoxReturn(NativeUI::EditBox *editBox);

		/**
		 * \brief This function handles the edid did end event from the editBox;
		 * 		  this function is inherited from the NativeUI::EditBoxListener class.
		 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
		 */
		void editBoxEditingDidEnd(NativeUI::EditBox* editBox);
	private:
		/**
		 * \brief This function is used for triggering the UI creation
		 */
		void _createUI();

		/**
		 * \brief This function is used for creating the checkbox group; note that this will behave like
		 * 		  a radio button control
		 */
		NativeUI::VerticalLayout* _createCheckBoxGroup();

		/**
		 * \brief This function is used for creating the buttons from the bottom of the dialog
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createBottomButtonBar();

		/**
		 * \brief This function is used for creating the amount bars (and related UI elements)
		 * @param maxVal const int& the maximal value for the amount editBox placeholder
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createAmountBar(const int& maxVal);

		/**
		 * \brief This function is used for creating the spacer from the bottom of the dialog
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createBottomSpacer();

		/**
		 * \brief This function is used for creating the datePicker and related controls
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createDatePicker();

		/**
		 * \brief This function is used for creating the timePicker and related controls
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createTimePicker();

		/**
		 * \brief This function is used for creating the image related box (capture and folder buttons)
		 */
		void _createImageBox();

		/**
		 * \brief This function is used for creating the description
		 */
		void _createDescriptionBox();

		// References for HomeScreen and ListScreen; used for navigating back to the proper screen
		HomeScreen* _homeScreenRef;
		ListScreen* _listScreenRef;

		// UI related variables
		NativeUI::VerticalLayout* _mainLayout;
		NativeUI::VerticalLayout* _descriptionBoxParent;
		NativeUI::VerticalLayout* _imageBoxAndToggleLayout;
		NativeUI::HorizontalLayout* _imageButtonsParentLayout;
		NativeUI::ImageButton* _selectImageButton;
		NativeUI::ImageButton* _captureImageButton;
		NativeUI::ToggleButton* _descriptionToggleButton;
		NativeUI::ToggleButton* _imageAtachementToggleButton;
		NativeUI::Button* _addButton;
		NativeUI::Button* _cancelButton;
		NativeUI::EditBox* _descriptionEditBox;
		NativeUI::EditBox* _amountEditBox;
		NativeUI::Label* _amountLabel;
		NativeUI::Label* _categoryLabel;
		NativeUI::DatePicker* _datePicker;
		NativeUI::TimePicker* _timePicker;

		// A vector of checkbox objects
		MAUtil::Vector<NativeUI::CheckBox*>* checkBoxVector;

		// Observer reference
		Logical::Observer* _observerReference;

		MAUtil::String _categoryValue;
		MAUtil::String _coin;

		double _availableBudget;
		double _acceptedDept;

		bool _launcedFromHomeScreen;
	};
}

#endif /* ADDEXPENSEDIALOG_H_ */

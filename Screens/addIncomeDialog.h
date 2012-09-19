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
 * \file addIncomeDialog.h
 * \author Ciprian Filipas
 * \date Jul 3, 2012
 */

#ifndef ADDINCOMEDIALOG_H_
#define ADDINCOMEDIALOG_H_

#include <NativeUI/Dialog.h>
#include <NativeUI/CheckBoxListener.h>
#include <NativeUI/ButtonListener.h>
#include <NativeUI/ToggleButtonListener.h>
#include <NativeUI/SliderListener.h>
#include <NativeUI/DatePickerListener.h>
#include <MAUtil/String.h>
#include <MAUtil/Vector.h>
#include "../Model/util.h"

// Forward declarations
namespace NativeUI
{
	class HorizontalLayout;
	class VerticalLayout;
	class Label;
	class CheckBox;
	class EditBox;
	class Button;
	class Slider;
	class ToggleButton;
	class DatePicker;
	class TimePicker;
}


namespace GUI
{
	class HomeScreen;
	class ListScreen;

	/**
	 * \brief Class for the dialog shown when the user requires an income addition
	 */
	class AddIncomeDialog : public NativeUI::Dialog, public NativeUI::CheckBoxListener,
							public NativeUI::ButtonListener, public NativeUI::ToggleButtonListener,
							public NativeUI::SliderListener, public NativeUI::DatePickerListener
	{
	public:
		/**
		 * \brief Constructor
		 */
		AddIncomeDialog();

		/**
		 * \brief Constructor
		 * @param coin const MAUtil::String& the new coin value
		 */
		AddIncomeDialog(const MAUtil::String& coin);

		/**
		 * \brief Destructor
		 */
		~AddIncomeDialog();

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
		 * \brief This function is called when the state of the toggle button was changed
		 * 	 	  by the user. Inherited from the NativeUI::ToggleButtonListener class
		 * @param toggleButton NativeUI::ToggleButton* pointer to the object that triggered the event
		 * @param state bool True if the toggle button is checked, false otherwise.
		 */
		void toggleButtonStateChanged(NativeUI::ToggleButton* toggleButton, bool state);

		/**
		 * \brief This function is called when the state of the slider is changed by the user.
		 * 		  Inherited from the NativeUI::SliderListener class
		 * @param s NativeUI::Slider* pointer to the object that triggered the event
		 * @param sliderValue int the new value of the slider object
		 */
		void sliderValueChanged(NativeUI::Slider* s, int sliderValue);

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
		 * \brief This function is used for setting the _launcedFromHomeScreen boolean
		 * @param value bool true if the this dialog was launched from the home screen, false otherwise
		 */
		void setLaunchedFromHomeScreen(bool value);

		/**
		 * \brief This function handles the date picker value changed event, inherited
		 * 		  from the NativeUI::DatePickerListener class
		 * @param datePicker NativeUI::DatePicker* pointer to the date picker that triggered the event
		 * @param selected const NativeUI::Date& the selected date
		 */
		void datePickerValueChanged(NativeUI::DatePicker* datePicker, const NativeUI::Date& selectedDate){}
	private:
		/**
		 * \brief This function is used for triggering the UI creation
		 */
		void _createUI();

		/**
		 * \brief This function is used for creating the checkbox group; note that this will behave like
		 * 		  a radio button control
		 * @param categories const MAUtil::String[] the array of categories
		 * @param length int the length of the category array
		 * @return NativeUI::VerticalLayout* the newly created layout
		 */
		NativeUI::VerticalLayout* _createCheckBoxGroup(const MAUtil::String categories[], int length);

		/**
		 * \brief This function is used for creating the buttons from the bottom of the dialog
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createBottomButtonBar();

		/**
		 * \brief This function is used for creating the amount bars (sliders and related UI elements)
		 * @return NativeUI::HorizontalLayout* the newly created layout
		 */
		NativeUI::HorizontalLayout* _createAmountBars();

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
		 * \brief This function is used for creating the transaction info box
		 */
		void _createTransactionInfoBox();

		/**
		 * \brief This function is used for creating the description
		 */
		void _createDescriptionBox();

		/**
		 * \brief This function is used for setting the _isWP7 bool value
		 */
		void _setPlatform();

		// References for HomeScreen and ListScreen; used for navigating back to the proper screen
		HomeScreen* _homeScreenRef;
		ListScreen* _listScreenRef;

		// UI related variables
		NativeUI::VerticalLayout* _mainLayout;
		NativeUI::Label* _categoryLabel;
		NativeUI::Label* _amountLabel;
		NativeUI::EditBox* _descriptionEditBox;
		NativeUI::EditBox* _transactionInformationEditBox;
		NativeUI::Button* _addButton;
		NativeUI::Button* _cancelButton;
		NativeUI::Slider* _amountSliderThousands;
		NativeUI::Slider* _amountSliderUnits;
		NativeUI::DatePicker* _datePicker;
		NativeUI::TimePicker* _timePicker;
		NativeUI::ToggleButton* _descriptionToggleButton;
		NativeUI::ToggleButton* _transactionInformationToggleButton;
		NativeUI::VerticalLayout* _descriptionBoxParent;
		NativeUI::VerticalLayout* _transactionInfoBoxParent;

		// A vector of checkbox objects
		MAUtil::Vector<NativeUI::CheckBox*>* _checkBoxVector;

		// Observer reference
		Logical::Observer* _observerReference;

		MAUtil::String _typeValue;
		MAUtil::String _coin;

		bool _launcedFromHomeScreen;
		bool _isWP7;
	};
}
#endif /* ADDINCOMEDIALOG_H_ */

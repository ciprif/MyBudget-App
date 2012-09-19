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
 * \file settingsScreen.h
 * \author Ciprian Filipas
 * \date Jul 17, 2012
 */

#ifndef SETTINGSSCREEN_H_
#define SETTINGSSCREEN_H_

#include <NativeUI/Screen.h>
#include <NativeUI/ScreenListener.h>
#include <NativeUI/ButtonListener.h>
#include <NativeUI/CheckBoxListener.h>
#include <NativeUI/NumberPickerListener.h>
#include <NativeUI/DatePickerListener.h>
#include <NativeUI/ListViewListener.h>
#include <MAUtil/String.h>
#include <NativeUI/EditBoxListener.h>

#include "../Logical/observer.h"
#include "../Model/ModelUtil.h"

// The list of coins or currencies
const MAUtil::String COINS[] = { "EUR", "USD", "GBP", "CAD", "AUD", "JPY", "INR", "NZD",
							     "CHF", "ZAR", "DZD", "ARS", "BHD", "BRL", "BGN", "CLP",
							     "CNY", "COP", "CRC", "HRK", "CZK", "DKK", "DOP", "EGP",
							     "EEK", "FJD", "HKD", "HUF", "ISK", "IDR", "ILS", "JMD",
							     "JOD", "KES", "KRW", "KWD", "LBP", "MYR", "MUR", "MXN",
							     "MAD", "NOK", "OMR", "PKR", "PEN", "PHP", "PLN", "QAR",
							     "RON", "RUB", "SAR", "SGD", "LKR", "TWD", "THB", "TTD",
							     "TND", "TRY", "AED", "VEB", "VND", "ZMK" };

// The number of elements in the COINS array
#define NUMBER_OF_COINS 62

// Forward declarations
namespace NativeUI
{
	class Button;
	class CheckBox;
	class VerticalLayout;
	class HorizontalLayout;
	class ListView;
	class ListViewItem;
	class Label;
	class EditBox;
	class DatePicker;
	class NumberPicker;
}

namespace GUI
{

	/**
	 * \brief Class for the settings screen
	 */
	class SettingsScreen : public NativeUI::Screen, public NativeUI::ScreenListener,
						   public NativeUI::CheckBoxListener, public NativeUI::ButtonListener,
						   public NativeUI::ListViewListener, public NativeUI::DatePickerListener,
						   public NativeUI::NumberPickerListener, public NativeUI::EditBoxListener
	{
	public:
		/**
		 * \brief No parameters constructor.
		 */
		SettingsScreen();

		/**
		 * \brief Destructor
		 */
		~SettingsScreen();

		/**
		 * \brief Sets the observer reference
		 * @param obs Logical::Observer* pointer to the application wide observer
		 */
		void setObserver(Logical::Observer* obs);

		/**
		 * \brief The button click event handler, inherited from the NativeUI::ButtonListener class
		 * @param button NativeUI::Widget* pointer to the button that triggered the event
		 */
		void buttonClicked(NativeUI::Widget* button);

		/**
		 * \brief The list view item clicked event handler, inherited from NativeUI::ListViewListener class
		 * @param listView NativeUI::ListView* pointer to the list view that triggered the event
		 * @param index int the index of the clicked list view item
		 */
		void listViewItemClicked(NativeUI::ListView* listView, int index);

		/**
		 * \brief The list view item clicked event handler, inherited from NativeUI::ListViewListener class
		 * @param listView NativeUI::ListView* pointer to the list that triggered the event
		 * @param listViewItem NativeUI::ListViewItem* pointer to the list view item that was clicked
		 *
		 * \note This function has an empty body because it's not used or necessary in my current implementation
		 */
		void listViewItemClicked(NativeUI::ListView* listView, NativeUI::ListViewItem* listViewItem) {}

		/**
		 * \brief This function handles the menu closed event, inherited from the NativeUI::Screen class
		 * @param screen NativeUI::Screen* pointer to the screen that triggered the menu closed event
		 *
		 * \note This function has an empty body because it's not used in the current implementation
		 */
		void optionsMenuClosed(NativeUI::Screen* screen) {}

		/**
		 * \brief This function handles the options menu item selected event from a screen, inherited
		 * 	      from the NativeUI::Screen class
		 * @param screen NativeUI::Screen* pointer to the screen that triggers the event
		 * @param index int index for the button that was clicked
		 */
		void optionsMenuItemSelected(NativeUI::Screen* screen, int index);

		/**
		 * \brief This function handles the checkbox state changed event, inherited from the NativeUI::CheckBoxListener class
		 * @param checkBox NativeUI::CheckBox* pointer to the CheckBox that triggered the event
		 * @param state bool true or false, indicating the current state of the checkbox
		 */
		void checkBoxStateChanged(NativeUI::CheckBox* checkBox, bool state);

		/**
		 * \brief This function handles the number picker value changed event, inherited
		 * 		  from the NativeUI::NumberPickerListener class
		 * @param numberPicker NativeUI::NumberPicker* pointer to the number picker that triggered the event
		 * @param value int the current value of the picker
		 */
		void numberPickerValueChanged(NativeUI::NumberPicker* numberPicker, int value);

		/**
		 * \brief This function handles the date picker value changed event, inherited
		 * 		  from the NativeUI::DatePickerListener class
		 * @param datePicker NativeUI::DatePicker* pointer to the date picker that triggered the event
		 * @param selected const NativeUI::Date& the selected date
		 */
		void datePickerValueChanged(NativeUI::DatePicker* datePicker, const NativeUI::Date& selected) {}

		/**
		 * \brief This function handles the return press button event from the querty keyboard;
		 * 		  this function is inherited from the NativeUI::EditBoxListener class.
		 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
		 */
		void editBoxReturn(NativeUI::EditBox *editBox);

		/**
		 * \brief This function is used for creating the options menu related controls
		 */
		void createOptionsMenu();
	private:
		/**
		 * \brief This function calls the UI creation functions
		 */
		void _createUI();

		/**
		 * \brief This function synchronizes the values from the settings screen UI with those stored in the repository
		 */
		void _updateValues();

		/**
		 * \brief This functions creates the coin setting sections
		 * @return NativeUI::VerticalLayout* pointer to the newly created layout
		 */
		NativeUI::VerticalLayout* _createCoinSettingsLayout();

		/**
		 * \brief This function creates the transaction list section
		 * @return NativeUI::VerticalLayout* pointer to the newly created layout
		 */
		NativeUI::VerticalLayout* _createTransactionListSettingsLayout();

		/**
		 * \brief This function creates the debt value section
		 * @return NativeUI::VerticalLayout* pointer to the newly created layout
		 */
		NativeUI::VerticalLayout* _createDebtValueSettingsLayout();

		/**
		 * \brief This function is called in order to collapse the coin list
		 */
		void _collapseCoinList();

		/**
		 * \brief This function is called in order to expend the coin list
		 */
		void _expendCoinList();

		/**
		 * \brief This function is called to handle the AllItems checkBox state changed; called
		 * 	 	  from the checkBoxStateChanged event handler
		 */
		void _handleAllItemsCheckboxStateChenged();

		/**
		 * \brief This function is called to handle the Monthly checkBox state changed; called
		 * 	 	  from the checkBoxStateChanged event handler
		 */
		void _handleMonthlyCheckboxStateChenged();

		/**
		 * \brief This function is called to handle the FromDate checkBox state changed; called
		 * 	 	  from the checkBoxStateChanged event handler
		 */
		void _handleFromDateCheckboxStateChenged();

		/**
		 * \brief This function stores the date settings that were updated from the SettingsScreen UI;
		 * 		  Called from the save button clicked event handle;
		 * @param d Model::DateStruct& the destination DateStruct
		 *
		 * \note This function does not actually save the values to the settings manager and repositories,
		 * 		 it only stores the data into the structure.
		 */
		void _saveDateSettings(Model::DateStruct& d);

		/**
		 * \brief This function stores the coin settings that were updated from the SettingsScreen UI;
		 * 		  Called from the save button clicked event handle;
		 * @param d Model::DateStruct& the destination DateStruct
		 *
		 * \note This function does not actually save the values to the settings manager and repositories,
		 * 		 it only stores the data into the structure.
		 */
		void _saveCoinSettings();

		/**
		 * \brief This function stores the debt settings that were updated from the SettingsScreen UI;
		 * 		  Called from the save button clicked event handle;
		 * @param d Model::DateStruct& the destination DateStruct
		 *
		 * \note This function does not actually save the values to the settings manager and repositories,
		 * 		 it only stores the data into the structure.
		 */
		void _saveDebtSettings();

		/**
		 * \brief This function is called in order to synchronize the date values according to those stored
		 * 		  in the repository
		 */
		void _resetDateValue();

		/**
		 * \brief This function is called in order to synchronize the debt values according to those stored
		 * 		  in the repository
		 */
		void _resetDebtValue();

		// Observer reference
		Logical::Observer* _observerReference;

		// UI related variables
		NativeUI::Label* _debtValueLabel;
		NativeUI::Label* _coinLabel;
		NativeUI::Button* _coinChangeToggle;
		NativeUI::VerticalLayout* _coinSettingsLayout;
		NativeUI::VerticalLayout* _transactionSettingsLayout;
		NativeUI::VerticalLayout* _mainLayout;
		NativeUI::ListView* _coinsList;
		NativeUI::CheckBox* _allItems;
		NativeUI::CheckBox* _monthly;
		NativeUI::CheckBox* _fromDate;
		NativeUI::EditBox* _newDebtValueEditBox;
		NativeUI::EditBox* _numberPickerReplace;
		NativeUI::DatePicker* _datePicker;
		NativeUI::NumberPicker* _numberPicker;

		// _saveButtonIndex is the index of the save button from the options menu
		int _saveButtonIndex;

		// _restoreButtonIndex is the index of the restore button from the options menu
		int _restoreButtonIndex;

		// Storage variables
		// _coin will keep the selected currency
		MAUtil::String _coin;

		// _debtValue will keep the debt value that was set by the user
		double _debtValue;

		// _itemWidth stores the width of an UI item
		int _itemWidth;

		// _isAllItems, _isMonthly, _isFromDate store the selected checkbox state
		bool _isAllItems;
		bool _isMonthly;
		bool _isFromDate;

		// _dayValue stores the selected day value
		int _dayValue;
	};
}

#endif /* SETTINGSSCREEN_H_ */

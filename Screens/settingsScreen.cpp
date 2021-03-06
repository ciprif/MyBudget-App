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
 * \file settingsScreen.cpp
 * \author Ciprian Filipas
 * \date Jul 17, 2012
 */

#include <NativeUI/VerticalLayout.h>
#include <NativeUI/HorizontalLayout.h>
#include <NativeUI/ToggleButton.h>
#include <NativeUI/CheckBox.h>
#include <NativeUI/Button.h>
#include <NativeUI/ListView.h>
#include <NativeUI/ListViewItem.h>
#include <NativeUI/Label.h>
#include <NativeUI/EditBox.h>
#include <NativeUI/NumberPicker.h>
#include <NativeUI/DatePicker.h>
#include <NativeUI/RelativeLayout.h>

#include "GUIUtil.h"
#include "settingsScreen.h"
#include "../Logical/settingsManager.h"

//global count variable
short clickCount;

namespace GUI
{
	/**
	 * \brief No parameters constructor.
	 */
	SettingsScreen::SettingsScreen() : _debtValue(0.0), _isAllItems(true), _isFromDate(false), _isMonthly(false), _optionsButton(NULL)
	{
		_coin = COINS[0]; //Default value

		clickCount = 0;
		DeterminePlatform();
		_createUI();
	}

	/**
	 * \brief Destructor
	 */
	SettingsScreen::~SettingsScreen()
	{
		_allItems->removeCheckBoxListener(this);
		_monthly->removeCheckBoxListener(this);
		_fromDate->removeCheckBoxListener(this);
		_datePicker->removeDatePickerListener(this);
		_newDebtValueEditBox->removeEditBoxListener(this);
		if(_WindowsPhone7) _numberPicker->removeNumberPickerListener(this);
		if(_IPhoneOS) _optionsButton->removeButtonListener(this);
		else _numberPickerReplace->removeEditBoxListener(this);
	}

	/**
	 * \brief Sets the observer reference
	 * @param obs Logical::Observer* pointer to the application wide observer
	 */
	void SettingsScreen::setObserver(Logical::Observer* obj)
	{
		_observerReference = obj;

		_updateValues();
	}

	/**
	 * \brief The button click event handler, inherited from the NativeUI::ButtonListener class
	 * @param button NativeUI::Widget* pointer to the button that triggered the event
	 */
	void SettingsScreen::buttonClicked(NativeUI::Widget* b)
	{
		if(b == _coinChangeToggle)
		{
			if(1 == _coinSettingsLayout->countChildWidgets())
			{
				_expendCoinList();
			}
			else
			{
				_collapseCoinList();
			}
		}
		else if(b == _optionsButton)
		{
			MAUtil::WString boxTitle(L"Options");
			MAUtil::WString destructiveButton(L"");
			MAUtil::WString cancelButton(L"Cancel");
			int count = sizeof(int);
			for(int i = 0; i < SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH; i++)
			{
				count += (SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[i].length() * sizeof(wchar)) + sizeof(wchar);
			}

			char* buffer = new char[count];
			*(int*) buffer = SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH;

			wchar_t* dest = (wchar_t*)(buffer + sizeof(int));

			for(int i = 0; i < SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH; i++)
			{
				const wchar_t* array = SETTINGS_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[i].pointer();
				while (true)
				{
					*dest = *array;
					dest++;
					if(*array == 0)
					{
						break;
					}
					array++;
				}
			}

			MAAddress memoryAdd = buffer;
			maOptionsBox(boxTitle.pointer(), destructiveButton.pointer(),
							 cancelButton.pointer(), memoryAdd, count);
			delete[] buffer;
		}
	}

	/**
	 * \brief The list view item clicked event handler, inherited from NativeUI::ListViewListener class
	 * @param listView NativeUI::ListView* pointer to the list view that triggered the event
	 * @param index int the index of the clicked list view item
	 */
	void SettingsScreen::listViewItemClicked(NativeUI::ListView* l, int index)
	{
		if(l == _coinsList)
		{
			_coin = COINS[index];

			MAUtil::String coinLabelText = "The current coin: ";
			coinLabelText += _coin;
			coinLabelText += " (not applied)";
			_coinLabel->setText(coinLabelText);
			_collapseCoinList();
		}
	}

	/**
	 * \brief This function handles the options menu item selected event from a screen, inherited
	 * 	      from the NativeUI::Screen class
	 * @param screen NativeUI::Screen* pointer to the screen that triggers the event
	 * @param index int index for the button that was clicked
	 */
	void SettingsScreen::optionsMenuItemSelected(NativeUI::Screen*, int index)
	{
		if(index == _saveButtonIndex)
		{
			if(_WindowsPhone7) maAlert("Alert!", "Are you sure you want to save these settings?", "OK", "", "Cancel");
			else maAlert("Alert!", "Are you sure you want to save these settings?", "OK", "Cancel", "");
		}
		else if(index == _restoreButtonIndex)
		{
			_collapseCoinList();
			_updateValues(); //reset values with the saved ones observer
		}
	}

	/**
	 * \brief This function handles the checkbox state changed event, inherited from the NativeUI::CheckBoxListener class
	 * @param checkBox NativeUI::CheckBox* pointer to the CheckBox that triggered the event
	 * @param state bool true or false, indicating the current state of the checkbox
	 */
	void SettingsScreen::checkBoxStateChanged(NativeUI::CheckBox* checkBox, bool state)
	{
		_collapseCoinList();
		if(state == true)
		{
			if(checkBox == _monthly)
			{
				_handleMonthlyCheckboxStateChenged();
			}
			else if(checkBox == _allItems)
			{
				_handleAllItemsCheckboxStateChenged();
			}
			else if(checkBox == _fromDate)
			{
				_handleFromDateCheckboxStateChenged();
			}
		}
		else
		{
			if(_isAllItems && checkBox == _allItems) checkBox->setState(true);
			if(_isMonthly && checkBox == _monthly) checkBox->setState(true);
			if(_isFromDate && checkBox == _fromDate) checkBox->setState(true);
		}
	}

	/**
	 * \brief This function handles the number picker value changed event, inherited
	 * 		  from the NativeUI::NumberPickerListener class
	 * @param numberPicker NativeUI::NumberPicker* pointer to the number picker that triggered the event
	 * @param value int the current value of the picker
	 */
	void SettingsScreen::numberPickerValueChanged(NativeUI::NumberPicker* picker, int value)
	{
		_collapseCoinList();
		if(picker == _numberPicker)
		{
			_dayValue = value;
		}
	}

	/**
	 * \brief This function handles the date picker value changed event, inherited
	 * 		  from the NativeUI::DatePickerListener class
	 * @param datePicker NativeUI::DatePicker* pointer to the date picker that triggered the event
	 * @param selected const NativeUI::Date& the selected date
	 */
	void SettingsScreen::datePickerValueChanged(NativeUI::DatePicker* datePicker, const NativeUI::Date& selected)
	{
		_collapseCoinList();
	}

	/**
	 * \brief This function handles the return press button event from the querty keyboard;
	 * 		  this function is inherited from the NativeUI::EditBoxListener class.
	 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
	 */
	void SettingsScreen::editBoxReturn(NativeUI::EditBox *editBox)
	{
		editBox->hideKeyboard();
	}

    /**
     * This method is called when an edit box gains focus.
     * The virtual keyboard is shown.
     * Platform: Android, WP7 and iOS.
     * @param editBox The edit box object that generated the event.
     */
    void SettingsScreen::editBoxEditingDidBegin(NativeUI::EditBox* editBox)
    {
    	_collapseCoinList();
    }
	/**
	 * \brief This function is used for handling the custom event triggered by the alert box
	 * @param event const MAEvent& the event type
	 */
	void SettingsScreen::customEvent(const MAEvent& event)
	{
		if(event.type == EVENT_TYPE_ALERT)
		{
			_collapseCoinList();
			if(1 == event.alertButtonIndex)
			{
				Model::DateStruct d;
				_saveDateSettings(d);

				_saveCoinSettings();

				_saveDebtSettings();
				_observerReference->requestSaveSettings(_isAllItems, _isMonthly, _isFromDate, _debtValue, d, _coin);
			}
		}
		else if(event.type == EVENT_TYPE_OPTIONS_BOX_BUTTON_CLICKED)
		{
			_collapseCoinList();
			if(0 == event.optionsBoxButtonIndex) //save
			{
				maAlert("Alert!", "Are you sure you want to save these settings?", "OK", "Cancel", "");
			}
			else if(1 == event.optionsBoxButtonIndex) //restore
			{
				_updateValues();
			}
		}
	}

	/**
	 * \brief This function is used for creating the options menu related controls
	 */
	void SettingsScreen::createOptionsMenu()
	{
		if(!_IPhoneOS)
		{
			_saveButtonIndex = addOptionsMenuItem("Save", MAW_OPTIONS_MENU_ICON_CONSTANT_SAVE, false);
			_restoreButtonIndex = addOptionsMenuItem("Restore", MAW_OPTIONS_MENU_ICON_CONSTANT_CLOSE_CLEAR_CANCEL, false);
		}
		else
		{
			if(NULL == _optionsButton)
			{
				_optionsButton = new NativeUI::Button();
				_optionsButton->fillSpaceHorizontally();
				_optionsButton->setText("Options");
				_optionsButton->addButtonListener(this);
				_optionsButton->setWidth(_itemWidth);

				NativeUI::HorizontalLayout* spacer = new NativeUI::HorizontalLayout();
				spacer->setHeight(_checkboxLayoutHeightIOS / 2);

				_mainLayout->addChild(spacer);
				_mainLayout->addChild(_optionsButton);
			}
		}
	}

	/**
	 * \brief This function calls the UI creation functions
	 */
	void SettingsScreen::_createUI()
	{
		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		SetSizeRelatedVariables();

		NativeUI::VerticalLayout* parent = new NativeUI::VerticalLayout();
		_mainLayout = new NativeUI::VerticalLayout();

		if(_WindowsPhone7)
		{
			_itemWidth = 9 * (screenWidth / 10) - screenWidth / 30;

			parent->setHeight(370);
			_mainLayout->setScrollable(true);
			parent->addChild(_mainLayout);
			this->setMainWidget(parent);

			_coinSettingsLayout = _createCoinSettingsLayout();
			_mainLayout->addChild(_coinSettingsLayout);
			_mainLayout->addChild(_createDebtValueSettingsLayout());
			_transactionSettingsLayout = _createTransactionListSettingsLayout();
			_mainLayout->addChild(_transactionSettingsLayout);
		}
		else
		{
			_mainLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
			_itemWidth = (int)(screenWidth * 0.90);

			if(LARGE_SCREEN != _screenType)
			{
				_mainLayout->setHeight(screenHeight + DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_MEDIUM);
			}

			if(_IPhoneOS)
			{
				_mainLayout->setHeight(_settingsScreenHeightIOS);

				NativeUI::RelativeLayout* relativeLayout = new NativeUI::RelativeLayout();
				relativeLayout->setScrollable(true);
				relativeLayout->addChild(_mainLayout);

				setMainWidget(relativeLayout);
			}
			else this->setMainWidget(_mainLayout);

			_coinSettingsLayout = _createCoinSettingsLayout();
			_mainLayout->addChild(_coinSettingsLayout);
			_mainLayout->addChild(_createDebtValueSettingsLayout());
			_transactionSettingsLayout = _createTransactionListSettingsLayout();
			_mainLayout->addChild(_transactionSettingsLayout);
		}

		createOptionsMenu();

		addScreenListener(this);
	}

	/**
	 * \brief This function synchronizes the values from the settings screen UI with those stored in the repository
	 */
	void SettingsScreen::_updateValues()
	{
		_coin = _observerReference->requestCoin();
		_isAllItems = _observerReference->requestIsShowAll();
		_isMonthly = _observerReference->requestIsShowMonthly();
		_isFromDate = _observerReference->requestIsShowFromDate();
		_debtValue = _observerReference->requestDebtValue();

		MAUtil::String* coinLabelText = new MAUtil::String();

		*coinLabelText = "The current coin: ";

		coinLabelText->append(_coin.c_str(), _coin.length());
		_coinLabel->setText(*coinLabelText);
		delete coinLabelText;

		_resetDateValue();

		_dayValue = _observerReference->requestFromDate()._day;

		_resetDebtValue();
	}

	/**
	 * \brief This functions creates the coin setting sections
	 * @return NativeUI::VerticalLayout* pointer to the newly created layout
	 */
	NativeUI::VerticalLayout* SettingsScreen::_createCoinSettingsLayout()
	{
		NativeUI::VerticalLayout* parentLayout = new NativeUI::VerticalLayout();
		parentLayout->wrapContentVertically();

		_coinLabel = new NativeUI::Label();

		MAUtil::String coinLabelText = "The current coin: ";
		coinLabelText += _coin;

		_coinLabel->setText(coinLabelText);
		if(_Android) _coinLabel->fillSpaceHorizontally();
		else if(_IPhoneOS)
		{
			_coinLabel->setWidth(_itemWidth);
			_coinLabel->setHeight(_checkboxLayoutHeightIOS);
		}
		_coinLabel->setFontSize(_dialogFontSize);
		_coinLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);

		NativeUI::VerticalLayout* toggleAndLabelParent = new NativeUI::VerticalLayout();
		toggleAndLabelParent->wrapContentVertically();

		if(!_WindowsPhone7) toggleAndLabelParent->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		_coinChangeToggle = new NativeUI::Button();
		_coinChangeToggle->setText("Change coin");
		_coinChangeToggle->setWidth(_itemWidth);
		_coinChangeToggle->addButtonListener(this);

		toggleAndLabelParent->addChild(_coinLabel);
		toggleAndLabelParent->addChild(_coinChangeToggle);

		_coinsList = new NativeUI::ListView();
		_coinsList->setWidth(_itemWidth);
		if(_IPhoneOS) _coinsList->setHeight(_descriptionBoxHeight * 2);
		else _coinsList->setHeight(_descriptionBoxHeight);

		for(int i = 0; i < NUMBER_OF_COINS; i++)
		{
			NativeUI::ListViewItem* item = new NativeUI::ListViewItem();
			item->setText(COINS[i]);
			item->setFontSize(_dialogFontSize);
			_coinsList->addChild(item);
		}

		parentLayout->addChild(toggleAndLabelParent);

		return parentLayout;
	}

	/**
	 * \brief This function creates the transaction list section
	 * @return NativeUI::VerticalLayout* pointer to the newly created layout
	 */
	NativeUI::VerticalLayout* SettingsScreen::_createTransactionListSettingsLayout()
	{
		NativeUI::VerticalLayout* transactionListSettings = new NativeUI::VerticalLayout();
		transactionListSettings->wrapContentVertically();
		if(!_WindowsPhone7)
		{
			transactionListSettings->fillSpaceHorizontally();
			transactionListSettings->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		}

		_allItems = new NativeUI::CheckBox();
		_monthly = new NativeUI::CheckBox();
		_fromDate = new NativeUI::CheckBox();

		_allItems->addCheckBoxListener(this);
		_monthly->addCheckBoxListener(this);
		_fromDate->addCheckBoxListener(this);

		NativeUI::HorizontalLayout* checkBoxLabelLayoutAll = new NativeUI::HorizontalLayout();
		NativeUI::HorizontalLayout* checkBoxLabelLayoutMonthly = new NativeUI::HorizontalLayout();
		NativeUI::HorizontalLayout* checkBoxLabelLayoutFromDate = new NativeUI::HorizontalLayout();

		checkBoxLabelLayoutAll->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		checkBoxLabelLayoutAll->fillSpaceHorizontally();
		checkBoxLabelLayoutAll->wrapContentVertically();
		checkBoxLabelLayoutMonthly->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		checkBoxLabelLayoutMonthly->fillSpaceHorizontally();
		checkBoxLabelLayoutMonthly->wrapContentVertically();
		checkBoxLabelLayoutFromDate->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		checkBoxLabelLayoutFromDate->fillSpaceHorizontally();
		checkBoxLabelLayoutFromDate->wrapContentVertically();

		NativeUI::Label* checkBoxLabelAllItems = new NativeUI::Label();
		checkBoxLabelAllItems->setText("All items");

		NativeUI::Label* checkBoxLabelMonthly = new NativeUI::Label();
		checkBoxLabelMonthly->setText("Monthly");

		NativeUI::Label* checkBoxLabelFromDate = new NativeUI::Label();
		checkBoxLabelFromDate->setText("Starting from date");

		checkBoxLabelAllItems->setFontSize(_dialogFontSize);
		checkBoxLabelMonthly->setFontSize(_dialogFontSize);
		checkBoxLabelFromDate->setFontSize(_dialogFontSize);

		if(_IPhoneOS)
		{
			NativeUI::HorizontalLayout* auxLayout = new NativeUI::HorizontalLayout();
			auxLayout->fillSpaceHorizontally();
			auxLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_RIGHT);
			auxLayout->addChild(_allItems);
			auxLayout->setHeight(_checkboxLayoutHeightIOS);

			checkBoxLabelLayoutAll->addChild(checkBoxLabelAllItems);
			checkBoxLabelLayoutAll->addChild(auxLayout);

			auxLayout = new NativeUI::HorizontalLayout();
			auxLayout->fillSpaceHorizontally();
			auxLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_RIGHT);
			auxLayout->addChild(_monthly);
			auxLayout->setHeight(_checkboxLayoutHeightIOS);

			checkBoxLabelLayoutMonthly->addChild(checkBoxLabelMonthly);
			checkBoxLabelLayoutMonthly->addChild(auxLayout);

			auxLayout = new NativeUI::HorizontalLayout();
			auxLayout->fillSpaceHorizontally();
			auxLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_RIGHT);
			auxLayout->addChild(_fromDate);
			auxLayout->setHeight(_checkboxLayoutHeightIOS);

			checkBoxLabelLayoutFromDate->addChild(checkBoxLabelFromDate);
			checkBoxLabelLayoutFromDate->addChild(auxLayout);

			checkBoxLabelLayoutAll->setWidth(_itemWidth);
			checkBoxLabelLayoutMonthly->setWidth(_itemWidth);
			checkBoxLabelLayoutFromDate->setWidth(_itemWidth);
		}
		else
		{
			checkBoxLabelLayoutAll->addChild(_allItems);
			checkBoxLabelLayoutAll->addChild(checkBoxLabelAllItems);

			checkBoxLabelLayoutMonthly->addChild(_monthly);
			checkBoxLabelLayoutMonthly->addChild(checkBoxLabelMonthly);

			checkBoxLabelLayoutFromDate->addChild(_fromDate);
			checkBoxLabelLayoutFromDate->addChild(checkBoxLabelFromDate);
		}

		transactionListSettings->addChild(checkBoxLabelLayoutAll);
		transactionListSettings->addChild(checkBoxLabelLayoutMonthly);
		transactionListSettings->addChild(checkBoxLabelLayoutFromDate);

		_datePicker = new NativeUI::DatePicker();
		if(_WindowsPhone7)
		{
			_numberPicker = new NativeUI::NumberPicker();
			_numberPicker->addNumberPickerListener(this);

			_numberPicker->setMaxValue(31);
			_numberPicker->setMinValue(1);
		}
		else
		{
			_numberPickerReplace = new NativeUI::EditBox();
			_numberPickerReplace->setPlaceholder("Insert the day of the month!");
			_numberPickerReplace->setInputMode(NativeUI::EDIT_BOX_INPUT_MODE_NUMERIC);
			_numberPickerReplace->setWidth(_itemWidth);
			_numberPickerReplace->wrapContentVertically();
			_numberPickerReplace->addEditBoxListener(this);
		}

		_datePicker->addDatePickerListener(this);

		_datePicker->setDayOfMonth(Logical::DEFAULT_DAY);
		_datePicker->setMonth(Logical::DEFAULT_MONTH);
		_datePicker->setYear(Logical::DEFAULT_YEAR);

		return transactionListSettings;
	}

	/**
	 * \brief This function creates the debt value section
	 * @return NativeUI::VerticalLayout* pointer to the newly created layout
	 */
	NativeUI::VerticalLayout* SettingsScreen::_createDebtValueSettingsLayout()
	{
		NativeUI::VerticalLayout* debtValueSettingsParent = new NativeUI::VerticalLayout();
		debtValueSettingsParent->wrapContentVertically();

		MAUtil::String debtValueLabelText = "The current debt value: ";
		debtValueLabelText += MAUtil::doubleToString(_debtValue, 2);
		debtValueLabelText += " ";
		debtValueLabelText += _coin;

		_debtValueLabel = new NativeUI::Label(debtValueLabelText);
		_debtValueLabel->setFontSize(_dialogFontSize);
		if(_Android) _debtValueLabel->fillSpaceHorizontally();
		else if(_IPhoneOS)
		{
			_debtValueLabel->setWidth(_itemWidth);
			_debtValueLabel->setHeight(_checkboxLayoutHeightIOS);
		}

		_debtValueLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);

		_newDebtValueEditBox = new NativeUI::EditBox();
		_newDebtValueEditBox->setInputMode(NativeUI::EDIT_BOX_INPUT_MODE_NUMERIC);
		_newDebtValueEditBox->setWidth(_itemWidth);
		_newDebtValueEditBox->wrapContentVertically();
		_newDebtValueEditBox->setPlaceholder("New debt value");
		_newDebtValueEditBox->addEditBoxListener(this);

		debtValueSettingsParent->addChild(_debtValueLabel);
		debtValueSettingsParent->addChild(_newDebtValueEditBox);

		if(_IPhoneOS)
		{
			NativeUI::HorizontalLayout* spacer = new NativeUI::HorizontalLayout();
			spacer->setHeight(_checkboxLayoutHeightIOS / 2);

			debtValueSettingsParent->addChild(spacer);
		}

		if(!_WindowsPhone7) debtValueSettingsParent->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		return debtValueSettingsParent;
	}

	/**
	 * \brief This function is called in order to collapse the coin list
	 */
	void SettingsScreen::_collapseCoinList()
	{
		if(1 < _coinSettingsLayout->countChildWidgets())
		{
			_recalculateMainLayoutHeight(_coinsList->getHeight(), false);
			_coinSettingsLayout->removeChild(_coinsList);
			_coinsList->removeListViewListener(this);
		}
	}

	/**
	 * \brief This function is called in order to expend the coin list
	 */
	void SettingsScreen::_expendCoinList()
	{
		_coinSettingsLayout->addChild(_coinsList);
		_recalculateMainLayoutHeight(_coinsList->getHeight(), true);
		if(!_WindowsPhone7) _coinSettingsLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		_coinsList->addListViewListener(this);
	}

	/**
	 * \brief This function is called to handle the AllItems checkBox state changed; called
	 * 	 	  from the checkBoxStateChanged event handler
	 */
	void SettingsScreen::_handleAllItemsCheckboxStateChenged()
	{
		if(_transactionSettingsLayout->countChildWidgets() == 4)
		{
			NativeUI::Widget* widget = _transactionSettingsLayout->getChild(3);
			_recalculateMainLayoutHeight(widget->getHeight(), false);
			_transactionSettingsLayout->removeChild(widget);
		}

		if(_IPhoneOS || _WindowsPhone7)
		{
			_allItems->setState(true);
			_fromDate->setState(false);
			_monthly->setState(false);
		}
		else
		{
			if(_isFromDate) _fromDate->setState(false);
			else if(_isMonthly) _monthly->setState(false);
		}

		_isMonthly = false;
		_isAllItems = true;
		_isFromDate = false;
	}

	/**
	 * \brief This function is called to handle the Monthly checkBox state changed; called
	 * 	 	  from the checkBoxStateChanged event handler
	 */
	void SettingsScreen::_handleMonthlyCheckboxStateChenged()
	{
		if(_transactionSettingsLayout->countChildWidgets() == 4)
		{
			NativeUI::Widget* widget = _transactionSettingsLayout->getChild(3);
			_recalculateMainLayoutHeight(widget->getHeight(), false);

			if(widget == _datePicker)
				_transactionSettingsLayout->removeChild(_datePicker);
		}

		if(_WindowsPhone7) _transactionSettingsLayout->addChild(_numberPicker);
		else
		{
			_recalculateMainLayoutHeight(_numberPickerReplace->getHeight(), true);
			_transactionSettingsLayout->addChild(_numberPickerReplace);
		}

		if(_IPhoneOS || _WindowsPhone7)
		{
			_monthly->setState(true);
			_allItems->setState(false);
			_fromDate->setState(false);
		}
		else
		{
			if(_isAllItems) _allItems->setState(false);
			else if(_isFromDate) _fromDate->setState(false);
		}

		_isMonthly = true;
		_isAllItems = false;
		_isFromDate = false;
	}

	/**
	 * \brief This function is called to handle the FromDate checkBox state changed; called
	 * 	 	  from the checkBoxStateChanged event handler
	 */
	void SettingsScreen::_handleFromDateCheckboxStateChenged()
	{
		if(_transactionSettingsLayout->countChildWidgets() == 4)
		{
			NativeUI::Widget* widget = _transactionSettingsLayout->getChild(3);

			if(widget == _numberPicker && _WindowsPhone7)
			{
				_transactionSettingsLayout->removeChild(_numberPicker);
			}
			else if(widget == _numberPickerReplace)
			{
				_transactionSettingsLayout->removeChild(_numberPickerReplace);
				_recalculateMainLayoutHeight(_numberPickerReplace->getHeight(), false);
			}
		}

		_transactionSettingsLayout->addChild(_datePicker);
		_recalculateMainLayoutHeight(_datePicker->getHeight(), true);

		if(_IPhoneOS || _WindowsPhone7)
		{
			_fromDate->setState(true);
			_allItems->setState(false);
			_monthly->setState(false);
		}
		else
		{
			if(_isAllItems) _allItems->setState(false);
			else if(_isMonthly) _monthly->setState(false);
		}

		_isMonthly = false;
		_isAllItems = false;
		_isFromDate = true;
	}

	/**
	 * \brief This function stores the date settings that were updated from the SettingsScreen UI;
	 * 		  Called from the save button clicked event handle;
	 * @param d Model::DateStruct& the destination DateStruct
	 *
	 * \note This function does not actually save the values to the settings manager and repositories,
	 * 		 it only stores the data into the structure.
	 */
	void SettingsScreen::_saveDateSettings(Model::DateStruct& d)
	{
		d._day = _datePicker->getDayOfMonth();
		d._mounth = _datePicker->getMonth();
		d._year = _datePicker->getYear();

		if(_isMonthly)
		{
			if(_WindowsPhone7)
			{
				d._day = _dayValue;
			}
			else
			{
				d._day = MAUtil::stringToInteger(_numberPickerReplace->getText().c_str());
			}

			struct tm * dateTime = new tm;
			split_time(maTime(), dateTime);

			d._mounth = dateTime->tm_mon + 1;
			d._year = dateTime->tm_year + 1900;
		}
		else if(_isAllItems)
		{
			d._day = 1;
			d._mounth = 1;
			d._year = 1601;
		}
	}

	/**
	 * \brief This function stores the coin settings that were updated from the SettingsScreen UI;
	 * 		  Called from the save button clicked event handle;
	 * @param d Model::DateStruct& the destination DateStruct
	 *
	 * \note This function does not actually save the values to the settings manager and repositories,
	 * 		 it only stores the data into the structure.
	 */
	void SettingsScreen::_saveCoinSettings()
	{
		MAUtil::String coinLabelText = "The current coin: ";
		coinLabelText += _coin;
		_coinLabel->setText(coinLabelText);
	}

	/**
	 * \brief This function stores the debt settings that were updated from the SettingsScreen UI;
	 * 		  Called from the save button clicked event handle;
	 * @param d Model::DateStruct& the destination DateStruct
	 *
	 * \note This function does not actually save the values to the settings manager and repositories,
	 * 		 it only stores the data into the structure.
	 */
	void SettingsScreen::_saveDebtSettings()
	{
		if(_newDebtValueEditBox->getText().length() > 0) _debtValue = MAUtil::stringToDouble(_newDebtValueEditBox->getText().c_str());

		MAUtil::String* debtValueLabelText = new MAUtil::String("The current debt value: ");
		debtValueLabelText->append(MAUtil::doubleToString(_debtValue, 2).c_str(), MAUtil::doubleToString(_debtValue, 2).length());
		debtValueLabelText->append(" ", 1);
		debtValueLabelText->append(_coin.c_str(), _coin.length());

		_debtValueLabel->setText(*debtValueLabelText);

		delete debtValueLabelText;

		_newDebtValueEditBox->setText("");
		_newDebtValueEditBox->setPlaceholder("New debt value");
	}

	/**
	 * \brief This function is called in order to synchronize the date values according to those stored
	 * 		  in the repository
	 */
	void SettingsScreen::_resetDateValue()
	{
		if(_isAllItems)
		{
			if(_WindowsPhone7 || _IPhoneOS) checkBoxStateChanged(_allItems, true);
			else
			{
				_allItems->setState(true);
				_monthly->setState(false);
				_fromDate->setState(false);
			}
		}
		else if(_isMonthly)
		{
			if(_WindowsPhone7 || _IPhoneOS) checkBoxStateChanged(_monthly, true);
			else
			{
				_monthly->setState(true);
				_fromDate->setState(false);
				_allItems->setState(false);
			}
			_datePicker->setDayOfMonth(_observerReference->requestFromDate()._day);
			_datePicker->setMonth(_observerReference->requestFromDate()._mounth);
			_datePicker->setYear(_observerReference->requestFromDate()._year);
		}
		else if(_isFromDate)
		{
			if(_WindowsPhone7 || _IPhoneOS) checkBoxStateChanged(_fromDate, true);
			else
			{
				_fromDate->setState(true);
				_monthly->setState(false);
				_allItems->setState(false);
			}
			_datePicker->setDayOfMonth(_observerReference->requestFromDate()._day);
			_datePicker->setMonth(_observerReference->requestFromDate()._mounth);
			_datePicker->setYear(_observerReference->requestFromDate()._year);
		}
	}

	/**
	 * \brief This function is called in order to synchronize the debt values according to those stored
	 * 		  in the repository
	 */
	void SettingsScreen::_resetDebtValue()
	{
		MAUtil::String* debtValueLabelText = new MAUtil::String("The current debt value: ");
		debtValueLabelText->append(MAUtil::doubleToString(_debtValue, 2).c_str(), MAUtil::doubleToString(_debtValue, 2).length());
		debtValueLabelText->append(" ", 1);
		debtValueLabelText->append(_coin.c_str(), _coin.length());

		_debtValueLabel->setText(*debtValueLabelText);
		delete debtValueLabelText;
	}

   /**
    * \brief This function is called when a height recalculation is required for the main layout.
    * @param widgetHeight the height of the newly added or removed widget
    * @param addition bool true if the widgetHeight value needs to be added, false otherwise
    */
	void SettingsScreen::_recalculateMainLayoutHeight(int widgetHeight, bool addition)
	{
		if(_IPhoneOS)
		{
			int height = _mainLayout->getHeight();
			if(addition) height += widgetHeight;
			else height -= widgetHeight;
			_mainLayout->setHeight(height);
		}
	}
}

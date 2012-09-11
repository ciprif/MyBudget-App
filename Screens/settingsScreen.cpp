/*
 * settingsScreen.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: Cipri
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

#include "settingsScreen.h"
#include "../Logical/settingsManager.h"

//global count variable

short clickCount;

namespace GUI
{
	SettingsScreen::SettingsScreen() : _debtValue(0.0), _isAllItems(true), _isFromDate(false), _isMontly(false)
	{
		_coin = COINS[0]; //Default value

		clickCount = 0;
		_setPlatform();
		_createUI();
	}

	SettingsScreen::~SettingsScreen()
	{
		_allItems->removeCheckBoxListener(this);
		_monthly->removeCheckBoxListener(this);
		_fromDate->removeCheckBoxListener(this);
		if(_isWP7) _numberPicker->removeNumberPickerListener(this);
		_datePicker->removeDatePickerListener(this);
	}

	void SettingsScreen::_setPlatform()
	{
		char buffer[BUFF_SIZE];
		maGetSystemProperty("mosync.device.OS", buffer, BUFF_SIZE);

		if(strcmp(buffer, "iOS") == 0 || strcmp(buffer, "Android") == 0)
		{
			_isWP7 = false;
		}
		else
		{
			_isWP7 = true;
		}
	}

	void SettingsScreen::_createUI()
	{
		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		SetSizeRelatedVariables();

		NativeUI::VerticalLayout* parent = new NativeUI::VerticalLayout();
		_mainLayout = new NativeUI::VerticalLayout();

		if(_isWP7)
		{
			_itemWidth = 9 * (screenWidth / 10) - screenWidth / 30;

			parent->setHeight(370);
			_mainLayout->setScrollable(true);
			parent->addChild(_mainLayout);
			this->setMainWidget(parent);
		}
		else
		{
			_mainLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
			_itemWidth = screenWidth * 0.95;
			this->setMainWidget(_mainLayout);
		}

		_coinSettingsLayout = _createCoinSettingsLayout();
		_mainLayout->addChild(_coinSettingsLayout);
		_mainLayout->addChild(_createDebtValueSettingsLayout());
		_transactionSettingsLayout = _createTransactionListSettingsLayout();
		_mainLayout->addChild(_transactionSettingsLayout);

		createOptionsMenu();

		addScreenListener(this);
	}

	NativeUI::VerticalLayout* SettingsScreen::_createCoinSettingsLayout()
	{
		NativeUI::VerticalLayout* parentLayout = new NativeUI::VerticalLayout();
		parentLayout->wrapContentVertically();
		_coinLabel = new NativeUI::Label();

		MAUtil::String coinLabelText = "The current coin: ";
		coinLabelText += _coin;

		_coinLabel->setText(coinLabelText);
		if(!_isWP7) _coinLabel->fillSpaceHorizontally();
		lprintfln("settings screen dialog font size %d", _dialogFontSize);
		_coinLabel->setFontSize(_dialogFontSize);
		_coinLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);

		NativeUI::VerticalLayout* toggleAndLabelParent = new NativeUI::VerticalLayout();

		if(!_isWP7) toggleAndLabelParent->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		_coinChangeToggle = new NativeUI::Button();
		_coinChangeToggle->setText("Change coin");
		_coinChangeToggle->setWidth(_itemWidth);
		_coinChangeToggle->addButtonListener(this);

		toggleAndLabelParent->addChild(_coinLabel);
		toggleAndLabelParent->addChild(_coinChangeToggle);

		_coinsList = new NativeUI::ListView();
		_coinsList->setWidth(_itemWidth);
		_coinsList->setHeight(_descriptionBoxHeight);

		for(int i = 0; i < NUMBER_OF_COINS; i++)
		{
			NativeUI::ListViewItem* item = new NativeUI::ListViewItem();
			item->setText(COINS[i]);
			_coinsList->addChild(item);
		}

		parentLayout->addChild(toggleAndLabelParent);

		return parentLayout;
	}

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
		if(!_isWP7) _debtValueLabel->fillSpaceHorizontally();
		_debtValueLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);

		_newDebtValueEditBox = new NativeUI::EditBox();
		_newDebtValueEditBox->setInputMode(NativeUI::EDIT_BOX_INPUT_MODE_NUMERIC);
		_newDebtValueEditBox->setWidth(_itemWidth);
		_newDebtValueEditBox->wrapContentVertically();
		_newDebtValueEditBox->setPlaceholder("New debt value");

		debtValueSettingsParent->addChild(_debtValueLabel);
		debtValueSettingsParent->addChild(_newDebtValueEditBox);

		if(!_isWP7) debtValueSettingsParent->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		return debtValueSettingsParent;
	}

	NativeUI::VerticalLayout* SettingsScreen::_createTransactionListSettingsLayout()
	{
		NativeUI::VerticalLayout* transactionListSettings = new NativeUI::VerticalLayout();
		transactionListSettings->wrapContentVertically();
		if(!_isWP7)
		{
			transactionListSettings->fillSpaceHorizontally();
			transactionListSettings->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		}

		_allItems = new NativeUI::CheckBox();
		_monthly = new NativeUI::CheckBox();
		_fromDate = new NativeUI::CheckBox();

		_allItems->setState(true);

		_allItems->addCheckBoxListener(this);
		_monthly->addCheckBoxListener(this);
		_fromDate->addCheckBoxListener(this);

		NativeUI::HorizontalLayout* checkBoxLabelLayoutAll = new NativeUI::HorizontalLayout();
		NativeUI::HorizontalLayout* checkBoxLabelLayoutMontly = new NativeUI::HorizontalLayout();
		NativeUI::HorizontalLayout* checkBoxLabelLayoutFromDate = new NativeUI::HorizontalLayout();

		checkBoxLabelLayoutAll->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		checkBoxLabelLayoutAll->fillSpaceHorizontally();
		checkBoxLabelLayoutMontly->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		checkBoxLabelLayoutMontly->fillSpaceHorizontally();
		checkBoxLabelLayoutFromDate->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		checkBoxLabelLayoutFromDate->fillSpaceHorizontally();

		NativeUI::Label* checkBoxLabelAllItems = new NativeUI::Label();
		checkBoxLabelAllItems->setText("All items");

		NativeUI::Label* checkBoxLabelMonthly = new NativeUI::Label();
		checkBoxLabelMonthly->setText("Monthly");

		NativeUI::Label* checkBoxLabelFromDate = new NativeUI::Label();
		checkBoxLabelFromDate->setText("Starting from date");

		checkBoxLabelAllItems->setFontSize(_dialogFontSize);
		checkBoxLabelMonthly->setFontSize(_dialogFontSize);
		checkBoxLabelFromDate->setFontSize(_dialogFontSize);

		checkBoxLabelLayoutAll->addChild(_allItems);
		checkBoxLabelLayoutAll->addChild(checkBoxLabelAllItems);

		checkBoxLabelLayoutMontly->addChild(_monthly);
		checkBoxLabelLayoutMontly->addChild(checkBoxLabelMonthly);

		checkBoxLabelLayoutFromDate->addChild(_fromDate);
		checkBoxLabelLayoutFromDate->addChild(checkBoxLabelFromDate);

		transactionListSettings->addChild(checkBoxLabelLayoutAll);
		transactionListSettings->addChild(checkBoxLabelLayoutMontly);
		transactionListSettings->addChild(checkBoxLabelLayoutFromDate);

		_datePicker = new NativeUI::DatePicker();
		if(_isWP7)
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
		}

		_datePicker->addDatePickerListener(this);

		_datePicker->setDayOfMonth(Logical::DEFAULT_DAY);
		_datePicker->setMonth(Logical::DEFAULT_MONTH);
		_datePicker->setYear(Logical::DEFAULT_YEAR);

		return transactionListSettings;
	}

	void SettingsScreen::_updateValues()
	{
		_coin = _observerReference->requestCoin();
		_isAllItems = _observerReference->requestIsShowAll();
		_isMontly = _observerReference->requestIsMonthly();
		_isFromDate = _observerReference->requestIsShowFromDate();
		_debtValue = _observerReference->requestDebtValue();

		MAUtil::String* coinLabelText = new MAUtil::String();

		*coinLabelText = "The current coin: ";

		coinLabelText->append(_coin.c_str(), _coin.length());
		_coinLabel->setText(*coinLabelText);
		delete coinLabelText;

		if(_isAllItems)
		{
			if(_isWP7) checkBoxStateChanged(_allItems, true);
			else
			{
				_allItems->setState(true);
				_monthly->setState(false);
				_fromDate->setState(false);
			}
		}
		else if(_isMontly)
		{
			if(_isWP7) checkBoxStateChanged(_monthly, true);
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
			if(_isWP7) checkBoxStateChanged(_fromDate, true);
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

		_dayValue = _observerReference->requestFromDate()._day;

		MAUtil::String* debtValueLabelText = new MAUtil::String("The current debt value: ");
		debtValueLabelText->append(MAUtil::doubleToString(_debtValue, 2).c_str(), MAUtil::doubleToString(_debtValue, 2).length());
		debtValueLabelText->append(" ", 1);
		debtValueLabelText->append(_coin.c_str(), _coin.length());

		_debtValueLabel->setText(*debtValueLabelText);
		delete debtValueLabelText;
	}

	void SettingsScreen::setObserver(Logical::Observer* obj)
	{
		_observerReference = obj;

		_updateValues();
	}

	void SettingsScreen::buttonClicked(NativeUI::Widget* b)
	{
		if(b == _coinChangeToggle)
		{
			if(clickCount % 2 == 0)
			{
				_coinSettingsLayout->addChild(_coinsList);
				if(!_isWP7) _coinSettingsLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
				_coinsList->addListViewListener(this);
				clickCount++;
				clickCount %= 2;
			}
			else
			{
				_coinSettingsLayout->removeChild(_coinsList);
				_coinsList->removeListViewListener(this);
				clickCount++;
				clickCount %= 2;
			}
		}
	}

	void SettingsScreen::listViewItemClicked(NativeUI::ListView* l, int index)
	{
		if(l == _coinsList)
		{
			_coin = COINS[index];

			MAUtil::String coinLabelText = "The current coin: ";
			coinLabelText += _coin;
			coinLabelText += " (not applied)";
			_coinLabel->setText(coinLabelText);

			_coinSettingsLayout->removeChild(_coinsList);
			_coinsList->removeListViewListener(this);

			clickCount++;
			clickCount %= 2;
		}
	}

	void SettingsScreen::checkBoxStateChanged(NativeUI::CheckBox* checkBox, bool state)
	{
		if(state == true)
		{
			//checkBox->setState(true);
			if(checkBox == _monthly)
			{
				if(_transactionSettingsLayout->countChildWidgets() == 4)
				{
					if(_transactionSettingsLayout->getChild(3) == _datePicker)
						_transactionSettingsLayout->removeChild(_datePicker);
				}

				if(_isWP7) _transactionSettingsLayout->addChild(_numberPicker);
				else _transactionSettingsLayout->addChild(_numberPickerReplace);

				if(_isAllItems) _allItems->setState(false);
				else if(_isFromDate) _fromDate->setState(false);

				_isMontly = true;
				_isAllItems = false;
				_isFromDate = false;
			}
			else if(checkBox == _allItems)
			{
				if(_transactionSettingsLayout->countChildWidgets() == 4)
				{
					_transactionSettingsLayout->removeChild(_transactionSettingsLayout->getChild(3));
				}

				if(_isFromDate) _fromDate->setState(false);
				else if(_isMontly) _monthly->setState(false);

				_isMontly = false;
				_isAllItems = true;
				_isFromDate = false;
			}
			else if(checkBox == _fromDate)
			{
				if(_transactionSettingsLayout->countChildWidgets() == 4)
				{
					if(_transactionSettingsLayout->getChild(3) == _numberPicker && _isWP7)
						_transactionSettingsLayout->removeChild(_numberPicker);
					else if(_transactionSettingsLayout->getChild(3) == _numberPickerReplace)
						_transactionSettingsLayout->removeChild(_numberPickerReplace);
				}

				_transactionSettingsLayout->addChild(_datePicker);

				if(_isAllItems) _allItems->setState(false);
				else if(_isMontly) _monthly->setState(false);

				_isMontly = false;
				_isAllItems = false;
				_isFromDate = true;
			}
		}
		else
		{
			if(_isAllItems && checkBox == _allItems) checkBox->setState(true);
			if(_isMontly && checkBox == _monthly) checkBox->setState(true);
			if(_isFromDate && checkBox == _fromDate) checkBox->setState(true);
		}
	}

	void SettingsScreen::numberPickerValueChanged(NativeUI::NumberPicker* picker, int value)
	{
		if(picker == _numberPicker)
		{
			_dayValue = value;
		}
	}

	void SettingsScreen::datePickerValueChanged(NativeUI::DatePicker* picker, const NativeUI::Date& selected)
	{
	}

	void SettingsScreen::createOptionsMenu()
	{
		_saveButtonIndex = addOptionsMenuItem("Save", MAW_OPTIONS_MENU_ICON_CONSTANT_SAVE, false);
		_restoreButtonIndex = addOptionsMenuItem("Restore", MAW_OPTIONS_MENU_ICON_CONSTANT_CLOSE_CLEAR_CANCEL, false);
	}

	void SettingsScreen::optionsMenuItemSelected(NativeUI::Screen*, int index)
	{
		if(index == _saveButtonIndex)
		{
			Model::DateStruct d;
			d._day = _datePicker->getDayOfMonth();
			d._mounth = _datePicker->getMonth();
			d._year = _datePicker->getYear();

			if(_isMontly)
			{
				if(_isWP7)
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

			MAUtil::String coinLabelText = "The current coin: ";
			coinLabelText += _coin;
			_coinLabel->setText(coinLabelText);

			if(_newDebtValueEditBox->getText().length() > 0) _debtValue = MAUtil::stringToDouble(_newDebtValueEditBox->getText().c_str());

			MAUtil::String* debtValueLabelText = new MAUtil::String("The current debt value: ");
			debtValueLabelText->append(MAUtil::doubleToString(_debtValue, 2).c_str(), MAUtil::doubleToString(_debtValue, 2).length());
			debtValueLabelText->append(" ", 1);
			debtValueLabelText->append(_coin.c_str(), _coin.length());

			_debtValueLabel->setText(*debtValueLabelText);

			delete debtValueLabelText;

			_newDebtValueEditBox->setText("");
			_newDebtValueEditBox->setPlaceholder("New debt value");

			_observerReference->requestSaveSettings(_isAllItems, _isMontly, _isFromDate, _debtValue, d, _coin);
		}
		else if(index == _restoreButtonIndex)
		{
			_updateValues(); //reset values with the saved ones observer
		}
	}
}

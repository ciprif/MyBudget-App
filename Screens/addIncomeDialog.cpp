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
 * \file addIncomeDialog.cpp
 * \author Ciprian Filipas
 * \date Jul 3, 2012
 */

#include <NativeUI/HorizontalLayout.h>
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/RelativeLayout.h>
#include <NativeUI/Label.h>
#include <NativeUI/CheckBox.h>
#include <NativeUI/EditBox.h>
#include <NativeUI/Button.h>
#include <NativeUI/Slider.h>
#include <NativeUI/ToggleButton.h>
#include <NativeUI/TimePicker.h>
#include <NativeUI/DatePicker.h>

#include "../Logical/observer.h"
#include "addIncomeDialog.h"
#include "homeScreen.h"
#include "listScreen.h"
#include "MAHeaders.h"
#include "../Model/ModelUtil.h"
#include "GUIUtil.h"

#include "../Logical/settingsManager.h"

namespace GUI
{
	/**
	 * \brief Constructor
	 */
	AddIncomeDialog::AddIncomeDialog() : _checkBoxVector(NULL)
	{
		_coin = GUI::DEFAULT_COIN;

		DeterminePlatform();
		SetSizeRelatedVariables();
		_createUI();
	}

	/**
	 * \brief Constructor
	 * @param coin const MAUtil::String& the new coin value
	 */
	AddIncomeDialog::AddIncomeDialog(const MAUtil::String& coin) : _checkBoxVector(NULL)
	{
		_coin = coin;

		DeterminePlatform();
		SetSizeRelatedVariables();
		_createUI();
	}

	/**
	 * \brief Destructor
	 */
	AddIncomeDialog::~AddIncomeDialog()
	{
		_addButton->removeButtonListener(this);
		_cancelButton->removeButtonListener(this);
		_descriptionToggleButton->removeToggleButtonListener(this);
		_transactionInformationToggleButton->removeToggleButtonListener(this);
		_amountEditBox->removeEditBoxListener(this);
		_descriptionEditBox->removeEditBoxListener(this);
		_transactionInformationEditBox->removeEditBoxListener(this);
		MAUtil::Vector<NativeUI::CheckBox*>::iterator it;
		for(it = _checkBoxVector->begin(); it != _checkBoxVector->end(); it++)
		{
			(*it)->removeCheckBoxListener(this);
		}
	}


	 /**
	 * This method is called if the touch-up event was inside the
	 * bounds of the button.
	 * Platform: iOS, Android, Windows Phone.
	 * @param button The button object that generated the event.
	 */
	void AddIncomeDialog::buttonClicked(Widget* button)
	{
		if(button == _addButton)
		{
			double value = (double)MAUtil::stringToDouble(_amountEditBox->getText());

			if(0 < value || _amountEditBox->getText().length() != 0)
			{
				this->hide();
				if(_WindowsPhone7)
				{
					if(_launcedFromHomeScreen) _homeScreenRef->createOptionsMenu();
					else _listScreenRef->createOptionsMenu();
				}

				NativeUI::Date d = _datePicker->getDate();

				Model::DateStruct date;
				date._year = d.year;
				date._mounth = d.month;
				date._day = d.day;

				Model::TimeStruct time;
				time._hour = _timePicker->getHour();
				time._minutes = _timePicker->getMinute();

				_observerReference->requestIncomeAddition(value, _typeValue, _descriptionEditBox->getText(), _transactionInformationEditBox->getText(), date, time);
			}
			else
			{
				maAlert("Alert!", "The value of the income must be higher than 0!", "OK", "", "");
			}
		}
		else if(button == _cancelButton)
		{
			this->hide();
			if(_WindowsPhone7)
			{
					if(_launcedFromHomeScreen) _homeScreenRef->createOptionsMenu();
					else _listScreenRef->createOptionsMenu();
			}
		}
	}

	/**
	 * This method is called when the state of the check box was changed
	 * by the user.
	 * @param checkBox The check box object that generated the event.
	 * @param state True if the check box is checked, false otherwise.
	 */
	void AddIncomeDialog::checkBoxStateChanged(NativeUI::CheckBox* checkBox, bool state)
	{
		if(state == true)
		{
			for(int i = 0; i < _checkBoxVector->size(); i++)
			{
				if((*_checkBoxVector)[i] != checkBox)
					(*_checkBoxVector)[i]->setState(false);
				else
				{
					_typeValue = Model::INCOME_TYPES_LIST[i];
				}
			}
		}
		else
		{
			bool checked = false;
			for(int i = 0; i < _checkBoxVector->size(); i++)
				if((*_checkBoxVector)[i]->isChecked())
				{
					checked = true;
					break;
				}
			if(!checked)
				checkBox->setState(true);
		}
	}

	/**
	 * \brief This function is used for setting the HomeScreen reference
	 * @param homeScreenRef HomeScreen* pointer to the HomeScreen screen
	 */
	void AddIncomeDialog::setHomeScreenRef(HomeScreen* parent)
	{
		_homeScreenRef = parent;
		//at this part the dialog is loaded
		_homeScreenRef->addIncomesDialogLoaded();
	}

	/**
	 * \brief This function is used for setting the ListScreen reference
	 * @param homeScreenRef ListScreen* pointer to the HomeScreen screen
	 */
	void AddIncomeDialog::setListScreenRef(ListScreen* listScreenRef)
	{
		_listScreenRef = listScreenRef;
	}

	/**
	 * \brief This function is called when the state of the toggle button was changed
	 * 	 	  by the user. Inherited from the NativeUI::ToggleButtonListener class
	 * @param toggleButton NativeUI::ToggleButton* pointer to the object that triggered the event
	 * @param state bool True if the toggle button is checked, false otherwise.
	 */
	void AddIncomeDialog::toggleButtonStateChanged(NativeUI::ToggleButton* toggleButton, bool state)
	{
		if(toggleButton == _descriptionToggleButton)
		{
			if(true == state)
			{
				_descriptionBoxParent->addChild(_descriptionEditBox);
				if(_IPhoneOS) _mainLayout->setHeight(_mainLayout->getHeight() + DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE);
			}
			else
			{
				_descriptionBoxParent->removeChild(_descriptionEditBox);
				_descriptionEditBox->setText("");
				if(_IPhoneOS) _mainLayout->setHeight(_mainLayout->getHeight() - DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE);
			}
		}
		else if(toggleButton == _transactionInformationToggleButton)
		{
			if(true == state)
			{
				_transactionInfoBoxParent->addChild(_transactionInformationEditBox);
				if(_IPhoneOS) _mainLayout->setHeight(_mainLayout->getHeight() + DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE);
			}
			else
			{
				_transactionInfoBoxParent->removeChild(_transactionInformationEditBox);
				_transactionInformationEditBox->setText("");
				if(_IPhoneOS) _mainLayout->setHeight(_mainLayout->getHeight() - DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE);
			}
		}
	}

	/**
	 * \brief This function is used for showing the UI. Inherited from the NativeUI::Dialog class
	 */
	void AddIncomeDialog::show()
	{
		MAUtil::Vector<NativeUI::CheckBox*>::iterator it;
		for(it = _checkBoxVector->begin(); it != _checkBoxVector->end(); it++)
		{
			NativeUI::CheckBox* cb = *it;
			cb->setState(false);
		}

		it = _checkBoxVector->begin();
		(*it)->setState(true);

		_typeValue = Model::INCOME_TYPES_LIST[0];

		_amountEditBox->setText("");
		_amountLabel->setText("Set the value of your income: ");

		_descriptionToggleButton->setCheckedState(false);
		_transactionInformationToggleButton->setCheckedState(false);

		_descriptionBoxParent->removeChild(_descriptionEditBox);
		_descriptionEditBox->setText("");
		_descriptionEditBox->addEditBoxListener(this);
		_transactionInfoBoxParent->removeChild(_transactionInformationEditBox);
		_transactionInformationEditBox->setText("");
		_transactionInformationEditBox->addEditBoxListener(this);

		NativeUI::Dialog::show();

		if(_WindowsPhone7)
		{
			if(_launcedFromHomeScreen) _homeScreenRef->removeOptionsMenu();
			else _listScreenRef->removeOptionsMenu();
		}
	}

	/**
	 * \brief This function is used for setting the observer reference
	 * @param obs Logical::Observer* pointer to the application wide observer
	 */
	void AddIncomeDialog::setObserver(Logical::Observer* obs)
	{
		_observerReference = obs;
	}

	/**
	 * \brief This function is used for setting the _coin value
	 * @param coin const MAUtil::String& the new coin value
	 */
	void AddIncomeDialog::setCoin(const MAUtil::String& coin)
	{
		_coin = coin;
	}

	/**
	 * \brief This function is used for setting the _launcedFromHomeScreen boolean
	 * @param value bool true if the this dialog was launched from the home screen, false otherwise
	 */
	void AddIncomeDialog::setLaunchedFromHomeScreen(bool value)
	{
		_launcedFromHomeScreen = value;
	}

	/**
	 * \brief This function handles the return press button event from the querty keyboard;
	 * 		  this function is inherited from the NativeUI::EditBoxListener class.
	 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
	 */
	void AddIncomeDialog::editBoxReturn(NativeUI::EditBox *editBox)
	{
		editBox->hideKeyboard();
	}

	/**
	 * \brief This function handles the edid did end event from the editBox;
	 * 		  this function is inherited from the NativeUI::EditBoxListener class.
	 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
	 */
	void AddIncomeDialog::editBoxEditingDidEnd(NativeUI::EditBox* editBox)
	{
		if(editBox == _amountEditBox)
		{
			MAUtil::String value = editBox->getText();
			double doubleValue = MAUtil::stringToDouble(value);

			if(doubleValue < 0.0000001)
			{
				if(_WindowsPhone7) maAlert("Alert!", "The value you are trying to insert is 0!", "OK", NULL, NULL);
				else maAlert("Alert!", "The value you are trying to insert is 0!", "OK", "", "");

				_amountEditBox->setText("");
			}
			else if(value.length() != 0)
			{
				char budgetString[BUFFER_SIZE];
				sprintf(budgetString, "Set the value of your income: %s %s", value.c_str(), _coin.c_str());

				_amountLabel->setText(budgetString);
			}
		}
	}

	/**
	 * \brief This function is used for triggering the UI creation
	 */
	void AddIncomeDialog::_createUI()
	{
		if (_IPhoneOS)
		{
			NativeUI::RelativeLayout* relativeLayout = new NativeUI::RelativeLayout();
			relativeLayout->setScrollable(true);

			_mainLayout = new NativeUI::VerticalLayout();
			_mainLayout->setHeight(DIALOG_HEIGHT_IOS + 30);

			_mainLayout->addChild(_createCheckBoxGroup(Model::INCOME_TYPES_LIST, Model::NO_OF_INCOMES));
			_mainLayout->addChild(_createAmountBars());

			_mainLayout->addChild(_createDatePicker());
			_mainLayout->addChild(_createTimePicker());

			_createDescriptionBox();
			_mainLayout->addChild(_descriptionBoxParent);

			_createTransactionInfoBox();
			_mainLayout->addChild(_transactionInfoBoxParent);
			_mainLayout->addChild(_createBottomSpacer());
			_mainLayout->addChild(_createBottomButtonBar());

			relativeLayout->addChild(_mainLayout);
			setMainWidget(relativeLayout);
		}
		else
		{
			NativeUI::VerticalLayout* parent = new NativeUI::VerticalLayout();
			if(_WindowsPhone7) parent->setHeight(DIALOG_HEIGHT);
			_mainLayout = new NativeUI::VerticalLayout();
			_mainLayout->setScrollable(true);

			_mainLayout->addChild(_createCheckBoxGroup(Model::INCOME_TYPES_LIST, Model::NO_OF_INCOMES));
			_mainLayout->addChild(_createAmountBars());

			_mainLayout->addChild(_createDatePicker());
			_mainLayout->addChild(_createTimePicker());

			_createDescriptionBox();
			_mainLayout->addChild(_descriptionBoxParent);

			_createTransactionInfoBox();
			_mainLayout->addChild(_transactionInfoBoxParent);
			_mainLayout->addChild(_createBottomSpacer());
			_mainLayout->addChild(_createBottomButtonBar());

			parent->addChild(_mainLayout);
			setMainWidget(parent);
		}
	}

	/**
	 * \brief This function is used for creating the checkbox group; note that this will behave like
	 * 		  a radio button control
	 * @param categories const MAUtil::String[] the array of categories
	 * @param length int the length of the category array
	 * @return NativeUI::VerticalLayout* the newly created layout
	 */
	NativeUI::VerticalLayout* AddIncomeDialog::_createCheckBoxGroup(const MAUtil::String categories[], int length)
	{
		if(NULL == _checkBoxVector)
			_checkBoxVector = new MAUtil::Vector<NativeUI::CheckBox*>();

		NativeUI::VerticalLayout* checkBoxGroupParentLayout = new NativeUI::VerticalLayout();
		checkBoxGroupParentLayout->wrapContentVertically();

		NativeUI::Label* categoryLabel = new NativeUI::Label();
		categoryLabel->setText("Choose a type:");


		categoryLabel->setFontSize(_dialogFontSize);
		NativeUI::HorizontalLayout* checkBoxLabelLayout;

		checkBoxGroupParentLayout->addChild(categoryLabel);
		for(int i = 0; i < length; i++)
		{
			checkBoxLabelLayout = new NativeUI::HorizontalLayout();
			checkBoxLabelLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
			checkBoxLabelLayout->fillSpaceHorizontally();
			checkBoxLabelLayout->wrapContentVertically();

			NativeUI::Label* checkBoxLabel = new NativeUI::Label();
			NativeUI::CheckBox* checkBox = new NativeUI::CheckBox();
			checkBox->addCheckBoxListener(this);

			checkBoxLabel->setText(categories[i]);
			checkBoxLabel->setFontSize(_dialogFontSize);

			checkBoxLabelLayout->addChild(checkBox);
			checkBoxLabelLayout->addChild(checkBoxLabel);

			checkBoxGroupParentLayout->addChild(checkBoxLabelLayout);

			_checkBoxVector->add(checkBox);
		}

		return checkBoxGroupParentLayout;
	}

	/**
	 * \brief This function is used for creating the buttons from the bottom of the dialog
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddIncomeDialog::_createBottomButtonBar()
	{
		NativeUI::HorizontalLayout* buttonGroup = new NativeUI::HorizontalLayout();
		buttonGroup->fillSpaceHorizontally();
		buttonGroup->wrapContentVertically();
		buttonGroup->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		_addButton = new NativeUI::Button();
		_addButton->setText("Save");
		_addButton->setWidth(_dialogButtonWidth);
		_addButton->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		_addButton->addButtonListener(this);

		_cancelButton = new NativeUI::Button();
		_cancelButton->setText("Cancel");
		_cancelButton->setWidth(_dialogButtonWidth);
		_cancelButton->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		_cancelButton->addButtonListener(this);

		buttonGroup->addChild(_addButton);
		buttonGroup->addChild(_cancelButton);

		return buttonGroup;
	}

	/**
	 * \brief This function is used for creating the amount bars (and related UI elements)
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddIncomeDialog::_createAmountBars()
	{
		NativeUI::HorizontalLayout* amountBar = new NativeUI::HorizontalLayout();
		amountBar->fillSpaceHorizontally();
		amountBar->wrapContentVertically();

		NativeUI::VerticalLayout* labelEditBoxParentLayout = new NativeUI::VerticalLayout();
		labelEditBoxParentLayout->fillSpaceHorizontally();
		labelEditBoxParentLayout->wrapContentVertically();

		_amountEditBox = new NativeUI::EditBox();
		_amountEditBox->addEditBoxListener(this);
		_amountEditBox->setInputMode(NativeUI::EDIT_BOX_INPUT_MODE_NUMERIC);

		_amountEditBox->fillSpaceHorizontally();
		_amountEditBox->setPlaceholder("Income value...");

		_amountLabel = new NativeUI::Label();
		_amountLabel->fillSpaceHorizontally();
		_amountLabel->setFontSize(_dialogFontSize);
		_amountLabel->setText("Set the value of your income: ");

		labelEditBoxParentLayout->addChild(_amountLabel);
		labelEditBoxParentLayout->addChild(_amountEditBox);

		amountBar->addChild(labelEditBoxParentLayout);

		return amountBar;
	}

	/**
	 * \brief This function is used for creating the spacer from the bottom of the dialog
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddIncomeDialog::_createBottomSpacer()
	{
		NativeUI::HorizontalLayout* line = new NativeUI::HorizontalLayout();
		line->fillSpaceHorizontally();
		line->setHeight(DIALOG_BOTTOM_LINE_HEIGHT);

		return line;
	}


	/**
	 * \brief This function is used for creating the datePicker and related controls
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddIncomeDialog::_createDatePicker()
	{
		NativeUI::HorizontalLayout* datePickerBar = new NativeUI::HorizontalLayout();
		datePickerBar->fillSpaceHorizontally();
		datePickerBar->wrapContentVertically();

		NativeUI::VerticalLayout* labelDPParentLayout = new NativeUI::VerticalLayout();
		labelDPParentLayout->fillSpaceHorizontally();
		labelDPParentLayout->wrapContentVertically();

		_datePicker = new NativeUI::DatePicker();
		_datePicker->fillSpaceHorizontally();
		_datePicker->setDayOfMonth(Logical::DEFAULT_DAY);
		_datePicker->setMonth(Logical::DEFAULT_MONTH);
		_datePicker->setYear(Logical::DEFAULT_YEAR);

		_datePicker->addDatePickerListener(this);

		NativeUI::Label* datePickerLabel = new NativeUI::Label();

		datePickerLabel->setFontSize(_dialogFontSize);
		datePickerLabel->setText("Chose a date:");

		labelDPParentLayout->addChild(datePickerLabel);
		labelDPParentLayout->addChild(_datePicker);

		datePickerBar->addChild(labelDPParentLayout);

		return datePickerBar;
	}

	/**
	 * \brief This function is used for creating the timePicker and related controls
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddIncomeDialog::_createTimePicker()
	{
		NativeUI::HorizontalLayout* timePickerBar = new NativeUI::HorizontalLayout();
		timePickerBar->fillSpaceHorizontally();
		timePickerBar->wrapContentVertically();

		NativeUI::VerticalLayout* labelTPParentLayout = new NativeUI::VerticalLayout();
		labelTPParentLayout->fillSpaceHorizontally();
		labelTPParentLayout->wrapContentVertically();

		_timePicker = new NativeUI::TimePicker();
		_timePicker->fillSpaceHorizontally();

		NativeUI::Label* timePickerLabel = new NativeUI::Label();

		timePickerLabel->setFontSize(_dialogFontSize);
		timePickerLabel->setText("Chose a time:");

		labelTPParentLayout->addChild(timePickerLabel);
		labelTPParentLayout->addChild(_timePicker);

		timePickerBar->addChild(labelTPParentLayout);

		return timePickerBar;
	}

	/**
	 * \brief This function is used for creating the transaction info box
	 */
	void AddIncomeDialog::_createTransactionInfoBox()
	{
		_transactionInfoBoxParent = new NativeUI::VerticalLayout();
		_transactionInfoBoxParent->wrapContentVertically();

		NativeUI::VerticalLayout* transactionToggleAndLabelParent = new NativeUI::VerticalLayout();
		transactionToggleAndLabelParent->wrapContentVertically();
		NativeUI::Label* transactionToggleLabel = new NativeUI::Label();

		_transactionInformationToggleButton = new NativeUI::ToggleButton();
		_transactionInformationToggleButton->fillSpaceHorizontally();
		_transactionInformationToggleButton->setCheckedState(false);

		_transactionInformationEditBox = new NativeUI::EditBox();
		_transactionInformationEditBox->setHeight(_descriptionBoxHeight);

		transactionToggleLabel->setText("Transaction info:");
		transactionToggleLabel->setFontSize(_dialogFontSize);
		transactionToggleAndLabelParent->addChild(transactionToggleLabel);
		transactionToggleAndLabelParent->addChild(_transactionInformationToggleButton);

		_transactionInformationEditBox->setEditMode(NativeUI::EDIT_BOX_MODE_TEXT);
		_transactionInformationEditBox->fillSpaceHorizontally();

		_transactionInformationToggleButton->addToggleButtonListener(this);

		_transactionInfoBoxParent->addChild(transactionToggleAndLabelParent);
	}

	/**
	 * \brief This function is used for creating the description
	 */
	void AddIncomeDialog::_createDescriptionBox() //this is void because _descriptionBoxParent needs to be accessed from another function call. A return for NativeUI::VerticalLayout* would be redundant
	{
		_descriptionBoxParent = new NativeUI::VerticalLayout();
		_descriptionBoxParent->wrapContentVertically();

		NativeUI::VerticalLayout* descritionToggleAndLabelParent = new NativeUI::VerticalLayout();
		descritionToggleAndLabelParent->wrapContentVertically();
		NativeUI::Label* descriptionToggleLabel = new NativeUI::Label();

		_descriptionToggleButton = new NativeUI::ToggleButton();
		_descriptionToggleButton->fillSpaceHorizontally();
		_descriptionToggleButton->setCheckedState(false);

		_descriptionEditBox = new NativeUI::EditBox();
		_descriptionEditBox->setHeight(_descriptionBoxHeight);

		descriptionToggleLabel->setText("Description:");
		descriptionToggleLabel->setFontSize(_dialogFontSize);
		descritionToggleAndLabelParent->addChild(descriptionToggleLabel);
		descritionToggleAndLabelParent->addChild(_descriptionToggleButton);

		_descriptionEditBox->fillSpaceHorizontally();
		_descriptionToggleButton->addToggleButtonListener(this);
		_descriptionBoxParent->addChild(descritionToggleAndLabelParent);
	}
}

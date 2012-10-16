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
 * \file addExpenseDialog.cpp
 * \author Ciprian Filipas
 * \date Jun 19, 2012
 */

#include <NativeUI/HorizontalLayout.h>
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/RelativeLayout.h>
#include <NativeUI/Label.h>
#include <NativeUI/CheckBox.h>
#include <NativeUI/EditBox.h>
#include <NativeUI/Button.h>
#include <NativeUI/ToggleButton.h>
#include <NativeUI/TimePicker.h>
#include <NativeUI/DatePicker.h>

#include "../Logical/observer.h"
#include "../Logical/settingsManager.h"
#include "GUIUtil.h"
#include "addExpenseDialog.h"
#include "homeScreen.h"
#include "listScreen.h"
#include "MAHeaders.h"

NativeUI::CheckBox* recursiveStateChangedCB;

namespace GUI
{
	/**
	 * \brief Constructor
	 */
	AddExpenseDialog::AddExpenseDialog()
	{
		_coin = DEFAULT_COIN;
		_availableBudget = MAX_VALUE;
		_acceptedDept = MAX_VALUE;
		checkBoxVector = NULL;
		DeterminePlatform();
		SetSizeRelatedVariables();
		_createUI();
	}

	/**
	 * \brief Constructor with 3 parameters
	 * @param availableBudget const Double& the availabale budget
	 * @param posibleDeptValue const Double& the posible debt value
	 * @param coin const MAUtil::String& the coin
	 */
	AddExpenseDialog::AddExpenseDialog(const double& availableBudget, const double& posibleDeptValue, const MAUtil::String& coin)
	{
		_coin = coin;
		_availableBudget = availableBudget;
		_acceptedDept = posibleDeptValue;
		checkBoxVector = NULL;
		DeterminePlatform();
		SetSizeRelatedVariables();
		_createUI();
	}

	/**
	 * \brief Destructor
	 */
	AddExpenseDialog::~AddExpenseDialog()
	{
		_addButton->removeButtonListener(this);
		_cancelButton->removeButtonListener(this);
		_descriptionToggleButton->removeToggleButtonListener(this);
		_descriptionEditBox->removeEditBoxListener(this);
		_amountEditBox->removeEditBoxListener(this);

		MAUtil::Vector<NativeUI::CheckBox*>::iterator it;
		for(it = checkBoxVector->begin(); it != checkBoxVector->end(); it++)
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
	void AddExpenseDialog::buttonClicked(Widget* button)
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

				_observerReference->requestExpenseAddition(value, _categoryValue, _descriptionEditBox->getText(), "", date, time);
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
			else if(_IPhoneOS)
			{
				_mainLayout->setHeight(_dialogHeightIOS);
			}
		}
		else
		{
			maAlert("Alert!", "Feature is not yet available. Please wait for a future update.", "OK", "", "");
		}
	}

	/**
	 * This method is called when the state of the check box was changed
	 * by the user.
	 * @param checkBox The check box object that generated the event.
	 * @param state True if the check box is checked, false otherwise.
	 */
	void AddExpenseDialog::checkBoxStateChanged(NativeUI::CheckBox* checkBox, bool state)
	{
		if(state == true)
		{
			for(int i = 0; i < checkBoxVector->size(); i++)
			{
				if((*checkBoxVector)[i] != checkBox)
					(*checkBoxVector)[i]->setState(false);
				else
				{
					_categoryValue = Model::CATEGORY_LIST[i];
				}
			}
		}
		else
		{
			bool checked = false;
			for(int i = 0; i < checkBoxVector->size(); i++)
				if((*checkBoxVector)[i]->isChecked())
				{
					checked = true;
					break;
				}
			if(!checked)
				checkBox->setState(true);
		}
	}

	/**
	 * This method is called when the state of the toggle button was changed
	 * by the user.
	 * @param toggleButton The toggle button object that generated the event.
	 * @param state True if the toggle button is checked, false otherwise.
	 */
	void AddExpenseDialog::toggleButtonStateChanged(NativeUI::ToggleButton* toggleButton, bool state)
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
	}

	/**
	 * \brief This function is used for setting the HomeScreen reference
	 * @param homeScreenRef HomeScreen* pointer to the HomeScreen screen
	 */
	void AddExpenseDialog::setHomeScreenRef(HomeScreen* homeScreenRef)
	{
		_homeScreenRef = homeScreenRef;
		//at this part the dialog is loaded
		_homeScreenRef->addExpensesDialogLoaded();
	}

	/**
	 * \brief This function is used for setting the ListScreen reference
	 * @param homeScreenRef ListScreen* pointer to the HomeScreen screen
	 */
	void AddExpenseDialog::setListScreenRef(ListScreen* listScreenRef)
	{
		_listScreenRef = listScreenRef;
	}

	/**
	 * \brief This function is used for showing the UI. Inherited from the NativeUI::Dialog class
	 */
	void AddExpenseDialog::show()
	{
		MAUtil::Vector<NativeUI::CheckBox*>::iterator it;
		for(it = checkBoxVector->begin(); it != checkBoxVector->end(); it++)
		{
			NativeUI::CheckBox* cb = *it;
			cb->setState(false);
		}

		it = checkBoxVector->begin();
		(*it)->setState(true);

		_categoryValue = Model::CATEGORY_LIST[0];

		_amountEditBox->setText("");
		_amountLabel->setText("Set the value of your expense: ");

		_descriptionToggleButton->setCheckedState(false);

		_descriptionBoxParent->removeChild(_descriptionEditBox);
		_descriptionEditBox->setText("");

		if(_WindowsPhone7)
		{
			if(_launcedFromHomeScreen) _homeScreenRef->removeOptionsMenu();
			else _listScreenRef->removeOptionsMenu();
		}

		NativeUI::Dialog::show();
	}

	/**
	 * \brief This function is used for setting the observer reference
	 * @param obs Logical::Observer* pointer to the application wide observer
	 */
	void AddExpenseDialog::setObserver(Logical::Observer* obs)
	{
		_observerReference = obs;
	}

	/**
	 * \brief This function is used for setting the _coin value
	 * @param coin const MAUtil::String& the new coin value
	 */
	void AddExpenseDialog::setCoin(const MAUtil::String& coin)
	{
		_coin = coin;
	}

	/**
	 * \brief This function is used for setting the available budget
	 * @param availableBudget const double& the new value
	 */
	void AddExpenseDialog::setAvailableBudget(const double& availableBudget)
	{
		_availableBudget = availableBudget;
	}

	/**
	 * \brief This function is used for setting the posible debt value
	 * @param value const double& the new value
	 */
	void AddExpenseDialog::setAcceptedDebtValue(const double& value)
	{
		_acceptedDept = value;
	}

	/**
	 * \brief This function is used for updating the maximum value of the placeholder of the amount editBox
	 */
	void AddExpenseDialog::updateAmountValue()
	{
		MAUtil::String placeholder = "Available amount: ";
		placeholder += MAUtil::doubleToString(_availableBudget + _acceptedDept);
		_amountEditBox->setPlaceholder(placeholder);
	}

	/**
	 * \brief This function is used for setting the _launcedFromHomeScreen boolean
	 * @param value bool true if the this dialog was launched from the home screen, false otherwise
	 */
	void AddExpenseDialog::setLaunchedFromHomeScreen(bool value)
	{
		_launcedFromHomeScreen = value;
	}

	/**
	 * \brief This function handles the return press button event from the querty keyboard;
	 * 		  this function is inherited from the NativeUI::EditBoxListener class.
	 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
	 */
	void AddExpenseDialog::editBoxReturn(NativeUI::EditBox *editBox)
	{
		editBox->hideKeyboard();
	}

	/**
	 * \brief This function handles the edid did end event from the editBox;
	 * 		  this function is inherited from the NativeUI::EditBoxListener class.
	 * @param editBox NativeUI::EditBox* pointer to the edit box that triggered the event
	 */
	void AddExpenseDialog::editBoxEditingDidEnd(NativeUI::EditBox* editBox)
	{
		if(editBox == _amountEditBox)
		{
			MAUtil::String value = editBox->getText();
			double doubleValue = MAUtil::stringToDouble(value);

			if((doubleValue - (_availableBudget + _acceptedDept)) > 0)
			{
				if(_WindowsPhone7) maAlert("Alert!", "The value you are trying to insert is higher then the available amount of currency!", "OK", NULL, NULL);
				else maAlert("Alert!", "The value you are trying to insert is higher then the available amount of currency!", "OK", "", "");

				_amountEditBox->setText("");
			}
			else if(doubleValue < 0.0000001)
			{
				if(_WindowsPhone7) maAlert("Alert!", "The value you are trying to insert is 0!", "OK", NULL, NULL);
				else maAlert("Alert!", "The value you are trying to insert is 0!", "OK", "", "");

				_amountEditBox->setText("");
			}
			else if(value.length() != 0)
			{
				char budgetString[BUFFER_SIZE];
				sprintf(budgetString, "Set the value of your expense: %s %s", value.c_str(), _coin.c_str());

				_amountLabel->setText(budgetString);
			}
		}
	}

	/**
	 * \brief This function is used for triggering the UI creation
	 */
	void AddExpenseDialog::_createUI()
	{
		NativeUI::VerticalLayout* parent = new NativeUI::VerticalLayout();
		_mainLayout = new NativeUI::VerticalLayout();

		if (_IPhoneOS)
		{
			MAExtent size = maGetScrSize();
			int screenWidth = EXTENT_X(size);
			int screenHeight = EXTENT_Y(size);
			_itemWidth = (int)(screenWidth * 0.90);
			this->setTitle("Add an expense");
			_mainLayout->setHeight(_dialogHeightIOS);

			NativeUI::RelativeLayout* relativeLayout = new NativeUI::RelativeLayout();
			relativeLayout->setScrollable(true);
			relativeLayout->addChild(_mainLayout);
			_mainLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
			_mainLayout->addChild(_createCheckBoxGroup());
			_mainLayout->addChild(_createAmountBar(_availableBudget + _acceptedDept));

			_mainLayout->addChild(_createDatePicker());
			_mainLayout->addChild(_createTimePicker());

			_createDescriptionBox();
			_mainLayout->addChild(_descriptionBoxParent);

			_mainLayout->addChild(_createBottomSpacer());
			_mainLayout->addChild(_createBottomButtonBar());

			setMainWidget(relativeLayout);
		}
		else
		{
			if(_WindowsPhone7) parent->setHeight(DIALOG_HEIGHT);

			_mainLayout->setScrollable(true);

			_mainLayout->addChild(_createCheckBoxGroup());
			_mainLayout->addChild(_createAmountBar((int)(_availableBudget + _acceptedDept)));

			_mainLayout->addChild(_createDatePicker());
			_mainLayout->addChild(_createTimePicker());

			_createDescriptionBox();
			_mainLayout->addChild(_descriptionBoxParent);

			_mainLayout->addChild(_createBottomSpacer());
			_mainLayout->addChild(_createBottomButtonBar());

			parent->addChild(_mainLayout);
			setMainWidget(parent);
		}
	}

	/**
	 * \brief This function is used for creating the checkbox group; note that this will behave like
	 * 		  a radio button control
	 */
	NativeUI::VerticalLayout* AddExpenseDialog::_createCheckBoxGroup()
	{
		if(NULL == checkBoxVector)
			checkBoxVector = new MAUtil::Vector<NativeUI::CheckBox*>();

		NativeUI::VerticalLayout* checkBoxGroupParentLayout = new NativeUI::VerticalLayout();
		checkBoxGroupParentLayout->wrapContentVertically();

		NativeUI::Label* categoryLabel = new NativeUI::Label();
		categoryLabel->setText("Chose a category:");

		if(_IPhoneOS)
		{
			categoryLabel->setHeight(_checkboxLayoutHeightIOS);
			checkBoxGroupParentLayout->setWidth(_itemWidth);
		}

		categoryLabel->setFontSize(_dialogFontSize);
		checkBoxGroupParentLayout->addChild(categoryLabel);
		NativeUI::HorizontalLayout* checkBoxLabelLayout;
		for(int i = 0; i < Model::NO_OF_CATEGORIES; i++)
		{
			checkBoxLabelLayout = new NativeUI::HorizontalLayout();
			checkBoxLabelLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
			checkBoxLabelLayout->fillSpaceHorizontally();
			checkBoxLabelLayout->wrapContentVertically();

			NativeUI::Label* checkBoxLabel = new NativeUI::Label();
			NativeUI::CheckBox* checkBox = new NativeUI::CheckBox();
			checkBox->addCheckBoxListener(this);
			checkBoxLabel->setText(Model::CATEGORY_LIST[i]);
			checkBoxLabel->setFontSize(_dialogFontSize);
			if(_IPhoneOS)
			{
				NativeUI::HorizontalLayout* auxLayout = new NativeUI::HorizontalLayout();
				auxLayout->fillSpaceHorizontally();
				auxLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_RIGHT);
				auxLayout->addChild(checkBox);
				auxLayout->setHeight(_checkboxLayoutHeightIOS);
				checkBoxLabelLayout->addChild(checkBoxLabel);
				checkBoxLabelLayout->addChild(auxLayout);
			}
			else
			{
				checkBoxLabelLayout->addChild(checkBox);
				checkBoxLabelLayout->addChild(checkBoxLabel);
			}

			checkBoxGroupParentLayout->addChild(checkBoxLabelLayout);
			checkBoxVector->add(checkBox);
		}

		return checkBoxGroupParentLayout;
	}

	/**
	 * \brief This function is used for creating the buttons from the bottom of the dialog
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddExpenseDialog::_createBottomButtonBar()
	{
		NativeUI::HorizontalLayout* buttonGroup = new NativeUI::HorizontalLayout();
		buttonGroup->fillSpaceHorizontally();
		buttonGroup->wrapContentVertically();
		buttonGroup->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		_addButton = new NativeUI::Button();
		_cancelButton = new NativeUI::Button();

		_addButton->setText("Save");
		_addButton->setWidth(_dialogButtonWidth);
		_addButton->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		_addButton->addButtonListener(this);
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
	 * @param maxVal const int& the maximal value for the slider
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddExpenseDialog::_createAmountBar(const double& maxVal)
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

		MAUtil::String placeholder = "Available amount: ";

		placeholder += MAUtil::doubleToString(maxVal);

		_amountEditBox->fillSpaceHorizontally();
		_amountEditBox->setPlaceholder(placeholder);

		_amountLabel = new NativeUI::Label();
		_amountLabel->fillSpaceHorizontally();
		_amountLabel->setFontSize(_dialogFontSize);
		_amountLabel->setText("Set the value of your expense: ");

		if(_IPhoneOS)
		{
			_amountLabel->setHeight(_checkboxLayoutHeightIOS);
			amountBar->setWidth(_itemWidth);
		}

		labelEditBoxParentLayout->addChild(_amountLabel);
		labelEditBoxParentLayout->addChild(_amountEditBox);

		amountBar->addChild(labelEditBoxParentLayout);

		return amountBar;
	}

	/**
	 * \brief This function is used for creating the spacer from the bottom of the dialog
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddExpenseDialog::_createBottomSpacer()
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
	NativeUI::HorizontalLayout* AddExpenseDialog::_createDatePicker()
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

		NativeUI::Label* datePickerLabel = new NativeUI::Label();
		datePickerLabel->setFontSize(_dialogFontSize);
		datePickerLabel->setText("Chose a date:");

		if(_IPhoneOS)
		{
			datePickerLabel->setHeight(_checkboxLayoutHeightIOS);
			datePickerLabel->setWidth(_itemWidth);
			labelDPParentLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		}

		labelDPParentLayout->addChild(datePickerLabel);
		labelDPParentLayout->addChild(_datePicker);

		datePickerBar->addChild(labelDPParentLayout);

		return datePickerBar;
	}

	/**
	 * \brief This function is used for creating the timePicker and related controls
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddExpenseDialog::_createTimePicker()
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

		if(_IPhoneOS)
		{
			timePickerLabel->setHeight(_checkboxLayoutHeightIOS);
			timePickerLabel->setWidth(_itemWidth);
			labelTPParentLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		}

		labelTPParentLayout->addChild(timePickerLabel);
		labelTPParentLayout->addChild(_timePicker);

		timePickerBar->addChild(labelTPParentLayout);

		return timePickerBar;
	}

	/**
	 * \brief This function is used for creating the description
	 */
	void AddExpenseDialog::_createDescriptionBox() //this is void because _descriptionBoxParent needs to be accessed from another function call. A return for NativeUI::VerticalLayout* would be redundant
	{
		_descriptionBoxParent = new NativeUI::VerticalLayout();
		_descriptionBoxParent->wrapContentVertically();

		NativeUI::Layout* descritionToggleAndLabelParent;

		if(_IPhoneOS)
		{
			descritionToggleAndLabelParent = new NativeUI::HorizontalLayout();
			descritionToggleAndLabelParent->setHeight(_checkboxLayoutHeightIOS);
		}
		else
		{
			descritionToggleAndLabelParent = new NativeUI::VerticalLayout();
			descritionToggleAndLabelParent->wrapContentVertically();
		}

		NativeUI::Label* descriptionToggleLabel = new NativeUI::Label();

		_descriptionToggleButton = new NativeUI::ToggleButton();
		_descriptionToggleButton->setCheckedState(false);

		_descriptionEditBox = new NativeUI::EditBox();

		descriptionToggleLabel->setText("Description:");
		descriptionToggleLabel->setFontSize(_dialogFontSize);

		if(_IPhoneOS)
		{
			NativeUI::HorizontalLayout* auxLayout = new NativeUI::HorizontalLayout();
			auxLayout->fillSpaceHorizontally();
			auxLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_RIGHT);
			auxLayout->setChildVerticalAlignment(MAW_ALIGNMENT_CENTER);
			auxLayout->addChild(_descriptionToggleButton);
			auxLayout->setHeight(_checkboxLayoutHeightIOS);
			descritionToggleAndLabelParent->addChild(descriptionToggleLabel);
			descritionToggleAndLabelParent->addChild(auxLayout);
			((NativeUI::HorizontalLayout*) descritionToggleAndLabelParent)->setChildVerticalAlignment(MAW_ALIGNMENT_CENTER);
			_descriptionBoxParent->setWidth(_itemWidth);
		}
		else
		{
			_descriptionToggleButton->fillSpaceHorizontally();
			descritionToggleAndLabelParent->addChild(descriptionToggleLabel);
			descritionToggleAndLabelParent->addChild(_descriptionToggleButton);
		}

		//_descriptionEditBox->setMaxLines(DESCRIPTION_EDIT_BOX_LINES);
		switch(_screenType)
		{
		case 0:
			_descriptionEditBox->setHeight(DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_SMALL);
			break;
		case 1:
			_descriptionEditBox->setHeight(DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_MEDIUM);
			break;
		default:
			_descriptionEditBox->setHeight(DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE);
			break;
		}

		_descriptionEditBox->fillSpaceHorizontally();
		_descriptionEditBox->addEditBoxListener(this);
		_descriptionToggleButton->addToggleButtonListener(this);
		_descriptionBoxParent->addChild(descritionToggleAndLabelParent);
	}
}

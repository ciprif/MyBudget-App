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
#include <NativeUI/ImageButton.h>
#include <NativeUI/Button.h>
#include <NativeUI/Slider.h>
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
		_imageAtachementToggleButton->removeToggleButtonListener(this);
		_amountSlider->removeSliderListener(this);
		_selectImageButton->removeButtonListener(this);
		_captureImageButton->removeButtonListener(this);

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
			double value = (double)_amountSlider->getValue();
			if(0 < value)
			{
				this->hide();
				if(_launcedFromHomeScreen) _homeScreenRef->createOptionsMenu();
				else _listScreenRef->createOptionsMenu();

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
			if(_launcedFromHomeScreen) _homeScreenRef->createOptionsMenu();
			else _listScreenRef->createOptionsMenu();
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
			}
			else
			{
				_descriptionBoxParent->removeChild(_descriptionEditBox);
				_descriptionEditBox->setText("");
			}
		}
		else if(toggleButton == _imageAtachementToggleButton)
		{
			if(true == state)
			{
				_imageBoxAndToggleLayout->addChild(_imageButtonsParentLayout);
			}
			else
			{
				_imageBoxAndToggleLayout->removeChild(_imageButtonsParentLayout);
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
	 * \brief This function is called when the state of the slider is changed by the user.
	 * 		  Inherited from the NativeUI::SliderListener class
	 * @param s NativeUI::Slider* pointer to the object that triggered the event
	 * @param sliderValue int the new value of the slider object
	 */
	void AddExpenseDialog::sliderValueChanged(NativeUI::Slider* s, int sliderValue)
	{
		if(_amountSlider == s)
		{
			char budgetString[BUFFER_SIZE];
			sprintf(budgetString, "The value of the expense: %d %s", sliderValue, _coin.c_str());

			_amountLabel->setText(budgetString);
		}
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

		_amountSlider->setValue(0);
		_descriptionToggleButton->setCheckedState(false);
		_imageAtachementToggleButton->setCheckedState(false);

		_imageBoxAndToggleLayout->removeChild(_imageButtonsParentLayout);
		_descriptionBoxParent->removeChild(_descriptionEditBox);
		_descriptionEditBox->setText("");

		if(_launcedFromHomeScreen) _homeScreenRef->removeOptionsMenu();
		else _listScreenRef->removeOptionsMenu();

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
	 * \brief This function is used for updating the maximum value of the amount slider
	 */
	void AddExpenseDialog::updateAmountSliderValue()
	{
		_amountSlider->setMaximumValue((int)(_availableBudget + _acceptedDept));
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
	 * \brief This function is used for triggering the UI creation
	 */
	void AddExpenseDialog::_createUI()
	{
		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		NativeUI::VerticalLayout* parent = new NativeUI::VerticalLayout();
		if(_WindowsPhone7) parent->setHeight(DIALOG_HEIGHT);
		_mainLayout = new NativeUI::VerticalLayout();
		_mainLayout->setScrollable(true);

		_mainLayout->addChild(_createCheckBoxGroup());
		_mainLayout->addChild(_createAmountBar((int)(_availableBudget + _acceptedDept)));

		_mainLayout->addChild(_createDatePicker());
		_mainLayout->addChild(_createTimePicker());

		_createDescriptionBox();
		_mainLayout->addChild(_descriptionBoxParent);

		_createImageBox();
		_mainLayout->addChild(_imageBoxAndToggleLayout);
		_mainLayout->addChild(_createBottomSpacer());
		_mainLayout->addChild(_createBottomButtonBar());

		parent->addChild(_mainLayout);
		setMainWidget(parent);
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

		NativeUI::Label* categoryLabel = new NativeUI::Label();
		categoryLabel->setText("Choose a category:");
		categoryLabel->setFontSize(_dialogFontSize);

		checkBoxGroupParentLayout->addChild(categoryLabel);
		NativeUI::HorizontalLayout* checkBoxLabelLayout;
		for(int i = 0; i < Model::NO_OF_CATEGORIES; i++)
		{
			checkBoxLabelLayout = new NativeUI::HorizontalLayout();
			checkBoxLabelLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
			checkBoxLabelLayout->fillSpaceHorizontally();

			NativeUI::Label* checkBoxLabel = new NativeUI::Label();
			NativeUI::CheckBox* checkBox = new NativeUI::CheckBox();
			checkBox->addCheckBoxListener(this);
			checkBoxLabel->setText(Model::CATEGORY_LIST[i]);
			checkBoxLabel->setFontSize(_dialogFontSize);
			checkBoxLabelLayout->addChild(checkBox);
			checkBoxLabelLayout->addChild(checkBoxLabel);
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
	 * \brief This function is used for creating the amount bars (sliders and related UI elements)
	 * @param maxVal const int& the maximal value for the slider
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddExpenseDialog::_createAmountBar(const int& maxVal)
	{
		NativeUI::HorizontalLayout* amountBar = new NativeUI::HorizontalLayout();
		amountBar->fillSpaceHorizontally();

		NativeUI::VerticalLayout* labelSliderParentLayout = new NativeUI::VerticalLayout();
		labelSliderParentLayout->fillSpaceHorizontally();
		labelSliderParentLayout->fillSpaceVertically();

		_amountSlider = new NativeUI::Slider();
		_amountSlider->addSliderListener(this);
		_amountSlider->setMaximumValue(maxVal);
		_amountSlider->setValue(0);
		_amountSlider->fillSpaceHorizontally();

		_amountLabel = new NativeUI::Label();
		_amountLabel->fillSpaceHorizontally();
		_amountLabel->setFontSize(_dialogFontSize);
		_amountLabel->setText("Set the value of your expense: ");

		labelSliderParentLayout->addChild(_amountLabel);
		labelSliderParentLayout->addChild(_amountSlider);

		amountBar->addChild(labelSliderParentLayout);

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

		NativeUI::VerticalLayout* labelDPParentLayout = new NativeUI::VerticalLayout();
		labelDPParentLayout->fillSpaceHorizontally();
		labelDPParentLayout->fillSpaceVertically();

		_datePicker = new NativeUI::DatePicker();
		_datePicker->fillSpaceHorizontally();

		_datePicker->setDayOfMonth(Logical::DEFAULT_DAY);
		_datePicker->setMonth(Logical::DEFAULT_MONTH);
		_datePicker->setYear(Logical::DEFAULT_YEAR);

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
	NativeUI::HorizontalLayout* AddExpenseDialog::_createTimePicker()
	{
		NativeUI::HorizontalLayout* timePickerBar = new NativeUI::HorizontalLayout();
		timePickerBar->fillSpaceHorizontally();

		NativeUI::VerticalLayout* labelTPParentLayout = new NativeUI::VerticalLayout();
		labelTPParentLayout->fillSpaceHorizontally();
		labelTPParentLayout->fillSpaceVertically();

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
	 * \brief This function is used for creating the image related box (capture and folder buttons)
	 */
	void AddExpenseDialog::_createImageBox()
	{
		NativeUI::VerticalLayout* imageToggleAndLabelParent = new NativeUI::VerticalLayout();
		NativeUI::Label* imageToggleLabel = new NativeUI::Label();

		_imageBoxAndToggleLayout = new NativeUI::VerticalLayout();
		_imageButtonsParentLayout = new NativeUI::HorizontalLayout();
		_imageAtachementToggleButton = new NativeUI::ToggleButton();

		_imageAtachementToggleButton->fillSpaceHorizontally();
		_imageAtachementToggleButton->setCheckedState(false);
		_imageAtachementToggleButton->addToggleButtonListener(this);

		_captureImageButton = new NativeUI::ImageButton();
		_selectImageButton = new NativeUI::ImageButton();

		_selectImageButton->addButtonListener(this);
		_captureImageButton->addButtonListener(this);

		imageToggleLabel->setText("Atach an image:");
		imageToggleLabel->setFontSize(_dialogFontSize);

		imageToggleAndLabelParent->addChild(imageToggleLabel);
		imageToggleAndLabelParent->addChild(_imageAtachementToggleButton);

		NativeUI::RelativeLayout* captureButtonWrapper = new NativeUI::RelativeLayout();
		NativeUI::RelativeLayout* selectButtonWrapper = new NativeUI::RelativeLayout();

		captureButtonWrapper->setSize(_imageButtonWidth, (int)(_imageButtonHeight + 0.2 * _imageButtonHeight));
		selectButtonWrapper->setSize(_imageButtonWidth, (int)(_imageButtonHeight + 0.2 * _imageButtonHeight));

		NativeUI::Label* captureLabel = new NativeUI::Label();
		NativeUI::Label* selectLabel = new NativeUI::Label();

		captureLabel->setText("Capture");
		captureLabel->setTopPosition(80);
		captureLabel->setWidth(_imageButtonWidth);
		captureLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		selectLabel->setText("Select");
		selectLabel->setTopPosition(80);
		selectLabel->setWidth(_imageButtonWidth);
		selectLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		_captureImageButton->setSize(_imageButtonWidth, _imageButtonHeight);
		_captureImageButton->setTopPosition(0);
		_selectImageButton->setSize(_imageButtonWidth, _imageButtonHeight);
		_selectImageButton->setTopPosition(0);

		switch (GUI::_screenType) {
			case 0:
				_captureImageButton->setBackgroundImage(RES_BUTTON_CAMERA_SMALL);
				_selectImageButton->setBackgroundImage(RES_BUTTON_FOLDER_SMALL);
				break;
			case 1:
				_captureImageButton->setBackgroundImage(RES_BUTTON_CAMERA_MEDIUM);
				_selectImageButton->setBackgroundImage(RES_BUTTON_FOLDER_MEDIUM);
				break;
			case 2:
				_captureImageButton->setBackgroundImage(RES_BUTTON_CAMERA_LARGE);
				_selectImageButton->setBackgroundImage(RES_BUTTON_FOLDER_LARGE);
				break;
		}

		captureButtonWrapper->addChild(_captureImageButton);
		captureButtonWrapper->addChild(captureLabel);
		selectButtonWrapper->addChild(_selectImageButton);
		selectButtonWrapper->addChild(selectLabel);

		NativeUI::HorizontalLayout* spacer = new NativeUI::HorizontalLayout();
		spacer->setWidth(_imageButtonWidth);

		_imageButtonsParentLayout->addChild(captureButtonWrapper);
		_imageButtonsParentLayout->addChild(spacer);
		_imageButtonsParentLayout->addChild(selectButtonWrapper);
		_imageButtonsParentLayout->fillSpaceVertically();
		_imageButtonsParentLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);

		_imageBoxAndToggleLayout->addChild(imageToggleAndLabelParent);
	}

	/**
	 * \brief This function is used for creating the description
	 */
	void AddExpenseDialog::_createDescriptionBox() //this is void because _descriptionBoxParent needs to be accessed from another function call. A return for NativeUI::VerticalLayout* would be redundant
	{
		_descriptionBoxParent = new NativeUI::VerticalLayout();

		NativeUI::VerticalLayout* descritionToggleAndLabelParent = new NativeUI::VerticalLayout();
		NativeUI::Label* descriptionToggleLabel = new NativeUI::Label();

		_descriptionToggleButton = new NativeUI::ToggleButton();
		_descriptionToggleButton->fillSpaceHorizontally();
		_descriptionToggleButton->setCheckedState(false);

		_descriptionEditBox = new NativeUI::EditBox();

		descriptionToggleLabel->setText("Description:");
		descriptionToggleLabel->setFontSize(_dialogFontSize);

		descritionToggleAndLabelParent->addChild(descriptionToggleLabel);
		descritionToggleAndLabelParent->addChild(_descriptionToggleButton);

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
		_descriptionToggleButton->addToggleButtonListener(this);
		_descriptionBoxParent->addChild(descritionToggleAndLabelParent);
	}
}

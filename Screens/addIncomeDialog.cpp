/*
 * addIncomeDialog.cpp
 *
 *  Created on: Jul 3, 2012
 *      Author: Cipri
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
#include "../Model/util.h"

#include "../Logical/settingsManager.h"

namespace GUI
{
	/**
	 * \brief Constructor
	 */
	AddIncomeDialog::AddIncomeDialog() : _checkBoxVector(NULL)
	{
		_coin = GUI::DEFAULT_COIN;

		_setPlatform();
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

		_setPlatform();
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
		_amountSliderThousands->removeSliderListener(this);
		_amountSliderUnits->removeSliderListener(this);
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
			//@todo change the path from hardcoded version to actual value
			double value = _amountSliderThousands->getValue() * 1000 + _amountSliderUnits->getValue()*9;

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

				_observerReference->requestIncomeAddition(value, _typeValue, _descriptionEditBox->getText(), _transactionInformationEditBox->getText(), date, time);
			}
			else
			{
				maAlert("Alert!", "The value of the income must be higher than 0!", NULL, NULL, NULL);
			}
		}
		else if(button == _cancelButton)
		{
			this->hide();
			if(_launcedFromHomeScreen) _homeScreenRef->createOptionsMenu();
			else _listScreenRef->createOptionsMenu();
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
			}
			else
			{
				_descriptionBoxParent->removeChild(_descriptionEditBox);
				_descriptionEditBox->setText("");
			}
		}
		else if(toggleButton == _transactionInformationToggleButton)
		{
			if(true == state)
			{
				_transactionInfoBoxParent->addChild(_transactionInformationEditBox);
			}
			else
			{
				_transactionInfoBoxParent->removeChild(_transactionInformationEditBox);
				_transactionInformationEditBox->setText("");
			}
		}
	}

	/**
	 * \brief This function is called when the state of the slider is changed by the user.
	 * 		  Inherited from the NativeUI::SliderListener class
	 * @param s NativeUI::Slider* pointer to the object that triggered the event
	 * @param sliderValue int the new value of the slider object
	 */
	void AddIncomeDialog::sliderValueChanged(NativeUI::Slider* s, int sliderValue)
	{
		if(_amountSliderThousands == s)
		{
			char budgetString[BUFFER_SIZE];
			sprintf(budgetString, "The value of the income: %d %s", sliderValue*1000 + _amountSliderUnits->getValue()*9, _coin.c_str());

			_amountLabel->setText(budgetString);
		}
		else if(_amountSliderUnits == s)
		{
			char budgetString[BUFFER_SIZE];
			sprintf(budgetString, "The value of the income: %d %s", _amountSliderThousands->getValue()*1000 + sliderValue*9, _coin.c_str());

			_amountLabel->setText(budgetString);
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

		_amountSliderThousands->setValue(0);
		_amountSliderUnits->setValue(0);

		_descriptionToggleButton->setCheckedState(false);
		_transactionInformationToggleButton->setCheckedState(false);

		_descriptionBoxParent->removeChild(_descriptionEditBox);
		_descriptionEditBox->setText("");
		_descriptionEditBox->addEditBoxListener(this);
		_transactionInfoBoxParent->removeChild(_transactionInformationEditBox);
		_transactionInformationEditBox->setText("");
		_transactionInformationEditBox->addEditBoxListener(this);

		NativeUI::Dialog::show();

		if(_launcedFromHomeScreen) _homeScreenRef->removeOptionsMenu();
		else _listScreenRef->removeOptionsMenu();
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
	 * \brief This function is used for triggering the UI creation
	 */
	void AddIncomeDialog::_createUI()
	{
		NativeUI::VerticalLayout* parent = new NativeUI::VerticalLayout();
		if(_isWP7) parent->setHeight(DIALOG_HEIGHT);
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
	 * \brief This function is used for creating the amount bars (sliders and related UI elements)
	 * @return NativeUI::HorizontalLayout* the newly created layout
	 */
	NativeUI::HorizontalLayout* AddIncomeDialog::_createAmountBars()
	{
		NativeUI::HorizontalLayout* amountBar = new NativeUI::HorizontalLayout();
		amountBar->fillSpaceHorizontally();
		NativeUI::VerticalLayout* labelSliderParentLayout = new NativeUI::VerticalLayout();

		NativeUI::VerticalLayout* labelSliderParentLayoutThousands = new NativeUI::VerticalLayout();
		labelSliderParentLayoutThousands->fillSpaceHorizontally();
		labelSliderParentLayoutThousands->fillSpaceVertically();
		NativeUI::VerticalLayout* labelSliderParentLayoutUnits = new NativeUI::VerticalLayout();
		labelSliderParentLayoutUnits->fillSpaceHorizontally();
		labelSliderParentLayoutUnits->fillSpaceVertically();

		NativeUI::Label* _amountThousandsLabel = new NativeUI::Label();
		NativeUI::Label* _amountUnitsLabel = new NativeUI::Label();
		_amountLabel = new NativeUI::Label();

		_amountLabel->setFontSize(_dialogFontSize);
		_amountThousandsLabel->setText("Thousands");
		_amountThousandsLabel->setFontSize(_dialogSmallFontSize);
		_amountUnitsLabel->setFontSize(_dialogSmallFontSize);
		_amountUnitsLabel->setText("Units");
		_amountThousandsLabel->fillSpaceHorizontally();
		_amountUnitsLabel->fillSpaceHorizontally();

		_amountSliderThousands = new NativeUI::Slider();
		_amountSliderThousands->addSliderListener(this);
		_amountSliderThousands->setMaximumValue(30);
		_amountSliderThousands->setValue(0);
		_amountSliderThousands->fillSpaceHorizontally();

		_amountSliderUnits = new NativeUI::Slider();
		_amountSliderUnits->addSliderListener(this);
		_amountSliderUnits->setMaximumValue(111);
		_amountSliderUnits->setValue(0);
		_amountSliderUnits->fillSpaceHorizontally();

		_amountLabel->fillSpaceHorizontally();

		_amountLabel->setText("Set the value of your income: ");

		labelSliderParentLayout->addChild(_amountLabel);
		labelSliderParentLayout->addChild(_amountThousandsLabel);
		labelSliderParentLayout->addChild(_amountSliderThousands);
		labelSliderParentLayout->addChild(_amountUnitsLabel);
		labelSliderParentLayout->addChild(_amountSliderUnits);

		amountBar->addChild(labelSliderParentLayout);

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

		NativeUI::VerticalLayout* labelDPParentLayout = new NativeUI::VerticalLayout();
		labelDPParentLayout->fillSpaceHorizontally();
		labelDPParentLayout->fillSpaceVertically();

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
	 * \brief This function is used for creating the transaction info box
	 */
	void AddIncomeDialog::_createTransactionInfoBox()
	{
		_transactionInfoBoxParent = new NativeUI::VerticalLayout();

		NativeUI::VerticalLayout* transactionToggleAndLabelParent = new NativeUI::VerticalLayout();
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

		NativeUI::VerticalLayout* descritionToggleAndLabelParent = new NativeUI::VerticalLayout();
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

	/**
	 * \brief This function is used for setting the _isWP7 bool value
	 */
	void AddIncomeDialog::_setPlatform()
	{
		char buffer[BUFF_SIZE];
		maGetSystemProperty("mosync.device.OS", buffer, BUFF_SIZE);

		if(strcmp(buffer, "iPhone OS") == 0 || strcmp(buffer, "Android") == 0)
		{
			_isWP7 = false;
		}
		else
		{
			_isWP7 = true;
		}
	}
}

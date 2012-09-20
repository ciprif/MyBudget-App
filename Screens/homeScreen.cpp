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
 * \file homeScreen.cpp
 * \author Ciprian Filipas
 * \date Jun 12, 2012
 */

#include <NativeUI/Label.h>
#include <NativeUI/HorizontalLayout.h>
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/Dialog.h>
#include <NativeUI/PanoramaView.h>
#include <NativeUI/ActivityIndicator.h>
#include <NativeUI/RelativeLayout.h>
#include <NativeUI/Button.h>

#include "addExpenseDialog.h"
#include "addIncomeDialog.h"
#include "../Logical/observer.h"
#include "homeScreen.h"
#include "GUIUtil.h"

#include "MAHeaders.h"

namespace GUI
{
	/**
	 * \brief Constructor
	 */
	HomeScreen::HomeScreen()
	{
		_budgetConsumedValue = 0;
		_budgetTotalValue = 0;
		_coin = "EUR";
		_debtBudget = 0;

		_categoryGraphicsMap = new MAUtil::Map<const MAUtil::String, NativeUI::HorizontalLayout*>();
		_valueMap = new MAUtil::Map<const MAUtil::String, double>();
		for(int i = 0; i < Model::NO_OF_CATEGORIES; i++)
		{
			_valueMap->insert(Model::CATEGORY_LIST[i], 0);
		}

		addScreenListener(this);

		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		DeterminePlatform();
		SetSizeRelatedVariables();
		_createUI(screenHeight, screenWidth);
	}

	/**
	 * \brief Destructor
	 */
	HomeScreen::~HomeScreen()
	{
		delete _valueMap;
		delete _categoryGraphicsMap;
	}

	/**
	 * This method is called when an options menu item is selected.
	 * @param index The index on which the item is placed into the
	 * OptionsMenu.
	 * @param screen The screen that generated the event.
	 */
	void HomeScreen::optionsMenuItemSelected(NativeUI::Screen* screen, int index)
	{
		if(screen == this)
		{
			if(_addExpenseIndex == index) //add expense screen
			{
				_createAddExpenseDialog();
			}
			else if(_addIncomeIndex == index) //settings screen
			{
				_createAddIncomeDialog();
			}
		}
	}

	/**
	 * \brief This function creates the options menu
	 */
	void HomeScreen::createOptionsMenu()
	{
		if(!_IPhoneOS)
		{
			if(_WindowsPhone7)
			{
				_addExpenseIndex = addOptionsMenuItem("Expense", "addIncome.png", true);
				_addIncomeIndex = addOptionsMenuItem("Income", MAW_OPTIONS_MENU_ICON_CONSTANT_ADD, false);
			}
			else
			{
				_addExpenseIndex = addOptionsMenuItem("Add expense");
				_addIncomeIndex = addOptionsMenuItem("Add income");
			}
		}
		else
		{
			if(NULL == _optionsButton)
			{
				_optionsButton = new NativeUI::Button();
				_optionsButton->setBackgroundColor(255, 0, 0);
				_optionsButton->fillSpaceHorizontally();
				_optionsButton->setText("Options");
				_optionsButton->addButtonListener(this);
				_mainLayout->addChild(_optionsButton);
			}
		}
	}


	void HomeScreen::buttonClicked(NativeUI::Widget* button)
	{
		if(button == _optionsButton)
		{
			MAUtil::WString boxTitle(L"Options");
			MAUtil::WString destructiveButton(L"");
			MAUtil::WString cancelButton(L"Cancel");
			int count = sizeof(int);
			for(int i = 0; i < HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH; i++)
			{
				count += (HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[i].length() * sizeof(wchar)) + sizeof(wchar);
			}

			char* buffer = new char[count];
			*(int*) buffer = HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH;

			wchar_t* dest = (wchar_t*)(buffer + sizeof(int));

			for(int i = 0; i < HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH; i++)
			{
				const wchar_t* array = HOME_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[i].pointer();
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
	 * \brief This function is used for handling the custom event triggered by the alert box
	 * @param event const MAEvent& the event type
	 */
	void HomeScreen::customEvent(const MAEvent& event)
	{
		lprintfln("HomeScreen");
		if(event.type == EVENT_TYPE_OPTIONS_BOX_BUTTON_CLICKED)
		{
			if(0 == event.optionsBoxButtonIndex) //add income
			{
				_createAddIncomeDialog();
			}
			else if(1 == event.optionsBoxButtonIndex) //add expense
			{
				_createAddExpenseDialog();
			}
		}
	}

	/**
	 * \brief This function sets the observer for this screen
	 * @param observer the observer for this screen
	 */
	void HomeScreen::setObserver(Logical::Observer* observer)
	{
		_observerReference = observer;
		updateValues();
	}

	/**
	 * \brief This function updates the values for the graphics
	 * @param value the value
	 * @param isExpense true if the transaction is an expense -> the value will be withdrawn
	 * @param category != "" if the transaction is an expense, will keep the category
	 */
	void HomeScreen::updateBudgetValues(const double& value, bool isExpense, const MAUtil::String& category)
	{
		if(isExpense)
		{
			(*_valueMap)[category] += value;
			_budgetConsumedValue += value;
			for(int i = 0; i < Model::NO_OF_CATEGORIES; i++)
				_updateConsumeBar((*_categoryGraphicsMap)[Model::CATEGORY_LIST[i]], (*_valueMap)[Model::CATEGORY_LIST[i]]);
			updateSimpleGraphic();
		}
		else
		{
			_budgetTotalValue += value;
			updateSimpleGraphic();
		}
	}

	/**
	 * \brief This function updates the total budget value
	 * @param const double& the new value
	 */
	void HomeScreen::updateTotalBudget(const double& value)
	{
		_budgetTotalValue = value;
	}

	/**
	 * \brief This function updates the consumed budget value
	 * @param const double& the new value
	 */
	void HomeScreen::updateConsumedBudget(const double& value)
	{
		_budgetConsumedValue = value;
	}

	/**
	 * \brief This function updates the debt budget value
	 * @param const double& the new value
	 */
	void HomeScreen::updateDebtBudget(const double& value)
	{
		_debtBudget = value;
	}

	/**
	 * \brief This function is used fot updating the UI of the simple graphic
	 */
	void HomeScreen::updateSimpleGraphic()
	{
		if((_budgetTotalValue - _budgetConsumedValue) >= 0)
		{
			double valueWidth = 0;

			if(0 < _budgetTotalValue) valueWidth = (_budgetConsumedValue / _budgetTotalValue) * _parentLayoutWidth;

			if((int)(_budgetTotalValue - _budgetConsumedValue) == 0)
			{
				_budgetSimpleGraphicConsumedBudgetLayout->setWidth(1);
			}
			else
			{
				if(0 == valueWidth) valueWidth = 1;
				_budgetSimpleGraphicConsumedBudgetLayout->setWidth((int)valueWidth);
			}

			char budgetString[BUFFER_SIZE];
			sprintf(budgetString, "Consumed %.2f / %.2f %s", _budgetConsumedValue, _budgetTotalValue, _coin.c_str());

			_budgetLabel->setText(budgetString);
			_budgetSimpleGraphicConsumedBudgetLayout->setBackgroundColor(YELLOW);
			_budgetSimpleGraphicTotalBudgetLayout->setBackgroundColor(GREEN);
		}
		else
		{
			double valueWidth = 0;
			if(0 != _budgetTotalValue) valueWidth = ((_budgetConsumedValue / _budgetTotalValue) * _parentLayoutWidth) - _parentLayoutWidth;
			_budgetSimpleGraphicConsumedBudgetLayout->setWidth((int)valueWidth);
			_budgetSimpleGraphicConsumedBudgetLayout->setBackgroundColor(RED);
			_budgetSimpleGraphicTotalBudgetLayout->setBackgroundColor(YELLOW);

			char budgetString[BUFFER_SIZE];
			sprintf(budgetString, "Consumed %.2f / %.2f %s", _budgetConsumedValue, _budgetTotalValue, _coin.c_str());

			_budgetLabel->setText(budgetString);
		}
	}

	/**
	 * \brief This function is used for setting the addExpenses dialog reference
	 * @param obj NativeUI::Dialog* the pointer to the application wide addExpenses dialog
	 */
	void HomeScreen::setAddExpensesDialogReference(NativeUI::Dialog* obj)
	{
		_addExpensesDialog = (AddExpenseDialog*)obj;
		_addExpensesDialog->setHomeScreenRef(this);
	}

	/**
	 * \brief This function is used for setting the addIncomes dialog reference
	 * @param obj NativeUI::Dialog* the pointer to the application wide addIncomes dialog
	 */
	void HomeScreen::setAddIncomesDialogReference(NativeUI::Dialog* obj)
	{
		_addIncomeDialog = (AddIncomeDialog*)obj;
		_addIncomeDialog->setHomeScreenRef(this);
	}

	/**
	 * \brief This function notifies the homeScreen that the addExpensesDialog was successfully loaded
	 * 		  and it stops the activity indicator
	 */
	void HomeScreen::addExpensesDialogLoaded()
	{
		_expensesDialogLoaded = true;
		if(_incomesDialogLoaded)
			_removeActivityIndicator();
	}

	/**
	 * \brief This function notifies the homeScreen that the addIncomesDialog was successfully loaded
	 * 		  and it stopes the activity indicator
	 */
	void HomeScreen::addIncomesDialogLoaded()
	{
		_incomesDialogLoaded = true;
		if(_expensesDialogLoaded)
			_removeActivityIndicator();
	}

	/**
	 * \brief This function is used to set the new _coin value; it also updates the UI
	 * @param const MAUtil::String& the new coin value
	 */
	void HomeScreen::setCoin(const MAUtil::String& coin)
	{
		_coin = coin;

		char budgetString[BUFFER_SIZE];
		sprintf(budgetString, "Consumed %.2f / %.2f %s", _budgetConsumedValue, _budgetTotalValue, _coin.c_str());

		_budgetLabel->setText(budgetString);
	}

	/**
	 * \brief This function is used for synchronizing the budget values
	 */
	void HomeScreen::updateValues()
	{
		_budgetTotalValue =  _observerReference->requestTotalBudget();
		_budgetConsumedValue = _observerReference->requestConsumedBudget();
		_debtBudget = _observerReference->requestDebtValue();

		_addExpensesDialog->setAcceptedDebtValue(_debtBudget);

		for(int i = 0; i < Model::NO_OF_CATEGORIES; i++)
		{
			(*_valueMap)[Model::CATEGORY_LIST[i]] = _observerReference->requestCategoryAmount(Model::CATEGORY_LIST[i]);
		}
		updateBudgetValues(0, true, Model::CATEGORY_LIST[0]);
	}

	/**
	 * \brief creates the main layout and sets some enviroment variables. This is the point where
	 * 		  all the UI is initialized;
	 * @param screenHeight the screen height.
	 * @param screenWidth the screen width.
	 */
	void HomeScreen::_createUI(const int& screenHeight, const int& screenWidth)
	{
		//create the activity indicator
		_activityIndicator = new NativeUI::ActivityIndicator();

		//create the main layout
		_mainLayout = new NativeUI::VerticalLayout();

		if(_WindowsPhone7)
		{
			_parentLayoutWidth = 8 * (screenWidth / 10);
			_activityIndicator->fillSpaceHorizontally();

			_mainLayout->addChild(_activityIndicator);
			this->setMainWidget(_mainLayout);
		}
		else
		{
			NativeUI::RelativeLayout* activityIndicatorLayoutSupport =
					new NativeUI::RelativeLayout();

			_activityIndicatorLayout = new NativeUI::VerticalLayout();

			_activityIndicatorLayout->fillSpaceHorizontally();
			_activityIndicatorLayout->fillSpaceVertically();
			_activityIndicatorLayout->setAlpha(0.6);
			_activityIndicatorLayout->setBackgroundColor(GUI::GRAY);

			_activityIndicatorLayout->addChild(_activityIndicator);
			_activityIndicatorLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
			_activityIndicatorLayout->setChildVerticalAlignment(MAW_ALIGNMENT_CENTER);

			_mainLayout->setWidth(screenWidth);
			_mainLayout->fillSpaceVertically();

			_mainLayout->setPaddingLeft(screenWidth / 20);
			_mainLayout->setPaddingRight(screenWidth / 20);
			_mainLayout->setPaddingBottom(screenHeight / 20);

			activityIndicatorLayoutSupport->addChild(_mainLayout);
			activityIndicatorLayoutSupport->addChild(_activityIndicatorLayout);

			_parentLayoutWidth = screenWidth * 0.95;

			this->setMainWidget(activityIndicatorLayoutSupport);
		}

		_categoryGraphicWidth = (int)(_parentLayoutWidth - 2.5 * ( screenWidth / 10 ));
		_categoryGraphicHeight = screenHeight / 20;

		_activityIndicator->show();

		_createBudgetLabel(screenHeight / 20);
		_createSimpleExpensesGraphic((int)_parentLayoutWidth, screenHeight / 20);
		_createDetailedGraphic((int)_parentLayoutWidth, screenHeight / 20);
		createOptionsMenu();
	}

	/**
	 * \brief This function creates the budget information label and the related parent controls.
	 * @param height the height of the screen.
	 */
	void HomeScreen::_createBudgetLabel(int heigth)
	{
		_budgetLabel = new NativeUI::Label();

		char budgetString[BUFFER_SIZE];
		sprintf(budgetString, "Consumed %.2f / %.2f %s", _budgetConsumedValue, _budgetTotalValue, _coin.c_str());

		_budgetLabel->fillSpaceHorizontally();
		_budgetLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_CENTER);
		_budgetLabel->setText(budgetString);
		_budgetLabel->setFontSize(_dialogFontSize);
		_budgetLabel->setHeight(heigth);
		_budgetLabel->setTextVerticalAlignment(MAW_ALIGNMENT_TOP);
	}

	/**
	 * \brief This function creates the simple graphic bar the illustrates the amount consumed.
	 * @param width the screen width
	 * @param height the screen height
	 */
	void HomeScreen::_createSimpleExpensesGraphic(const int& width, const int& height)
	{
		_budgetSimpleGraphicParentLayout = new NativeUI::VerticalLayout();

		_budgetSimpleGraphicParentLayout->addChild(_budgetLabel);

		_budgetSimpleGraphicTotalBudgetLayout = new NativeUI::HorizontalLayout();
		_budgetSimpleGraphicTotalBudgetLayout->setBackgroundColor(GREEN);
		_budgetSimpleGraphicTotalBudgetLayout->setSize( width, height);

		_budgetSimpleGraphicConsumedBudgetLayout = new NativeUI::HorizontalLayout();
		_budgetSimpleGraphicConsumedBudgetLayout->setBackgroundColor(YELLOW);

		double valueWidth = 0;
		if(0 != _budgetTotalValue) double valueWidth = (_budgetConsumedValue / _budgetTotalValue) * width;

		_budgetSimpleGraphicConsumedBudgetLayout->setHeight(height);
		_budgetSimpleGraphicConsumedBudgetLayout->setWidth((int)valueWidth);

		_spacerSimpleGraphic = new NativeUI::HorizontalLayout();
		_spacerSimpleGraphic->setBackgroundColor(RED);
		_spacerSimpleGraphic->setSize(SPACER_WIDTH, height);

		_budgetSimpleGraphicTotalBudgetLayout->addChild(_budgetSimpleGraphicConsumedBudgetLayout);
		_budgetSimpleGraphicTotalBudgetLayout->addChild(_spacerSimpleGraphic);

		_budgetSimpleGraphicParentLayout->addChild(_budgetSimpleGraphicTotalBudgetLayout);

		_mainLayout->addChild(_budgetSimpleGraphicParentLayout);
	}

	/**
	 * \brief This function creates the detailed graphic, bar for each category of expense
	 * @param width the width of the screen
	 * @param height the height of the screen
	 */
	void HomeScreen::_createDetailedGraphic(const int& width, const int& height)
	{
		NativeUI::HorizontalLayout* customSpacer = new NativeUI::HorizontalLayout();
		customSpacer->setHeight(SPACER_HEIGHT);
		_mainLayout->addChild(customSpacer);

		for(int i = 0; i < _valueMap->size(); i++)
		{
			_mainLayout->addChild(_createCategoryGraphic(Model::CATEGORY_LIST[i], _budgetTotalValue,
								  (*_valueMap)[Model::CATEGORY_LIST[i]],
								  width, height, i));
		}
	}

	/**
	 * \brief This function creates a category graphic
	 * @param category the category as a string
	 * @param total the total budget
	 * @param consumed the budget consumed of this category of expenses
	 * @param width the width of the screen
	 * @param height the height of the screen
	 * @return NativeUI::VerticalLayout* the vertical layout containing the desired information
	 */
	NativeUI::VerticalLayout* HomeScreen::_createCategoryGraphic(const MAUtil::String& category, const double& total, const double& consumed, const int& width, const int& height, const int& index)
	{
		NativeUI::VerticalLayout* categoryGraphicParentLayout = new NativeUI::VerticalLayout();
		NativeUI::HorizontalLayout* categoryGraphicLayout = new NativeUI::HorizontalLayout();
		NativeUI::HorizontalLayout* consumeBar = new NativeUI::HorizontalLayout();
		NativeUI::Label* categoryLabel = new NativeUI::Label();
		NativeUI::HorizontalLayout* spacer = new NativeUI::HorizontalLayout();

		spacer->setSize(SPACER_WIDTH, height);

		categoryLabel->setText(category);
		categoryLabel->setWidth((int)(width / 3.5));
		categoryLabel->setFontSize(_dialogFontSize);

		consumeBar->setBackgroundColor(YELLOW);
		double valueWidth = 0;
		if(0 != total) valueWidth = (consumed / total) * width;
		consumeBar->setSize((int)valueWidth, (int)height);

		if((int)valueWidth == 0)
			consumeBar->setWidth(1);

		categoryGraphicLayout->addChild(spacer);
		categoryGraphicLayout->addChild(categoryLabel);
		categoryGraphicLayout->addChild(consumeBar);

		categoryGraphicLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);

		categoryGraphicParentLayout->addChild(categoryGraphicLayout);
		categoryGraphicParentLayout->fillSpaceHorizontally();

		_categoryGraphicsMap->insert(Model::CATEGORY_LIST[index], consumeBar);

		return categoryGraphicParentLayout;
	}

	/**
	 * \brief This function updates the UI for the consume bar of a certain category
	 * @param consumeBar NativeUI::HorizontalLayout* pointer to the consumeBar
	 * @param value const double& the new value
	 */
	void HomeScreen::_updateConsumeBar(NativeUI::HorizontalLayout* consumeBar, const double& value)
	{
		if(0 == value)
			consumeBar->setWidth(1);
		else
		{
			double valueWidth = 0.0;

			if(0 < _budgetTotalValue) valueWidth = (value / _budgetTotalValue) * _categoryGraphicWidth;
			else if(0 != _budgetConsumedValue) valueWidth = (value / _budgetConsumedValue) * _categoryGraphicWidth;

			if(valueWidth > _categoryGraphicWidth)
			{
				NativeUI::HorizontalLayout* debtPart = new NativeUI::HorizontalLayout();
				debtPart->setSize((int)(valueWidth - _categoryGraphicWidth), _categoryGraphicHeight);
				debtPart->setBackgroundColor(RED);
				consumeBar->addChild(debtPart);
				consumeBar->setChildHorizontalAlignment(MAW_ALIGNMENT_LEFT);
				consumeBar->setWidth(_categoryGraphicWidth);
			}
			else
			{
				consumeBar->setWidth((int)valueWidth);
				consumeBar->setBackgroundColor(YELLOW);
			}
		}
	}

	/**
	 * \brief This function hides the ativity indicator for WP7 and destroys it for the other platforms
	 */
	void HomeScreen::_removeActivityIndicator()
	{
		_activityIndicator->hide();
		if(!_WindowsPhone7) maWidgetDestroy(_activityIndicatorLayout->getWidgetHandle());
	}

	/**
	 *\brief This function shows the AddIncome dialog if needed
	 */
	void HomeScreen::_createAddIncomeDialog()
	{
		if(NULL != _addIncomeDialog)
		{
			_addIncomeDialog->setCoin(_coin);
			_addIncomeDialog->setObserver(_observerReference);
			_addIncomeDialog->setLaunchedFromHomeScreen(true);
			_addIncomeDialog->show();
		}
	}

	/**
	 *\brief This function shows the AddExpense dialog if needed
	 */
	void HomeScreen::_createAddExpenseDialog()
	{
		if(NULL != _addExpensesDialog)
		{
			_addExpensesDialog->setObserver(_observerReference);

			_budgetTotalValue =  _observerReference->requestTotalBudget();
			_budgetConsumedValue = _observerReference->requestConsumedBudget();

			_addExpensesDialog->setAvailableBudget(_budgetTotalValue - _budgetConsumedValue);
			_addExpensesDialog->setAcceptedDebtValue(_debtBudget);
			_addExpensesDialog->setCoin(_coin);
			_addExpensesDialog->updateAmountSliderValue();
			_addExpensesDialog->setLaunchedFromHomeScreen(true);
			_addExpensesDialog->show();
		}
	}
}

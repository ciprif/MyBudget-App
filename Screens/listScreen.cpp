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
 * \file expensesListScreen.cpp
 * \author Ciprian Filipas
 * \date Jun 18, 2012
 */

#include "listScreen.h"
#include <NativeUI/VerticalLayout.h>
#include <NativeUI/HorizontalLayout.h>
#include <MAUtil/Vector.h>
#include <NativeUI/ListView.h>
#include <NativeUI/ListViewItem.h>
#include <NativeUI/Button.h>

#include "GUIUtil.h"
#include "../Model/expenseObject.h"
#include "../Model/incomeObject.h"
#include "../Model/listItemModel.h"
#include "addExpenseDialog.h"
#include "addIncomeDialog.h"

#include "MAHeaders.h"

namespace GUI
{
	// Auxiliar variable used for the Alert button clicked validation
	bool isFromRemove;

	int _sortByDateCriteria(const Model::ListItemModel&, const Model::ListItemModel&);
	int _sortByTypeCriteria(const Model::ListItemModel&, const Model::ListItemModel&);
	int _sortByAmountCriteria(const Model::ListItemModel&, const Model::ListItemModel&);

	/**
	 * \brief This function will be used as a criteria for the date related sort function
	 * @param a The comparison date
	 * @param b The compared date
	 * @return int 1 if a > b; -1 if a < b; 0 if a == b
	 */
	int _sortByDateCriteria(const Model::ListItemModel& a, const Model::ListItemModel& b)
	{
		Model::DateStruct d1, d2;
		if(a.IsExpense())
			d1 = a.getExpenseObject().getDate();
		else if(a.IsIncome())
			d1 = a.getIncomeObject().getDate();

		if(b.IsExpense())
			d2 = b.getExpenseObject().getDate();
		else if(b.IsIncome())
			d2 = b.getIncomeObject().getDate();

		return Model::CompareDateObjects(d1, d2);
	}

	/**
	 * \brief This function will be used as a criteria for the type related sort function
	 * @param a The comparison type
	 * @param b The compared type
	 * @return int 1, -1, or 0
	 */
	int _sortByTypeCriteria(const Model::ListItemModel& a, const Model::ListItemModel& b)
	{
		if(a.IsExpense() && b.IsIncome()) return 1;
		else if(a.IsIncome() && b.IsExpense()) return -1;

		return 0;
	}

	/**
	 * \brief This function will be used as a criteria for the amount related sort function
	 * @param a The comparison amount
	 * @param b The compared amount
	 * @return 1 if a > b; -1 if a < b; 0 if the amounts are equal
	 */
	int _sortByAmountCriteria(const Model::ListItemModel& a, const Model::ListItemModel& b)
	{
		double a_amount, b_amount;

		if(a.IsExpense()) a_amount = a.getExpenseObject().getAmount();
		else a_amount = a.getIncomeObject().getAmount();

		if(b.IsExpense()) b_amount = b.getExpenseObject().getAmount();
		else b_amount = b.getIncomeObject().getAmount();

		if((a_amount - b_amount) > 0) return 1;

		return -1;
	}

	/**
	 * \brief No parameter constructor
	 */
	ListScreen::ListScreen() : _listView(NULL), _optionsButton(NULL)
	{
		isFromRemove = false;

		 // Initialize the vector of labels containing the details of a transaction
		_detailsVector = new MAUtil::Vector<NativeUI::Label*>();

		// Initialize the vector that will store the ListItemModels
		_itemsVector = new MAUtil::Vector<Model::ListItemModel>();

		// Default coin value, will be updated if needed
		_coin = "EUR";

		// Default dete values
		_startFromDate->_day = 1;
		_startFromDate->_mounth = 1;
		_startFromDate->_year = 1900;

		// Initialize the click counters
		_countClicksAmount = 0;
		_countClicksCategory = 0;
		_countClicksDates = 0;

		// Initialize the _startFromDate DateStruct object
		_startFromDate = new Model::DateStruct();

		addScreenListener(this);

		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		DeterminePlatform();

		if(_WindowsPhone7) _itemWidth = 9 * (screenWidth / 10) - screenWidth / 30;
		else _itemWidth = (int)(screenWidth * 0.95);

		_createUI();
	}

	ListScreen::~ListScreen()
	{
		_listView->removeListViewListener(this);
		if(_IPhoneOS) _optionsButton->removeButtonListener(this);
	}

	/**
	 * \brief This function populates the expenses list with values from the repository
	 */
	void ListScreen::populateExpensesList()
	{
		MAUtil::Vector<Model::ExpenseObject*>* expList = _observerReference->expensesListRequest();

		Model::ListItemModel* obj;

		for(int i = 0; i < expList->size(); i++)
		{
			if(Model::CompareDateObjects(*_startFromDate, (*expList)[i]->getDate()) == -1 || Model::CompareDateObjects(*_startFromDate, (*expList)[i]->getDate()) == 0)
			{
				obj = new Model::ListItemModel();
				obj->setExpense(*(*expList)[i]);
				_listView->addChild(_createListItem(*obj, _listView->countChildWidgets()));
				_itemsVector->add(*obj);
				delete obj;
			}
		}
	}

	/**
	 * \brief This function populates the incomes list with values from the repository
	 */
	void ListScreen::populateIncomesList()
	{
		MAUtil::Vector<Model::IncomeObject*>* incList = _observerReference->incomesListRequest();

		Model::ListItemModel* obj;
		for(int i = 0; i < incList->size(); i++)
		{
			if(Model::CompareDateObjects(*_startFromDate, (*incList)[i]->getDate()) == -1 || Model::CompareDateObjects(*_startFromDate, (*incList)[i]->getDate()) == 0)
			{
				obj = new Model::ListItemModel();
				obj->setIncome(*(*incList)[i]);
				_listView->addChild(_createListItem(*obj, _listView->countChildWidgets()));
				_itemsVector->add(*obj);
				delete obj;
			}
		}
	}

	/**
	 * \brief This function sorts this list by date
	 * @param descending if true the list will be sorted descending otherwise ascending
	 */
	void ListScreen::sortListByDate(bool descending)
	{
		if(descending) _sortList(_sortByDateCriteria, 0, _itemsVector->size() - 1, 1);
		else _sortList(_sortByDateCriteria, 0, _itemsVector->size() - 1, -1);

		for(int k = 0; k < _itemsVector->size(); k++)
		{
			Model::DateStruct d;
			if((*_itemsVector)[k].IsExpense())
			{
				d = (*_itemsVector)[k].getExpenseObject().getDate();
			}
			else d = (*_itemsVector)[k].getIncomeObject().getDate();
		}

		_clearAndRepopulateList();
	}

	/**
	 * \brief This function sorts this list by the type of the transactions
	 * @param incomesFirst if true the incomes will have priority otherwise the expeses will
	 */
	void ListScreen::sortListByType(bool incomesFirst)
	{
		if(incomesFirst) _sortList(&_sortByTypeCriteria, 0, _itemsVector->size() - 1, 1);
		else _sortList(&_sortByTypeCriteria, 0, _itemsVector->size() - 1, -1);

		_clearAndRepopulateList();
	}

	/**
	 * \brief This function sorts this list by amount value
	 * @param descending if true the list will be sorted descending otherwise ascending
	 */
	void ListScreen::sortListByAmount(bool descending)
	{
		if(descending) _sortList(&_sortByAmountCriteria, 0, _itemsVector->size() - 1, 1);
		else _sortList(&_sortByAmountCriteria, 0, _itemsVector->size() - 1, -1);

		_clearAndRepopulateList();
	}

	/**
	 * \brief This function cleares the UI list
	 */
	void ListScreen::clearList()
	{
		for(int i = 0; i < _listView->countChildWidgets(); i++)
		{
			Widget* w = _listView->getChild(i);
			if (w->countChildWidgets() > 1)
			{
				w->removeChild((*_detailsVector)[i]);
			}
		}

		for(int i = 0; i < _detailsVector->size(); i++)
		{
			maWidgetDestroy((*_detailsVector)[i]->getWidgetHandle());
			delete (*_detailsVector)[i];
		}

		while(0 < _listView->countChildWidgets())
		{
			NativeUI::Widget* w = _listView->getChild(_listView->countChildWidgets() - 1);
			_listView->removeChild(w);
			maWidgetDestroy(w->getWidgetHandle());
			delete w;
		}

		delete _itemsVector;
		delete _detailsVector;

		_itemsVector = new MAUtil::Vector<Model::ListItemModel>();
		_detailsVector = new MAUtil::Vector<NativeUI::Label*>();
	}

	/**
	 * \brief This function sets the _coin value to coin
	 * @param coin const MAUtil::String& the new value
	 */
	void ListScreen::setCoin(const MAUtil::String& coin)
	{
		_coin = coin;
	}

	/**
	 * \brief This function is used to set the value of the observer reference to the
	 * 		  application wide observer
	 * @param obs Logical::Observer* pointer to the application wide observer
	 */
	void ListScreen::setObserver(Logical::Observer* obs)
	{
		_observerReference = obs;
	}

	/**
	 * This method is called when a list view item is clicked.
	 * @param listView The list view object that generated the event.
	 * @param index The index on which the list view item is positioned.
	 */
	void ListScreen::listViewItemClicked(NativeUI::ListView* listView, int index)
	{
		if(listView == _listView)
		{
			NativeUI::ListViewItem* item = (NativeUI::ListViewItem*) listView->getChild(index);
			NativeUI::VerticalLayout* itemLayout = (NativeUI::VerticalLayout*) item->getChild(0);
			if(itemLayout->getChild(1) == NULL)
			{
				itemLayout->addChild((*_detailsVector)[index]);
				itemLayout->getChild(1)->setBackgroundColor(0x736F6E);
				itemLayout->getChild(0)->setBackgroundColor(0x413D3C);
			}
			else
			{
				itemLayout->removeChild((*_detailsVector)[index]);
				itemLayout->getChild(0)->setBackgroundColor(0);
			}
		}
	}

	/**
	 * \brief This function is called from the observer in order to notify the
	 *        screen that a new expense item was added to the repository.
	 *        Through this function a UI update call will be triggered.
	 * @param obj const Model::ExpeseObject& the object that contains the model data
	 * 		  for the item added into the repository
	 */
	void ListScreen::addExpenseNotification(const Model::ExpenseObject& obj)
	{
		Model::ListItemModel list_obj;
		list_obj.setExpense(obj);

		_listView->addChild(_createListItem(list_obj, _listView->countChildWidgets()));
		_itemsVector->add(list_obj);
	}

	/**
	 * \brief This function is called from the observer in order to notify the
	 *        screen that a new income item was added to the repository.
	 *        Through this function a UI update call will be triggered.
	 * @param obj const Model::IncomeObject& the object that contains the model data
	 * 		  for the item added into the repository
	 */
	void ListScreen::addIncomeNotification(const Model::IncomeObject& obj)
	{
		Model::ListItemModel list_obj;
		list_obj.setIncome(obj);

		_listView->addChild(_createListItem(list_obj, _listView->countChildWidgets()));
		_itemsVector->add(list_obj);
	}

	/**
	 * \brief This function is used to create the options menu and the related controls
	 */
	void ListScreen::createOptionsMenu()
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

			_clearListIndex = addOptionsMenuItem("Clear list", MAW_OPTIONS_MENU_ICON_CONSTANT_DELETE, false);

			_sortByDateIndex = addOptionsMenuItem("Sort by date");
			_sortByCategoryIndex = addOptionsMenuItem("Sort by category");
			_sortByAmountIndex = addOptionsMenuItem("Sort by amount");
		}
		else
		{
			if(NULL == _optionsButton)
			{
				_optionsButton = new NativeUI::Button();
				_optionsButton->setBackgroundColor(255, 0, 0);
				_optionsButton->setWidth((int)(_itemWidth * 0.95));
				_optionsButton->setText("Options");
				_optionsButton->addButtonListener(this);
				_mainLayout->setChildHorizontalAlignment(MAW_ALIGNMENT_CENTER);
				_mainLayout->addChild(_optionsButton);

				NativeUI::VerticalLayout* spacer = new NativeUI::VerticalLayout();
				spacer->setHeight(SPACER_HEIGHT_IOS);
				_mainLayout->addChild(spacer);
			}
		}
	}

	/**
	 * \brief This function handles the options menu item selected event from a screen, inherited
	 * 	      from the NativeUI::Screen class
	 * @param screen NativeUI::Screen* pointer to the screen that triggers the event
	 * @param index int index for the button that was clicked
	 */
	void ListScreen::optionsMenuItemSelected(NativeUI::Screen* screen, int index)
	{
		if(screen == this)
		{
			if(_addExpenseIndex == index) //add expense screen
			{
				_handleAddExpenseButtonClicked();
			}
			else if(_addIncomeIndex == index)
			{
				_handleAddIncomeButtonClicked();
			}
			else if(_sortByCategoryIndex == index)
			{
				_handleSortByTypeButtonClicked();
			}
			else if(_sortByDateIndex == index)
			{
				_handleSortByDateButtonClicked();
			}
			else if(_sortByAmountIndex == index)
			{
				_handleSortByAmountButtonClicked();
			}
			else if(_clearListIndex == index)
			{
				_handleClearListButtonClicked();
			}
		}
	}

	/**
	 * \brief This function handles the button click event.
	 * @param button NativeUI::Widget* pointer to the button that triggered the event
	 */
	void ListScreen::buttonClicked(NativeUI::Widget* button)
	{
		if(button == _optionsButton)
		{

			MAUtil::WString boxTitle(L"Options");
			MAUtil::WString destructiveButton(L"Clear list");
			MAUtil::WString cancelButton(L"Cancel");
			int count = sizeof(int);
			for(int i = 0; i < LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH; i++)
			{
				count += (LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[i].length() * sizeof(wchar)) + sizeof(wchar);
			}

			char* buffer = new char[count];
			*(int*) buffer = LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH;

			wchar_t* dest = (wchar_t*)(buffer + sizeof(int));

			for(int i = 0; i < LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES_LENGTH; i++)
			{
				const wchar_t* array = LIST_SCREEN_OPTIONS_BOX_BUTTONS_TITLES[i].pointer();
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
	 * \brief This function is used for setting the AddExpenseDialog reference
	 * @param expensesDialog AddExpenseDialog* the pointer to the application wide expense dialog
	 */
	void ListScreen::setAddExpensesDialogReference(AddExpenseDialog* expensesDialog)
	{
		_addExpensesDialog = expensesDialog;
		_addExpensesDialog->setListScreenRef(this);
	}

	/**
	 * \brief This function is used for setting the AddIncomeDialog reference
	 * @param expensesDialog AddIncomeDialog* the pointer to the application wide expense dialog
	 */
	void ListScreen::setAddIncomeDialogReference(AddIncomeDialog* incomesDialog)
	{
		_addIncomeDialog = incomesDialog;
		_addIncomeDialog->setListScreenRef(this);
	}

	/**
	 * \brief This function is used for seting the _startFromDate member
	 * @param dateFrom const Model::DateStruct& the date object
	 */
	void ListScreen::setDateFrom(const Model::DateStruct& dateFrom)
	{
		*_startFromDate = dateFrom;
	}

	/**
	 * \brief This function is used for synchronizing the _debtBudget value
	 * 	 	  with the value stored into the repository
	 */
	void ListScreen::updateDebtValue()
	{
		_debtBudget = _observerReference->requestDebtValue();
	}

	/**
	 * \brief This function is used for handling the custom event triggered by the alert box
	 * @param event const MAEvent& the event type
	 */
	void ListScreen::customEvent(const MAEvent& event)
	{
		if(event.type == EVENT_TYPE_ALERT)
		{
			if(1 == event.alertButtonIndex && isFromRemove)
			{
				_observerReference->requestClearTransactionList();
				isFromRemove = false;
			}
		}
		else if(event.type == EVENT_TYPE_OPTIONS_BOX_BUTTON_CLICKED)
		{
			switch(event.optionsBoxButtonIndex)
			{
			case 0:
				_handleClearListButtonClicked();
				break;
			case 1:
				_handleAddIncomeButtonClicked();
				break;
			case 2:
				_handleAddExpenseButtonClicked();
				break;
			case 3:
				_handleSortByDateButtonClicked();
				break;
			case 4:
				_handleSortByTypeButtonClicked();
				break;
			case 5:
				_handleSortByAmountButtonClicked();
				break;
			}
		}
	}

	/**
	 * \brief This function calls the UI creation functions
	 */
	void ListScreen::_createUI()
	{
		SetSizeRelatedVariables();

		_mainLayout = new NativeUI::VerticalLayout();
		_listView = new NativeUI::ListView();
		_listView->fillSpaceHorizontally();
		_listView->fillSpaceVertically();
		_listView->addListViewListener(this);

		_mainLayout->addChild(_listView);
		setMainWidget(_mainLayout);

		createOptionsMenu();

		//populateList();
	}

	/**
	 * \brief This function is used for sorting the list
	 * @param criteria a pointer to a function used for sorting elements
	 * @param left
	 * @param right
	 * @param value the value returned by the criteria function when the first param is "<" then the second one
	 */
	void ListScreen::_sortList(int (*criteria)(const Model::ListItemModel&, const Model::ListItemModel&), int left, int right, int value)
	{
		for(int k = 0; k < _itemsVector->size(); k++)
		{
			Model::DateStruct d;
			if((*_itemsVector)[k].IsExpense())
			{
				d = (*_itemsVector)[k].getExpenseObject().getDate();
			}
			else d = (*_itemsVector)[k].getIncomeObject().getDate();
		}
		int i = left, j = right;
		Model::ListItemModel* pivot = new Model::ListItemModel((*_itemsVector)[(left + right) / 2]);
		Model::ListItemModel* aux;

		while(i <= j)
		{
			while( (*criteria)((*_itemsVector)[i], *pivot) == value)
			{
				i++;
			}
			while( (*criteria)(*pivot, (*_itemsVector)[j]) == value)
			{
				j--;
			}

			if(i <= j)
			{
				if(i != j)
				{
					aux = new Model::ListItemModel((*_itemsVector)[i]);
					(*_itemsVector)[i] = (*_itemsVector)[j];
					(*_itemsVector)[j] = *aux;

					delete aux;
				}
				i++;
				j--;
			}
		}
		delete pivot;
		if (left < j)
			_sortList(criteria, left, j, value);
		if (i < right)
			_sortList(criteria, i, right, value);
	}

	/**
	 * \brief This function is used for clearing and repopulating the list
	 */
	void ListScreen::_clearAndRepopulateList()
	{
		for(int i = 0; i < _listView->countChildWidgets(); i++)
		{
			NativeUI::ListViewItem* item = (NativeUI::ListViewItem*) _listView->getChild(i);
			NativeUI::VerticalLayout* itemLayout = (NativeUI::VerticalLayout*) item->getChild(0);
			if (itemLayout->countChildWidgets() > 1)
			{
				itemLayout->removeChild((*_detailsVector)[i]);
			}
		}

		for(int i = 0; i < _detailsVector->size(); i++)
		{
			maWidgetDestroy((*_detailsVector)[i]->getWidgetHandle());
			delete (*_detailsVector)[i];
		}

		while(0 < _listView->countChildWidgets())
		{
			NativeUI::Widget* w = _listView->getChild(_listView->countChildWidgets() - 1);
			_listView->removeChild(w);
			maWidgetDestroy(w->getWidgetHandle());
			delete w;
		}

		delete _detailsVector;
		_detailsVector = new MAUtil::Vector<NativeUI::Label*>();

		for(int i = 0; i < _itemsVector->size(); i++)
		{
			_listView->addChild(_createListItem((*_itemsVector)[i], i));
		}
	}

	/**
	 * \brief This function is called to create a list item
	 * @param obj const Model::ListItemModel& the model object for the list view item
	 * @param index int the index of the item
	 * @return NativeUI::ListViewItem* pointer to the list view item.
	 */
	NativeUI::ListViewItem* ListScreen::_createListItem(const Model::ListItemModel& obj, int index)
	{
		NativeUI::ListViewItem* item = new NativeUI::ListViewItem();
		NativeUI::VerticalLayout* itemParent = new NativeUI::VerticalLayout();
		if(_WindowsPhone7) itemParent->setWidth(_itemWidth);
		else itemParent->fillSpaceHorizontally();
		if (_IPhoneOS) itemParent->wrapContentVertically();

		NativeUI::HorizontalLayout* visiblePart = new NativeUI::HorizontalLayout();
		if (_IPhoneOS) visiblePart->wrapContentVertically();
		NativeUI::Label* typeLabel = new NativeUI::Label();
		NativeUI::Label* sumLabel = new NativeUI::Label();

		NativeUI::Label* details = new NativeUI::Label();

		typeLabel->setFontSize(_dialogFontSize);
		sumLabel->setFontSize(_dialogFontSize);
		typeLabel->fillSpaceHorizontally();
		sumLabel->fillSpaceHorizontally();
		if (_IPhoneOS)
		{
			typeLabel->wrapContentVertically();
			sumLabel->wrapContentVertically();
		}

		if(!_WindowsPhone7) sumLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_RIGHT);

		if(obj.IsExpense())
		{
			_createExpenseListViewItem(details, typeLabel, sumLabel, obj);
		}
		else
		{
			_createIncomeListViewItem(details, typeLabel, sumLabel, obj);
		}

		visiblePart->addChild(typeLabel);
		visiblePart->addChild(sumLabel);

		itemParent->addChild(visiblePart);

		_detailsVector->add(details);

		item->addChild(itemParent);
		return item;
	}

	/**
	 * \brief This function is used for creating an income type listViewItem
	 * @param details NativeUI::Label* pointer to the details label
	 * @param typeLabel NativeUI::Label* pointer to the type label
	 * @param sumLabel NativeUI::Label* pointer to the sum label
	 */
	void ListScreen::_createIncomeListViewItem(NativeUI::Label* details,
											   NativeUI::Label* typeLabel,
											   NativeUI::Label* sumLabel,
											   const Model::ListItemModel& obj)
	{
		typeLabel->setText(obj.getIncomeObject().getType());
		typeLabel->setFontColor(GREEN);

		char amountString[10];
		sprintf(amountString, "%.2f", obj.getIncomeObject().getAmount());

		MAUtil::String aux(amountString);
		aux += " ";
		aux += _coin;

		sumLabel->setText(aux);

		aux.clear();
		aux += "Description:\n";
		aux += obj.getIncomeObject().getDescription();
		aux += "\nTransaction info:\n";
		aux += obj.getIncomeObject().getTransactionInformation();
		aux += "\nDate:\n";
		aux += Model::DateStructToString(obj.getIncomeObject().getDate());
		aux += "\nTime:\n";
		aux += Model::TimeStructToString(obj.getIncomeObject().getTime());

		details->setText(aux);
		details->setMaxNumberOfLines(10);

		if(_WindowsPhone7) details->setWidth(_itemWidth);
		else details->fillSpaceHorizontally();

		details->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);
	}

	/**
	 * \brief This function is used for creating an expense type listViewItem
	 * @param details NativeUI::Label* pointer to the details label
	 * @param typeLabel NativeUI::Label* pointer to the type label
	 * @param sumLabel NativeUI::Label* pointer to the sum label
	 */
	void ListScreen::_createExpenseListViewItem(NativeUI::Label* details,
											   NativeUI::Label* typeLabel,
											   NativeUI::Label* sumLabel,
											   const Model::ListItemModel& obj)
	{
		typeLabel->setText(obj.getExpenseObject().getCategory());
		typeLabel->setFontColor(RED);

		char amountString[10];
		sprintf(amountString, "%.2f", obj.getExpenseObject().getAmount());
		MAUtil::String aux(amountString);
		aux += " ";
		aux += _coin;
		sumLabel->setText(aux);
		aux.clear();
		aux += "Description:\n";
		aux += obj.getExpenseObject().getDescription();
		aux += "\nDate:\n";
		aux += Model::DateStructToString(obj.getExpenseObject().getDate());
		aux += "\nTime:\n";
		aux += Model::TimeStructToString(obj.getExpenseObject().getTime());
		details->setText(aux);
		details->setMaxNumberOfLines(10);

		if(_WindowsPhone7) details->setWidth(_itemWidth);
		else details->fillSpaceHorizontally();

		details->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);
	}

	/**
	 * \brief This function handles the add expense button click event
	 */
	void ListScreen::_handleAddExpenseButtonClicked()
	{

		if(NULL != _addExpensesDialog)
		{
			_addExpensesDialog->setObserver(_observerReference);

			_budgetTotalValue =  _observerReference->requestTotalBudget();
			_budgetConsumedValue = _observerReference->requestConsumedBudget();

			_addExpensesDialog->setAvailableBudget(_budgetTotalValue - _budgetConsumedValue);
			_addExpensesDialog->setAcceptedDebtValue(_debtBudget);
			_addExpensesDialog->setCoin(_coin);
			_addExpensesDialog->setLaunchedFromHomeScreen(false);
			_addExpensesDialog->updateAmountSliderValue();
			_addExpensesDialog->show();
		}
	}

	/**
	 * \brief This function handles the add income button click event
	 */
	void ListScreen::_handleAddIncomeButtonClicked()
	{
		if(NULL != _addIncomeDialog)
		{
			_addIncomeDialog->setCoin(_coin);
			_addIncomeDialog->setObserver(_observerReference);
			_addIncomeDialog->setLaunchedFromHomeScreen(false);
			_addIncomeDialog->show();
		}
	}

	/**
	 * \brief This function handles the sort by date button click event
	 */
	void ListScreen::_handleSortByDateButtonClicked()
	{
		if(_countClicksDates % 2 == 0)
			sortListByDate(true);
		else
			sortListByDate(false);

		_countClicksDates = (_countClicksDates + 1) % 2;
		_countClicksAmount = 0;
		_countClicksCategory = 0;
	}

	/**
	 * \brief This function handles the sort by type button click event
	 */
	void ListScreen::_handleSortByTypeButtonClicked()
	{
		if(_countClicksCategory % 2 == 0)
			sortListByType(true);
		else
			sortListByType(false);

		_countClicksCategory = (_countClicksCategory + 1) % 2;
		_countClicksAmount = 0;
		_countClicksDates = 0;
	}

	/**
	 * \brief This function handles the sort by amount button click event
	 */
	void ListScreen::_handleSortByAmountButtonClicked()
	{
		if(_countClicksAmount % 2 == 0)
			sortListByAmount(true);
		else
			sortListByAmount(false);

		_countClicksAmount = (_countClicksAmount + 1) % 2;
		_countClicksCategory = 0;
		_countClicksDates = 0;
	}

	/**
	 * \brief This function handles the clear list button click event
	 */
	void ListScreen::_handleClearListButtonClicked()
	{
		if(_WindowsPhone7) maAlert("Alert!", "Are you sure you want to clear the list? Note that this action has a permanent effect.", "OK", "", "Cancel");
		else maAlert("Alert!", "Are you sure you want to clear the list? Note that this action has a permanent effect.", "OK", "Cancel", "");
		isFromRemove = true;
	}
}



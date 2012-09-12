/*
 * expensesListScreen.cpp
 *
 *  Created on: Jun 18, 2012
 *      Author: Cipri
 */

#include "listScreen.h"
#include <NativeUI/HorizontalLayout.h>
#include <NativeUI/VerticalLayout.h>
#include <MAUtil/Vector.h>

#include <NativeUI/ListView.h>
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
	ListScreen::ListScreen() : _listView(NULL)
	{
		isFromRemove = false;

		/* Attached this as Custom event listener in order to capture the button clicked event
		   from an alert */
		 MAUtil::Environment::getEnvironment().addCustomEventListener(this);

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

		_setPlatform();

		if(_isWP7) _itemWidth = 9 * (screenWidth / 10) - screenWidth / 30;
		else _itemWidth = (int)(screenWidth * 0.95);

		_createUI();
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
		_listView->removeListViewListener(this);
		delete _listView;

		_listView = new NativeUI::ListView();
		_listView->fillSpaceHorizontally();
		_listView->fillSpaceVertically();
		_listView->addListViewListener(this);

		_mainLayout->addChild(_listView);
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
			if(((NativeUI::VerticalLayout*)listView->getChild(index))->getChild(1) == NULL)
			{
				((NativeUI::VerticalLayout*)listView->getChild(index))->addChild((*_detailsVector)[index]);
				((NativeUI::VerticalLayout*)listView->getChild(index))->getChild(1)->setBackgroundColor(0x736F6E);
				((NativeUI::VerticalLayout*)listView->getChild(index))->getChild(0)->setBackgroundColor(0x413D3C);
			}
			else
			{
				((NativeUI::VerticalLayout*)listView->getChild(index))->removeChild((*_detailsVector)[index]);
				((NativeUI::VerticalLayout*)listView->getChild(index))->getChild(1)->setBackgroundColor(0);
				((NativeUI::VerticalLayout*)listView->getChild(index))->getChild(0)->setBackgroundColor(0);
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
		if(_isWP7)
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
			else if(_addIncomeIndex == index)
			{
				if(NULL != _addIncomeDialog)
				{
					_addIncomeDialog->setCoin(_coin);
					_addIncomeDialog->setObserver(_observerReference);
					_addIncomeDialog->setLaunchedFromHomeScreen(false);
					_addIncomeDialog->show();
				}
			}
			else if(_sortByCategoryIndex == index)
			{
				if(_countClicksCategory % 2 == 0)
					sortListByType(true);
				else
					sortListByType(false);

				_countClicksCategory = (_countClicksCategory + 1) % 2;
				_countClicksAmount = 0;
				_countClicksDates = 0;
			}
			else if(_sortByDateIndex == index)
			{
				if(_countClicksDates % 2 == 0)
					sortListByDate(true);
				else
					sortListByDate(false);

				_countClicksDates = (_countClicksDates + 1) % 2;
				_countClicksAmount = 0;
				_countClicksCategory = 0;
			}
			else if(_sortByAmountIndex == index)
			{
				if(_countClicksAmount % 2 == 0)
					sortListByAmount(true);
				else
					sortListByAmount(false);

				_countClicksAmount = (_countClicksAmount + 1) % 2;
				_countClicksCategory = 0;
				_countClicksDates = 0;
			}
			else if(_clearListIndex == index)
			{
				maAlert("Alert!", "Are you sure you want to clear the list? Note that this action has a permanent effect.", "OK", "Cancel", NULL);
				isFromRemove = true;
			}
		}
	}

	void ListScreen::_setPlatform()
	{
		char buffer[Model::BUFF_SIZE];
		maGetSystemProperty("mosync.device.OS", buffer, Model::BUFF_SIZE);

		if(strcmp(buffer, "iOS") == 0 || strcmp(buffer, "Android") == 0)
		{
			_isWP7 = false;
		}
		else
		{
			_isWP7 = true;
		}
	}

	void ListScreen::_clearAndRepopulateList()
	{
		_listView->removeListViewListener(this);

		delete _listView;

		_listView = new NativeUI::ListView();
		_listView->fillSpaceHorizontally();
		_listView->fillSpaceVertically();
		_listView->addListViewListener(this);

		_mainLayout->addChild(_listView);

		for(int i = 0; i < _detailsVector->size(); i++)
			delete (*_detailsVector)[i];

		delete _detailsVector;
		_detailsVector = new MAUtil::Vector<NativeUI::Label*>();

		for(int i = 0; i < _itemsVector->size(); i++)
		{
			_listView->addChild(_createListItem((*_itemsVector)[i], i));
		}
	}



	void ListScreen::_createUI()
	{
		_mainLayout = new NativeUI::HorizontalLayout();
		_listView = new NativeUI::ListView();
		_listView->fillSpaceHorizontally();
		_listView->fillSpaceVertically();
		_listView->addListViewListener(this);

		_mainLayout->addChild(_listView);

		setMainWidget(_mainLayout);

		createOptionsMenu();

		//populateList();
	}



	NativeUI::VerticalLayout* ListScreen::_createListItem(const Model::ListItemModel& obj, int index)
	{
		NativeUI::VerticalLayout* itemParent = new NativeUI::VerticalLayout();
		if(_isWP7) itemParent->setWidth(_itemWidth);
		else itemParent->fillSpaceHorizontally();

		NativeUI::HorizontalLayout* visiblePart = new NativeUI::HorizontalLayout();
		NativeUI::Label* typeLabel = new NativeUI::Label();
		NativeUI::Label* sumLabel = new NativeUI::Label();

		NativeUI::Label* details = new NativeUI::Label();

		typeLabel->setFontSize(_dialogFontSize);
		sumLabel->setFontSize(_dialogFontSize);
		typeLabel->fillSpaceHorizontally();
		sumLabel->fillSpaceHorizontally();

		if(!_isWP7) sumLabel->setTextHorizontalAlignment(MAW_ALIGNMENT_RIGHT);

		if(obj.IsExpense())
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

			if(_isWP7) details->setWidth(_itemWidth);
			else details->fillSpaceHorizontally();

			details->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		}
		else
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

			if(_isWP7) details->setWidth(_itemWidth);
			else details->fillSpaceHorizontally();

			details->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);
		}

		visiblePart->addChild(typeLabel);
		visiblePart->addChild(sumLabel);

		itemParent->addChild(visiblePart);

		_detailsVector->add(details);

		return itemParent;
	}

	void ListScreen::optionsMenuClosed(NativeUI::Screen* screen)
	{
		//Dance for me baby
	}

	void ListScreen::setAddExpensesDialogReference(AddExpenseDialog* expensesDialog)
	{
		_addExpensesDialog = expensesDialog;
		_addExpensesDialog->setListScreenRef(this);
	}
	void ListScreen::setAddIncomeDialogReference(AddIncomeDialog* incomesDialog)
	{
		_addIncomeDialog = incomesDialog;
		_addIncomeDialog->setListScreenRef(this);
	}

	void ListScreen::setDateFrom(const Model::DateStruct& dateFrom)
	{
		*_startFromDate = dateFrom;
	}

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

	void ListScreen::updateDebtValue()
	{
		_debtBudget = _observerReference->requestDebtValue();
	}

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
	}
}



/*
 * listScreen.h
 *
 *  Created on: Jun 18, 2012
 *      Author: Cipri
 */

#ifndef LISTSCREEN_H_
#define LISTSCREEN_H_

#include <NativeUI/Screen.h>
#include <NativeUI/ScreenListener.h>
#include <NativeUI/ListViewListener.h>
#include <NativeUI/Label.h>
#include <MAUtil/String.h>
#include <MAUtil/Map.h>
#include <MAUtil/Environment.h>
#include "../Logical/observer.h"
#include "../Model/util.h"

// Forward declarations
namespace Model
{
	class ExpenseObject;
	class IncomeObject;
	class ListItemModel;
}

namespace NativeUI
{
	class HorizontalLayout;
	class VerticalLayout;
	class ListView;
	class ListViewItem;
}

namespace GUI
{
	//Forward declarations
	class AddExpenseDialog;
	class AddIncomeDialog;

	class ListScreen : public NativeUI::Screen, public NativeUI::ListViewListener, public NativeUI::ScreenListener, public MAUtil::CustomEventListener
	{
	public:
		/**
		 * \brief No parameter constructor
		 */
		ListScreen();

		/**
		 * \brief This function populates the expenses list with values from the repository
		 */
		void populateExpensesList();

		/**
		 * \brief This function populates the incomes list with values from the repository
		 */
		void populateIncomesList();

		/**
		 * \brief This function sorts this list by date
		 * @param descending if true the list will be sorted descending otherwise ascending
		 */
		void sortListByDate(bool descending);

		/**
		 * \brief This function sorts this list by the type of the transactions
		 * @param incomesFirst if true the incomes will have priority otherwise the expeses will
		 */
		void sortListByType(bool incomesFirst);

		/**
		 * \brief This function sorts this list by amount value
		 * @param descending if true the list will be sorted descending otherwise ascending
		 */
		void sortListByAmount(bool descending);

		/**
		 * \brief This function cleares the UI list
		 */
		void clearList();

		/**
		 * \brief This function sets the _coin value to coin
		 * @param coin const MAUtil::String& the new value
		 */
		void setCoin(const MAUtil::String& coin);

		/**
		 * \brief This function is used to set the value of the observer reference to the
		 * 		  application wide observer
		 * @param obs Logical::Observer* pointer to the application wide observer
		 */
		void setObserver(Logical::Observer* obs);

		/**
		 * This method is called when a list view item is clicked.
		 * @param listView The list view object that generated the event.
		 * @param listViewItem The ListViewItem object that was clicked.
		 */
		void listViewItemClicked(
			NativeUI::ListView* listView,
			NativeUI::ListViewItem* listViewItem){}

		/**
		 * This method is called when a list view item is clicked.
		 * @param listView The list view object that generated the event.
		 * @param index The index on which the list view item is positioned.
		 */
		void listViewItemClicked(
			NativeUI::ListView* listView,
			int index);

		/**
		 * \brief This function is called from the observer in order to notify the
		 *        screen that a new expense item was added to the repository.
		 *        Through this function a UI update call will be triggered.
		 * @param obj const Model::ExpeseObject& the object that contains the model data
		 * 		  for the item added into the repository
		 */
		void addExpenseNotification(const Model::ExpenseObject& obj);

		/**
		 * \brief This function is called from the observer in order to notify the
		 *        screen that a new income item was added to the repository.
		 *        Through this function a UI update call will be triggered.
		 * @param obj const Model::IncomeObject& the object that contains the model data
		 * 		  for the item added into the repository
		 */
		void addIncomeNotification(const Model::IncomeObject& obj);

		/**
		 * \brief This function is used to create the options menu and the related controls
		 */
		void createOptionsMenu();

		/**
		 * \brief This function handles the options menu item selected event from a screen, inherited
		 * 	      from the NativeUI::Screen class
		 * @param screen NativeUI::Screen* pointer to the screen that triggers the event
		 * @param index int index for the button that was clicked
		 */
		void optionsMenuItemSelected(NativeUI::Screen* screen, int index);
		void optionsMenuClosed(NativeUI::Screen* screen);

		void setAddExpensesDialogReference(AddExpenseDialog* expensesDialog);
		void setAddIncomeDialogReference(AddIncomeDialog* incomesDialog);

		void setDateFrom(const Model::DateStruct& dateFrom);

		void updateDebtValue();

		void customEvent(const MAEvent& event);
	private:
		void _createUI();
		/**
		 *
		 * @param criteria a pointer to a function used for sorting elements
		 * @param left
		 * @param right
		 * @param value the value returned by the criteria function when the first param is "<" then the second one
		 */
		void _sortList(int (*criteria)(const Model::ListItemModel&, const Model::ListItemModel&), int left, int right, int value);
		void _clearAndRepopulateList();
		void _setPlatform();

		NativeUI::VerticalLayout* _createListItem(const Model::ListItemModel& obj, int index);

		/** @todo add separate function for init and show on dialogs in order to make the options menu item selected code **/
		MAUtil::Vector<Model::ListItemModel>* _itemsVector;
		MAUtil::Vector<NativeUI::Label*>* _detailsVector;
		NativeUI::HorizontalLayout* _mainLayout;
		NativeUI::ListView* _listView;

		MAUtil::String _coin;

		Logical::Observer* _observerReference;
		int _itemWidth;

		int _addIncomeIndex;
		int _addExpenseIndex;
		int _clearListIndex;
		int _sortByDateIndex;
		int _sortByCategoryIndex;
		int _sortByAmountIndex;

		AddExpenseDialog* _addExpensesDialog;
		AddIncomeDialog* _addIncomeDialog;

		double _budgetTotalValue;
		double _budgetConsumedValue;
		double _debtBudget;

		Model::DateStruct* _startFromDate;

		int _countClicksDates;
		int _countClicksAmount;
		int _countClicksCategory;

		bool _isWP7;

		//int (*criteria)(const Model::ListItemModel&, const Model::ListItemModel&);
		MAUtil::Vector<int> _characteristicArrayForSorting;
	};
}


#endif /* LISTSCREEN_H_ */

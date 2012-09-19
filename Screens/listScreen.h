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
 * \file listScreen.h
 * \author Ciprian Filipas
 * \date Jun 18, 2012
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
#include "../Model/ModelUtil.h"

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

	/**
	 * \brief Class for the transactions screen
	 */
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

		/**
		 * \brief This function handles the menu closed event, inherited from the NativeUI::Screen class
		 * @param screen NativeUI::Screen* pointer to the screen that triggered the menu closed event
		 *
		 * \note This function has an empty body because it's not used in the current implementation
		 */
		void optionsMenuClosed(NativeUI::Screen* screen) {}

		/**
		 * \brief This function is used for setting the AddExpenseDialog reference
		 * @param expensesDialog AddExpenseDialog* the pointer to the application wide expense dialog
		 */
		void setAddExpensesDialogReference(AddExpenseDialog* expensesDialog);

		/**
		 * \brief This function is used for setting the AddIncomeDialog reference
		 * @param expensesDialog AddIncomeDialog* the pointer to the application wide expense dialog
		 */
		void setAddIncomeDialogReference(AddIncomeDialog* incomesDialog);

		/**
		 * \brief This function is used for seting the _startFromDate member
		 * @param dateFrom const Model::DateStruct& the date object
		 */
		void setDateFrom(const Model::DateStruct& dateFrom);

		/**
		 * \brief This function is used for synchronizing the _debtBudget value
		 * 	 	  with the value stored into the repository
		 */
		void updateDebtValue();

		/**
		 * \brief This function is used for handling the custom event triggered by the alert box
		 * @param event const MAEvent& the event type
		 */
		void customEvent(const MAEvent& event);
	private:
		/**
		 * \brief This function calls the UI creation functions
		 */
		void _createUI();

		/**
		 * \brief This function is used for sorting the list
		 * @param criteria a pointer to a function used for sorting elements
		 * @param left
		 * @param right
		 * @param value the value returned by the criteria function when the first param is "<" then the second one
		 */
		void _sortList(int (*criteria)(const Model::ListItemModel&, const Model::ListItemModel&), int left, int right, int value);

		/**
		 * \brief This function is used for clearing and repopulating the list
		 */
		void _clearAndRepopulateList();

		/**
		 * \brief This function is called to create a list item
		 * @param obj const Model::ListItemModel& the model object for the list view item
		 * @param index int the index of the item
		 * @return NativeUI::VerticalLayout* the layout containing the listViewItem
		 */
		NativeUI::VerticalLayout* _createListItem(const Model::ListItemModel& obj, int index);

		/**
		 * \brief This function is used for creating an expense type listViewItem
		 * @param details NativeUI::Label* pointer to the details label
		 * @param typeLabel NativeUI::Label* pointer to the type label
		 * @param sumLabel NativeUI::Label* pointer to the sum label
		 */
		void _createExpenseListViewItem(NativeUI::Label* details, NativeUI::Label* typeLabel,
								        NativeUI::Label* sumLabel, const Model::ListItemModel& obj);

		/**
		 * \brief This function is used for creating an income type listViewItem
		 * @param details NativeUI::Label* pointer to the details label
		 * @param typeLabel NativeUI::Label* pointer to the type label
		 * @param sumLabel NativeUI::Label* pointer to the sum label
		 */
		void _createIncomeListViewItem(NativeUI::Label* details, NativeUI::Label* typeLabel,
									   NativeUI::Label* sumLabel, const Model::ListItemModel& obj);

		/**
		 * \brief This function handles the add expense button click event
		 */
		void _handleAddExpenseButtonClicked();

		/**
		 * \brief This function handles the add income button click event
		 */
		void _handleAddIncomeButtonClicked();

		/**
		 * \brief This function handles the sort by date button click event
		 */
		void _handleSortByDateButtonClicked();

		/**
		 * \brief This function handles the sort by type button click event
		 */
		void _handleSortByTypeButtonClicked();

		/**
		 * \brief This function handles the sort by amount button click event
		 */
		void _handleSortByAmountButtonClicked();

		/**
		 * \brief This function handles the clear list button click event
		 */
		void _handleClearListButtonClicked();

		// Member data
		double _budgetTotalValue;
		double _budgetConsumedValue;
		double _debtBudget;

		int _itemWidth;
		int _addIncomeIndex;
		int _addExpenseIndex;
		int _clearListIndex;
		int _sortByDateIndex;
		int _sortByCategoryIndex;
		int _sortByAmountIndex;
		int _countClicksDates;
		int _countClicksAmount;
		int _countClicksCategory;

		MAUtil::String _coin;

		// DateStruct object used as date reference
		Model::DateStruct* _startFromDate;

		// Vector of ListItemModel items; the data of the list
		MAUtil::Vector<Model::ListItemModel>* _itemsVector;

		// Vector of the detailed parts of list view items; revealed on click
		MAUtil::Vector<NativeUI::Label*>* _detailsVector;

		//Observer reference
		Logical::Observer* _observerReference;

		// UI related variables
		NativeUI::HorizontalLayout* _mainLayout;
		NativeUI::ListView* _listView;

		// Dialogs references
		AddExpenseDialog* _addExpensesDialog;
		AddIncomeDialog* _addIncomeDialog;
	};
}


#endif /* LISTSCREEN_H_ */

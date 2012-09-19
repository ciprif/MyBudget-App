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
 * \file observer.h
 * \author Ciprian Filipas
 * \date Jun 21, 2012
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <MAUtil/String.h>
#include <MAUtil/Vector.h>


namespace Model
{
	class ExpenseObject;
	class IncomeObject;
	struct TimeStruct;
	struct DateStruct;
}

namespace Repositories
{
	class DBManager;
}

namespace GUI
{
	class HomeScreen;
	class ListScreen;
	class SettingsScreen;
}
namespace Logical
{
	class SettingsManager;

	/**
	 * \brief This class represents the observer of the application
	 */
	class Observer
	{
	public:
		/**
		 * \brief Constructor
		 */
		Observer();

		/**
		 * \brief Destructor
		 */
		~Observer() {}

		/**
		 * \brief This function is used to trigger an expense addition
		 * @param amount const double& the amount value
		 * @param category const MAUtil::String& the category value
		 * @param description const MAUtil::String& the description value
		 * @param imgPath const MAUtil::String& the image path value
		 * @param date const DateStruct& the date value
		 * @param time const TimeStruct& the time value
		 */
		void requestExpenseAddition(const double& amount, const MAUtil::String& category, const MAUtil::String& description,
								   const MAUtil::String& imgPath, const Model::DateStruct& date, const Model::TimeStruct& time);

		/**
		 * \brief This function is used to trigger and income addition
		 * @param amount const double& the amount value
		 * @param type const MAUtil::String& the type value
		 * @param description const MAUtil::String& the description value
		 * @param transactionInfo const MAUtil::String& the transaction information value
		 * @param date const DateStruct& the date value
		 * @param time const TimeStruct& the time value
		 */
		void requestIncomeAddition(const double& amount, const MAUtil::String& type, const MAUtil::String& description,
								  const MAUtil::String& transactionInfo, const Model::DateStruct& date, const Model::TimeStruct& time);

		/**
		 * \brief This function is used to trigger a homeScreen update
		 * @param value const double& the value of the transaction
		 * @param isExpense bool this is true if the transaction is an expense, otherwise false
		 * @param category const MAUtil::String& the category of an expense or income
		 */
		void updateScreenNotification(const double& value, bool isExpense, const MAUtil::String& category);

		/**
		 * \brief This function is used to trigger a total budget update
		 */
		void updateTotalBudgetNotification();

		/**
		 * \brief This function is used to trigger a consumed budget update
		 */
		void updateConsumedBudgetNotification();

		/**
		 * \brief This function is used to trigger a debt budget update
		 */
		void updateDebtBudgetNotification();

		/**
		 * \brief Setter for the HomeScreen reference
		 * @param homeScreen GUI::HomeScreen* pointer to the home screen
		 */
		void setHomeScreenRef(GUI::HomeScreen* homeScreen);

		/**
		 * \brief Setter for the ListScreen reference
		 * @param listScreen GUI::ListScreen* pointer the list screen
		 */
		void setListScreenRef(GUI::ListScreen* listScreen);

		/**
		 * \brief Setter for the SettingsScreen reference
		 * @param settingsScreen GUI::SettingsScreen* pointer to the settings screen
		 */
		void setSettingsScreenRef(GUI::SettingsScreen* settingsScreen);

		/**
		 * \brief This function is called from the UI in order to get an updated list of incomes
		 * @return MAUtil::Vector<Model::IncomeObject*>* pointer to the application wide incomes list
		 */
		MAUtil::Vector<Model::IncomeObject*>* incomesListRequest();

		/**
		 * \brief This function is called from the UI in order to get an updated list of expenses
		 * @return MAUtil::Vector<Model::ExpenseObject*>* pointer to the application wide expenses list
		 */
		MAUtil::Vector<Model::ExpenseObject*>* expensesListRequest();

		/**
		 * \brief This function is called from the UI in order to get the updated value of the total budget
		 * @return double
		 */
		double requestTotalBudget() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the consumed budget
		 * @return double
		 */
		double requestConsumedBudget() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the possible debt value
		 * @return double
		 */
		double debtBudgetReqeust() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the coin
		 * @return MAUtil::String&
		 */
		MAUtil::String& requestCoin() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the isShowAll variable
		 * @return MAUtil::String&
		 */
		bool requestIsShowAll() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the isShowMonthly variable
		 * @return MAUtil::String&
		 */
		bool requestIsShowMonthly() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the isShowFromDate variable
		 * @return MAUtil::String&
		 */
		bool requestIsShowFromDate() const;

		/**
		 * \brief This function is called from the UI in order to get the updated value of the reference date
		 * @return Model::DateStruct&
		 */
		Model::DateStruct& requestFromDate();

		/**
		 * \brief This function is called from the UI in order to get the updated value of the possible debt
		 * @return Model::DateStruct&
		 */
		double& requestDebtValue();

		/**
		 * \brief This function is called from the UI in order to get the updated value of the amount
		 * 		  for a certain category
		 * @param category const MAUtil::String& the category value
		 * @return double
		 */
		double requestCategoryAmount(const MAUtil::String& category);

		/**
		 * \brief This function is called from the UI in order to trigger a save settings call
		 * @param isShowAll bool the value of the isShowAll variable
		 * @param isMonthly bool the value of the isShowMonthly variable
		 * @param isFromDate bool the vlaue of the isFromDate variable
		 * @param debtValue const double& the possible debt value
		 * @param date const Model::DateStruct& the reference date
		 * @param coin const MAUtil::String the coin
		 */
		void requestSaveSettings(bool isShowAll, bool isMonthly, bool isFromDate, const double& debtValue, const Model::DateStruct& date, const MAUtil::String& coin);

		/**
		 * \brief This function is called from the UI in order to trigger a clear transaction list call
		 */
		void requestClearTransactionList();
	private:
		/**
		 * \brief This function is used for triggering the process for applying the settings
		 */
		void applySettings();

		// Screen references (pointers)
		GUI::HomeScreen* _homeScreenReference;
		GUI::ListScreen* _listScreenReference;
		GUI::SettingsScreen* _settingsScreenReference;

		// The DB Manager
		Repositories::DBManager* _DBManager;

		// The Settings Manager
		SettingsManager* _settingsManager;

		double _totalBudget;
		double _consumedBudget;
		double _debtBudget;
	};
}

#endif /* OBSERVER_H_ */

/*
 * DBManager.h
 *
 *  Created on: Jun 26, 2012
 *      Author: Cipri
 */

#ifndef DBMANAGER_H_
#define DBMANAGER_H_

#include <MAUtil/Vector.h>
#include <ma.h>
#include <MAUtil/String.h>

#include "../Model/expenseObject.h"
#include "../Model/incomeObject.h"

#define EXPENSES_FILE "expensesDB.txt"
#define INCOMES_FILE "incomesDB.txt"

#define BUFF_SIZE 256

namespace Repositories
{
	class DBManager
	{
	public:
		/**
		 * \brief Constructor
		 */
		DBManager();

		/**
		 * \brief Destructor
		 */
		~DBManager();

		/**
		 * \brief This function is used for creating/reading the expenses file
		 */
		void readExpenses();

		/**
		 * \brief This function is used for creating/reading the incomes file
		 */
		void readIncomes();

		/**
		 * \brief Getter for the number of expenses currently loaded
		 * @return int
		 */
		int getNumberOfExpenses() const;

		/**
		 * \brief Getter for the number of incomes currently loaded
		 * @return int
		 */
		int getNumberOfIncomes() const;

		/**
		 * \brief Gets the ExpenseObject placed at the position index inside the list of expenses
		 * @param index const int& the index of the ExpenseObject
		 * @return Model::ExpenseObject&
		 */
		Model::ExpenseObject& getExpenseObject(const int& index) const;

		/**
		 * \brief Gets the IncomeObject placed at the position index inside the list of incomes
		 * @param index const int& the index of the IncomeObject
		 * @return Model::IncomeObject&
		 */
		Model::IncomeObject& getIncomeObject(const int& index) const;

		/**
		 * \brief This function creates a string representation of the expense object
		 * 	      and appends it to the expenses file
		 * @param obj Model::ExpenseObject* the expense object
		 */
		void addExpense(Model::ExpenseObject* obj);

		/**
		 * \brief This function creates a string representation of the income object
		 * 	      and appends it to the incomes file
		 * @param obj Model::IncomeObject* the incomes object
		 */
		void addIncome(Model::IncomeObject* obj);

		/**
		 * \brief This function returns the total budget value (the amount of all the incomes)
		 * @return double
		 */
		double getTotalBudget();

		/**
		 * \brief This function returns the consumed budget value (the amount of all the expenses)
		 * @return double
		 */
		double getConsumedBudget();

		/**
		 * \brief This function returns the amount of money spent for each category of expenses
		 * @param category const MAUtil::String& the category string
		 * @return double
		 */
		double getCategoryAmount(const MAUtil::String& category);

		/**
		 * \brief This function returns a pointer to the expenses list
		 * @return MAUtil::Vector<Model::ExpenseObject*>*
		 */
		MAUtil::Vector<Model::ExpenseObject*>* getExpenses();

		/**
		 * \brief This function returns a pointer to the incomes list
		 * @return MAUtil::Vector<Model::IncomeObject*>*
		 */
		MAUtil::Vector<Model::IncomeObject*>* getIncomes();

		/**
		 * \brief This function is used for setting the reference date
		 * @param d const Model::DateStruct& the reference date
		 */
		void setDate(const Model::DateStruct& d);

		/**
		 * \brief This function is used for clearing the
		 */
		void clearFiles();
	private:
		/**
		 * \brief This function is used for adding an expense object to the list of expenses
		 * @param amount const double& the amount value
		 * @param category const MAUtil::String& the category value
		 * @param description const MAUtil::String& the description value
		 * @param imgPath const MAUtil::String& the image path value
		 * @param year const int& the year value
		 * @param month const int& the month value
		 * @param day const int& the day value
		 * @param hour const int& the hours value
		 * @param minute const int& the minutes value
		 */
		void addExpenseToList(const double& amount, const MAUtil::String& category, const MAUtil::String& description, const MAUtil::String& imgPath,
							  const int& year, const int& month, const int& day, const int& hour, const int& minute);

		/**
		 * \brief This function is used for adding an income object to the list of incomes
		 * @param amount const double& the amount value
		 * @param type const MAUtil::String& the type value
		 * @param description const MAUtil::String& the description value
		 * @param transactionInfo const MAUtil::String& the transactionInfo value
		 * @param year const int& the year value
		 * @param month const int& the month value
		 * @param day const int& the day value
		 * @param hour const int& the hours value
		 * @param minute const int& the minutes value
		 */
		void addIncomeToList(const double& amount, const MAUtil::String& type, const MAUtil::String& description, const MAUtil::String& transactionInfo,
									  const int& year, const int& month, const int& day, const int& hour, const int& minute);
		// The file handles
		MAHandle _DBExpenses;
		MAHandle _DBIncomes;

		// The expenses/incomes lists
		MAUtil::Vector<Model::ExpenseObject*>* _expenses;
		MAUtil::Vector<Model::IncomeObject*>* _incomes;

		// The file contents are loaded into these variables
		MAUtil::String* _expensesFileContent;
		MAUtil::String* _incomesFileContent;

		// The budget values
		double _totalBudget;
		double _consumedBudget;

		// The reference date
		Model::DateStruct _date;
	};
}

#endif /* DBMANAGER_H_ */

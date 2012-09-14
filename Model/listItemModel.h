/*
 * listItemModel.h
 *
 *  Created on: Jul 4, 2012
 *      Author: Cipri
 */

#ifndef LISTITEMMODEL_H_
#define LISTITEMMODEL_H_

#include "expenseObject.h"
#include "incomeObject.h"
#include <MAUtil/util.h>

namespace Model
{
	class ListItemModel
	{
	public:
		/**
		 * \brief Constructor
		 */
		ListItemModel();

		/**
		 * \brief The copy constructor
		 * @param obj const ListItemModel& the source object
		 */
		ListItemModel(const ListItemModel& obj);

		/**
		 * \brief The destructor
		 */
		~ListItemModel();

		/**
		 * \brief Setter for the expense object
		 * @param obj const ExpenseObject& the new ExpenseObject value
		 */
		void setExpense(const ExpenseObject& obj);

		/**
		 * \brief Setter for the income object
		 * @param obj const IncomeObject& the new IncomeObject value
		 */
		void setIncome(const IncomeObject& obj);

		/**
		 * \brief Getter for the ExpenseObject
		 * @return const ExpenseObject&
		 */
		const ExpenseObject& getExpenseObject() const;

		/**
		 * \brief Getter for the IncomeObject
		 * @return const IncomeObject&
		 */
		const IncomeObject& getIncomeObject() const;

		/**
		 * \brief Returns true if the object contains an expense object
		 * @return bool
		 */
		bool IsExpense() const;

		/**
		 * \brief Returns true if the object contains an income object
		 * @return bool
		 */
		bool IsIncome() const;

		/**
		 * \brief The = operator override
		 * @param obj const ListItemModel& the source object
		 * @return
		 */
		ListItemModel& operator=(const ListItemModel& obj);
	private:
		// Private member data
		ExpenseObject* expense;
		IncomeObject* income;
		bool isExpense;
		bool isIncome;
	};
}


#endif /* LISTITEMMODEL_H_ */

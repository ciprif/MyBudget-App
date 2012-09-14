/*
 * listItemModel.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: Cipri
 */

#include "listItemModel.h"
#include "expenseObject.h"
#include "incomeObject.h"
#include <conprint.h>

namespace Model
{
	/**
	 * \brief Constructor
	 */
	ListItemModel::ListItemModel() : income(NULL), expense(NULL)
	{
		isExpense = false;
		isIncome = false;
	}

	/**
	 * \brief The copy constructor
	 * @param obj const ListItemModel& the source object
	 */
	ListItemModel::ListItemModel(const ListItemModel& obj)
	{
		isExpense = obj.isExpense;
		isIncome = obj.isIncome;
		if(isExpense)
			expense = new ExpenseObject(*obj.expense);
		if(isIncome)
			income = new IncomeObject(*obj.income);
	}

	/**
	 * \brief The destructor
	 */
	ListItemModel::~ListItemModel()
	{
		if(NULL != expense) delete expense;
		if(NULL != income) delete income;
	}

	/**
	 * \brief Setter for the expense object
	 * @param obj const ExpenseObject& the new ExpenseObject value
	 */
	void ListItemModel::setExpense(const ExpenseObject& obj)
	{
		if(!isIncome)
		{
			if(NULL == expense) expense = new ExpenseObject();

			*expense = obj;
			isExpense = true;
		}
	}

	/**
	 * \brief Setter for the income object
	 * @param obj const IncomeObject& the new IncomeObject value
	 */
	void ListItemModel::setIncome(const IncomeObject& obj)
	{
		if(!isExpense)
		{
			if(NULL == income) income = new IncomeObject();

			*income = obj;
			isIncome = true;
		}
	}

	/**
	 * \brief Getter for the ExpenseObject
	 * @return const ExpenseObject&
	 */
	const ExpenseObject& ListItemModel::getExpenseObject() const
	{
		return *expense;
	}

	/**
	 * \brief Getter for the IncomeObject
	 * @return const IncomeObject&
	 */
	const IncomeObject& ListItemModel::getIncomeObject() const
	{
		return *income;
	}

	/**
	 * \brief Returns true if the object contains an expense object
	 * @return bool
	 */
	bool ListItemModel::IsExpense() const
	{
		return isExpense;
	}

	/**
	 * \brief Returns true if the object contains an income object
	 * @return bool
	 */
	bool ListItemModel::IsIncome() const
	{
		return isIncome;
	}

	/**
	 * \brief The = operator override
	 * @param obj const ListItemModel& the source object
	 * @return
	 */
	ListItemModel& ListItemModel::operator=(const ListItemModel& obj)
	{
		delete expense;
		delete income;

		if(obj.isExpense)
		{
			expense = new ExpenseObject(obj.getExpenseObject());
			isExpense = true;
			isIncome = false;
			income = NULL;
		}
		else if(obj.isIncome)
		{
			income = new IncomeObject(obj.getIncomeObject());
			isExpense = false;
			isIncome = true;
			expense = NULL;
		}

		return *this;
	}
}

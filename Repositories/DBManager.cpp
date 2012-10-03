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
 * \file DBManager.cpp
 * \author Ciprian Filipas
 * \date Jun 26, 2012
 */

#include <maapi.h>
#include <MAUtil/String.h>
#include <MAUtil/util.h>
#include <MAFS/File.h>
#include <conprint.h>
#include "../Screens/GUIUtil.h"
#include "../Model/ModelUtil.h"
#include "DBManager.h"

namespace Repositories
{
	/**
	 * \brief Constructor
	 */
	DBManager::DBManager()
	{
		GUI::DeterminePlatform();

		char path[Model::BUFF_SIZE];

		maGetSystemProperty("mosync.path.local", path, Model::BUFF_SIZE);

		_incomesFileCompletePath = new MAUtil::String(path);
		_expensesFileCompletePath = new MAUtil::String(path);

		if(false == GUI::_IPhoneOS) (*_incomesFileCompletePath) += "/";
		(*_incomesFileCompletePath) += INCOMES_FILE;

		if(false == GUI::_IPhoneOS) (*_expensesFileCompletePath) += "/";
		(*_expensesFileCompletePath) += EXPENSES_FILE;

		_DBExpenses = maFileOpen(_expensesFileCompletePath->c_str(), MA_ACCESS_READ_WRITE);
		_DBIncomes = maFileOpen(_incomesFileCompletePath->c_str(), MA_ACCESS_READ_WRITE);

		if(0 == maFileExists(_DBExpenses))
		{
			maFileCreate(_DBExpenses);
		}
		if(0 == maFileExists(_DBIncomes))
		{
			maFileCreate(_DBIncomes);
		}
		maFileClose(_DBExpenses);
		maFileClose(_DBIncomes);
		_expenses = new MAUtil::Vector<Model::ExpenseObject*>();
		_incomes = new MAUtil::Vector<Model::IncomeObject*>();

		_totalBudget = 0.0;
		_consumedBudget = 0.0;
	}

	/**
	 * \brief Destructor
	 */
	DBManager::~DBManager()
	{
		for(int i = 0; i <= _expenses->size(); i++)
			delete (*_expenses)[i];
		for(int i = 0; i <= _incomes->size(); i++)
					delete (*_incomes)[i];
		delete _expenses;
		delete _incomes;
		delete _expensesFileContent;
		delete _incomesFileContent;
	}

	/**
	 * \brief This function is used for creating/reading the expenses file
	 */
	void DBManager::readExpenses()
	{
		_DBExpenses = maFileOpen(_expensesFileCompletePath->c_str(), MA_ACCESS_READ);

		if(0 == maFileExists(_DBExpenses)) return;

		if(NULL != _expensesFileContent)
			delete _expensesFileContent;
		_expensesFileContent = new MAUtil::String("\0", 5*Model::BUFF_SIZE);

		char buffer[Model::BUFF_SIZE];
		bool reading = true;
		int remainingBytes = maFileSize(_DBExpenses);
		int length = Model::BUFF_SIZE;
		while(0 < remainingBytes)
		{
			if(remainingBytes < Model::BUFF_SIZE)
			{
				length = remainingBytes;
			}
			if(0 <= maFileRead(_DBExpenses, &buffer, length) && strlen(buffer) > 0)
			{
				remainingBytes -= length;
				_expensesFileContent->append(buffer, strlen(buffer));
				memset(&buffer, 0, length);
			}
			else break;
		}
		maFileClose(_DBExpenses);

		int offset = 0;
		int position = _expensesFileContent->find("|", offset);
		while(-1 != position)
		{
			MAUtil::String expenseString = _expensesFileContent->substr(offset, position - offset);

			int inner_offset = 0;
			int inner_position = expenseString.find("~", inner_offset);

			MAUtil::Vector<MAUtil::String> tokens;

			while(-1 != inner_position)
			{
				//getting each token of an expense string
				tokens.add(expenseString.substr(inner_offset, inner_position - inner_offset));
				inner_offset = inner_position + 1;
				inner_position = expenseString.findFirstOf('~', inner_offset);
			}
			tokens.add(expenseString.substr(inner_offset, expenseString.length() - inner_offset - 1));

			inner_offset = 0;
			inner_position = tokens[4].find("-", inner_offset);
			//extracting date and time from the last 2 tokens
			int count = 0;
			int year, month, day;

			while(-1 != inner_position)
			{
				int val = 0;
				val = MAUtil::stringToInteger(tokens[4].substr(inner_offset, inner_position - inner_offset), 10);
				inner_offset = inner_position + 1;
				inner_position = tokens[4].findFirstOf('-', inner_offset);
				switch(count)
				{
				case 0:
					year = val;
					break;
				case 1:
					month = val;
					break;
				}
				count++;
			}
			day = MAUtil::stringToInteger(tokens[4].substr(inner_offset, tokens[4].length() - inner_offset), 10);

			inner_offset = 0;
			inner_position = tokens[5].find("-", inner_offset);

			int hour = MAUtil::stringToInteger(tokens[5].substr(inner_offset, inner_position), 10);
			inner_offset = inner_position + 1;
			inner_position = tokens[5].find("-", inner_offset);
			int minute = MAUtil::stringToInteger(tokens[5].substr(inner_offset, tokens[5].length() - inner_offset), 10);
			addExpenseToList(MAUtil::stringToDouble(tokens[0]), tokens[1], tokens[2], tokens[3], year, month, day, hour, minute);

			offset = position + 1;
			position = _expensesFileContent->find("|", offset);
		}

		delete _expensesFileContent;
		_expensesFileContent = NULL;
	}

	/**
	 * \brief This function is used for creating/reading the incomes file
	 */
	void DBManager::readIncomes()
	{
		_DBIncomes = maFileOpen(_incomesFileCompletePath->c_str(), MA_ACCESS_READ);

		if(0 == maFileExists(_DBIncomes)) return;

		if(NULL != _incomesFileContent)
			delete _incomesFileContent;
		_incomesFileContent = new MAUtil::String("\0", 5*Model::BUFF_SIZE);

		char buffer[Model::BUFF_SIZE];
		bool reading = true;
		int remainingBytes = maFileSize(_DBIncomes);
		int length = Model::BUFF_SIZE;

		while(0 < remainingBytes)
		{
			if(remainingBytes < Model::BUFF_SIZE)
			{
				length = remainingBytes;
			}
			if(0 <= maFileRead(_DBIncomes, &buffer, length) && strlen(buffer) > 0)
			{
				remainingBytes -= length;
				_incomesFileContent->append(buffer, strlen(buffer));
				memset(&buffer, 0, length);
			}
			else break;
		}

		maFileClose(_DBIncomes);

		int offset = 0;
		int position = _incomesFileContent->find("|", offset);

		while(-1 != position)
		{
			MAUtil::String incomeString = _incomesFileContent->substr(offset, position - offset);

			int inner_offset = 0;
			int inner_position = incomeString.find("~", inner_offset);

			MAUtil::Vector<MAUtil::String> tokens;

			while(-1 != inner_position)
			{
				//getting each token of an expense string
				tokens.add(incomeString.substr(inner_offset, inner_position - inner_offset));
				inner_offset = inner_position + 1;
				inner_position = incomeString.findFirstOf('~', inner_offset);
			}
			tokens.add(incomeString.substr(inner_offset, incomeString.length() - inner_offset));

			inner_offset = 0;
			inner_position = tokens[4].find("-", inner_offset);

			//extracting date and time from the last 2 tokens
			int count = 0;
			int year, month, day;
			while(-1 != inner_position)
			{
				int val = 0;
				val = MAUtil::stringToInteger(tokens[4].substr(inner_offset, inner_position - inner_offset), 10);
				inner_offset = inner_position + 1;
				inner_position = tokens[4].findFirstOf('-', inner_offset);
				switch(count)
				{
				case 0:
					year = val;
					break;
				case 1:
					month = val;
					break;
				}
				count++;
			}
			day = MAUtil::stringToInteger(tokens[4].substr(inner_offset, tokens[4].length() - inner_offset), 10);

			inner_offset = 0;
			inner_position = tokens[5].find("-", inner_offset);

			int hour = MAUtil::stringToInteger(tokens[5].substr(inner_offset, inner_position), 10);
			inner_offset = inner_position + 1;
			inner_position = tokens[5].find("-", inner_offset);
			int minute = MAUtil::stringToInteger(tokens[5].substr(inner_offset, tokens[5].length() - inner_offset), 10);

			addIncomeToList(MAUtil::stringToDouble(tokens[0]), tokens[1], tokens[2], tokens[3], year, month, day, hour, minute);

			offset = position + 1;
			position = _incomesFileContent->find("|", offset);
		}

		delete _incomesFileContent;
		_incomesFileContent = NULL;
	}

	/**
	 * \brief Getter for the number of expenses currently loaded
	 * @return int
	 */
	int DBManager::getNumberOfExpenses() const
	{
		return _expenses->size();
	}

	/**
	 * \brief Getter for the number of incomes currently loaded
	 * @return int
	 */
	int DBManager::getNumberOfIncomes() const
	{
		return _incomes->size();
	}

	/**
	 * \brief Gets the ExpenseObject placed at the position index inside the list of expenses
	 * @param index const int& the index of the ExpenseObject
	 * @return Model::ExpenseObject&
	 */
	Model::ExpenseObject& DBManager::getExpenseObject(const int& index) const
	{
		if(index < _expenses->size())
		{
			return *((*_expenses)[index]);
		}
	}

	/**
	 * \brief Gets the IncomeObject placed at the position index inside the list of incomes
	 * @param index const int& the index of the IncomeObject
	 * @return Model::IncomeObject&
	 */
	Model::IncomeObject& DBManager::getIncomeObject(const int& index) const
	{
		if(index < _incomes->size())
		{
			return (*(*_incomes)[index]);
		}
	}

	/**
	 * \brief This function creates a string representation of the expense object
	 * 	      and appends it to the expenses file
	 * @param obj Model::ExpenseObject* the expense object
	 */
	void DBManager::addExpense(Model::ExpenseObject* obj)
	{
		_DBExpenses = maFileOpen(_expensesFileCompletePath->c_str(), MA_ACCESS_READ_WRITE);
		if(0 == maFileExists(_DBExpenses)) return;

		MAUtil::String expenseContent;
		expenseContent.append(MAUtil::doubleToString(obj->getAmount()).c_str(), MAUtil::doubleToString(obj->getAmount()).length());
		expenseContent.append("~", 1);
		expenseContent.append(obj->getCategory().c_str(), obj->getCategory().length());
		expenseContent.append("~", 1);
		expenseContent.append(obj->getDescription().c_str(), obj->getDescription().length());
		expenseContent.append("~", 1);
		expenseContent.append(obj->getImagePath().c_str(), obj->getImagePath().length());
		expenseContent.append("~", 1);
		MAUtil::String date = Model::DateStructToString(obj->getDate());
		expenseContent.append(date.c_str(), date.length());
		expenseContent.append("~", 1);
		MAUtil::String time = Model::TimeStructToString(obj->getTime());
		expenseContent.append(time.c_str(), time.length());
		expenseContent.append("|", 1);

		maFileSeek(_DBExpenses, 0, SEEK_END);
		maFileWrite(_DBExpenses, expenseContent.c_str(), expenseContent.length());
		maFileClose(_DBExpenses);

		_expenses->add(obj);
		_consumedBudget += (obj->getAmount());
	}

	/**
	 * \brief This function creates a string representation of the income object
	 * 	      and appends it to the incomes file
	 * @param obj Model::IncomeObject* the incomes object
	 */
	void DBManager::addIncome(Model::IncomeObject* obj)
	{
		_DBIncomes = maFileOpen(_incomesFileCompletePath->c_str(), MA_ACCESS_READ_WRITE);
		if(0 == maFileExists(_DBIncomes)) return;
		MAUtil::String incomeContent;
		incomeContent.append(MAUtil::doubleToString(obj->getAmount()).c_str(), MAUtil::doubleToString(obj->getAmount()).length());
		incomeContent.append("~", 1);
		incomeContent.append(obj->getType().c_str(), obj->getType().length());
		incomeContent.append("~", 1);
		incomeContent.append(obj->getDescription().c_str(), obj->getDescription().length());
		incomeContent.append("~", 1);
		incomeContent.append(obj->getTransactionInformation().c_str(), obj->getTransactionInformation().length());
		incomeContent.append("~", 1);
		MAUtil::String date = Model::DateStructToString(obj->getDate());
		incomeContent.append(date.c_str(), date.length());
		incomeContent.append("~", 1);
		MAUtil::String time = Model::TimeStructToString(obj->getTime());
		incomeContent.append(time.c_str(), time.length());
		incomeContent.append("|", 1);

		maFileSeek(_DBIncomes, 0, SEEK_END);
		maFileWrite(_DBIncomes, incomeContent.c_str(), incomeContent.length());
		maFileClose(_DBIncomes);

		_incomes->add(obj);
		_totalBudget += (obj->getAmount());
	}

	/**
	 * \brief This function returns the total budget value (the amount of all the incomes)
	 * @return double
	 */
	double DBManager::getTotalBudget()
	{
		_totalBudget = 0.0;
		for(int i = 0; i < _incomes->size(); i++)
		{
			if(Model::CompareDateObjects(_date, (*_incomes)[i]->getDate()) == -1)
			{
				_totalBudget += (*_incomes)[i]->getAmount();
			}
		}
		return _totalBudget;
	}

	/**
	 * \brief This function returns the consumed budget value (the amount of all the expenses)
	 * @return double
	 */
	double DBManager::getConsumedBudget()
	{
		_consumedBudget = 0.0;
		for(int i = 0; i < _expenses->size(); i++)
		{
			if(Model::CompareDateObjects(_date, (*_expenses)[i]->getDate()) == -1 ||
			   Model::CompareDateObjects(_date, (*_expenses)[i]->getDate()) == 0)
			{
				_consumedBudget += (*_expenses)[i]->getAmount();
			}
		}
		return _consumedBudget;
	}

	/**
	 * \brief This function returns the amount of money spent for each category of expenses
	 * @param category const MAUtil::String& the category string
	 * @return double
	 */
	double DBManager::getCategoryAmount(const MAUtil::String& category)
	{
		double value = 0.0;
		for(int i = 0; i < _expenses->size(); i++)
		{
			if(strcmp((*_expenses)[i]->getCategory().c_str(), category.c_str()) == 0 &&
			   (Model::CompareDateObjects(_date, (*_expenses)[i]->getDate()) == -1 ||
			    Model::CompareDateObjects(_date, (*_expenses)[i]->getDate()) == 0))
			{
				value += (*_expenses)[i]->getAmount();
			}
		}
		return value;
	}

	/**
	 * \brief This function returns a pointer to the expenses list
	 * @return MAUtil::Vector<Model::ExpenseObject*>*
	 */
	MAUtil::Vector<Model::ExpenseObject*>* DBManager::getExpenses()
	{
		return _expenses;
	}

	/**
	 * \brief This function returns a pointer to the incomes list
	 * @return MAUtil::Vector<Model::IncomeObject*>*
	 */
	MAUtil::Vector<Model::IncomeObject*>* DBManager::getIncomes()
	{
		return _incomes;
	}

	/**
	 * \brief This function is used for setting the reference date
	 * @param d const Model::DateStruct& the reference date
	 */
	void DBManager::setDate(const Model::DateStruct& d)
	{
		_date._day = d._day;
		_date._mounth = d._mounth;
		_date._year = d._year;
	}

	/**
	 * \brief This function is used for clearing the
	 */
	void DBManager::clearFiles()
	{
		_DBIncomes = maFileOpen(_incomesFileCompletePath->c_str(), MA_ACCESS_READ_WRITE);
		maFileTruncate(_DBIncomes, 0);
		maFileClose(_DBIncomes);

		_DBExpenses = maFileOpen(_expensesFileCompletePath->c_str(), MA_ACCESS_READ_WRITE);
		maFileTruncate(_DBExpenses, 0);
		maFileClose(_DBExpenses);

		_incomes->clear();
		_expenses->clear();
	}

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
	void DBManager::addExpenseToList(const double& amount, const MAUtil::String& category, const MAUtil::String& description, const MAUtil::String& imgPath,
								  const int& year, const int& month, const int& day, const int& hour, const int& minute)
	{
		Model::ExpenseObject* obj = new Model::ExpenseObject(amount, category, description, imgPath, year, month, day, hour, minute);
		_expenses->add(obj);
	}

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
	void DBManager::addIncomeToList(const double& amount, const MAUtil::String& type, const MAUtil::String& description, const MAUtil::String& transactionInfo,
								  const int& year, const int& month, const int& day, const int& hour, const int& minute)
	{
		Model::IncomeObject* obj = new Model::IncomeObject(amount, type, description, transactionInfo, year, month, day, hour, minute);
		_incomes->add(obj);
	}
}

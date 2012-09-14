/*
 * observer.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: Cipri
 */

#include "../Screens/homeScreen.h"
#include "../Screens/listScreen.h"
#include "../Screens/settingsScreen.h"
#include "observer.h"
#include "../Repositories/DBManager.h"
#include "settingsManager.h"

namespace Logical
{
	/**
	 * \brief Constructor
	 */
	Observer::Observer()
	{
		_DBManager = new Repositories::DBManager();
		_settingsManager = new Logical::SettingsManager();

		_DBManager->readExpenses();
		_DBManager->readIncomes();
		_totalBudget = _DBManager->getTotalBudget();
		_consumedBudget = _DBManager->getConsumedBudget();
	}

	/**
	 * \brief This function is used to trigger an expense addition
	 * @param amount const double& the amount value
	 * @param category const MAUtil::String& the category value
	 * @param description const MAUtil::String& the description value
	 * @param imgPath const MAUtil::String& the image path value
	 * @param date const DateStruct& the date value
	 * @param time const TimeStruct& the time value
	 */
	void Observer::requestExpenseAddition(const double& amount, const MAUtil::String& category, const MAUtil::String& description,
										 const MAUtil::String& imgPath, const Model::DateStruct& date, const Model::TimeStruct& time)
	{
		Model::ExpenseObject* obj = new Model::ExpenseObject();
		obj->setAmount(amount);
		obj->setCategory(category);
		obj->setDescription(description);
		obj->setImagePath(imgPath);
		obj->setDate(date);
		obj->setTime(time);

		updateScreenNotification(obj->getAmount(), true, obj->getCategory());
		if(NULL != _listScreenReference)
		{
			_listScreenReference->addExpenseNotification(*obj);
		}
		_DBManager->addExpense(obj);
		_consumedBudget = _DBManager->getConsumedBudget();
	}

	/**
	 * \brief This function is used to trigger and income addition
	 * @param amount const double& the amount value
	 * @param type const MAUtil::String& the type value
	 * @param description const MAUtil::String& the description value
	 * @param transactionInfo const MAUtil::String& the transaction information value
	 * @param date const DateStruct& the date value
	 * @param time const TimeStruct& the time value
	 */
	void Observer::requestIncomeAddition(const double& amount, const MAUtil::String& type, const MAUtil::String& description,
									    const MAUtil::String& transactionInfo, const Model::DateStruct& date, const Model::TimeStruct& time)
	{
		Model::IncomeObject* obj = new Model::IncomeObject();
		obj->setAmount(amount);
		obj->setType(type);
		obj->setDescription(description);
		obj->setTransactionInformation(transactionInfo);
		obj->setDate(date);
		obj->setTime(time);

		updateScreenNotification(obj->getAmount(), false, "");
		if(NULL != _listScreenReference)
		{
			_listScreenReference->addIncomeNotification(*obj);
		}
		_DBManager->addIncome(obj);
		_totalBudget = _DBManager->getTotalBudget();
	}

	/**
	 * \brief This function is used to trigger a homeScreen update
	 * @param value const double& the value of the transaction
	 * @param isExpense bool this is true if the transaction is an expense, otherwise false
	 * @param category const MAUtil::String& the category of an expense or income
	 */
	void Observer::updateScreenNotification(const double& value, bool isExpense, const MAUtil::String& category)
	{
		_homeScreenReference->updateBudgetValues(value, isExpense, category);
	}

	/**
	 * \brief This function is used to trigger a total budget update
	 */
	void Observer::updateTotalBudgetNotification()
	{
		_homeScreenReference->updateTotalBudget(_totalBudget);
	}

	/**
	 * \brief This function is used to trigger a consumed budget update
	 */
	void Observer::updateConsumedBudgetNotification()
	{
		_homeScreenReference->updateConsumedBudget(_consumedBudget);
	}

	/**
	 * \brief This function is used to trigger a debt budget update
	 */
	void Observer::updateDebtBudgetNotification()
	{
		_homeScreenReference->updateDebtBudget(_settingsManager->getDebtValue());
	}

	/**
	 * \brief Setter for the HomeScreen reference
	 * @param homeScreen GUI::HomeScreen* pointer to the home screen
	 */
	void Observer::setHomeScreenRef(GUI::HomeScreen* homeScreen)
	{
		_homeScreenReference = homeScreen;
		_homeScreenReference->setObserver(this);
	}

	/**
	 * \brief Setter for the ListScreen reference
	 * @param listScreen GUI::ListScreen* pointer the list screen
	 */
	void Observer::setListScreenRef(GUI::ListScreen* listScreen)
	{
		_listScreenReference = listScreen;
		_listScreenReference->setObserver(this);
	}

	/**
	 * \brief Setter for the SettingsScreen reference
	 * @param settingsScreen GUI::SettingsScreen* pointer to the settings screen
	 */
	void Observer::setSettingsScreenRef(GUI::SettingsScreen* settingsScreen)
	{
		_settingsScreenReference = settingsScreen;
		_settingsScreenReference->setObserver(this);

		applySettings();
	}

	/**
	 * \brief This function is called from the UI in order to get an updated list of incomes
	 * @return MAUtil::Vector<Model::IncomeObject*>* pointer to the application wide incomes list
	 */
	MAUtil::Vector<Model::IncomeObject*>* Observer::incomesListRequest()
	{
		return _DBManager->getIncomes();
	}

	/**
	 * \brief This function is called from the UI in order to get an updated list of expenses
	 * @return MAUtil::Vector<Model::ExpenseObject*>* pointer to the application wide expenses list
	 */
	MAUtil::Vector<Model::ExpenseObject*>* Observer::expensesListRequest()
	{
		return _DBManager->getExpenses();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the total budget
	 * @return double
	 */
	double Observer::requestTotalBudget() const
	{
		return _totalBudget;
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the consumed budget
	 * @return double
	 */
	double Observer::requestConsumedBudget() const
	{
		return _consumedBudget;
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the possible debt value
	 * @return double
	 */
	double Observer::debtBudgetReqeust() const
	{
		return 500;
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the coin
	 * @return MAUtil::String&
	 */
	MAUtil::String& Observer::requestCoin() const
	{
		return _settingsManager->getCoin();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the isShowAll variable
	 * @return MAUtil::String&
	 */
	bool Observer::requestIsShowAll() const
	{
		return _settingsManager->isShowAll();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the isShowMonthly variable
	 * @return MAUtil::String&
	 */
	bool Observer::requestIsShowMonthly() const
	{
		return _settingsManager->isShowMonthly();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the isShowFromDate variable
	 * @return MAUtil::String&
	 */
	bool Observer::requestIsShowFromDate() const
	{
		return _settingsManager->isShowFromDate();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the reference date
	 * @return Model::DateStruct&
	 */
	Model::DateStruct& Observer::requestFromDate()
	{
		return _settingsManager->getDate();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the possible debt
	 * @return Model::DateStruct&
	 */
	double& Observer::requestDebtValue()
	{
		return _settingsManager->getDebtValue();
	}

	/**
	 * \brief This function is called from the UI in order to get the updated value of the amount
	 * 		  for a certain category
	 * @param category const MAUtil::String& the category value
	 * @return double
	 */
	double Observer::requestCategoryAmount(const MAUtil::String& category)
	{
		return _DBManager->getCategoryAmount(category);
	}

	/**
	 * \brief This function is called from the UI in order to trigger a save settings call
	 * @param isShowAll bool the value of the isShowAll variable
	 * @param isMonthly bool the value of the isShowMonthly variable
	 * @param isFromDate bool the vlaue of the isFromDate variable
	 * @param debtValue const double& the possible debt value
	 * @param date const Model::DateStruct& the reference date
	 * @param coin const MAUtil::String the coin
	 */
	void Observer::requestSaveSettings(bool isShowAll, bool isMonthly, bool isFromDate, const double& debtValue, const Model::DateStruct& date, const MAUtil::String& coin)
	{
		_settingsManager->setCoin(coin);
		_settingsManager->setDateFrom(date);
		_settingsManager->setDebtValue(debtValue);
		_settingsManager->setIsShowAll(isShowAll);
		_settingsManager->setIsShowFromDate(isFromDate);
		_settingsManager->setIsShowMontly(isMonthly);

		_settingsManager->ApplySettings();

		applySettings();
	}

	/**
	 * \brief This function is called from the UI in order to trigger a clear transaction list call
	 */
	void Observer::requestClearTransactionList()
	{
		_listScreenReference->clearList();
		_DBManager->clearFiles();

		_totalBudget = 0.0;
		_consumedBudget = 0.0;
		_homeScreenReference->updateValues();
	}

	/**
	 * \brief This function is used for triggering the process for applying the settings
	 */
	void Observer::applySettings()
	{
		_DBManager->setDate(requestFromDate());

		if(NULL != _homeScreenReference)
		{
			updateDebtBudgetNotification();
			_homeScreenReference->setCoin(requestCoin());

			MAUtil::Vector<Model::IncomeObject*>* incomes =  _DBManager->getIncomes();
			MAUtil::Vector<Model::ExpenseObject*>* expenses =  _DBManager->getExpenses();

			_totalBudget = 0.0;
			_consumedBudget = 0.0;

			for(int i = 0; i < incomes->size(); i++)
			{
				if(Model::CompareDateObjects(requestFromDate(), (*incomes)[i]->getDate()) == -1 || Model::CompareDateObjects(requestFromDate(), (*incomes)[i]->getDate()) == 0)
				{
					_totalBudget += (*incomes)[i]->getAmount();
				}
			}

			for(int i = 0; i < expenses->size(); i++)
			{
				if(Model::CompareDateObjects(requestFromDate(), (*expenses)[i]->getDate()) == -1 || Model::CompareDateObjects(requestFromDate(), (*expenses)[i]->getDate()) == 0)
				{
					_consumedBudget += (*expenses)[i]->getAmount();
				}
			}

			_homeScreenReference->updateValues();
		}

		if(NULL != _listScreenReference)
		{
			_listScreenReference->setDateFrom(requestFromDate());
			_listScreenReference->updateDebtValue();
			_listScreenReference->setCoin(requestCoin());
			_listScreenReference->clearList();
			_listScreenReference->populateIncomesList();
			_listScreenReference->populateExpensesList();
		}
	}
}



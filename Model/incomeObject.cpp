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
 * \file incomeObject.cpp
 * \author Ciprian Filipas
 * \date Jun 13, 2012
 */

#include "incomeObject.h"
#include "util.h"

namespace Model
{
	/**
	 * \brief The constructor
	 */
	IncomeObject::IncomeObject()
	{
		_amount = 0.0;
		_description = new MAUtil::String("");
		_incomeType = new MAUtil::String("Other");
		_transactionInformation = new MAUtil::String("");
	}

	/**
	 * \brief The parameter constructor
	 * @param amount const double& amount value
	 * @param type const MAUtil::String& the type
	 * @param description const MAUtil::String& the description
	 * @param transactionInformation const MAUtil::String& the transactionInformation
	 * @param year const int& the year value
	 * @param month const int& the month value
	 * @param day const int& the day value
	 * @param hour const int& the hours value
	 * @param minute const int& the minutes value
	 */
	IncomeObject::IncomeObject(const double& amount, const MAUtil::String& type, const MAUtil::String& description,
			 	 	 	 	  const MAUtil::String& transactionInformation, const int& year, const int& month, const int& day, const int& hour, const int& minute)
	{
		_amount = amount;
		_description = new MAUtil::String(description);
		_incomeType = new MAUtil::String(type);
		_transactionInformation = new MAUtil::String(transactionInformation);
		_date._year = year;
		_date._mounth = month;
		_date._day = day;
		_time._hour = hour;
		_time._minutes = minute;
	}

	/**
	 * \brief The copy constructor
	 * @param obj const IncomeObject& the source object
	 */
	IncomeObject::IncomeObject(const IncomeObject& obj)
	{
		_amount = obj._amount;
		_incomeType = new MAUtil::String(*(obj._incomeType));
		_transactionInformation = new MAUtil::String(*(obj._transactionInformation));
		_description =new MAUtil::String(*(obj._description));
		_date._day = obj._date._day;
		_date._year = obj._date._year;
		_date._mounth = obj._date._mounth;
		_time._hour = obj._time._hour;
		_time._minutes = obj._time._minutes;
	}

	/**
	 * \brief Destructor
	 */
	IncomeObject::~IncomeObject()
	{
		delete _description;
		delete _transactionInformation;
		delete _incomeType;
	}

	/**
	 * \brief Getter for the type
	 * @return const MAUtil::String& the type value
	 */
	const MAUtil::String& IncomeObject::getType() const
	{
		return *_incomeType;
	}

	/**
	 * \brief Getter for the amount
	 * @return const double& the ammount value
	 */
	const double& IncomeObject::getAmount() const
	{
		return _amount;
	}

	/**
	 * \brief Getter for the description
	 * @return const MAUtil::String& the description
	 */
	const MAUtil::String& IncomeObject::getDescription() const
	{
		return *_description;
	}

	/**
	 * \brief Getter for the image path
	 * @return const MAUtil::String& the path
	 */
	const MAUtil::String& IncomeObject::getTransactionInformation() const
	{
		return *_transactionInformation;
	}

	/**
	 * \brief Getter for the date object
	 * @return const DateStruct& the date
	 */
	void IncomeObject::setType(const MAUtil::String& type)
	{
		*_incomeType = type;
	}

	/**
	 * \brief Getter for the time object
	 * @return const TimeStruct& the time
	 */
	const TimeStruct& IncomeObject::getTime() const
	{
		return _time;
	}

	/**
	 * \brief Setter for the type
	 * @param amount const MAUtil::String& the new type value
	 */
	const DateStruct& IncomeObject::getDate() const
	{
		return _date;
	}

	/**
	 * \brief Setter for the amount
	 * @param amount const double& the new amount value
	 */
	void IncomeObject::setAmount(const double& amount)
	{
		_amount = amount;
	}

	/**
	 * \brief Setter for the description
	 * @param description const MAUtil::String& the new description value
	 */
	void IncomeObject::setDescription(const MAUtil::String& description)
	{
		*_description = description;
	}

	/**
	 * \brief Setter for the transaction information
	 * @param info const MAUtil::String& the new transaction information value
	 */
	void IncomeObject::setTransactionInformation(const MAUtil::String& transactionInfo)
	{
		delete _transactionInformation;
		_transactionInformation = new MAUtil::String(transactionInfo);
	}

	/**
	 * \brief Setter for the date object
	 * @param date const DateStruct& the new date
	 */
	void IncomeObject::setDate(const DateStruct& date)
	{
		_date = date;
	}

	/**
	 * \brief Setter for the time object
	 * @param time const TimeStruct& the new time
	 */
	void IncomeObject::setTime(const TimeStruct& time)
	{
		_time = time;
	}

	/**
	 * \brief The = operator override
	 * @param obj const IncomeObject& the source object
	 * @return IncomeObject& reference to the copied object
	 */
	IncomeObject& IncomeObject::operator=(const IncomeObject& obj)
	{
		_amount = obj._amount;

		delete _incomeType;
		delete _transactionInformation;
		delete _description;

		_incomeType = new MAUtil::String(*(obj._incomeType));
		_transactionInformation = new MAUtil::String(*(obj._transactionInformation));
		_description =new MAUtil::String(*(obj._description));

		_date._day = obj._date._day;
		_date._year = obj._date._year;
		_date._mounth = obj._date._mounth;
		_time._hour = obj._time._hour;
		_time._minutes = obj._time._minutes;

		return *this;
	}
}




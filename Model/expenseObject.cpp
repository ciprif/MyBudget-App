/*
 * expenseObject.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: Cipri
 */

#include "expenseObject.h"

namespace Model
{
	/**
	 * \brief Constructor
	 */
	ExpenseObject::ExpenseObject()
	{
		_amount = 0.0;
		_description = new MAUtil::String("");
		_category = new MAUtil::String("Other");
		_imagePath = new MAUtil::String("");
	}

	/**
	 * \brief Parameter constructor
	 * @param amount const double& the amount value
	 * @param category const MAUtil::String& the category of the expense
	 * @param description const MAUtil::String& the description of the expense
	 * @param imgPath const MAUtil::String& the image path
	 * @param year const int& the year of the expense
	 * @param month const int& the month of the expense
	 * @param day const int& the day of the expense
	 * @param hour const int& the hour of the expense
	 * @param minute const int& the minute of the expense
	 */
	ExpenseObject::ExpenseObject(const double& amount, const MAUtil::String& category, const MAUtil::String& description, const MAUtil::String& imgPath,
			  const int& year, const int& month, const int& day, const int& hour, const int& minute)
	{
		_amount = amount;
		_description = new MAUtil::String(description);
		_category = new MAUtil::String(category);
		_imagePath = new MAUtil::String(imgPath);
		_date._year = year;
		_date._mounth = month;
		_date._day = day;
		_time._hour = hour;
		_time._minutes = minute;
	}

	/**
	 * \brief The copy constructor
	 * @param const ExpenseObject& the source object
	 */
	ExpenseObject::ExpenseObject(const ExpenseObject& obj)
	{
		_amount = obj._amount;
		_description = new MAUtil::String(*(obj._description));
		_category = new MAUtil::String(*(obj._category));
		_imagePath =new MAUtil::String(*(obj._imagePath));
		_date._day = obj._date._day;
		_date._year = obj._date._year;
		_date._mounth = obj._date._mounth;
		_time._hour = obj._time._hour;
		_time._minutes = obj._time._minutes;
	}

	/**
	 * \brief Destructor
	 */
	ExpenseObject::~ExpenseObject()
	{
		delete _description;
		delete _imagePath;
		delete _category;
	}

	/**
	 * \brief Getter for the category
	 * @return const MAUtil::String& the category value
	 */
	const MAUtil::String& ExpenseObject::getCategory() const
	{
		return *_category;
	}

	/**
	 * \brief Getter for the amount
	 * @return const double& the ammount value
	 */
	const double& ExpenseObject::getAmount() const
	{
		return _amount;
	}

	/**
	 * \brief Getter for the description
	 * @return const MAUtil::String& the description
	 */
	const MAUtil::String& ExpenseObject::getDescription() const
	{
		return *_description;
	}

	/**
	 * \brief Getter for the image path
	 * @return const MAUtil::String& the path
	 */
	const MAUtil::String& ExpenseObject::getImagePath() const //to be discussed
	{
		return *_imagePath;
	}

	/**
	 * \brief Getter for the date object
	 * @return const DateStruct& the date
	 */
	const DateStruct& ExpenseObject::getDate() const
	{
		return _date;
	}

	/**
	 * \brief Getter for the time object
	 * @return const TimeStruct& the time
	 */
	const TimeStruct& ExpenseObject::getTime() const
	{
		return _time;
	}

	/**
	 * \brief Setter for the category
	 * @param category const MAUtil::String& the new category value
	 */
	void ExpenseObject::setCategory(const MAUtil::String& category)
	{
		delete _category;
		_category = new MAUtil::String(category);
	}

	/**
	 * \brief Setter for the amount
	 * @param amount const double& the new amount value
	 */
	void ExpenseObject::setAmount(const double& amount)
	{
		_amount = amount;
	}

	/**
	 * \brief Setter for the description
	 * @param description const MAUtil::String& the new description value
	 */
	void ExpenseObject::setDescription(const MAUtil::String& description)
	{
		delete _description;
		_description = new MAUtil::String(description);
	}

	/**
	 * \brief Setter for the image path
	 * @param path const MAUtil::String& the new image path value
	 */
	void ExpenseObject::setImagePath(const MAUtil::String& path)
	{
		delete _imagePath;
		_imagePath = new MAUtil::String(path);
	}

	/**
	 * \brief Setter for the date object
	 * @param date const DateStruct& the new date
	 */
	void ExpenseObject::setDate(const DateStruct& date)
	{
		_date = date;
	}


	/**
	 * \brief Setter for the time object
	 * @param time const TimeStruct& the new time
	 */
	void ExpenseObject::setTime(const TimeStruct& time)
	{
		_time = time;
	}

	/**
	 * \brief The = operator override
	 * @param obj const ExpenseObject& the source object
	 * @return ExpenseObject& reference to the copied object
	 */
	ExpenseObject& ExpenseObject::operator=(const ExpenseObject& obj)
	{
		_amount = obj._amount;
		delete _description;
		delete _category;
		delete _imagePath;

		_description = new MAUtil::String(*(obj._description));
		_category = new MAUtil::String(*(obj._category));
		_imagePath =new MAUtil::String(*(obj._imagePath));

		_date._day = obj._date._day;
		_date._year = obj._date._year;
		_date._mounth = obj._date._mounth;
		_time._hour = obj._time._hour;
		_time._minutes = obj._time._minutes;

		return *this;
	}
}

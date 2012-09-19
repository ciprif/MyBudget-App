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
 * \file expenseObject.h
 * \author Ciprian Filipas
 * \date Jun 21, 2012
 */

#ifndef EXPENSEOBJECT_H_
#define EXPENSEOBJECT_H_

#include <MAUtil/String.h>
#include "ModelUtil.h"

namespace Model
{
	/**
	 * \brief This class represents the model of an expense
	 */
	class ExpenseObject
	{
	public:
		/**
		 * \brief Constructor
		 */
		ExpenseObject();

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
		ExpenseObject(const double& amount, const MAUtil::String& category, const MAUtil::String& description, const MAUtil::String& imgPath,
					  const int& year, const int& month, const int& day, const int& hour, const int& minute);

		/**
		 * \brief The copy constructor
		 * @param const ExpenseObject& the source object
		 */
		ExpenseObject(const ExpenseObject&);

		/**
		 * \brief Destructor
		 */
		~ExpenseObject();

		/**
		 * \brief Getter for the category
		 * @return const MAUtil::String& the category value
		 */
		const MAUtil::String& getCategory() const;

		/**
		 * \brief Getter for the amount
		 * @return const double& the ammount value
		 */
		const double& getAmount() const;

		/**
		 * \brief Getter for the description
		 * @return const MAUtil::String& the description
		 */
		const MAUtil::String& getDescription() const;

		/**
		 * \brief Getter for the image path
		 * @return const MAUtil::String& the path
		 */
		const MAUtil::String& getImagePath() const;

		/**
		 * \brief Getter for the date object
		 * @return const DateStruct& the date
		 */
		const DateStruct& getDate() const;

		/**
		 * \brief Getter for the time object
		 * @return const TimeStruct& the time
		 */
		const TimeStruct& getTime() const;

		/**
		 * \brief Setter for the category
		 * @param category const MAUtil::String& the new category value
		 */
		void setCategory(const MAUtil::String& category);

		/**
		 * \brief Setter for the amount
		 * @param amount const double& the new amount value
		 */
		void setAmount(const double& amount);

		/**
		 * \brief Setter for the description
		 * @param description const MAUtil::String& the new description value
		 */
		void setDescription(const MAUtil::String& description);

		/**
		 * \brief Setter for the image path
		 * @param path const MAUtil::String& the new image path value
		 */
		void setImagePath(const MAUtil::String& path);

		/**
		 * \brief Setter for the date object
		 * @param date const DateStruct& the new date
		 */
		void setDate(const DateStruct& date);

		/**
		 * \brief Setter for the time object
		 * @param time const TimeStruct& the new time
		 */
		void setTime(const TimeStruct& time);

		/**
		 * \brief The = operator override
		 * @param obj const ExpenseObject& the source object
		 * @return ExpenseObject& reference to the copied object
		 */
		ExpenseObject& operator=(const ExpenseObject& obj);
	private:
		//private member data
		MAUtil::String* _category;
		MAUtil::String* _description;
		MAUtil::String* _imagePath;
		double _amount;

		DateStruct _date;
		TimeStruct _time;
	};
}
#endif /* EXPENSEOBJECT_H_ */

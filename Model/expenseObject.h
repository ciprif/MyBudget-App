/*
 * expenseObject.h
 *
 *  Created on: Jun 21, 2012
 *      Author: Cipri
 */

#ifndef EXPENSEOBJECT_H_
#define EXPENSEOBJECT_H_

#include <MAUtil/String.h>
#include "util.h"

namespace Model
{
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

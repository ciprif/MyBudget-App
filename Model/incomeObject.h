/*
 * incomeObject.h
 *
 *  Created on: Jun 13, 2012
 *      Author: Cipri
 */

#ifndef INCOMEOBJECT_H_
#define INCOMEOBJECT_H_

#include <MAUtil/String.h>
#include "util.h"

namespace Model
{
	class IncomeObject
	{
	public:
		/**
		 * \brief The constructor
		 */
		IncomeObject();

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
		IncomeObject(const double& amount, const MAUtil::String& type, const MAUtil::String& description,
					 const MAUtil::String& transactionInformation, const int& year, const int& month, const int& day, const int& hour, const int& minute);

		/**
		 * \brief The copy constructor
		 * @param obj const IncomeObject& the source object
		 */
		IncomeObject(const IncomeObject& obj);

		/**
		 * \brief Destructor
		 */
		~IncomeObject();

		/**
		 * \brief Getter for the type
		 * @return const MAUtil::String& the type value
		 */
		const MAUtil::String& getType() const;

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
		const MAUtil::String& getTransactionInformation() const;

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
		 * \brief Setter for the type
		 * @param amount const MAUtil::String& the new type value
		 */
		void setType(const MAUtil::String& type);

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
		 * \brief Setter for the transaction information
		 * @param info const MAUtil::String& the new transaction information value
		 */
		void setTransactionInformation(const MAUtil::String& info);

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
		 * @param obj const IncomeObject& the source object
		 * @return IncomeObject& reference to the copied object
		 */
		IncomeObject& operator=(const IncomeObject& obj);
	private:
		// Private member data
		MAUtil::String* _description;
		MAUtil::String* _incomeType;
		MAUtil::String* _transactionInformation;
		double _amount;

		DateStruct _date;
		TimeStruct _time;
	};
}


#endif /* INCOMEOBJECT_H_ */

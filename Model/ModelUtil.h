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
 * \file ModelUtil.h
 * \author Ciprian Filipas
 * \date Sep 19, 2012
 */

#ifndef MODELUTIL_H_
#define MODELUTIL_H_

#include <MAUtil/String.h>
#include <MAUtil/util.h>
#include <matime.h>

namespace Model
{
	// The number of expenses categories
	const int NO_OF_CATEGORIES = 5;

	// The number of incomes type
	const int NO_OF_INCOMES = 4;

	// The list of income types
	const MAUtil::String INCOME_TYPES_LIST[] = { "Bank Transfer", "Salary", "Debt", "Other" };

	// The list of expense categories
	const MAUtil::String CATEGORY_LIST[] = { "Food", "Clothes", "Bills", "Fun", "Other" };

	// A buffer size
	const int BUFF_SIZE = 1024;

	/**
	 * \brief This structure is used for storing the date values
	 */
	struct DateStruct
	{
		int _year;
		int _mounth;
		int _day;

		DateStruct() : _day(1), _mounth(1), _year(1601) {}
	};

	/**
	 * \brief This structure is used for storing the time values
	 */
	struct TimeStruct
	{
		int _hour;
		int _minutes;
	};

	/**
	 * \brief Static function used for converting DateStruct objects to MAUtil::String
	 * @param date const DateStruct& the date object
	 * @return MAUtil::String the string representation of a date object
	 */
	static MAUtil::String DateStructToString(const DateStruct& date)
	{
		MAUtil::String s;
		s.append(MAUtil::integerToString(date._year, 10).c_str(), MAUtil::integerToString(date._year, 10).length());
		s.append("-", 1);
		s.append(MAUtil::integerToString(date._mounth, 10).c_str(), MAUtil::integerToString(date._mounth, 10).length());
		s.append("-", 1);
		s.append(MAUtil::integerToString(date._day, 10).c_str(), MAUtil::integerToString(date._day, 10).length());

		return s;
	}

	/**
	 * \brief Static function used for converting TimeStruct objects to MAUtil::String
	 * @param date const TimeStruct& the date object
	 * @return MAUtil::String the string representation of a time object
	 */
	static MAUtil::String TimeStructToString(const TimeStruct& time)
	{
		MAUtil::String s;
		s.append(MAUtil::integerToString(time._hour, 10).c_str(), MAUtil::integerToString(time._hour, 10).length());
		s.append("-", 1);
		s.append(MAUtil::integerToString(time._minutes, 10).c_str(), MAUtil::integerToString(time._minutes, 10).length());

		return s;
	}

	/**
	 * \brief Static function used for comparing 2 DateStruct objects
	 * @param date1 const DateStruct&
	 * @param date2 const DateStruct&
	 * @return 1 if date1 > date2, -1 if date1 < date2, 0 if date1 = date2
	 */
	static int CompareDateObjects(const DateStruct& date1, const DateStruct& date2)
	{
		if(date1._year > date2._year) return 1;
		else
		{
			if(date1._year < date2._year) return -1;
			else
			{
				if(date1._mounth > date2._mounth) return 1;
				else
				{
					if(date1._mounth < date2._mounth) return -1;
					else
					{
						if(date1._day > date2._day) return 1;
						else if(date1._day == date2._day) return 0;
						return -1;
					}
				}
			}
		}
	}
}

#endif /* MODELUTIL_H_ */

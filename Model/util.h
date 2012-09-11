/*
 * util.h
 *
 *  Created on: Jun 28, 2012
 *      Author: Cipri
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <MAUtil/String.h>
#include <MAUtil/util.h>
#include <matime.h>
#include <conprint.h>

namespace GUI
{
	const int GRAY = 0xC0C0C0;
	const int GREEN = 0x00FF00;
	const int YELLOW = 0xFFFF00;
	const int RED = 0xFF0000;
	const int DIALOG_FONT_SIZE_SCREEN_LARGE = 25;
	const int DIALOG_FONT_SIZE_SMALL_SCREEN_LARGE = 18;
	const int DIALOG_FONT_SIZE_SCREEN_MEDIUM = 18;
	const int DIALOG_FONT_SIZE_SMALL_SCREEN_MEDIUM = 13;
	const int DIALOG_FONT_SIZE_SCREEN_SMALL = 13;
	const int DIALOG_FONT_SIZE_SMALL_SCREEN_SMALL = 10;
	const int DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE = 200;
	const int DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_MEDIUM = 150;
	const int DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_SMALL = 100;
	const int DESCRIPTION_EDIT_BOX_LINES = 10;
	const int DIALOG_HEIGHT = 750;
	const int DIALOG_BUTTON_WIDTH_SCREEN_LARGE = 140;
	const int DIALOG_BUTTON_WIDTH_SCREEN_MEDIUM = 100;
	const int DIALOG_BUTTON_WIDTH_SCREEN_SMALL = 80;
	const int DIALOG_BOTTOM_LINE_HEIGHT = 20;
	const int DIALOG_COLOR_WHITE = 0xFFFFFF;
	const int MAX_VALUE = 9999;
	const char DEFAULT_COIN[] = "EUR";
	const int IMAGE_BUTTON_WIDTH_LARGE = 100;
	const int IMAGE_BUTTON_WIDTH_MEDIUM = 70;
	const int IMAGE_BUTTON_WIDTH_SMALL = 40;
	const int IMAGE_BUTTON_HEIGHT_LARGE = 100;
	const int IMAGE_BUTTON_HEIGHT_MEDIUM = 70;
	const int IMAGE_BUTTON_HEIGHT_SMALL = 40;

	static int _dialogFontSize, _dialogSmallFontSize, _descriptionBoxHeight, _dialogButtonWidth, _imageButtonWidth, _imageButtonHeight;
	static int _screenType;

	static void DetermineScreenSize(int screenHeight, int screenWidth)
	{
		if(480 > screenHeight && 320 > screenWidth) _screenType = 0;
		else if(480 < screenHeight && 640 > screenHeight && 320 < screenWidth && 480 > screenWidth) _screenType = 1;
		else _screenType = 2;
	}

	static void SetSizeRelatedVariables()
	{
		MAExtent size = maGetScrSize();
		int screenWidth = EXTENT_X(size);
		int screenHeight = EXTENT_Y(size);

		DetermineScreenSize(screenHeight, screenWidth);

		switch(_screenType)
		{
		case 0:
			_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_SMALL;
			_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_SMALL;
			_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_SMALL;
			_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_SMALL;
			_imageButtonWidth = IMAGE_BUTTON_WIDTH_SMALL;
			_imageButtonHeight = IMAGE_BUTTON_HEIGHT_SMALL;
			break;
		case 1:
			_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_MEDIUM;
			_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_MEDIUM;
			_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_MEDIUM;
			_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_MEDIUM;
			_imageButtonWidth = IMAGE_BUTTON_WIDTH_MEDIUM;
			_imageButtonHeight = IMAGE_BUTTON_HEIGHT_MEDIUM;
			break;
		case 2:
			_dialogFontSize = DIALOG_FONT_SIZE_SCREEN_LARGE;
			_dialogSmallFontSize = DIALOG_FONT_SIZE_SMALL_SCREEN_LARGE;
			_descriptionBoxHeight = DESCRIPTION_EDIT_BOX_HEIGHT_SCREEN_LARGE;
			_dialogButtonWidth = DIALOG_BUTTON_WIDTH_SCREEN_LARGE;
			_imageButtonWidth = IMAGE_BUTTON_WIDTH_LARGE;
			_imageButtonHeight = IMAGE_BUTTON_HEIGHT_LARGE;
			break;
		}
	}
}
namespace Model
{
	const int NO_OF_CATEGORIES = 5;
	const int NO_OF_INCOMES = 4;
	const MAUtil::String INCOME_TYPES_LIST[] = { "Bank Transfer", "Salary", "Debt", "Other" };
	const MAUtil::String CATEGORY_LIST[] = { "Food", "Clothes", "Bills", "Fun", "Other" };
	const int BUFF_SIZE = 1024;

	struct DateStruct
	{
		int _year;
		int _mounth;
		int _day;

		DateStruct() : _day(1), _mounth(1), _year(1601) {}
	};

	struct TimeStruct
	{
		int _hour;
		int _minutes;
	};

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

	static MAUtil::String TimeStructToString(const TimeStruct& time)
	{
		MAUtil::String s;
		s.append(MAUtil::integerToString(time._hour, 10).c_str(), MAUtil::integerToString(time._hour, 10).length());
		s.append("-", 1);
		s.append(MAUtil::integerToString(time._minutes, 10).c_str(), MAUtil::integerToString(time._minutes, 10).length());

		return s;
	}

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

#endif /* UTIL_H_ */

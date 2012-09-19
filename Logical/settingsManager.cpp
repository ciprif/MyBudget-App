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
 * \file settingsManager.cpp
 * \author Ciprian Filipas
 * \date Jul 17, 2012
 */

#include <maapi.h>
#include <MAUtil/String.h>
#include <MAUtil/util.h>
#include <MAFS/File.h>
#include <conprint.h>
#include <matime.h>

#include "settingsManager.h"

namespace Logical
{
	/**
	 * \brief Constructor
	 *
	 * \note This also creates the settings file if it doesn't exist, otherwise
	 *       it opens it and calls the LoadSettings() function
	 */
	SettingsManager::SettingsManager() : _coin("EUR"), _showAll(true), _showFromDate(false), _showMonthly(false), _debtValue(0.0)
	{
		_settingsFile = maFileOpen(SETTINGS_FILE, MA_ACCESS_READ_WRITE);

		if(maFileExists(_settingsFile))
		{
			LoadSettings();
			maFileClose(_settingsFile);
		}
		else
		{
			maFileCreate(_settingsFile);
			maFileClose(_settingsFile);
			_date._day = 1;
			_date._mounth = 1;
			_date._year = 1601;
			ApplySettings();
		}
	}

	/**
	 * \brief This function calls the _readSettings() function
	 */
	void SettingsManager::LoadSettings()
	{
		_readSettings();
	}

	/**
	 * \brief This function callse the _writeSettings() function
	 */
	void SettingsManager::ApplySettings()
	{
		_writeSettings();
	}

	/**
	 * \brief Getter for the coin value
	 * @return MAUtil::String&
	 */
	MAUtil::String& SettingsManager::getCoin()
	{
		return _coin;
	}

	/**
	 * \brief Getter for the date value
	 * @return Model::DateStruct&
	 */
	Model::DateStruct& SettingsManager::getDate()
	{
		return _date;
	}

	/**
	 * \brief Getter for the debt value
	 * @return double&
	 */
	double& SettingsManager::getDebtValue()
	{
		return _debtValue;
	}

	/**
	 * \brief Getter for the boolean that implies that the list should contain all the items
	 * @return bool
	 */
	bool SettingsManager::isShowAll()
	{
		return _showAll;
	}

	/**
	 * \brief Getter for the boolean that implies that the list should contain only the monthly items
	 * @return bool
	 */
	bool SettingsManager::isShowMonthly()
	{
		return _showMonthly;
	}

	/**
	 * \brief Getter for the boolean that implies that the list should contain all the items
	 * 		  that have a date greater than a user set starting date
	 * @return bool
	 */
	bool SettingsManager::isShowFromDate()
	{
		return _showFromDate;
	}

	/**
	 * \brief Setter for the isShowAll variable
	 * @param bool
	 */
	void SettingsManager::setIsShowAll(bool arg)
	{
		_showAll = arg;
	}

	/**
	 * \brief Setter for the isShowMonthly variable
	 * @param bool
	 */
	void SettingsManager::setIsShowMontly(bool arg)
	{
		_showMonthly  = arg;
	}

	/**
	 * \brief Setter for the reference date
	 * @param const Model::DateStruct& the new date
	 */
	void SettingsManager::setIsShowFromDate(bool arg)
	{
		_showFromDate = arg;
	}

	/**
	 * \brief Setter for the reference date
	 * @param const Model::DateStruct& the new date
	 */
	void SettingsManager::setDateFrom(const Model::DateStruct& arg)
	{
		_date._day = arg._day;
		_date._mounth = arg._mounth;
		_date._year = arg._year;
	}

	/**
	 * \brief Setter for the coin value
	 * @param const MAUtil::String& the new coin value
	 */
	void SettingsManager::setCoin(const MAUtil::String& arg)
	{
		_coin = arg;
	}

	/**
	 * \brief Setter for the debt value
	 * @param const double& the new debt value
	 */
	void SettingsManager::setDebtValue(const double& arg)
	{
		_debtValue = arg;
	}

	/**
	 * \brief This function is used for reading the settings from the settings file
	 */
	void SettingsManager::_readSettings()
	{
		char settingsFileContent[Model::BUFF_SIZE];
		MAUtil::String content;

		maFileRead(_settingsFile, settingsFileContent, Model::BUFF_SIZE);

		content.append(settingsFileContent, strlen(settingsFileContent));

		int offset = 0;
		int position = content.find("|", offset);
		_coin = content.substr(offset, position); //read the coin

		offset = position + 1;
		position = content.find("|", offset);

		int day = MAUtil::stringToInteger(content.substr(offset, position - offset), 10); //read the reset day

		offset = position + 1;
		position = content.find("|", offset);

		MAUtil::String dateString = content.substr(offset, position - offset); //read the dateString

		offset = position + 1;
		position = content.find("|", offset);

		MAUtil::String binaryMask = content.substr(offset, position - offset); //read the binary mask
		offset = position + 1;
		_debtValue = MAUtil::stringToDouble(content.substr(offset, content.length() - offset));

		if(binaryMask ==  "100")
		{
			_showAll = true;
			_showMonthly = false;
			_showFromDate = false;
		}
		else if(binaryMask == "010")
		{
			_showAll = false;
			_showMonthly = true;
			_showFromDate = false;
		}
		else if(binaryMask == "001")
		{
			_showAll = false;
			_showMonthly = false;
			_showFromDate = true;
		}

		if(_showMonthly)
		{
			_date._day = day;

			struct tm * dateTime = new tm;
			split_time(maTime(), dateTime);

			_date._mounth = dateTime->tm_mon + 1;
			_date._year = dateTime->tm_year + 1900;

			delete dateTime;
		}
		else if(_showFromDate)
		{
			offset = 0;
			position = dateString.find("-", offset);

			_date._year = MAUtil::stringToInteger(dateString.substr(offset, position), 10);

			offset = position + 1;
			position = dateString.find("-", offset);

			_date._mounth = MAUtil::stringToInteger(dateString.substr(offset, position - offset), 10);

			offset = position + 1;
			_date._day = MAUtil::stringToInteger(dateString.substr(offset, dateString.length() - offset), 10);
		}
		else if(_showAll)
		{
			_date._day = 1;
			_date._mounth = 1;
			_date._year = 1601;
		}
	}

	/**
	 * \brief This function is used for writing the settings to the settings file
	 */
	void SettingsManager::_writeSettings()
	{
		_settingsFile = maFileOpen(SETTINGS_FILE, MA_ACCESS_READ_WRITE);

		maFileTruncate(_settingsFile, 0);

		char buffer[Model::BUFF_SIZE];
		MAUtil::String binaryMask;

		if(_showAll) binaryMask = "100";
		else if(_showMonthly) binaryMask = "010";
		else if(_showFromDate) binaryMask = "001";

		sprintf(buffer, "%s|%d|%s|%s|%.2f", _coin.c_str(), _date._day, Model::DateStructToString(_date).c_str(), binaryMask.c_str(), _debtValue);

		maFileWrite(_settingsFile, buffer, strlen(buffer));
		maFileClose(_settingsFile);
	}
}


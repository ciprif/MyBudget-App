/*
 * settingsManager.h
 *
 *  Created on: Jul 17, 2012
 *      Author: Cipri
 */

#ifndef SETTINGSMANAGER_H_
#define SETTINGSMANAGER_H_

#include "../Model/util.h"

#define SETTINGS_FILE "settings.txt"
#define BUFF_SIZE 1024

namespace Logical
{
	const int DEFAULT_DAY = 1;
	const int DEFAULT_MONTH = 1;
	const int DEFAULT_YEAR = 2012;

	class SettingsManager
	{
	public:
		/**
		 * \brief Constructor
		 *
		 * \note This also creates the settings file if it doesn't exist, otherwise
		 *       it opens it and calls the LoadSettings() function
		 */
		SettingsManager();

		/**
		 * \brief Destructor
		 */
		~SettingsManager() {}

		/**
		 * \brief This function calls the _readSettings() function
		 */
		void LoadSettings();

		/**
		 * \brief This function callse the _writeSettings() function
		 */
		void ApplySettings();

		/**
		 * \brief Getter for the coin value
		 * @return MAUtil::String&
		 */
		MAUtil::String& getCoin();

		/**
		 * \brief Getter for the date value
		 * @return Model::DateStruct&
		 */
		Model::DateStruct& getDate();

		/**
		 * \brief Getter for the debt value
		 * @return double&
		 */
		double& getDebtValue();

		/**
		 * \brief Getter for the boolean that implies that the list should contain all the items
		 * @return bool
		 */
		bool isShowAll();

		/**
		 * \brief Getter for the boolean that implies that the list should contain only the monthly items
		 * @return bool
		 */
		bool isShowMonthly();

		/**
		 * \brief Getter for the boolean that implies that the list should contain all the items
		 * 		  that have a date greater than a user set starting date
		 * @return bool
		 */
		bool isShowFromDate();

		/**
		 * \brief Setter for the isShowAll variable
		 * @param bool
		 */
		void setIsShowAll(bool);

		/**
		 * \brief Setter for the isShowMonthly variable
		 * @param bool
		 */
		void setIsShowMontly(bool);

		/**
		 * \brief Setter for the IsShowFromDate variable
		 * @param bool
		 */
		void setIsShowFromDate(bool);

		/**
		 * \brief Setter for the reference date
		 * @param const Model::DateStruct& the new date
		 */
		void setDateFrom(const Model::DateStruct&);

		/**
		 * \brief Setter for the coin value
		 * @param const MAUtil::String& the new coin value
		 */
		void setCoin(const MAUtil::String&);

		/**
		 * \brief Setter for the debt value
		 * @param const double& the new debt value
		 */
		void setDebtValue(const double&);
	private:
		/**
		 * \brief This function is used for reading the settings from the settings file
		 */
		void _readSettings();

		/**
		 * \brief This function is used for writing the settings to the settings file
		 */
		void _writeSettings();

		// The file handle
		MAHandle _settingsFile;

		MAUtil::String _coin;
		Model::DateStruct _date;

		bool _showAll;
		bool _showMonthly;
		bool _showFromDate;

		double _debtValue;
	};
}


#endif /* SETTINGSMANAGER_H_ */

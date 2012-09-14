/*
 * homeScreen.h
 *
 *  Created on: Jun 12, 2012
 *      Author: Cipri
 */

#ifndef HOMESCREEN_H_
#define HOMESCREEN_H_

#include <NativeUI/Screen.h>
#include <NativeUI/ScreenListener.h>
#include <MAUtil/String.h>

#define BUFFER_SIZE 256

#define SPACER_WIDTH 10
#define SPACER_HEIGHT 40
#define FONT_SIZE 25


//Forward declarations
class AddExpenseDialog;
class AddIncomeDialog;

namespace NativeUI
{
	class Label;
	class HorizontalLayout;
	class VerticalLayout;
	class PanoramaView;
	class Dialog;
	class ActivityIndicator;
}

namespace Logical
{
	class Observer;
}

namespace GUI
{
	class HomeScreen : public NativeUI::Screen, public NativeUI::ScreenListener
	{
	public:
		/**
		 * \brief Constructor
		 */
		HomeScreen();

		/**
		 * \brief Destructor
		 */
		~HomeScreen();

		/**
		 * This method is called when the OptionsMenu is being closed
		 * (either by the user canceling the menu with the back/menu
		 * button, or when an item is selected.
		 * @param screen The screen that generated the event.
		 */
		void optionsMenuClosed(NativeUI::Screen* screen) {}

		/**
		 * This function is called when an options menu item is selected.
		 * @param index The index on which the item is placed into the
		 * OptionsMenu.
		 * @param screen The screen that generated the event.
		 */
		void optionsMenuItemSelected(NativeUI::Screen* screen, int index);

		/**
		 * \brief This function creates the options menu
		 */
		void createOptionsMenu();

		/**
		 * \brief This function sets the observer for this screen
		 * @param observer the observer for this screen
		 */
		void setObserver(Logical::Observer* observer);

		/**
		 * \brief This function updates the values for the graphics
		 * @param value the value
		 * @param isExpense true if the transaction is an expense -> the value will be withdrawn
		 * @param category != "" if the transaction is an expense, will keep the category
		 */
		void updateBudgetValues(const double& value, bool isExpense, const MAUtil::String& category);

		/**
		 * \brief This function updates the total budget value
		 * @param const double& the new value
		 */
		void updateTotalBudget(const double&);

		/**
		 * \brief This function updates the consumed budget value
		 * @param const double& the new value
		 */
		void updateConsumedBudget(const double&);

		/**
		 * \brief This function updates the debt budget value
		 * @param const double& the new value
		 */
		void updateDebtBudget(const double&);

		/**
		 * \brief This function is used fot updating the UI of the simple graphic
		 */
		void updateSimpleGraphic();

		/**
		 * \brief This function is used for setting the addExpenses dialog reference
		 * @param obj NativeUI::Dialog* the pointer to the application wide addExpenses dialog
		 */
		void setAddExpensesDialogReference(NativeUI::Dialog* obj);

		/**
		 * \brief This function is used for setting the addIncomes dialog reference
		 * @param obj NativeUI::Dialog* the pointer to the application wide addIncomes dialog
		 */
		void setAddIncomesDialogReference(NativeUI::Dialog* obj);

		/**
		 * \brief This function notifies the homeScreen that the addExpensesDialog was successfully loaded
		 * 		  and it stops the activity indicator
		 */
		void addExpensesDialogLoaded();

		/**
		 * \brief This function notifies the homeScreen that the addIncomesDialog was successfully loaded
		 * 		  and it stopes the activity indicator
		 */
		void addIncomesDialogLoaded();

		/**
		 * \brief This function is used to set the new _coin value; it also updates the UI
		 * @param const MAUtil::String& the new coin value
		 */
		void setCoin(const MAUtil::String&);

		/**
		 * \brief This function is used for synchronizing the budget values
		 */
		void updateValues();
	private:
		/**
		 * \brief This function creates the main layout and sets some enviroment variables. This is the point where
		 * 		  all the UI is initialized;
		 * @param screenHeight the screen height.
		 * @param screenWidth the screen width.
		 */
		void _createUI(const int& screenHeight, const int& screenWidth);

		/**
		 * \brief This function creates the budget information label and the related parent controls.
		 * @param height the height of the screen.
		 */
		void _createBudgetLabel(int height);

		/**
		 * \brief This function creates the simple graphic bar the illustrates the amount consumed.
		 * @param width the screen width
		 * @param height the screen height
		 */
		void _createSimpleExpensesGraphic(const int& width, const int& height);

		/**
		 * \brief This function creates the detailed graphic, bar for each category of expense
		 * @param width the width of the screen
		 * @param height the height of the screen
		 */
		void _createDetailedGraphic(const int& width, const int& height);

		/**
		 * \brief This function creates a category graphic
		 * @param category the category as a string
		 * @param total the total budget
		 * @param consumed the budget consumed of this category of expenses
		 * @param width the width of the screen
		 * @param height the height of the screen
		 * @return NativeUI::VerticalLayout* the vertical layout containing the desired information
		 */
		NativeUI::VerticalLayout* _createCategoryGraphic(const MAUtil::String& category, const double& total, const double& consumed, const int& width, const int& height, const int& index);

		/**
		 * \brief This function updates the UI for the consume bar of a certain category
		 * @param consumeBar NativeUI::HorizontalLayout* pointer to the consumeBar
		 * @param value const double& the new value
		 */
		void _updateConsumeBar(NativeUI::HorizontalLayout* consumeBar, const double& value);

		/**
		 * \brief This function hides the ativity indicator for WP7 and destroys it for the other platforms
		 */
		void _removeActivityIndicator();

		/**
		 * \brief This function sets the _isWP7 and _isIOS bool values
		 */
		void _setPlatform();

		// Member data
		double _budgetTotalValue;
		double _budgetConsumedValue;
		double _debtBudget;
		double _parentLayoutWidth;

		int _categoryGraphicHeight;
		int _categoryGraphicWidth;
		int _addExpenseIndex;
		int _settingsButtonIndex;
		int _addIncomeIndex;
		bool _incomesDialogLoaded;
		bool _expensesDialogLoaded;
		bool _progressBarActive;
		bool _isWP7;
		bool _isIOS;

		// Observer reference
		Logical::Observer* _observerReference;
		MAUtil::Map<const MAUtil::String, double>* _valueMap;
		MAUtil::Map<const MAUtil::String, NativeUI::HorizontalLayout*>* _categoryGraphicsMap;

		MAUtil::String _coin;

		// UI related variables
		NativeUI::VerticalLayout* _budgetSimpleGraphicParentLayout;
		NativeUI::VerticalLayout* _mainLayout;
		NativeUI::HorizontalLayout* _budgetSimpleGraphicTotalBudgetLayout;
		NativeUI::HorizontalLayout* _budgetSimpleGraphicConsumedBudgetLayout;
		NativeUI::HorizontalLayout* _spacerSimpleGraphic;

		NativeUI::Label* _budgetLabel;

		NativeUI::ActivityIndicator* _activityIndicator;
		NativeUI::VerticalLayout* _activityIndicatorLayout;

		// Dialogs references
		AddExpenseDialog* _addExpensesDialog;
		AddIncomeDialog* _addIncomeDialog;

	};
}
#endif /* HOMESCREEN_H_ */

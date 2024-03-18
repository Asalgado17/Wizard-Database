/* Program name: Main.cpp
*  Author: Andrea Salgado
*  Date last updated: 3/1/2024
* Purpose: Menu based program for choosen buisness
*/
#include <iostream>
#include <string>
#include <sqlite3.h>

//additonal includes
#include <limits> //numeric_limits<std::streamsize
#include <climits> //helps declare INT_MAX in scope
#include <iomanip> //for using setprecision on decimal data
#include <sstream> //for std::istringstream

void resetStream();
int rollback(sqlite3 *);


//mainMenu functions
void printMainMenu();
int mainMenu();
int addRow(sqlite3 *);
int updateRow(sqlite3 *);
int deleteRow(sqlite3 *);
int	transaction(sqlite3 *);
int retrieveData(sqlite3 *);

//functions for addRow(sqlite3 *);
int addWizard(sqlite3 *);
int addProduct(sqlite3 *);

//functions for updateRow(sqlite3 *);
int updateWizard (sqlite3 *); 
int updateWiz_Performance(sqlite3 *);
int updateWiz_Living(sqlite3 *);
int updateProduct (sqlite3 *);
int updateProductQuantity(sqlite3 *);

//functions for deleteRow(sqlite3 *);
int deleteInvoice (sqlite3 *);
int deleteProduct (sqlite3 *);

//functions for transcation(sqlite3 *);
int startTransaction(sqlite3 *);
int addCustomer(sqlite3 *);
int transcation(sqlite3 *);


//functions for retrieveData(sqlite3 *);
int genericRowCallback(void *, int, char **, char **); 
int viewCustomer (sqlite3 *); 
int viewInvoice (sqlite3 *);




int main()
{
	int choice;
	sqlite3 *db;
	int rc; 

	//opening cobblestone database
	rc = sqlite3_open_v2("CobbleStone.db", &db, SQLITE_OPEN_READWRITE, NULL);
	if (rc != SQLITE_OK)
	{
		std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Database opened successfully." << std::endl;
	}

	std::cout << "Welcome to CobbleStone" << std::endl;
	choice = mainMenu();

	while (true)
	{
		switch (choice) //user choice switch stament to call functions
		{
			case 1: addRow(db); 
			break;
			case 2: updateRow(db);
			break;
			case 3: deleteRow(db);
			break;
			case 4: transaction(db);
			break;
			case 5: retrieveData(db);
			break;

			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";
		choice = mainMenu();
	}
	// Close the database connection 
    int closeResult = sqlite3_close(db);
    if (closeResult == SQLITE_OK)
    {
        std::cout << "Database closed successfully." << std::endl;
    }
    else
    {
        std::cout << "Error closing the database: " << sqlite3_errmsg(db) << std::endl;
    }

    return 0;
}


//printing mainMenu
void printMainMenu()
{
	std::cout << "Please choose an option (enter -1 to quit):  " << std::endl;
	std::cout << "1. Add Row" << std::endl;
	std::cout << "2. Update Row  " << std::endl;
	std::cout << "3. Delete Row " << std::endl;
	std::cout << "4. Make a sale " << std::endl;
	std::cout << "5. View " << std::endl;
	std::cout << "Enter Choice: ";
}

int mainMenu()
{
	int choice = 0;

	printMainMenu();
	//verifying for valid input for menu choice
	std::cin >> choice;
	while ((!std::cin || choice < 1 || choice > 5) && choice != -1)
	{
		if (!std::cin)
		{
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		}
		std::cout << "That is not a valid choice." << std::endl;
		printMainMenu();
		std::cin >> choice;
	}
	return choice;
}


int addRow(sqlite3 *db)
{ 
	//submenu for adding rows
	int choice = 0;
	std::cout << "Selected: Add Row" << std::endl;
	std::cout << "Please choose an option (enter -1 to quit):  " << std::endl;
	std::cout << "1. Add new Wizard " << std::endl;
	std::cout << "2. Add new product" << std::endl;
	std::cin >> choice; //choice for deciding which table to add to

	switch (choice) 
		{
			case 1: addWizard (db); 
			break;
			case 2: addProduct (db);
			break;
		
			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";
		return 0;	
}
	

//functions for adding new row
int addWizard(sqlite3 *db)
{
	//Prompt for the new wizard data
	std::string wiz_fname, wiz_lname;
	int wiz_performance, wiz_departed; 
	sqlite3_stmt *pRes;
	std::string m_strLastError;

	std::cout << "Enter Wizard First Name: ";
	std::cin >> wiz_fname;

	std::cout << "Enter Wizard Last Name: ";
	std::cin >> wiz_lname;

	std::cout << "Enter Wizard Performance: ";
	std::cin >> wiz_performance;

	std::cout << "Enter Wizard Departed Status (1 for alive, 0 for deceased): ";
	std::cin >> wiz_departed;
    
	//SQL insert statement
	std::string query = "INSERT INTO wizards (wiz_fname,wiz_lname,wiz_performance,wiz_departed)VALUES(?,?,?,?)";

	// Prepare the SQL statement
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, wiz_fname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, wiz_lname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, wiz_performance);
    sqlite3_bind_int(stmt, 4, wiz_departed);

    // Executing the SQL statement if anything goes wrong error message
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Finalize the query 
    sqlite3_finalize(stmt);

    std::cout << "Wizard employee added successfully:" << std::endl;

    return 0;
}

int addProduct(sqlite3 *db)
{
	//Prompt for the new product data
	std::string p_name, p_description, p_day_night;
	int p_quanitiy;
	float p_price;
	sqlite3_stmt *pRes;
	std::string m_strLastError;


	std::cout << "Enter product name: ";
	std::cin.ignore(); 
	std::getline(std::cin, p_name);

	std::cout << "Enter product description: ";
	std::cin.ignore(); 
	std::getline(std::cin, p_description);

	std::cout << "Enter quanity in store: ";
	std::cin >> p_quanitiy;

	std::cout << "Enter price of individual product: ";
	std::cin >> p_price;

	std::cout << "Enter product avalibility (write 'day' or 'night'): ";
	std::cin >> p_day_night;
	
	// Validate p_day_night
    if(p_day_night != "day" && p_day_night != "night") {
    std::cout << "Invalid input for product availability. Operation canceled." << std::endl;
    return -1; 
}

	//SQL insert statement
	std::string query = "INSERT INTO product (p_name,p_description,p_quanity,p_price,p_day_night)VALUES(?,?,?,?,?)";

	//Prepare the SQL statement
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

	//Bind parameters to the prepared statement
    sqlite3_bind_text(stmt,1, p_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,2, p_description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, p_quanitiy);
	sqlite3_bind_double(stmt, 4, p_price);
	sqlite3_bind_text(stmt, 5, p_day_night.c_str(), -1, SQLITE_STATIC);
	
	// Executing the SQL statement if anything goes wrong error message
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Finalize the query 
    sqlite3_finalize(stmt);

    std::cout << "New product added successfully:" << std::endl;

    return 0;
}


//update a row
int updateRow(sqlite3 *db)
{
	//submenu for adding rows
	int choice = 0;
	std::cout << "Selected: Update Row" << std::endl;
	std::cout << "Please choose an option (enter -1 to quit):  " << std::endl;
	std::cout << "1. Update Wizard" << std::endl;
	std::cout << "2. Update product" << std::endl;
	std::cin >> choice; //choice for deciding which table to add to

	switch (choice) 
		{
			case 1: updateWizard (db); 
			break;
			case 2: updateProduct (db);
			break;
	
			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";	

    return 0;
}

//functions for updating
int updateWizard(sqlite3 *db)
{
	
	int choice = 0;
	std::cout << "Please choose an option (enter -1 to quit):  " << std::endl;
	std::cout << "Avalible updates: " << std::endl;
	std::cout << "1. Wizard performance" << std::endl;
	std::cout << "2. Wizard living status" << std::endl;
	std::cin >> choice; //choice for what wizard aspect to update

	switch (choice) 
		{
			case 1:  
			updateWiz_Performance(db);
			break;
			case 2: 
			updateWiz_Living(db);
			break;
		
			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";	

    return 0;
}

int updateWiz_Performance(sqlite3 *db)
{
	//making query to get list of wizards 
	//declaring
	sqlite3_stmt *pRes;
	std::string query;
	query = "select wizards.wiz_fname, wizards.wiz_lname, wizards.wiz_id from wizards";

	//checking wizard query for errors
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes);
		std::cout << "There was an error with the wizard query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	//for menu of wizards ID
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose the Wizard ID: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes, 2);
        std::cout << " - " << sqlite3_column_text(pRes, 0);
		std::cout << " " << sqlite3_column_text(pRes, 1);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes);
    }

	//Declared strings to hold wizard. 
	int wiz_id;
	std::string wiz_fname, wiz_lname;
	wiz_id = (sqlite3_column_int(pRes, 2));
	wiz_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 0));
	wiz_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 1));

	//confirmation as to the selected wizard
	std::cout << "Selected wizard to update: " << wiz_fname << " " << wiz_lname << std::endl;

	//updating selected wizard
	
	std::cout << "What is the wizards current performance level? (Scale 100-1)" << std::endl;
	std::cout << "Performance: ";
	int wiz_performance;
	std::cin >> wiz_performance;

	while (!std::cin || wiz_performance < 0 || wiz_performance > 100)
{
    if (!std::cin)
        resetStream();
    else
    {
        std::cout << "Invalid input! Please enter a value between 1 and 100." << std::endl;
    }

    std::cout << "Performance: ";
    std::cin >> wiz_performance;
}
//updating wizard's performance in the database
	std::string query2;
    query2 = "update wizards set wiz_performance = ?1 where wiz_id = ?2";
	sqlite3_stmt *res;
    int rc2 = sqlite3_prepare_v2(db, query2.c_str(), -1, &res, NULL);
    
	if (rc2 != SQLITE_OK)
    {
        sqlite3_finalize(res);
        std::cout << "unable to update wizard performance." << sqlite3_errmsg(db) << std::endl;
        std::cout << query2 << std::endl;
        return -1;
    }

	//bind parameters
    sqlite3_bind_int(res, 1, wiz_performance);
	sqlite3_bind_int(res, 2, wiz_id);

    rc2 = sqlite3_step(res);

	//checking if wizard performance can be updates
    if (rc2 != SQLITE_DONE)
    {
        std::cout << "Error updating wizard performance." << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(res);
        return -1;
    }

    sqlite3_finalize(res);
    return 0;
}

int updateWiz_Living(sqlite3 *db)
{
	//making query to get list of wizards 

	//declaring
	sqlite3_stmt *pRes;
	std::string query;
	query = "select wizards.wiz_fname, wizards.wiz_lname, wizards.wiz_id from wizards";

	//checking wizard query for errors
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes);
		std::cout << "There was an error with the wizard query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	//for menu of wizards ID
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose a wizard from the list: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes, 2);
        std::cout << " - " << sqlite3_column_text(pRes, 0);
		std::cout << " " << sqlite3_column_text(pRes, 1);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    std::cout << "List number: ";
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes);
    }

	//Declared strings to hold wizard. 
	int wiz_id;
	std::string wiz_fname, wiz_lname;
	wiz_id = (sqlite3_column_int(pRes, 2));
	wiz_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 0));
	wiz_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 1));

	//confirmation as to the selected wizard
	std::cout << "Selected wizard to update: " << wiz_fname << " " << wiz_lname << std::endl;

	//updating selected wizard
	std::cout << "Is the selected wizard alive? (1 for alive, 0 for deceased)" << std::endl;
	std::cout << "Alive Status: ";
	int wiz_departed;
	std::cin >> wiz_departed;

	while (!std::cin || wiz_departed < 0 || wiz_departed > 1)
{
    if (!std::cin)
        resetStream();
    else
    {
        std::cout << "Invalid input! Please enter a values 1 or 0." << std::endl;
    }

    std::cout << "Departed: " << wiz_departed << std::endl;
}
	std::string query2;
    query2 = "update wizards set wiz_departed = ?1 where wiz_id = ?2";
	sqlite3_stmt *res;
    int rc2 = sqlite3_prepare_v2(db, query2.c_str(), -1, &res, NULL);
    
	if (rc2 != SQLITE_OK)
    {
        sqlite3_finalize(res);
        std::cout << "unable to update wizard living status." << sqlite3_errmsg(db) << std::endl;
        std::cout << query2 << std::endl;
        return -1;
    }

	//bind parameters
    sqlite3_bind_int(res, 1, wiz_departed);
	sqlite3_bind_int(res, 2, wiz_id);

    rc2 = sqlite3_step(res);

	//checking if wizard performance can be updates
    if (rc2 != SQLITE_DONE)
    {
        std::cout << "Error updating wizard living status." << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(res);
        return -1;
    }
	
    return 0;
}

int updateProduct(sqlite3 *db)
{
	std::cout << "Please choose an option (enter -1 to quit):  " << std::endl;
	std::cout << "1. Update product quantity" << std::endl;
	int choice;
	std:: cin >> choice;
	switch (choice)
	{
		case 1:
		updateProductQuantity(db);
		
		case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";
	
    return 0;
}

int updateProductQuantity(sqlite3 *db){
	
	//making query to get list of products 

	//declaring
	sqlite3_stmt *pRes;
	std::string query;
	query = "select product.p_name, product.product_id from product";

	//checking product query for errors
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes);
		std::cout << "There was an error with the product query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	//for menu of product ID
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose a product from the list: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes, 1);
		std::cout << " " << sqlite3_column_text(pRes, 0);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    std::cout << "List Number: ";
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes);
    }

	//Declared strings to hold product. 
	 
	int product_id;
	std::string p_name;

	p_name = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 0));
	product_id = (sqlite3_column_int(pRes, 1));
	

	//confirmation as to the selected product
	std::cout << "Selected product to update: \n" << "ID: " << product_id << "\nName: " << p_name << std::endl;
	
	//updating selected wizard
	std::cout << "Update the quanity. What is the quanity currently in stock?" << std::endl;
	std::cout << "Quanity: ";
	int p_quanity;
	std::cin >> p_quanity;

	if (std::cin.fail()) {
        std::cout << "Invalid. Please enter an integer value." << std::endl;

        // Clear the input buffer
       resetStream();
		return 0;
	}


	std::string query2;
    query2 = "update product set p_quanity = ?1 where product_id = ?2";
	sqlite3_stmt *res;
    int rc2 = sqlite3_prepare_v2(db, query2.c_str(), -1, &res, NULL);
    
	if (rc2 != SQLITE_OK)
    {
        sqlite3_finalize(res);
        std::cout << "unable to update quanity." << sqlite3_errmsg(db) << std::endl;
        std::cout << query2 << std::endl;
        return -1;
    }

	//bind parameters
    sqlite3_bind_int(res, 1,p_quanity);
	sqlite3_bind_int(res, 2,product_id);

    rc2 = sqlite3_step(res);

	//checking if product quanity can be updated
    if (rc2 != SQLITE_DONE)
    {
        std::cout << "Error updating quanity." << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(res);
        return -1;
    }
	
    return 0;
}


//delete a row
int deleteRow(sqlite3 *db)
{

	int choice = 0;
	std::cout << "Selected: Delete Row" << std::endl;
	std::cout << "Please choose an what you want to delete (enter -1 to quit):  " << std::endl;
	std::cout << "1. Delete Invoice" << std::endl;
	std::cout << "2. Delete product" << std::endl;
	std::cin >> choice; //choice for deciding which table to add to

	switch (choice) 
		{
			case 1: deleteInvoice (db); 
			break;
			case 2: deleteProduct (db);
			break;
		
			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";	
				
    return 0;
}

int deleteInvoice(sqlite3 *db)
{
	//menu for invoices
	//declaring
	sqlite3_stmt *pRes;
	std::string query;
	query = "select invoice.order_id, invoice.o_date, customer.cus_fname, customer.cus_lname ";
	query += "from invoice ";
	query += "join customer on invoice.cus_id = customer.cus_id";

	//checking invoice query for errors
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes);
		std::cout << "There was an error with the invoice query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	//for menu of product ID
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose an invoice on the list: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". ID: " << sqlite3_column_text(pRes, 0);
		std::cout << " " << sqlite3_column_text(pRes, 1);
		std::cout << " " << sqlite3_column_text(pRes, 2);
		std::cout << " " << sqlite3_column_text(pRes, 3);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    std::cout << "List number: ";
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes);
    }

	//Declared strings to hold order. 
	int order_id;
	std::string cus_fname, cus_lname, o_date;

    order_id = (sqlite3_column_int(pRes, 0));
	o_date = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 1));
	cus_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 2));
	cus_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 3));

	//confirmation as to the selected invoice
	std::cout << "Selected invoice to delete: \n" << "Order ID: " << order_id << "\nName: " << cus_fname << " " << cus_lname << std::endl;
	

   //beginning the deletion of line using the order_id
	std::string queryDeleteLine;
	int rcDeleteLine;

 	std::string query0 = "begin transaction";
    int rc_transaction = sqlite3_exec(db, query0.c_str(), NULL,NULL, NULL);
    if (rc_transaction != SQLITE_OK)
    {
        std::cout << "There was an error - starting transaction: " << sqlite3_errmsg(db) << std::endl;
        return rc_transaction;
    }

	// Prepare and execute the first DELETE statement
	queryDeleteLine = "delete from line where order_id = " + std::to_string(order_id);
	sqlite3_stmt* pResDeleteLine;
	rcDeleteLine = sqlite3_exec(db, queryDeleteLine.c_str(), NULL, NULL, NULL);
    if (rcDeleteLine != SQLITE_OK)
    {
		sqlite3_finalize(pResDeleteLine);
    std::cout << "There was an error with delete: " << sqlite3_errmsg(db) << std::endl;
       std::cout << queryDeleteLine << std::endl;
	   rollback(db);
	   return 0;
    } 

//declaringg for invoice

	std::string queryDeleteInvoice;
	sqlite3_stmt* pResDeleteInvoice;
	int rcDeleteInvoice;
	queryDeleteInvoice = "delete from invoice where order_id = " + std::to_string(order_id);

	rcDeleteInvoice = sqlite3_exec(db, queryDeleteInvoice.c_str(), NULL, NULL, NULL);
    if (rcDeleteInvoice != SQLITE_OK)
    {
        std::cout << "There was an error delete: " << sqlite3_errmsg(db) << std::endl;
        std::cout << queryDeleteInvoice << std::endl;
		rollback(db);
		return 0;
    }

	//Commit transaction
	std::string queryCommit = "COMMIT";
int rcCommit = sqlite3_exec(db, queryCommit.c_str(), NULL, NULL, NULL);
if (rcCommit != SQLITE_OK) {
    std::cout << "Error committing transaction: " << sqlite3_errmsg(db) << std::endl;
	rollback(db);
	 sqlite3_finalize(pRes);
 	return rcCommit;
}

sqlite3_finalize(pResDeleteLine);
sqlite3_finalize(pResDeleteInvoice);
sqlite3_finalize(pRes);



    return 0;
}

int deleteProduct(sqlite3 *db)
{
	std::string query;
	sqlite3_stmt *pRes;
	


	//displaying product menu
    query = "select product_id, p_name ";
    query += "from product ";
  

    // Checking product query for errors
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(pRes);
        std::cout << "There was an error with the product query: " << sqlite3_errmsg(db) << std::endl;
        std::cout << query << std::endl;
        return -1;
    }

    // Menu of products
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose a product from the list: " << std::endl;
    i = 1;
    do {
        rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". ID: " << sqlite3_column_text(pRes, 0);
        std::cout << " " << sqlite3_column_text(pRes, 1);
        std::cout << std::endl;
        i++;
    } while (rc == SQLITE_ROW);
	std::cout << "Choice from product list: ";
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i) {
        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }

    sqlite3_reset(pRes);

      for (int j = 0; j < choice; j++) {
        rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
        break;
    }


	//Declared strings to hold product. 
	int product_id;
	std::string p_name;

    product_id = (sqlite3_column_int(pRes, 0));
	p_name = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 1));

	//confirmation as to the selected invoice
	std::cout << "Selected product to delete: \n" << "Product ID: " << product_id << "\nName: " << p_name <<  std::endl;

	
	std::string queryDelete;
	int rcDelete;
	

	//begin transaction
	std::string query0= "begin transaction";
    int rc_transaction = sqlite3_exec(db, query0 .c_str(), NULL, NULL, NULL);
    if (rc_transaction != SQLITE_OK)
    {
        std::cout << "There was an error - starting transaction: " << sqlite3_errmsg(db) << std::endl;
		rollback(db);
        return rc_transaction;
    }

	queryDelete = "delete from product where product_id = " + std::to_string(product_id);

    sqlite3_stmt *stmtDeleteProduct;
	rcDelete = sqlite3_prepare_v2(db, queryDelete.c_str(),-1, &stmtDeleteProduct, NULL);
    if (rcDelete != SQLITE_OK)
    {
        std::cout << "There was an error preparing delete product: " << sqlite3_errmsg(db) << std::endl;
        std::cout << queryDelete << std::endl;
		rollback(db);
		return -1;
    }

	 sqlite3_bind_int(stmtDeleteProduct, 1, product_id);

	  rcDelete = sqlite3_step(stmtDeleteProduct);
      if (rcDelete != SQLITE_DONE) {
        std::cout << "Error deleting product: " << sqlite3_errmsg(db) << std::endl;
        rollback(db);
        sqlite3_finalize(stmtDeleteProduct);
        sqlite3_finalize(pRes);
        return -1;
    }
	
	//finalizing the prepared statements
   sqlite3_finalize(stmtDeleteProduct);
   sqlite3_finalize(pRes);

	//Commit transaction
	std::string queryCommit = "COMMIT";
	int rcCommit = sqlite3_exec(db, queryCommit.c_str(), NULL, NULL, NULL);
	if (rcCommit != SQLITE_OK) {
    std::cout << "Error committing transaction: " << sqlite3_errmsg(db) << std::endl;
	rollback(db);
 	return rcCommit;
}
	 std::cout << "Product ID: " << product_id << " " << p_name << " deleted.";

   

	return 0;
}


//transaction
int transaction(sqlite3 *db)
{
	//menu for select either a new customer or make a new one
	std::cout << "Selected: Make a sale" << std::endl;
	std::cout << "Choose option or (enter -1 to quit): ";
	std::cout << "Is the sale for a returning customer a new one?" << std::endl;
	std::cout << "1. New customer" << std::endl;
	std::cout << "2. Returning customer" << std::endl;
	std::cout << "Option: ";
	int choice;
	std::cin >> choice;	
	switch (choice) 
		{
			case 1: addCustomer (db); 
			break;
			case 2: transcation(db);
			break;		
			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";	
				
    return 0;

	
}


//the full transaction
int transcation(sqlite3 *db)
{

//begin transcation!!!!
std::string query0 = "begin transaction";
    int rc_transaction = sqlite3_exec(db, query0.c_str(), NULL, NULL, NULL);
    if (rc_transaction != SQLITE_OK)
    {
        std::cout << "There was an error - starting transaction: " << sqlite3_errmsg(db) << std::endl;
        return rc_transaction;
    }


//picking the customer the sale is for

//declaring before selecting a cutomer from the database
	sqlite3_stmt *pRes_Cus;
	std::string query;
	query = "select customer.cus_fname, customer.cus_lname, customer.cus_id from customer order by customer.cus_id";

	//checking customer query for errors
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes_Cus, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes_Cus);
		std::cout << "There was an error with the customer query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

//for menu of customer ID
    int columnCount = sqlite3_column_count(pRes_Cus);
    int i, choice;
    std::cout << std::left;
    std::cout << "\nPlease choose the Customer ID for the sale: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes_Cus);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes_Cus, 2);
        std::cout << " - " << sqlite3_column_text(pRes_Cus, 0);
		std::cout << " " << sqlite3_column_text(pRes_Cus, 1);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes_Cus);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes_Cus);
    }

	//Declared strings to hold customer. 
	int cus_id;
	std::string cus_fname, cus_lname;
	cus_id = (sqlite3_column_int(pRes_Cus, 2));
	cus_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes_Cus, 0));
	cus_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes_Cus, 1));

	std::cout << "Selected customer to make sale with: " << cus_fname << " " << cus_lname << std::endl;
	
	//creating record for the order(invoice)

	//first inqury for sale details
	std::cout << "\nMaking sale. Enter the following information: " << std::endl;

	//ask for associated wizard_id to this sale. Give them a list
	//making query to get list of wizards 
	
	//declaring
	sqlite3_stmt *pRes_Wiz; //different pointer for wizard query
	std::string query2;
	query2 = "select wizards.wiz_fname, wizards.wiz_lname, wizards.wiz_id from wizards";
	int choice2; //second choice option for wiz query

	//checking wizard query for errors
	int rc_Wiz = sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes_Wiz, NULL);
	if(rc_Wiz != SQLITE_OK){
		sqlite3_finalize(pRes_Wiz);
		std::cout << "There was an error with the wizard query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query2 << std::endl;
		return 0;
	}

	//for menu of wizards ID
    std::cout << std::left;
    std::cout << "What wizard is making the sale? Select ID: " << std::endl;
	 i = 1;
    do
    {
       rc_Wiz = sqlite3_step(pRes_Wiz);
        if (rc_Wiz == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes_Wiz, 2);
        std::cout << " - " << sqlite3_column_text(pRes_Wiz, 0);
		std::cout << " " << sqlite3_column_text(pRes_Wiz, 1);
        std::cout << std::endl;
        i++;
	 } while (rc_Wiz == SQLITE_ROW);
    
    std::cin >> choice2;
    while (!std::cin || choice2 < 1 || choice2 >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice2;
    }
	sqlite3_reset(pRes_Wiz);
    for (int j = 0; j < choice2; j++)
    {
        sqlite3_step(pRes_Wiz);
    }

	//Declared strings to hold wizard. 
	int wiz_id;
	std::string wiz_fname, wiz_lname;
	wiz_id = (sqlite3_column_int(pRes_Wiz, 2));
	wiz_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes_Wiz, 0));
	wiz_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes_Wiz, 1));

	//confirmation as to the selected wizard
	std::cout << "Wizard sales person: " << wiz_fname << " " << wiz_lname << std::endl;

	//o_time give them 2 options AM or PM then Give them the options for those times 12 options 1:00, 2:00, 3:00 etc
	std::cout << "Choose the time of sales: " << std::endl;
	std::cout << "0. AM " << std::endl;
	std::cout << "1. PM " << std::endl;
	int amPmChoice; //choice for AM or PM
	std::cin >> amPmChoice;
	while (!std::cin || amPmChoice < 0 || amPmChoice > 1)
{
    if (!std::cin)
        resetStream();
    else
    {
        std::cout << "Invalid input! Please enter a values '1' (for AM) or '0' (for PM)." << std::endl;
    }
}

	// easier to valid numbers 1-12 so generate a list of 12 #'s + ":00" do auto loop through 12. Have them select list #
	
	// Choose the hour
    std::cout << "Choose the hour:" << std::endl;
    for (int i = 1; i <= 12; ++i) {
        std::cout << i << ". " << i << ":00" << std::endl;
	}

	int selectedHour;
    std::cin >> selectedHour;

        if (std::cin.fail() || selectedHour < 1 || selectedHour > 12) {
            // Invalid input
            std::cout << "Invalid input! Please enter a number between 1 and 12." << std::endl;

            // Clear the input buffer
           resetStream();
    		std::cin >> selectedHour;
			 return 1; 
        } 

	// Concatenate the selected hour with AM or PM to store into o_time
    std::string amPm = (amPmChoice == 0) ? "AM" : "PM";
    std::string o_time = std::to_string(selectedHour) + ":00 " + amPm; //we will insert this o_time

	std::cout << "Inputed Time: " << o_time << std::endl;
	

	//getting  current date
	char formatDate[80];
    time_t currentDate = time(NULL);
    strftime(formatDate, 80, "%F", localtime(&currentDate));
    std::string o_date(formatDate);

	std::cout << "\nCurrent date: " << o_date << std::endl;

  	 std::cout << "\nChoose the time of sales: " << std::endl;
	std::cout << "0. Night " << std::endl;
	std::cout << "1. Day " << std::endl;
	int choiceDayNight;
	sqlite3_stmt *pRes_Daynigt;
	std::cin >> choiceDayNight; 
	while (!std::cin || choiceDayNight < 0 || choiceDayNight > 1)
{
    if (!std::cin)
        resetStream();
    else
    {
        std::cout << "Invalid input! Please enter a values '1' (for day) or '0' (for night)." << std::endl;
		return 1; 
    }
}

	//declare
	sqlite3_stmt *pRes_DayNight;
    //if statement for product query
	if (choiceDayNight == 1) {

    // Declare SQLite statement for day
    std::string queryDay;
    queryDay = "select product.product_id, product.p_name, product.p_quanity, product.p_price ";
    queryDay += "from product ";
    queryDay += "where p_day_night = 'day'";

    // Checking day product query for errors
    int rc_Day = sqlite3_prepare_v2(db, queryDay.c_str(), -1, &pRes_DayNight, NULL);

    if (rc_Day != SQLITE_OK) {
        sqlite3_finalize(pRes_DayNight);
        std::cout << "There was an error with the day product query: " << sqlite3_errmsg(db) << std::endl;
        std::cout << queryDay << std::endl;
        return 1;
    }

    // For menu products for day
    int columnCount = sqlite3_column_count(pRes_DayNight);
    int i, choiceProduct;
    std::cout << std::left;
    std::cout << "Please choose a day product from the list: " << std::endl;
    i = 1;
    do {
        rc_Day = sqlite3_step(pRes_DayNight);
        if (rc_Day == SQLITE_DONE)
            break;
        std::cout << i << ". ID: " << sqlite3_column_text(pRes_DayNight, 0);
        std::cout << " " << sqlite3_column_text(pRes_DayNight, 1);
		std::cout << " - Available Quantity: " << sqlite3_column_int(pRes_DayNight, 2); // Show product quantity
		std::cout << " - Individual Price: " << sqlite3_column_double(pRes_DayNight, 3); // Show product price
        std::cout << std::endl;
        i++;
    } while (rc_Day == SQLITE_ROW);
	std::cout << "Choice from product list: ";
    std::cin >> choiceProduct;
    while (!std::cin || choiceProduct < 1 || choiceProduct >= i) {
        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choiceProduct;
    }

    sqlite3_reset(pRes_DayNight);
    for (int j = 0; j < choiceProduct; j++) {
        sqlite3_step(pRes_DayNight);
    }
}

if (choiceDayNight == 0) {

    // Declare
    std::string queryNight;
    queryNight = "select product.product_id, product.p_name, product.p_quanity, product.p_price ";
    queryNight += "from product ";
    queryNight += "where p_day_night = 'night'";

    // Checking night product query for errors
    int rc_Night = sqlite3_prepare_v2(db, queryNight.c_str(), -1, &pRes_DayNight, NULL);

    if (rc_Night != SQLITE_OK) {
        sqlite3_finalize(pRes_DayNight);
        std::cout << "There was an error with the night product query: " << sqlite3_errmsg(db) << std::endl;
        std::cout << queryNight << std::endl;
        return 1;
    }

    // For menu products for night
    int columnCount = sqlite3_column_count(pRes_DayNight);
    int i, choiceProduct;
    std::cout << std::left;
    std::cout << "Please choose a night product from the list:: " << std::endl;
    i = 1;
    do {
        rc_Night = sqlite3_step(pRes_DayNight);
        if (rc_Night == SQLITE_DONE)
            break;
        std::cout << i << ". ID: " << sqlite3_column_text(pRes_DayNight, 0);
        std::cout << " " << sqlite3_column_text(pRes_DayNight, 1);
		std::cout << " - Available Quantity: " << sqlite3_column_int(pRes_DayNight, 2); // Show product quantity
		std::cout << " - Individual Price: " << sqlite3_column_double(pRes_DayNight, 3); // Show product price
	
        std::cout << std::endl;
        i++;
    } while (rc_Night == SQLITE_ROW);
	std::cout << "Choice from product list: ";
    std::cin >> choiceProduct;
    while (!std::cin || choiceProduct < 1 || choiceProduct >= i) {
        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choiceProduct;
    }

    sqlite3_reset(pRes_DayNight);
    for (int j = 0; j < choiceProduct; j++) {
        sqlite3_step(pRes_DayNight);
		
    }
}
		//declaring for calculations and storing qunaity, price, and product_id
		int p_quanity, product_id,o_items;
		double o_subtotal, o_price, p_price, o_tax, o_total;
		p_quanity = sqlite3_column_int(pRes_DayNight, 2);
    	p_price = sqlite3_column_double(pRes_DayNight, 3);
    	product_id = sqlite3_column_int(pRes_DayNight, 0);
    	sqlite3_finalize(pRes_DayNight);
	
	
    	std::cout << "How many would you like? Enter 0 to cancel: ";
   		std::cin >> o_items;
		// Check if the requested quantity is greater than the available quantity
	if (o_items > p_quanity) {
    	std::cout << "That exceeds the available stock. You can purchase up to " << p_quanity << " items." << std::endl;
   		std::cout << "Enter 0 to cancel or choose a different quantity: ";
   		std::cin >> o_items;
   		std::cout << std::endl;
	}
	
// calculations

		//multiplies individual price of item times the desired quanity
		o_subtotal = p_price * o_items;
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Subtotal: "<<  o_subtotal << std::endl;

		//calculating tax set setprecision to the hundredths
		o_tax = o_subtotal * .16; 
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Tax: "<< o_tax << std::endl;

		//calculating total. Tax plus subtotal
		o_total = o_subtotal + o_tax;
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Total: "<< o_total  << std::endl;


	//insert invoice
	int order_id;
	std::string queryAddInvoice = "insert into invoice (o_time, o_date, o_items, o_tax, o_subtotal, o_total, wiz_id, cus_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

	// Prepare the SQL statement
	sqlite3_stmt *stmtInvoice;
	int rcInvoice = sqlite3_prepare_v2(db, queryAddInvoice.c_str(), -1, &stmtInvoice, NULL);

	if (rcInvoice != SQLITE_OK) {
    std::cout << "Error preparing invoice insert statement: " << sqlite3_errmsg(db) << std::endl;
    return rcInvoice;
	}

	//Bind values to the prepared statement
	sqlite3_bind_text(stmtInvoice, 1, o_time.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmtInvoice, 2, o_date.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmtInvoice, 3, o_items);
	sqlite3_bind_double(stmtInvoice, 4, o_tax);
	sqlite3_bind_double(stmtInvoice, 5, o_subtotal);
	sqlite3_bind_double(stmtInvoice, 6, o_total);
	sqlite3_bind_int(stmtInvoice, 7, wiz_id);
	sqlite3_bind_int(stmtInvoice, 8, cus_id);


	// Executing the SQL statement if anything goes wrong error message
    if (sqlite3_step(stmtInvoice) != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

//insert line
	std::string queryAddLine = "INSERT INTO line (order_id, p_id, p_quanity, p_price) VALUES (?, ?, ?, ?)";

	// Prepare the SQL statement for the "line" table
	sqlite3_stmt *stmtLine;
	int rcLine = sqlite3_prepare_v2(db, queryAddLine.c_str(), -1, &stmtLine, NULL);

	if (rcLine != SQLITE_OK) {
    std::cout << "Error preparing line insert statement: " << sqlite3_errmsg(db) << std::endl;
	rollback(db);
    return rcLine;
}
	//retriving the last order_id
	order_id = sqlite3_last_insert_rowid(db);

	// Bind values to the prepared statement for the "line" table
	sqlite3_bind_int(stmtLine, 1, order_id);
	sqlite3_bind_int(stmtLine, 2, product_id);
	sqlite3_bind_int(stmtLine, 3, o_items);
	sqlite3_bind_double(stmtLine, 4, p_price);

	// Execute the SQL statement for the "line" table
	if (sqlite3_step(stmtLine) != SQLITE_DONE) {
    std::cerr << "Error executing line SQL statement: " << sqlite3_errmsg(db) << std::endl;
	rollback(db);
    return -1;
}


	//update inventory of product quantiy by the product the customer bought
	std::string queryUpdateProduct = "update product set p_quanity = p_quanity - ? where product_id = ?";

	// Prepare the SQL statement for updating the product inventory
	sqlite3_stmt *stmtUpdateProduct;
	int rcUpdateProduct = sqlite3_prepare_v2(db, queryUpdateProduct.c_str(), -1, &stmtUpdateProduct, NULL);

	if (rcUpdateProduct != SQLITE_OK) {
    std::cout << "Error preparing update inventory statement: " << sqlite3_errmsg(db) << std::endl;
	rollback(db);
    return rcUpdateProduct;
	}
	
	//subrtacting the purchased quanity from product inventory
	p_quanity = p_quanity - o_items;
	std::cout << "Updated product quantity: " << p_quanity << std::endl;



	// Bind values to the prepared statement 
sqlite3_bind_int(stmtUpdateProduct, 1, o_items);
sqlite3_bind_int(stmtUpdateProduct, 2, product_id);

// Execute the SQL statement for updating product inventory
if (sqlite3_step(stmtUpdateProduct) != SQLITE_DONE) {
    std::cerr << "Error executing update inventory SQL statement: " << sqlite3_errmsg(db) << std::endl;
	rollback(db);
    return -1;
}


// Finalize the query 
sqlite3_finalize(stmtInvoice);
sqlite3_finalize(stmtLine);
sqlite3_finalize(stmtUpdateProduct);

	//Commit transaction
	std::string queryCommit = "COMMIT";
int rcCommit = sqlite3_exec(db, queryCommit.c_str(), NULL, NULL, NULL);
if (rcCommit != SQLITE_OK) {
    std::cout << "Error committing transaction: " << sqlite3_errmsg(db) << std::endl;
 return rcCommit;


}
return 0;
}

//adding new customer
int addCustomer(sqlite3 *db)
{
	//declarations 
int cus_id;
std::string cus_fname, cus_lname, cus_address, cus_phone, cus_email, cus_ssn;
sqlite3_stmt *pRes;
std::string m_strLastError;

std::cout << "Fill out the following information for the new customer: " << std::endl;

std::cout << "Enter Customer's First Name: ";
std::cin >> cus_fname;

std::cout << "Enter Customer's Last Name: ";
std::cin >> cus_lname;

std::cout << "Enter Customer's Address: ";
std::cin.ignore(); 
std::getline(std::cin, cus_address);

std::cout << "Enter Customer's Phone: ";
std::cin >> cus_phone;

std::cout << "Enter Customer's Email: ";
std::cin >> cus_email;

std::cout << "Enter Customer's Social Security Number: ";
std::cin >> cus_ssn;

//SQL insert statement
std::string query = "INSERT INTO customer (cus_fname,cus_lname,cus_address,cus_phone, cus_email, cus_ssn)VALUES(?,?,?,?,?,?)";

// Prepare the SQL statement
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

 // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, cus_fname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, cus_lname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, cus_address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, cus_phone.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, cus_email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, cus_ssn.c_str(), -1, SQLITE_STATIC);


    // Executing the SQL statement if anything goes wrong error message
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Finalize the query 
    sqlite3_finalize(stmt);

    std::cout << "Customer added successfully:" << std::endl;
	
	//starts the transcation
	transcation(db);

	return 0;
}

//retreve data
int retrieveData(sqlite3 *db)
{
	//submenu for retreiving data
	int choice = 0;
	std::cout << "Selected: View" << std::endl;
	std::cout << "Please choose an option (enter -1 to quit):  " << std::endl;
	std::cout << "1. View invoice " << std::endl;
	std::cout << "2. View customer " << std::endl;
	std::cin >> choice; //choice for what information to view

	switch (choice) 
		{
			case 1: viewInvoice (db);
			break;
			case 2: viewCustomer (db);
			break;
		
			case -1: return 0;
			default: std::cout << "That is not a valid choice." << std::endl;
		}
		std::cout << "\n\n";
		return 0;	
    return 0;
}

//functions to retrive data for view
int genericRowCallback(void *extData, int numCols, char **values, char **colNames)
{
    for (int i = 0; i < numCols; i++)
    {
        std::cout << colNames[i] << ": ";
        if (values[i] != NULL)
            std::cout << values[i];
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return SQLITE_OK;
}

int viewCustomer(sqlite3 *db)
{
//Declarations
std::string query;
query = "select cus_id, cus_fname, cus_lname from customer order by cus_id";
std::string query2;
std::string query3;
sqlite3_stmt *pRes;

 int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes);
		std::cout << "There was an error with the customer query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

//for menu of customer ID
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose the customer ID: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes, 0);
        std::cout << " - " << sqlite3_column_text(pRes, 1);
		std::cout << " " << sqlite3_column_text(pRes, 2);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes);
    }

	//Declared strings to hold customer. 
	std::string cus_id, cus_fname, cus_lname;
	cus_id = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 0));
	cus_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 1));
	cus_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 2));

	std::cout << "Selected Customer: " << cus_id << " - " << cus_fname << " " << cus_lname << std::endl;
	
	//next is displaying the info for the selected customer 

	//new sql statement
	sqlite3_stmt *pRes2;

	// customer information query
	query = "select customer.cus_address, customer.cus_phone, customer.cus_email, customer.cus_ssn from customer join invoice on customer.cus_id = invoice.cus_id where customer.cus_id = " + cus_id;


	//error message for query
	rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes2, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes2);
		std::cout << "There was an error with the customer info query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	sqlite3_step(pRes2);

    std::string cus_address, cus_phone, cus_email, cus_ssn;

	//Store customer info 
	cus_address = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 0));
	cus_phone = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 1));
	cus_email = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 2));
	cus_ssn = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 3));
	sqlite3_finalize(pRes2); 

	//printing customer info
    std::cout << "----Customer Information----" << std::endl;
	std::cout << "Name: " << cus_fname << " " << cus_lname << std::endl;
	std::cout << "Address: " << cus_address << std::endl;
	std::cout << "Phone Number: " << cus_phone << std::endl;
	std::cout << "Email: " << cus_email << std::endl;
	std::cout << "Social Security Number: " << cus_ssn << std::endl;

	//new sql statement
	sqlite3_stmt *pRes3;
    query3 = "select order_id from invoice where cus_id = " + cus_id;
	
	// Error message for query
    rc = sqlite3_prepare_v2(db, query3.c_str(), -1, &pRes3, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(pRes3);
        std::cout << "There was an error with the order history query: " << sqlite3_errmsg(db) << std::endl;
        std::cout << query3 << std::endl;
        return 0;
    }
	   
std::cout << "Order History ID's:" << std::endl;
while (sqlite3_step(pRes3) == SQLITE_ROW)
{
    int order_id = sqlite3_column_int(pRes3, 0);
    std::cout << order_id << std::endl;
}

sqlite3_finalize(pRes3);

  
  return 0;
}


int viewInvoice(sqlite3 *db)
{
//Declarations
std::string query;
query = "select order_id, o_time, o_date from invoice order by order_id";
std::string query2;
sqlite3_stmt *pRes;

int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes);
		std::cout << "There was an error with the invoice query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	//for menu of customer ID
    int columnCount = sqlite3_column_count(pRes);
    int i, choice;
    std::cout << std::left;
    std::cout << "Please choose the Order ID: " << std::endl;
	 i = 1;
    do
    {
       rc = sqlite3_step(pRes);
        if (rc == SQLITE_DONE)
            break;
        std::cout << i << ". " << sqlite3_column_text(pRes, 0);
        std::cout << " - " << sqlite3_column_text(pRes, 1);
		std::cout << " " << sqlite3_column_text(pRes, 2);
        std::cout << std::endl;
        i++;
	 } while (rc == SQLITE_ROW);
    
    std::cin >> choice;
    while (!std::cin || choice < 1 || choice >= i)
    {

        if (!std::cin)
            resetStream();
        std::cout << "That is not a valid choice! Try again!" << std::endl;
        std::cin >> choice;
    }
	sqlite3_reset(pRes);
    for (int j = 0; j < choice; j++)
    {
        sqlite3_step(pRes);
    }

	//Declared strings to hold customer.

	 
	std::string o_time, o_date, order_id;
	order_id = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 0));
	o_time = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 1));
	o_date = reinterpret_cast<const char *>(sqlite3_column_text(pRes, 2));

//prints out user choice
std::cout << "Selected Order ID: " << order_id << " - " << o_time << " " << o_date << std::endl;

//next is displaying the invoice info for the selected order_id



// invoice information query
query = "SELECT cus.cus_id, cus.cus_fname, cus.cus_lname, inv.o_items, inv.o_tax, inv.o_subtotal, inv.o_total, inv.wiz_id, wiz.wiz_fname, wiz.wiz_lname\n";
query += "FROM invoice AS inv\n";
query += "JOIN customer AS cus ON cus.cus_id = inv.cus_id\n";
query += "JOIN wizards AS wiz ON wiz.wiz_id = inv.wiz_id\n";
query += "WHERE inv.order_id = " + order_id;

//new sql statement
sqlite3_stmt *pRes2;

//error message for query
	rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pRes2, NULL);
	if(rc != SQLITE_OK){
		sqlite3_finalize(pRes2);
		std::cout << "There was an error with the customer info query: " << sqlite3_errmsg(db) << std::endl;
		std::cout << query << std::endl;
		return 0;
	}

	sqlite3_step(pRes2);
	
//Declared data types to hold customer. 
std::string wiz_id, cus_id, o_items, o_tax, o_subtotal, o_total;
std::string wiz_lname, wiz_fname, cus_lname, cus_fname;

//Store customer info 
	cus_id = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 0));
	cus_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 1));
	cus_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 2));
	o_items = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 3));
	o_tax = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 4));
	o_subtotal = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 5));
	o_total = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 6));
	wiz_id = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 7));
	wiz_fname = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 8));
	wiz_lname = reinterpret_cast<const char *>(sqlite3_column_text(pRes2, 9));

	sqlite3_finalize(pRes2); 

//printing customer info
    std::cout << "----Invoice Information----" << std::endl;
	std::cout << "Customer ID: " << cus_id << std::endl;
	std::cout << "Customer Name: " << cus_fname << " " << cus_lname << std::endl;
	std::cout << "Total Items: " << o_items << std::endl;
	std::cout << "Tax: " << o_tax << std::endl;
	std::cout << "Subtotal: " << o_subtotal << std::endl;
	std::cout << "Total: " << o_total << std::endl;
	std::cout << "----associated wizard-----" << std::endl;
	std::cout << "Wizard ID: " << wiz_id << std::endl;
	std::cout << "Wizard name: " << wiz_fname << " " << wiz_lname << std::endl;
 return 0;   
}


//////////////////////////////////////////////////

void resetStream()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int rollback(sqlite3 *db)
{
    std::string query = "rollback";
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
        std::cout << "There was an error - rolling back the transaction: " << sqlite3_errmsg(db) << std::endl;
        // rollback(db);
        return rc;
    }
    return SQLITE_OK;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Structure to represent an account
struct Account {
    int accountNumber;
    int balance;
    char pin[5];
    int wrongPinAttempts;
    int cardBlocked;
    int dailyWithdrawal;
    struct tm lastWithdrawalDate;  // Add a new field to track the last withdrawal date
};

void changeATMBalance(int *amountInATM);

void changeATMBalance(int *amountInATM) {
    printf("Current ATM Balance: %d\n", *amountInATM);
    int addedAmount;
    printf("Enter the amount to add to the ATM Balance: ");
    scanf("%d", &addedAmount);
    
    if (addedAmount >= 0) {
        *amountInATM += addedAmount; // Add the entered amount to the ATM balance
        printf("ATM Balance updated successfully. New ATM Balance: %d\n", *amountInATM);
    } else {
        printf("Invalid amount. Please enter a non-negative value.\n");
    }
}

void readAccountsFromFile(struct Account accounts[], int *numAccounts) {
    FILE *file = fopen("accounts.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    *numAccounts = 0;

    while (*numAccounts < 100 && fscanf(file, "%d %d %4s %d",
            &accounts[*numAccounts].accountNumber,
            &accounts[*numAccounts].balance,
            accounts[*numAccounts].pin,
            &accounts[*numAccounts].cardBlocked) == 4) {
        accounts[*numAccounts].wrongPinAttempts = 0;
        (*numAccounts)++;
    }

    fclose(file);
}

void writeAccountsToFile(struct Account accounts[], int numAccounts) {
    FILE *file = fopen("accounts.txt", "w");
    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    for (int i = 0; i < numAccounts; i++) {
        fprintf(file, "%d %d %s %d\n",
                accounts[i].accountNumber,
                accounts[i].balance,
                accounts[i].pin,
                accounts[i].cardBlocked); // Write the cardBlocked as an integer
    }

    fclose(file);
}


void initializeParameters(int *t, int *k, int *m, int *n) {
    *t = 100000;// atm balance 
    *k = 25000;//daily withdrawal per day transaction
    *m = 15000;//maximum withdrawl per transaction
    *n = 1000;//Minimum cash in atm machine
}

bool isRunningOutOfMoney(int amount, int t) {
    return amount > t;
}

bool verifyPin(const char *enteredPin, const struct Account *account) {
    return strcmp(account->pin, enteredPin) == 0;
}

void updateWithdrawalHistory(int accountNumber, int withdrawalAmount, int balance, const char *description) {
    FILE *file = fopen("withdrawal_history.txt", "a");
    if (file == NULL) {
        printf("Error opening the history file.\n");
        exit(1);
    }

    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);

    printf("\n\t\t\tSlip\n");
    printf("-------------------------------------------------\n");
    printf("Account Number  : %d\n", accountNumber);
    printf("Transaction Date: %02d:%02d:%02d\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    printf("Amount          : %d\n", withdrawalAmount);
    printf("Account Balance : %d\n", balance);
    printf("-------------------------------------------------\n");

    fprintf(file, "AccountNumber:%d Withdraw:%d Balance:%d Description:%s Time:%02d:%02d:%02d\n",
            accountNumber, withdrawalAmount, balance, description, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    fclose(file);
}

void showTransactionHistory(int accountNumber, int numTransactionsToDisplay) {
    FILE *file = fopen("withdrawal_history.txt", "r");
    if (file == NULL) {
        printf("Error opening the history file.\n");
        exit(1);
    }

    char line[100];
    int transactionsDisplayed = 0;

    while (fgets(line, sizeof(line), file) && transactionsDisplayed < numTransactionsToDisplay) {
        int historyAccountNumber;
        if (sscanf(line, "AccountNumber:%d", &historyAccountNumber) == 1 && historyAccountNumber == accountNumber) {
            printf("%s", line);
            transactionsDisplayed++;
        }
    }

    fclose(file);
}

bool verifyBankLogin(const char *userId, const char *password) {
    FILE *file = fopen("bank_users.txt", "r");
    if (file == NULL) {
        printf("Error opening the bank user file.\n");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char storedUserId[50], storedPassword[50];
        if (sscanf(line, "%s %s", storedUserId, storedPassword) == 2) {
            if (strcmp(storedUserId, userId) == 0 && strcmp(storedPassword, password) == 0) {
                fclose(file);
                return true;
            }
        }
    }

    fclose(file);
    return false;
}

void createBankAccount(struct Account accounts[], int *numAccounts) {
    if (*numAccounts >= 100) {
        printf("Cannot create more accounts. The maximum limit has been reached.\n");
        return;
    }

    int newAccountNumber = 10000000 + rand() % 90000000;
    int newPassword = 1000 + rand() % 9000;

    for (int i = 0; i < *numAccounts; i++) {
        if (accounts[i].accountNumber == newAccountNumber) {
            newAccountNumber = 10000000 + rand() % 90000000;
            i = -1;
        }
    }

            // Inside the createBankAccount function
        accounts[*numAccounts].accountNumber = newAccountNumber;
        accounts[*numAccounts].balance = 0;
        sprintf(accounts[*numAccounts].pin, "%04d", newPassword);
        accounts[*numAccounts].wrongPinAttempts = 0;
        accounts[*numAccounts].cardBlocked = 0; // Initialize as unblocked
        accounts[*numAccounts].dailyWithdrawal = 0;
        (*numAccounts)++;


    printf("Account created successfully:\n");
    printf("Account Number: %d\n", newAccountNumber);
    printf("PIN: %04d\n", newPassword);
}
void unblockCard(struct Account accounts[], int numAccounts) {
    int accountNumber;
    printf("Enter the account number to unblock the card: ");
    scanf("%d", &accountNumber);

    for (int i = 0; i < numAccounts; i++) {
        if (accounts[i].accountNumber == accountNumber) {
            if (accounts[i].cardBlocked == 1) { // Check if the card is blocked
                accounts[i].cardBlocked = 0; // Unblock the card
                printf("Card with account number %d has been unblocked.\n", accountNumber);
                return;
            } else {
                printf("Card with account number %d is already unblocked.\n", accountNumber);
                return;
            }
        }
    }
    printf("Account number not found. Card unblocking failed.\n");
}




int main() {
    struct Account accounts[100];
    int numAccounts = 0;
    int authorizedAccountIndex = -1;

    // Load account information from the file
    readAccountsFromFile(accounts, &numAccounts);

    int t, k, m, n;
    initializeParameters(&t, &k, &m, &n);

    int cardInserted = 0;
    int amountInATM = t; // Initialize ATM balance
    while(1){
        printf("Options:\n1. Customer Login\n2. Bank Login\n3. Exit\n-------------------------\nATM Balance: %d\n-------------------------\nSelect an option: ", amountInATM);

        int customeroption;
        scanf("%d", &customeroption);
        switch(customeroption){
            
        case 1:{
            
        while (1) {
        if (!cardInserted) {
            printf("Options:\n1. Insert Cash Card\n2. Exit\n-------------------------\nATM Balance: %d\n-------------------------\nSelect an option: ", amountInATM);  // Display ATM balance
            int option;
            scanf("%d", &option);

            switch (option) {
                case 1: {
                    cardInserted = 1;
                    char enteredCardNumber[10], enteredPin[5];

                    // Ask for the serial number of the account
                    printf("\nEnter the serial number of the account (e.g., karb1234): ");
                    scanf("%s", enteredCardNumber);

                    // Extract the account number from the entered serial number
                    int enteredAccountNumber;
                    if (sscanf(enteredCardNumber, "karb%d", &enteredAccountNumber) != 1) {
                        printf("\nInvalid serial number format. Please start with 'karb' followed by the account number.\n");
                        cardInserted = 0; // Reset card insertion flag
                        break;
                    }

                    // Find the account index by account number
                    for (int i = 0; i < numAccounts; i++) {
                        if (enteredAccountNumber == accounts[i].accountNumber) {
                            authorizedAccountIndex = i;
                            break;
                        }
                    }

                    if (authorizedAccountIndex >= 0) {
                        if (accounts[authorizedAccountIndex].cardBlocked) {
                            printf("Your card has been blocked. Please call the bank for assistance.\n");
                            cardInserted = 0;
                            authorizedAccountIndex = -1;
                            break;
                        } else {
                            printf("Account authorization successful! Welcome to Karnataka Bank!\n");
                            int pinAttempts = 0;

                            while (pinAttempts < 3) {
                                printf("Enter your PIN: ");
                                scanf("%s", enteredPin);

                                if (verifyPin(enteredPin, &accounts[authorizedAccountIndex])) {
                                    // Reset wrong PIN attempts on successful login
                                    accounts[authorizedAccountIndex].wrongPinAttempts = 0;
                                    printf("Authorization successful.\n");
                                     accounts[authorizedAccountIndex].dailyWithdrawal = 0;
                                    time_t currentTime;
                                    time(&currentTime);
                                    accounts[authorizedAccountIndex].lastWithdrawalDate = *localtime(&currentTime);

                                    break;
                                } else {
                                    // Increment wrong PIN attempts on unsuccessful login
                                    accounts[authorizedAccountIndex].wrongPinAttempts++;
                                    pinAttempts++;
                           if (pinAttempts < 3) {
                                    printf("Invalid PIN. Please try again.\n");
                                } else {
                                    printf("Invalid PIN. Your card has been blocked. Please call the bank for assistance.\n");
                                    accounts[authorizedAccountIndex].cardBlocked = true; // Set cardBlocked flag 
                                    cardInserted = 0;
                                    authorizedAccountIndex = -1;
                                    break;
                                }

                                }
                            }

                            if (accounts[authorizedAccountIndex].cardBlocked) {
                                break;  // Return to the main menu
                            }

                         
                            while (1) {
                                printf("Options:\n1. Show Transaction History\n2. Withdraw amount\n3. Display Account Balance\n4. Logout\n5. Deposit Amount\nATM Balance: %d\n", amountInATM);  // Display ATM balance
                                printf("Select an option: ");
                                int userOption;
                                scanf("%d", &userOption);
                                int numTransactionsToDisplay;
                                switch (userOption) {
                                    case 1:
                                        if (authorizedAccountIndex >= 0) {
                                            int numTransactionsToDisplay;
                                            printf("Enter the number of previous transactions to display: ");
                                            scanf("%d", &numTransactionsToDisplay);
                                            
                                            printf("Transaction History:\n");
                                            showTransactionHistory(accounts[authorizedAccountIndex].accountNumber, numTransactionsToDisplay);
                                        } else {
                                            printf("You are not logged in. Please insert your card to access transaction history.\n");
                                        }
                                        break;
                                   case 2: {
    int withdrawalAmount;
    printf("Enter the withdrawal amount: ");
    scanf("%d", &withdrawalAmount);

    if (withdrawalAmount <= accounts[authorizedAccountIndex].balance) {
        if (withdrawalAmount <= amountInATM) {
            if (withdrawalAmount >= n) {
                if (withdrawalAmount > m) {
                    printf("Maximum withdrawal limit per transaction exceeded. Please try a smaller amount.\n");
                } else if (amountInATM - withdrawalAmount < n) {
                    printf("Minimum ATM balance should exist is 1000, and your withdrawal is exceeding the limit.\n");
                } else {
                    // Get the current time
                    time_t currentTime;
                    time(&currentTime);
                    struct tm *localTime = localtime(&currentTime);

                    // Calculate the difference in days between the last withdrawal date and the current date
                    int daysDifference = localTime->tm_mday - accounts[authorizedAccountIndex].lastWithdrawalDate.tm_mday;

                    // Check if the last withdrawal date is not the same as the current date
                    if (daysDifference != 0) {
                        // If the last withdrawal date is not the same as the current date, reset the daily withdrawal amount
                        accounts[authorizedAccountIndex].dailyWithdrawal = 0;
                    }

                    if (accounts[authorizedAccountIndex].dailyWithdrawal + withdrawalAmount > k) {
                        printf("Withdrawal amount exceeds the daily limit. You can withdraw a maximum of %d tomorrow.\n", k - accounts[authorizedAccountIndex].dailyWithdrawal);
                    } else {
                        accounts[authorizedAccountIndex].balance -= withdrawalAmount;
                        accounts[authorizedAccountIndex].dailyWithdrawal += withdrawalAmount;
                        // Update lastWithdrawalDate
                        accounts[authorizedAccountIndex].lastWithdrawalDate = *localTime;
                        amountInATM -= withdrawalAmount; // Update ATM balance
                        printf("Withdrawal successful. New Balance: %d\n", accounts[authorizedAccountIndex].balance);

                        // Update transaction description
                        char transactionDescription[100];
                        snprintf(transactionDescription, sizeof(transactionDescription), "Withdrawal of %d", withdrawalAmount);

                        updateWithdrawalHistory(accounts[authorizedAccountIndex].accountNumber,
                                                withdrawalAmount,
                                                accounts[authorizedAccountIndex].balance,
                                                transactionDescription);

                        writeAccountsToFile(accounts, numAccounts);
                    }
                }
            } else {
                printf("Minimum cash in the ATM required to permit a transaction is 1000. Please try again later.\n");
            }
        } else {
            printf("Insufficient funds in the ATM. Please try a smaller amount.\n");
        }
    } else {
        printf("Insufficient balance. Please try a smaller amount.\n");
    }
    break;
}

                                    case 3:
                                        printf("Account Balance: %d\n", accounts[authorizedAccountIndex].balance);
                                        break;
                                    case 4:
                                        cardInserted = 0;
                                        authorizedAccountIndex = -1;
                                        printf("You have logged out from the account.\n");
                                        break;
                                    case 5: { // New option to deposit money
                                        int depositAmount;
                                        printf("Enter the deposit amount: ");
                                        scanf("%d", &depositAmount);
                                    
                                        if (depositAmount > 0) {
                                            accounts[authorizedAccountIndex].balance += depositAmount;
                                            printf("Deposit successful. New Balance: %d\n", accounts[authorizedAccountIndex].balance);
                                    
                                            // Update transaction description
                                            char transactionDescription[100];
                                            snprintf(transactionDescription, sizeof(transactionDescription), "Deposit of %d", depositAmount);
                                    
                                            updateWithdrawalHistory(accounts[authorizedAccountIndex].accountNumber,
                                                    depositAmount,
                                                    accounts[authorizedAccountIndex].balance,
                                                    transactionDescription);
                                    
                                            writeAccountsToFile(accounts, numAccounts);
                                        } else {
                                            printf("Invalid deposit amount. Please enter a positive amount.\n");
                                        }
                                    
                                        break;
                                    }
                                    default:
                                        printf("Invalid option. Please try again.\n");
                                        break;
                                }
                                if (userOption == 4) {
                                    break;
                                }
                            }
                        }
                    } else {
                        printf("Invalid serial number. Please try again.\n");
                        cardInserted = 0;
                    }
                    break;
                }
                case 2:
                    writeAccountsToFile(accounts, numAccounts);
                    printf("Thank you for using the ATM. Goodbye!\n");
                    return 0;
                default:
                    printf("Invalid option. Please try again.\n");
                    break;
                     }
                }
            }
    return 0;
        }
        case 2:
        {
                // Bank Login
                while (1) {
                    printf("Options:\n1. Login to the Bank system\n2. Exit\nSelect an option: ");
                    int bankLoginOption;
                    scanf("%d", &bankLoginOption);

                    switch (bankLoginOption) {
                        case 1: {
    printf("Bank Login\n");
    char enteredUserId[50], enteredPassword[50];

    printf("Enter User ID: ");
    scanf("%s", enteredUserId);
    printf("Enter Password: ");
    scanf("%s", enteredPassword);

    if (verifyBankLogin(enteredUserId, enteredPassword)) {
        printf("Bank login successful!\n");
        while (1) {
            printf("Options:\n1. Create an account\n2. Update ATM Balance\n3. Unblock Card\n4. Exit\nSelect an option: ");
            int bankMenuOption;
            scanf("%d", &bankMenuOption);
            switch (bankMenuOption) {
                case 1: {
                    createBankAccount(accounts, &numAccounts);
                    writeAccountsToFile(accounts, numAccounts);
                    break;
                }
                case 2: {
                    changeATMBalance(&amountInATM);
                    break;
                }
                case 3: {
                    unblockCard(accounts, numAccounts);
                    writeAccountsToFile(accounts, numAccounts);
                    break;
                }
                case 4: {
                    break;
                }
                default: {
                    printf("Invalid option. Please try again.\n");
                    break;
                }
            }
            if (bankMenuOption == 4) {
                break;  // Exit the bank menu and return to the Bank Login menu
            }
        }
    } else {
        printf("Invalid User ID or Password. Bank login failed.\n");
    }
    break;
}

                        case 2: {
                            break;
                        }
                        default: {
                            printf("Invalid option. Please try again.\n");
                            break;
                        }
                    }
                    if (bankLoginOption == 2) {
                        break;  // Exit the bank login loop and return to the main menu
                    }
                }
                break;
        }
       
        case 3:
        writeAccountsToFile(accounts, numAccounts);
        printf("Thank you for using the ATM. Goodbye!\n");
        return 0;
            
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }
    }
}
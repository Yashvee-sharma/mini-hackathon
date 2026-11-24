/*
 * PERSONAL EXPENSE TRACKER  (final version)
 *
 * Features:
 *   - Add, view, edit and delete expenses (amount, category, note)
 *   - Add income and see your balance
 *   - Totals per category, highest expense, average expense
 *   - Category percentages with a text bar chart
 *   - Monthly budget with warnings
 *   - Savings goal with a "goal reached" message
 *   - Input checks: bad input is rejected and asked again
 *
 * Compile:  gcc expense_tracker.c -o expense_tracker
 * Run:      ./expense_tracker     (on Windows: expense_tracker.exe)
 */

#include <stdio.h>   // for printf and scanf
#include <stdlib.h>  // for exit

#define MAX 100      // the most expenses we can store
#define NCAT 6       // number of categories

// The fixed list of categories (position 0 = Food, 1 = Travel, ...)
const char *catNames[NCAT] = {"Food", "Travel", "Bills", "Fun", "Shopping", "Other"};

// One expense = amount + category number + note
struct Expense {
    int amount;
    int category;      // a number from 0 to 5 (matches catNames)
    char note[30];
};

struct Expense list[MAX];  // the list of all expenses
int count = 0;             // how many expenses we have right now
int income = 0;            // total money received
int budget = 0;            // monthly budget (0 means "not set")
int goal = 0;              // savings goal (0 means "not set")

// ---------- Helper functions ----------

// Keeps asking until the user types a whole number between min and max
int readInt(const char *msg, int min, int max) {
    int n, result;
    while (1) {
        printf("%s", msg);
        result = scanf("%d", &n);
        if (result == EOF) {                 // no more input at all (safety)
            printf("\nInput ended.\n");
            exit(0);
        }
        if (result != 1) {                   // the user typed letters
            while (getchar() != '\n') { }    // throw away the bad input
            printf("Please type a number.\n");
        } else if (n < min || n > max) {
            printf("Please enter a number from %d to %d.\n", min, max);
        } else {
            return n;                        // good number, give it back
        }
    }
}

// Shows the category list and returns the chosen category (0 to 5)
int chooseCategory() {
    for (int i = 0; i < NCAT; i++) {
        printf("  %d. %s\n", i + 1, catNames[i]);
    }
    return readInt("Choose category: ", 1, NCAT) - 1;
}

// Adds up all expenses and gives back the total
int totalSpent() {
    int total = 0;
    for (int i = 0; i < count; i++) {
        total = total + list[i].amount;
    }
    return total;
}

// Adds up the expenses of one category
int categoryTotal(int c) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].category == c) {
            sum = sum + list[i].amount;
        }
    }
    return sum;
}

// Prints a warning if spending is close to or above the budget
void checkBudget() {
    int spent = totalSpent();
    if (budget == 0) {
        return;
    }
    if (spent > budget) {
        printf("WARNING: You are over budget by %d!\n", spent - budget);
    } else if (spent * 100 >= budget * 80) {
        printf("Careful: you have used 80%% or more of your budget.\n");
    }
}

// Prints a message about the savings goal
void checkGoal() {
    int balance = income - totalSpent();
    if (goal == 0) {
        return;
    }
    if (balance >= goal) {
        printf("GOAL REACHED! Your balance %d has passed your goal %d.\n", balance, goal);
    } else {
        printf("Savings goal: you still need %d more.\n", goal - balance);
    }
}

// Prints one row of the table
void printRow(int i) {
    printf("%-4d %-7d %-10s %s\n", i + 1, list[i].amount, catNames[list[i].category], list[i].note);
}

// ---------- Menu tasks ----------

// 1. Add a new expense
void addExpense() {
    if (count >= MAX) {
        printf("List is full!\n");
        return;
    }
    list[count].amount = readInt("Amount: ", 1, 1000000);
    list[count].category = chooseCategory();
    printf("Note (one word): ");
    scanf("%29s", list[count].note);
    count++;
    printf("Expense added!\n");
    checkBudget();
}

// 2. Show all expenses
void viewExpenses() {
    if (count == 0) {
        printf("No expenses yet.\n");
        return;
    }
    printf("\nNo.  Amount  Category   Note\n");
    for (int i = 0; i < count; i++) {
        printRow(i);
    }
}

// 3. Delete an expense
void deleteExpense() {
    if (count == 0) {
        printf("Nothing to delete.\n");
        return;
    }
    viewExpenses();
    int n = readInt("Number to delete: ", 1, count);
    for (int i = n - 1; i < count - 1; i++) {
        list[i] = list[i + 1];   // shift everything after it one step up
    }
    count--;
    printf("Deleted.\n");
}

// 4. Show the overall total and the total for one category
void showTotals() {
    printf("Total spent: %d\n", totalSpent());
    int c = chooseCategory();
    printf("Total for %s: %d\n", catNames[c], categoryTotal(c));
}

// 5. Add money received
void addIncome() {
    income = income + readInt("Money received: ", 1, 10000000);
    printf("Income added!\n");
    checkGoal();
}

// 6. Show income, spending and what is left
void showBalance() {
    printf("Income : %d\n", income);
    printf("Spent  : %d\n", totalSpent());
    printf("Balance: %d\n", income - totalSpent());
    if (budget > 0) {
        printf("Budget : %d (left: %d)\n", budget, budget - totalSpent());
    }
    checkGoal();
}

// 7. Edit an expense (replace its details)
void editExpense() {
    if (count == 0) {
        printf("Nothing to edit.\n");
        return;
    }
    viewExpenses();
    int n = readInt("Number to edit: ", 1, count);
    list[n - 1].amount = readInt("New amount: ", 1, 1000000);
    list[n - 1].category = chooseCategory();
    printf("New note (one word): ");
    scanf("%29s", list[n - 1].note);
    printf("Expense updated!\n");
    checkBudget();
}

// 8. Find and show the biggest expense
void highestExpense() {
    if (count == 0) {
        printf("No expenses yet.\n");
        return;
    }
    int big = 0;
    for (int i = 1; i < count; i++) {
        if (list[i].amount > list[big].amount) {
            big = i;
        }
    }
    printf("Biggest expense: %d on %s (%s)\n", list[big].amount, catNames[list[big].category], list[big].note);
}

// 9. Show only the expenses of one category
void searchCategory() {
    int c = chooseCategory();
    int found = 0;
    printf("\nNo.  Amount  Category   Note\n");
    for (int i = 0; i < count; i++) {
        if (list[i].category == c) {
            printRow(i);
            found++;
        }
    }
    if (found == 0) {
        printf("No expenses found in %s.\n", catNames[c]);
    }
}

// 10. Set the monthly budget
void setBudget() {
    budget = readInt("Enter your monthly budget: ", 1, 10000000);
    printf("Budget set to %d.\n", budget);
    checkBudget();
}

// 11. Show the average expense
void averageExpense() {
    if (count == 0) {
        printf("No expenses yet.\n");
        return;
    }
    printf("Average expense: %.2f\n", (float)totalSpent() / count);
}

// 12. Show percentage and a bar chart for each category
void categorySummary() {
    int total = totalSpent();
    if (total == 0) {
        printf("No expenses yet.\n");
        return;
    }
    printf("\nSpending by category (each # = 5%%)\n");
    for (int c = 0; c < NCAT; c++) {
        int sum = categoryTotal(c);
        int percent = sum * 100 / total;
        printf("%-9s %6d  %3d%%  ", catNames[c], sum, percent);
        for (int j = 0; j < percent / 5; j++) {
            printf("#");
        }
        printf("\n");
    }
}

// 13. Set a savings goal
void setGoal() {
    goal = readInt("Enter your savings goal (balance you want): ", 1, 100000000);
    printf("Goal set to %d.\n", goal);
    checkGoal();
}

int main() {
    int choice;
    do {
        printf("\n===== EXPENSE TRACKER =====\n");
        printf(" 1. Add expense\n");
        printf(" 2. View expenses\n");
        printf(" 3. Delete expense\n");
        printf(" 4. Show totals\n");
        printf(" 5. Add income\n");
        printf(" 6. Show balance\n");
        printf(" 7. Edit expense\n");
        printf(" 8. Highest expense\n");
        printf(" 9. Show one category\n");
        printf("10. Set monthly budget\n");
        printf("11. Average expense\n");
        printf("12. Category percentages and chart\n");
        printf("13. Set savings goal\n");
        printf(" 0. Exit\n");
        choice = readInt("Choose: ", 0, 13);

        switch (choice) {
            case 1:  addExpense();      break;
            case 2:  viewExpenses();    break;
            case 3:  deleteExpense();   break;
            case 4:  showTotals();      break;
            case 5:  addIncome();       break;
            case 6:  showBalance();     break;
            case 7:  editExpense();     break;
            case 8:  highestExpense();  break;
            case 9:  searchCategory();  break;
            case 10: setBudget();       break;
            case 11: averageExpense();  break;
            case 12: categorySummary(); break;
            case 13: setGoal();         break;
            case 0:  printf("Goodbye!\n"); break;
        }
    } while (choice != 0);

    return 0;
}

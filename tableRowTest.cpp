#include<iostream>
#include <string>
#include"TableRow.h"

int main() {

    TableRow<int, 2> tableRow;
    // tableRow2 = tableRow;
    // tableRow2.add(99);
    // tableRow2.add(999);
    // tableRow2.add(9999);
    // tableRow2.dump();


    // TableRow <int, 5>newRow {tableRow};
    // TableRow <int, 5>newRow;
    // newRow = tableRow;
    
    // newRow.add(20);

    // newRow.dump();
    // tableRow.dump();

    // newRow.clear();
    // std::cout << "asdasdad" << std::endl;
    // tableRow.clear();

    // for (size_t i = 1; i <= 10; i++) {
    //     tableRow.dump();
    //     tableRow.erase(i);
    // }


    for (size_t i = 1; i <= 10; i++) {
        tableRow.add(i);
    }

    std::string input = "";
    int operation = 0;
    while(true) {
        tableRow.dump();
        if (operation == 0) {
            std::cout << "Welcher Index soll removed werden? [i] ";
        } else {
            std::cout << "Welcher Index soll inserted werden? [r] ";
        }
        std::cin >> input;
        if (input == "i") {
            operation = 1; 
            continue;
        }
        else if (input == "r") {
            operation = 0; 
            continue;
        }

        int index = stoi(input);
        if (operation == 0) {
            tableRow.erase(index);    
        } else {
            if (!tableRow.find(index)) {
                tableRow.add(index);
            }
        }
        std::cout << std::endl;
    }





    // tableRow.dump();

    return 0;
}
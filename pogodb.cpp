#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

// AUTHOR: Baitos, January 3rd 2024
// TODO: Optimize modifying gyms so that when a node becomes the head due to renaming you don't need to reinitialize list (print works fine, not list?)
//       Maybe make it so write_to_gyms_list() doesn't completely wipe the file before rewriting to it? So if program crashes you don't just lose all data
//       On Jan 31st I wrote a shit ton of stuff: Added Number, completely changed adding a gold gym's string parameters. Seems to work fine but has a bunch of band-aid fixes. Might want to come back to it later    

struct gym_list {
    string name;
    string location;
    int number; 
    bool exists;
    struct gym_list *prev; // dont feel like using dynamic arrays so fuck it double double linked lists
    struct gym_list *next;
};

struct xl_candy_list {
    string name; // Pokemon name
    bool xl; // has enough xls?
    struct xl_candy_list *prev; // previous evolution 
    struct xl_candy_list *next; // next evolution 
    struct xl_candy_list *alt; // alternate forms, (alola, galar, etc.)
};

gym_list* make_node_from_string(string line) { // given a line, populates a node and returns it (like a constructor kinda)
    gym_list *gym = new gym_list();
    istringstream iss(line); 
    string token;
    for (int i = 0; i < 4; ++i) {
        getline(iss, token, ',');
        switch(i) {
            case 0: 
                gym->name = token; 
                break;
            case 1: 
                gym->location = token; 
                break;
            case 2: 
                try {
                    gym->number = stoi(token);
                    if (gym->number <= 0) {
                        gym->number = -1;
                    }
                } catch (invalid_argument a) {
                    gym->number = -1;
                }
                break;
            case 3: 
                token = token.substr(0, 1);               
                if ((token == "n") || (token == "N") || (token == "0")) {
                    gym->exists = 0;
                } else {
                    gym->exists = 1;
                } 
                break;
        }
    }
    return gym;
}

gym_list* initialize_gym_list() {
    ifstream file("pokemonGyms.txt");
    gym_list *head = NULL;
    gym_list *temp = NULL;
    string line;
    if (file.is_open()) {
        if (!(file.peek() == std::ifstream::traits_type::eof())) {
            getline(file, line);
            //cout << line << '\n';
            //istringstream iss(line);
            //string token;
            
            head = make_node_from_string(line);
            head->prev = nullptr;
            head->next = nullptr;
            temp = head;

            while (getline(file, line)) {
                gym_list *next = make_node_from_string(line); 
                next->prev = head;
                next->next = nullptr;
                head->next = next;
                head = head->next;
            }
        }
    }
    else {
        ofstream file("pokemonGyms.txt"); 
    }
    file.close();
    return temp;
}

void write_gyms_to_file(gym_list *list) { // reprint file to accomodate for addition, list should be head 
    gym_list *temp = list;
    ofstream file("pokemonGyms.txt", ios::trunc); 
    if (!file.is_open()) {
        cout << "Error: Unable to open gyms txt file to write. Might be some problems there.\n";
    } else {
        while (list != NULL) {
            file << list->name << "," << list->location << "," << list->number << "," << list->exists << '\n';
            list = list->next;
        }
    }
    file.close();
    list = temp;
}

void add_to_gym_list(gym_list **list, gym_list *node) {
    if (*list == NULL || (((*list)->name == "") && ((*list)->location == "") && ((*list)->exists == 0) && ((*list)->number == 0))) {
        *list = node;
    } else {
        while (1) {
            if (node->name < (*list)->name) {
                //cout << node->name << " < " << (*list)->name << "\n";
                
                node->next = *list;
                node->prev = (*list)->prev; 
                if ((*list)->prev != NULL) {
                    (*list)->prev->next = node;
                }
                (*list)->prev = node;

                break;
            
            } else {
                if ((*list)->next == NULL) {
                    node->prev = *list;
                    node->next = (*list)->next;
                    (*list)->next = node;                
                    break;
                } else {
                    *list = (*list)->next;
                }
            }
        }
        while ((*list)->prev != NULL) { // reset back to head
            *list = (*list)->prev;
        }
    }
    
    write_gyms_to_file(*list);
}

void delete_from_gym_list(gym_list **list, gym_list *node) { // delete from gym_list, list should be head
    if (*list == node) {
        //cout << "list = node next\n";
        *list = node->next;
        //(*list)->prev = NULL;
        //cout << "list = node next\n";
    }
    if (node->next != NULL) {
        //cout << "list = node next not null\n";
        node->next->prev = node->prev;
    } 
    if (node->prev != NULL) {
        //cout << "list = node prev not null\n";
        node->prev->next = node->next;
    }
    delete(node); 
    write_gyms_to_file(*list);
}

xl_candy_list* initialize_xl_list() {
    return NULL;
}

int print_gold_gyms(gym_list *list, string loc) { // print all gold gyms and return count
    int count = 0;
    gym_list *head = list;
    cout << "---------------------------------------------------------\n";
    if (loc == "All") { // print all gold gyms
        if (list != NULL && !((list->name == "") && (list->location == "") && (list->exists == 0) && (list->number == 0))) { // list may be empty
            //cout << "start\n";
            while (list != NULL) { 
                cout << (++count) << ". " << list->name << ", " << list->location << ", ";
                if (list->number != -1) {
                    cout << list->number << ", ";
                } else {
                    cout << "N/A, ";
                }
                if (list->exists == 1) {
                    cout << "Yes\n";
                } else {
                    cout << "No\n";
                }
                list = list->next;
                //++count;
            }
        } else {
            cout << "No Gold Gyms Found.\n";
        }
    } else { // print the location specific gold gyms
        if (list != NULL && !((list->name == "") && (list->location == "") && (list->exists == 0) && (list->number == 0))) { // list may be empty
            //cout << "start\n";
            while (list != NULL) { 
                if (list->location == loc) {
                    cout << (++count) << ". " << list->name << ", " << list->location << ", ";
                    if (list->number != -1) {
                    cout << list->number << ", ";
                    } else {
                        cout << " N/A, ";
                    }
                    if (list->exists == 1) {
                        cout << "Yes\n";
                    } else {
                        cout << "No\n";
                    }
                }
                list = list->next;
            }
        } else {
            cout << "No Gold Gyms Found.\n";
        }
    }
    cout << "---------------------------------------------------------\n";
    list = head;
    //cout << "You have " << count << " gold gyms.\n";
    return count;
}



gym_list* sort_gym_list(gym_list* list) { // quicksort a gym list to be in order of completion    
    
    return NULL;
}

void gold_gym_database() {
    int res;
    gym_list *list;// = initialize_gym_list();
    while (1) {
        list = initialize_gym_list(); // a necessary evil for the case where you change the head i could prob fix but idk
        /*if (list == NULL) { // in case the file doesn't exist
            break;
        }*/
        cout << "Gold Gym Database:\n";
        cout << "1. View Gold Gyms\n";
        cout << "2. Add Gold Gyms\n";
        cout << "3. Modify Gold Gyms\n"; // name, location, if removed from game
        cout << "0. Exit\n"; // name, location, if removed from game
        cin >> res;

        if (res == 1) { // View Gold Gyms
            // Print out all gold gyms, locations, and if removed
            while (1) {
                cout << "1. Print Gyms Alphabetically\n";
                cout << "2. Print Gyms by Location\n";
                cout << "3. Print Gyms by Order\n";
                cin >> res;
                if (res == 1) { // Print all Gold Gyms, locations, and if removed
                    print_gold_gyms(list, "All");
                    break;
                } else if (res == 2) { // Ask for location then print all of those
                    string loc;
                    cout << "Please Enter Location:\n";                   
                    cin.ignore();
                    getline(cin, loc);
                    print_gold_gyms(list, loc);
                    break;
                } else if (res == 3) { // sort and print gyms in order of completion
                    gym_list *sort = list;


                    //sort_gym_list(*sort, 0, );
                    print_gold_gyms(sort, "All");
                    break;
                } else  { // Dumbass
                    cout << "Please enter valid input!\n";
                    cin.clear();
                    cin.ignore();
                }
            }
            res = 1;
        } else if (res == 2) { // Add Gold Gyms
            cin.ignore();
            while (1) { 
                cout << "Type Gyms in Format: \"Name, Location, Number, Y/N\" or type 0 to quit\n";
                string gym;
                string yesOrNo;
                getline(cin, gym);
                if ((gym == "0")) {
                    break;
                }

                int count = 0;
                for (char c : gym) {
                    if (c == ',') {
                        count++;
                    }
                }
                if (count == 1) { // this is so dumb, all this does is prevent a null char from being printed to the txt file in the event the user types only "Name, Location" (WITH THE SPACE)
                    gym += ",";
                }
                count = 0;
                for (int i = 0; gym[i]; i++) { // remove spaces so tokens can be obtained
                    if (!(gym[i] == ' ' && i > 0 && gym[i - 1] == ',')) {
                        gym[count++] = gym[i]; 
                    }
                }
                gym[count] = '\0';

                gym_list *new_gym = make_node_from_string(gym);
                if ((new_gym->name == "") || (new_gym->location == "")) {
                    cout << "Please enter valid gym input!\n";
                    delete(new_gym);
                    cin.clear();
                } else {
                    cout << "Is this correct? (Y): " << new_gym->name << ", " << new_gym->location << ", ";
                    if (new_gym->number == -1) {
                        cout << "N/A, ";
                    } else {
                        cout << new_gym->number << ", ";
                    }
                    if (new_gym->exists == 1) {
                        cout << "Y\n";
                    } else {
                        cout << "N\n";
                    }
                    getline(cin, yesOrNo);
                    if ((yesOrNo == "Y") || (yesOrNo == "y")) { 
                        add_to_gym_list(&list, new_gym);
                    } else {
                        delete(new_gym);
                    }
                    cin.clear();
                }
            }
        } else if (res == 3) { // Modify Gold Gyms
            res = -1;
            int count = print_gold_gyms(list, "All");
            string choice;
            gym_list *temp = list; 
            if (count != 0) {   
                cout << "Please select number of gym to edit, or type 0 to quit. (Hint: Use CTRL+F to find name)\n";    
                 
                //bool success = 1;

                while (1) {
                    cin >> choice;
                    //cout << choice << '\n';
                    try {
                        res = stoi(choice);
                        //cout << "success!\n";
                        if (res <= count) {
                            break;
                        } else {
                            cout << "Too big! Please enter valid input!\n";
                            cin.clear();
                            cin.ignore();
                        }                   
                    } catch (invalid_argument a) {
                        cout << "Please enter valid input!\n";
                        cin.clear();
                        cin.ignore();
                    }
                }
                if (res != 0) {
                    for (int i = 1; i < res; i++) {
                        if (list->next != NULL) { 
                            list = list->next;
                        }
                    }
                    
                    gym_list *edit = list; // gym to potentially change
                    list = temp;

                    while (1) { // Choose what to do with gym
                        cout << "You have chosen: " << edit->name << ", " << edit->location << ", ";
                        if (edit->exists == 1) {
                            cout << "Yes, ";
                        } else {
                            cout << "No, ";
                        } 
                        if (edit->number == -1) {
                            cout << "N/A\n";
                        } else {
                            cout << edit->number << "\n";
                        }
                        cout << "1. Change Name\n";
                        cout << "2. Change Location\n";
                        cout << "3. Update Existence\n";
                        cout << "4. Update Position\n";
                        cout << "5. Delete Gym From List\n";
                        cout << "0. Exit\n";
                        //cin.clear();
                        //cin.ignore();
                        cin >> res;
                        //cout << "res = " << res << "\n";
                        if (res == 1) { // Change Gym Name and Reorganize
                            cin.ignore();
                            cout << "Type New Name, or type 0 to quit\n";
                            string new_name;
                            getline(cin, new_name);
                            if (list == edit) {
                                temp = temp->next;
                            }
                            delete_from_gym_list(&list, edit); 
                            if (!(new_name == "0")) {
                                edit->name = new_name;
                            }
                            add_to_gym_list(&list, edit);
                        } else if (res == 2) { // Change Location
                            cin.ignore();
                            cout << "Type New Location, or type 0 to quit\n";
                            string new_loc;
                            getline(cin, new_loc);
                            if (!(new_loc == "0")) {
                                edit->location = new_loc;
                            }
                            write_gyms_to_file(list);
                        } else if (res == 3) { // Change whether or not it exists
                            edit->exists = !edit->exists;
                            write_gyms_to_file(list);
                        } else if (res == 4) { // Update Gym Order
                            while (1) {
                                cin.ignore();
                                cout << "Type New Position, or type 0 to quit\n";
                                string num;
                                getline(cin, num);
                                try {
                                    int new_num = stoi(num);
                                    if (!(new_num == 0)) {
                                        edit->number = new_num;
                                    }
                                    if (edit->number < 0) {
                                        edit->number = -1;
                                    }
                                    break;
                                } catch (invalid_argument a) {
                                    cout << "Please enter valid order input!\n";
                                }       
                            }                     
                            write_gyms_to_file(list);
                        } else if (res == 5) { // Delete Gym
                            if (list == edit) {
                                temp = temp->next;
                            }
                            delete_from_gym_list(&list, edit);                            
                            break;
                        } else if (res == 0) { // Exit
                            //cin.clear();
                            //cin.ignore();
                            break;
                        }  else  { // Dumbass
                            cout << "Please enter valid input!\n";
                            cin.clear();
                            cin.ignore();
                        }
                    }
                }
            }
            list = temp;
            res = 3;        
        } else if (res == 0) { // Exit
            break;
        } else { // Dumbass
            cout << "Please enter valid input!\n";
            cin.clear();
            cin.ignore();
        }
    }
}

void xl_candy_database() {
    cout << "Under Construction!:\n";
    // Array of pokemon represented by Doubly linked list with 3rd link for alternate forms (prev = prev stage, next = next stage, third = alt forms, meowth gets two bc fuck)
}

int main() {
    int res;
    cout << "Welcome to the Pogo Database!\n";
    while (1) {
        cout << "1. Gold Gyms Database\n";
        cout << "2. XL Candy Database\n";
        cout << "0. Exit\n";
        cin >> res;
        if (res == 1) { // Gold Gyms
            gold_gym_database();
            
        } else if (res == 2) { // XLS
            xl_candy_database();
            
        } else if (res == 0) { // Exit
            cout << "Goodbye!\n";
            break;
        } else  { // Dumbass
            cout << "Please enter valid input!\n";
            cin.clear();
            cin.ignore();
            //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}


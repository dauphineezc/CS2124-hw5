// Zoe Dauphinee, hw04, Warriors and Nobles on the heap

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class Warrior {
    private:
        string name;
        double strength;
        bool employed;
    public:
        // getter for name
        const string& get_name() const {
            return name;
        }
        // getter for strength
        const double get_strength() const {
            return strength;
        }
        // change strength during battle
        void change_strength(double new_strength) {
            strength = new_strength;
        }
        // getter for employed
        const bool get_employment_status() const {
            return employed;
        }
        // changes employed to true when hired
        void change_employment_status(bool status) {
            employed = status;
        }

        // constructor
        Warrior(const string& name, double strength)
        : name(name), strength(strength), employed(false) {}

    // output operator
    friend ostream& operator<<(ostream& os, const Warrior& warrior);
};

class Noble {
    private:
        string name;
        bool dead;
        vector<Warrior*> army;

    public:
        // getter for name
        const string& get_name() const {
            return name;
        }
        // getter for dead/alive
        const bool get_life_status() const {
            return dead;
        }

        // constructor
        Noble(const string& name) 
        : name(name), dead(false), army(0) {}

        // hire a Warrior
        void hire(Warrior& warrior) {
            if (!dead) {
                // if Noble is not dead, check to see if Warrior is already employed
                if (warrior.get_employment_status()) {
                    cout << name << " failed to hire " << warrior.get_name() << endl;
                }
                else {
                    army.push_back(&warrior);
                    warrior.change_employment_status(true);
                }
            }
        }

        // fire a Warrior
        void fire(Warrior& warrior) {
            if (!dead) {
                // if Noble is not dead, check to see if Warrior is in Noble's army
                bool found = false;
                for (size_t index = 0; index < army.size(); ++index) {
                    if (army[index]->get_name() == warrior.get_name()) {
                        found = true;
                        warrior.change_employment_status(false);
                        cout << warrior.get_name() << ", you don't work for me any more! -- ";
                        cout << name << "." << endl;
                        swap(army[index], army[army.size() - 1]);
                        army.pop_back();
                        break;
                    }
                }
                // Warrior isn't in Noble's army
                if (!found) {
                    cout << name << " failed to fire " << warrior.get_name() << endl;
                }
            }
            // Noble doesn't exist
            if (dead) {
            cout << "failed to fire - Noble " << name << " doesn't exist!\n";
            }
        }

        // calculate strength of a Noble's army
        double army_strength() {
            double strength = 0;
            for (size_t index = 0; index < army.size(); ++index) {
                strength += army[index]->get_strength();
            }
            return strength;
        }

        // two Nobles battle, outcome based on army strengths
        void battle(Noble& opponent) {
            cout << endl << name << " battles " << opponent.name << endl;
            double my_strength = army_strength();
            double opp_strength = opponent.army_strength();

            if (dead && opponent.dead) {
                cout << "Oh, NO!  They're both dead!  Yuck!" << endl;
                return;
            }
            else if (dead) {
                cout << "He's dead, " << opponent.name << endl;
                return;
            }
            else if (opponent.dead) {
                cout << "He's dead, " << name << endl;
                return;
            }

            if (my_strength > opp_strength) {
                double reduce_by = (opp_strength/my_strength);
                for (size_t index = 0; index < army.size(); ++index) {
                    double new_strength = army[index]->get_strength() * (1-reduce_by);
                    army[index]->change_strength(new_strength);
                }
                for (size_t index = 0; index < opponent.army.size(); ++index) {
                    opponent.army[index]->change_strength(0);
                }
                opponent.dead = true;
                cout << name << " defeats " << opponent.name << endl;
            }
            else if (opp_strength > my_strength) {
                double reduce_by = (my_strength/opp_strength);
                for (size_t index = 0; index < opponent.army.size(); ++index) {
                    double new_strength = opponent.army[index]->get_strength() * (1-reduce_by);
                    opponent.army[index]->change_strength(new_strength);
                }
                for (size_t index = 0; index < army.size(); ++index) {
                    army[index]->change_strength(0);
                }
                dead = true;
                cout << opponent.name << " defeats " << name << endl;
            }
            else if (my_strength == opp_strength) {
                for (size_t index = 0; index < army.size(); ++index) {
                    army[index]->change_strength(0);
                }
                for (size_t index = 0; index < opponent.army.size(); ++index) {
                    opponent.army[index]->change_strength(0);
                }
                dead = true;
                opponent.dead = true;
                cout << "Mutual Annihilation: ";
                cout << name << " and " << opponent.name << " die at each other's hands\n";
            }
        }

    // output operator
    friend ostream& operator<<(ostream& os, const Noble& noble);
};


void open_stream(ifstream& nobleWarriors_file);

void interpret_file_info(ifstream& nobleWarriors_file, vector<Noble*>& nobles, vector<Warrior*>& warriors);

size_t get_noble(const vector<Noble*>& nobles, const string& noble_name);

size_t get_warrior(const vector<Warrior*>& warriors, const string& warrior_name);

void status(const vector<Noble*>& nobles, const vector<Warrior*>& warriors);

void clear(vector<Noble*>& nobles, vector<Warrior*>& warriors);


// creates vectors, calls to open and interpret file, closes file
int main() {
    cout << endl;
    vector<Warrior*> warriors;
    vector<Noble*> nobles;
    ifstream nobleWarriors_file;
    open_stream(nobleWarriors_file);
    interpret_file_info(nobleWarriors_file, nobles, warriors);
    nobleWarriors_file.close();
}


//opens file
void open_stream(ifstream& nobleWarriors_file) {
    nobleWarriors_file.open("nobleWarriors.txt");
    if(!nobleWarriors_file) {
        cerr << "failed to open nobleWarriors.txt\n";
        exit(1);
    }
}

//reads file and calls functions/methods based on the command
void interpret_file_info(ifstream& nobleWarriors_file, vector<Noble*>& nobles, vector<Warrior*>& warriors) {
    string command;
    string new_noble_name;
    string new_warrior_name;
    double new_strength;
    string noble_employer;
    string warrior_employee;
    string opponent1_name;
    string opponent2_name;

    while(nobleWarriors_file >> command) {
        if (command == "Noble") {
            (nobleWarriors_file >> new_noble_name);
            nobles.emplace_back(new Noble(new_noble_name));
        }

        else if (command == "Warrior") {
            (nobleWarriors_file >> new_warrior_name >> new_strength);
            warriors.emplace_back(new Warrior(new_warrior_name, new_strength));
        }

        else if (command == "Hire") {
            (nobleWarriors_file >> noble_employer >> warrior_employee);
            size_t index_of_noble = get_noble(nobles, noble_employer);
            size_t index_of_warrior = get_warrior(warriors, warrior_employee);
            // if both the noble and the warrior exist
            if (index_of_noble != nobles.size() && index_of_warrior != warriors.size()) {
                nobles[index_of_noble]->hire(*warriors[index_of_warrior]);
            }
            else if (index_of_warrior == warriors.size()) {
                cout << "Attempting to hire using unknown warrior: " << warrior_employee << endl;
            }
        }

        else if (command == "Fire") {
            (nobleWarriors_file >> noble_employer >> warrior_employee);
            size_t index_of_noble = get_noble(nobles, noble_employer);
            size_t index_of_warrior = get_warrior(warriors, warrior_employee);
            // if both the noble and the warrior exist
            if (index_of_noble != nobles.size() && index_of_warrior != warriors.size()) {
                nobles[index_of_noble]->fire(*warriors[index_of_warrior]);
            }
        }

        else if (command == "Status") {
            status(nobles, warriors);
        }

        else if (command == "Battle") {
            (nobleWarriors_file >> opponent1_name >> opponent2_name);
            size_t opp1_index = get_noble(nobles, opponent1_name);
            size_t opp2_index = get_noble(nobles, opponent2_name);
            nobles[opp1_index]->battle(*nobles[opp2_index]);
        }
        else if (command == "Clear") {
            clear(nobles, warriors);
        }
    }
}

// returns the index of the Noble given a name
size_t get_noble(const vector<Noble*>& nobles, const string& noble_name) {
    for (size_t index = 0; index < nobles.size(); ++index) {
        if (nobles[index]->get_name() == noble_name) {
            return index;
        }
    }
    return nobles.size();
}

// returns the index of the Warrior given a name
size_t get_warrior(const vector<Warrior*>& warriors, const string& warrior_name) {
    for (size_t index = 0; index < warriors.size(); ++index) {
        if (warriors[index]->get_name() == warrior_name) {
            return index;
        }
    }
    return warriors.size();
}

// shows Nobles, their armies, and unemployed Warriors
void status(const vector<Noble*>& nobles, const vector<Warrior*>& warriors) {
    cout << "Status\n======\nNobles:\n";
    if (nobles.size() == 0) {
        cout << "NONE\n";
    }
    else {
        for (size_t index = 0; index < nobles.size(); ++index) {
            cout << *nobles[index];
        }
        cout << "\nUnemployed Warriors:\n";
        bool all_employed = true;
        for (size_t index = 0; index < warriors.size(); ++index) {
            // if they're unemployed
            if (!warriors[index]->get_employment_status()) {
                all_employed = false;
                cout << "    " << *warriors[index];
            }
        }
        if (all_employed) {
            cout << "NONE\n";
        }
    }
    if (warriors.size() == 0) {
        cout << "\nUnemployed Warriors:\nNONE\n";
    }
}

// clears all Nobles and Warriors that were created
void clear(vector<Noble*>& nobles, vector<Warrior*>& warriors) {
    for (size_t index = 0; index < nobles.size(); ++index) {
        delete nobles[index];
    }
    nobles.clear();
    for (size_t index = 0; index < warriors.size(); ++index) {
        delete warriors[index];
    }
    warriors.clear();
}

// output operator for Noble
ostream& operator<<(ostream& os, const Noble& noble) {
    os << noble.get_name() << " has an army of " << noble.army.size() << endl;
    for (size_t index = 0; index < noble.army.size(); ++index) {
        os << "    " << *noble.army[index];
    }
    return os;
}

// output operator for Warrior
ostream& operator<<(ostream& os, const Warrior& warrior) {
    os << warrior.get_name() << ": " << warrior.get_strength() << endl;
    return os;
}
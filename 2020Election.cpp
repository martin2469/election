#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <getopt.h>
#include <iomanip>

using namespace std;

class election {
    struct state {
        double biden_votes = 0;
        double trump_votes = 0;
        double jorg_votes = 0;
        double hawkins_votes = 0;
        int electoral_votes = 0;
        string name = "";
    };
    map<string, state> states;
    double jorgToTrump;
    double hawkToTrump;
    double jorgToBiden;
    double hawkToBiden;

public:
    bool defaultSet;
    bool customSet;
    bool verboseSet;

    election() :
        jorgToTrump(15), hawkToTrump(5), jorgToBiden(30), hawkToBiden(30),
        defaultSet(true), customSet(false), verboseSet(false)
    {}
    void getOptions(int argc, char** argv);
    void custom();
    void verbose();
    void file_in();
    void shift();
    void flip(int original_diff, pair<string const, election::state> &state_in);
    void tally();
};

void election::getOptions(int argc, char** argv) {
    int option_index = 0, option = 0;
    opterr = false;

    struct option longOpts[] = { {"default",      no_argument,      nullptr, 'd' },
                                 {"custom",       no_argument,      nullptr, 'c' },
                                 {"verbose",      no_argument,      nullptr, 'v' },
                                 {"help",         no_argument,      nullptr, 'h' },
                                 {nullptr,        0,                nullptr, '\0'} };

    while ((option = getopt_long(argc, argv, "dcvh", longOpts, &option_index)) != -1) {
        switch (option) {
        case 'd':
            defaultSet = true;
            break;
        case 'c':
            customSet = true;
            defaultSet = false;
            break;
        case 'v':
            verboseSet = true;
            break;

        case 'h':
            cout << "This program simulates the 2020 U.S. Presidential Election if Trump and Biden had been the only candidates.\n";
            cout << "This simulation can be run with either a custom percentage of third-party turnover to either candidate\n(i.e. 45% of Jorgensen voters went for Trump) ";
            cout << "or with our default turnover.\n\n";
            cout << "Flag one of --custom (-c) or --default (-d) to activate a respective mode.\n";
            cout << "Flag --verbose (-v) to see more detailed election results.\n";
            cout << "Flag --help (-h) to repeat this message.\n";
            exit(0);
        }
    }
    if (customSet && defaultSet) {
        cerr << "Inavlid flags. At most, select one of --custom or --default.\n";
        exit(1);
    }
}

void election::custom() {
    cout << "Enter the percentage of Jo Jorgensen voters voting for Donald Trump: ";
    cin >> jorgToTrump;
    cout << "Enter the percentage of Jo Jorgensen voters voting for Joseph Biden: ";
    cin >> jorgToBiden;
    if (jorgToTrump + jorgToBiden > 100) {
        throw invalid_argument("Invalid input. Percentages add up to more than 100.\n");
    }
    cout << "Enter the percentage of Howie Hawkins voters voting for Donald Trump: ";
    cin >> hawkToTrump;
    cout << "Enter the percentage of Howie Hawkins voters voting for Joseph Biden: ";
    cin >> hawkToBiden;
    if (hawkToTrump + hawkToBiden > 100) {
        throw invalid_argument("Invalid input. Percentages add up to more than 100.\n");
    }
    if (jorgToTrump < 0 || jorgToBiden < 0 || hawkToBiden < 0 || hawkToTrump < 0) {
        throw invalid_argument("Invalid input. Percentages must be greater than 0.\n");
    }
    cout << '\n';
}

void election::verbose() {

    for (auto& it : states) {
        cout << it.second.name << " votes:\n";
        cout << "   Biden: " << it.second.biden_votes << "\n   Trump: " << it.second.trump_votes << '\n';

    }
}

void election::file_in() {

    string abb, full_name;
    char voted;
    int b = 0, t = 0, jj = 0, h = 0, ev = 0;
    string sjunk;
    char cjunk;
    ifstream fin("1976-2020-president.csv");
    getline(fin, sjunk);
    while (fin) {
        getline(fin, full_name, ',');
        getline(fin, abb, ',');
        states[abb].name = full_name;
        fin >> ev >> cjunk >>cjunk >> voted;
        states[abb].electoral_votes = ev;
        getline(fin, sjunk, '"');
        getline(fin, sjunk, ',');
        if (voted == 'B') {
            fin >> b;
            states[abb].biden_votes = b;

        }
        else if (voted == 'T') {
            fin >> t;
            states[abb].trump_votes = t;
        }
        else if (voted == 'J')   {
            fin >> jj;
            states[abb].jorg_votes = jj;
        }
        else {
            fin >> h;
            states[abb].hawkins_votes = h;
        }
        getline(fin, sjunk);
    }
    cout << "ORIGINAL RESULTS \n";
    tally();


    return;

}

void election::shift() {
    for (auto& it : states) {
        int diff = static_cast<int>(it.second.biden_votes - it.second.trump_votes);
        if (it.second.jorg_votes > 0) {
            it.second.biden_votes += it.second.jorg_votes * (jorgToBiden / 100.0);
            it.second.trump_votes += it.second.jorg_votes * (jorgToTrump / 100.0);
            it.second.jorg_votes = 0;
        }
        if (it.second.hawkins_votes > 0) {
            it.second.biden_votes += it.second.hawkins_votes * (hawkToBiden / 100.0);
            it.second.trump_votes += it.second.hawkins_votes * (hawkToTrump / 100.0);
            it.second.hawkins_votes = 0;
        }
        if (verboseSet) {
            flip(diff, it);
        }
    }
    return;
}

void election::flip(int original_diff, pair<string const, election::state>&state_in) {
    if (original_diff > 0) {
        if (state_in.second.biden_votes - state_in.second.trump_votes < 0) {
            cout << "FLIP: " << state_in.second.name << "'s " << state_in.second.electoral_votes << " electoral votes flip to Trump.\n";
        }
        return;
        
    }
    else {
        if (state_in.second.biden_votes - state_in.second.trump_votes > 0) {
            cout << "FLIP: " << state_in.second.name << "'s " << state_in.second.electoral_votes << " electoral votes flip to Biden.\n";
        }
        return;
    }
}

void election::tally() {
    pair<int, int> tallies(0, 0);
    for (auto& it : states) {
        if (it.second.biden_votes > it.second.trump_votes) {
            tallies.first += it.second.electoral_votes;
        }
        else {
            tallies.second += it.second.electoral_votes;
        }
    }
    cout << "\nElectoral Votes: \n   Biden: " << tallies.first << "    Trump: " << tallies.second << "\n\n";
}

int main(int argc, char** argv) {
    cout << fixed << setprecision(0);
    election e;
    e.getOptions(argc, argv);
    if (e.customSet) {
        try {
            e.custom();
        }
        catch (invalid_argument& error) {
            cerr << error.what() << '\n';
            exit(1);
        }
    }
    e.file_in();
    e.shift();
    cout << "\nNEW RESULTS \n\n";
    if (e.verboseSet) {
        e.verbose();
    }
    e.tally();
}
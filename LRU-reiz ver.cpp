#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
using namespace std;

class LRUCache {
    int capacity;
    list<int> order;                                  // front = most recent, back = least recent
    unordered_map<int, list<int>::iterator> pos;

public:
    LRUCache(int cap) : capacity(cap) {}

    // Returns true if page fault, false if hit
    bool access(int value, int &replaced) {
        replaced = -1;
        auto it = pos.find(value);
        if (it != pos.end()) {                        // HIT: move to front
            order.erase(it->second);
            order.push_front(value);
            pos[value] = order.begin();
            return false;
        }
        if ((int)order.size() == capacity) {          // PAGE FAULT and full: replace LRU
            replaced = order.back();
            order.pop_back();
            pos.erase(replaced);
        }
        order.push_front(value);
        pos[value] = order.begin();
        return true;
    }

    void display() const {
        cout << "  Frames (most recent -> least recent): ";
        for (int v : order) cout << v << " ";
        cout << "\n";
    }
};

int readPositive(const string& prompt) {
    int x;
    cout << prompt;
    while (!(cin >> x) || x <= 0) {
        cout << "Please enter a positive number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return x;
}

// Parses "1, 33, 2, 3" (commas and/or spaces). Returns false if invalid.
bool parseNumbers(const string& line, vector<int>& out) {
    string s = line;
    for (char &c : s) if (c == ',') c = ' ';
    stringstream ss(s);
    string tok;
    out.clear();
    while (ss >> tok) {
        if (tok.size() > 2) return false;             // only single or double digit
        for (char c : tok) if (c < '0' || c > '9') return false;
        out.push_back(stoi(tok));
    }
    return true;
}

int main() {
    int frames = readPositive("page fault: ");
    int total  = readPositive("how many number you will add? : ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<int> nums;
    while (true) {
        cout << "add number (eg., 1, 33, 2, 3, 3, 2): ";
        string line;
        getline(cin, line);
        if (!parseNumbers(line, nums)) {
            cout << "  Only single or double digit numbers (0-99) allowed. Try again.\n";
        } else if ((int)nums.size() != total) {
            cout << "  You must enter exactly " << total << " numbers (you entered "
                 << nums.size() << "). Try again.\n";
        } else break;
    }

    LRUCache cache(frames);
    int faults = 0, hits = 0;
    cout << "\n";
    for (int n : nums) {
        int replaced;
        bool fault = cache.access(n, replaced);
        cout << "Number " << n << ": " << (fault ? "PAGE FAULT" : "HIT") << "\n";
        if (replaced != -1) cout << "  Replaced: " << replaced << "\n";
        cache.display();
        fault ? faults++ : hits++;
    }

    cout << "\n--- Summary ---\n";
    cout << "Total page faults: " << faults << "\n";
    cout << "Total hits:        " << hits << "\n";
    cout << "Hit rate:          " << (100.0 * hits / total) << "%\n";
    return 0;
}

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <limits>
#include <numeric>
#include <climits>
#include <cctype>
using namespace std;

// Node for the doubly linked list
struct Node {
    int value;
    int frameNumber;
    Node* prev;
    Node* next;

    Node(int v, int f) {
        value = v;
        frameNumber = f;
        prev = nullptr;
        next = nullptr;
    }
};

class LRUCache {
private:
    map<int, Node*> cache;
    Node* head;
    Node* tail;
    int capacity;
    vector<int> frames;

public:
    int hits = 0;
    int pageFaults = 0;

    LRUCache(int size) {
        capacity = size;
        head = nullptr;
        tail = nullptr;
        frames.resize(capacity, -1);
    }

    ~LRUCache() {
        Node* current = head;

        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    // Move an existing node to the head
    void moveToHead(Node* node) {
        if (node == head) {
            return;
        }

        if (node->prev != nullptr) {
            node->prev->next = node->next;
        }

        if (node->next != nullptr) {
            node->next->prev = node->prev;
        }

        if (node == tail) {
            tail = node->prev;
        }

        node->prev = nullptr;
        node->next = head;

        if (head != nullptr) {
            head->prev = node;
        }

        head = node;

        if (tail == nullptr) {
            tail = head;
        }
    }

    // Insert a new node at the head
    void insertAtHead(Node* node) {
        node->prev = nullptr;
        node->next = head;

        if (head != nullptr) {
            head->prev = node;
        }

        head = node;

        if (tail == nullptr) {
            tail = node;
        }
    }

    // Remove the least recently used node
    // Return its frame number
    int removeTail(int& removedPage) {
        Node* oldTail = tail;

        removedPage = oldTail->value;
        int frameNumber = oldTail->frameNumber;

        if (tail->prev != nullptr) {
            tail = tail->prev;
            tail->next = nullptr;
        }
        else {
            head = nullptr;
            tail = nullptr;
        }

        cache.erase(removedPage);
        delete oldTail;

        return frameNumber;
    }

    // Process one page reference
    void accessPage(int page) {
        int removedPage = -1;
        int frameNumber = -1;
        bool hit = false;

        // Is the page already in the map?
        if (cache.find(page) != cache.end()) {
            Node* node = cache[page];

            moveToHead(node);

            hits++;
            hit = true;
        }
        else {
            pageFaults++;

            // Is the cache full?
            if (cache.size() >= static_cast<size_t>(capacity)) {
                // Remove the least recently used page
                frameNumber = removeTail(removedPage);
            }
            else {
                // Find the first empty frame
                for (int i = 0; i < capacity; i++) {
                    if (frames[i] == -1) {
                        frameNumber = i;
                        break;
                    }
                }
            }

            // Create a new node
            Node* newNode = new Node(page, frameNumber);

            // Insert the node at the head
            insertAtHead(newNode);

            // Add the page to the map and its frame
            cache[page] = newNode;
            frames[frameNumber] = page;
        }

        // Display the result for this reference
        cout << setw(8) << page;

        for (int i = 0; i < capacity; i++) {
            if (frames[i] == -1) {
                cout << setw(12) << "-";
            }
            else {
                cout << setw(12) << frames[i];
            }
        }

        if (hit) {
            cout << setw(12) << "H";
            cout << setw(14) << "-";
        }
        else {
            cout << setw(12) << "F";

            if (removedPage == -1) {
                cout << setw(14) << "-";
            }
            else {
                cout << setw(14) << removedPage;
            }
        }

        cout << endl;
    }
};

// Remove spaces at the beginning and end
string trim(string text) {
    size_t first = text.find_first_not_of(" \t\r\n");

    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");

    return text.substr(first, last - first + 1);
}

// Check whether a string contains only digits
bool isDigits(string text) {
    if (text.empty()) {
        return false;
    }

    for (char c : text) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    return true;
}

// Validate the reference string
bool validateReferenceString(string input,
                             vector<int>& references) {
    references.clear();

    if (input.empty()) {
        cout << "Please input real numbers.\n";
        return false;
    }

    stringstream ss(input);
    string value;

    while (getline(ss, value, ',')) {
        value = trim(value);

        // Check if the value is a number
        if (!isDigits(value)) {
            cout << "Invalid input. Please input real numbers "
                 << "separated by commas.\n";
            return false;
        }

        // Only allow 1 to 3 digits per page number
        if (value.length() < 1 || value.length() > 3) {
            cout << "Each page number must contain "
                 << "1 to 3 digits.\n";
            return false;
        }

        references.push_back(stoi(value));
    }

    // Reject a trailing comma
    if (!input.empty() && input.back() == ',') {
        cout << "Invalid input. Do not leave an empty entry "
             << "after a comma.\n";
        return false;
    }

    // Reject consecutive commas
    if (input.find(",,") != string::npos) {
        cout << "Invalid input. Do not leave empty entries "
             << "between commas.\n";
        return false;
    }

    return !references.empty();
}

int main() {
    string input;
    int capacity;

    cout << "====================================\n";
    cout << "       LRU PAGE REPLACEMENT\n";
    cout << "====================================\n";

    // Input and validate the number of page frames
    while (true) {
        cout << "\nEnter the number of page frames: ";
        getline(cin, input);

        input = trim(input);

        if (!isDigits(input)) {
            cout << "Please enter a positive whole number.\n";
            continue;
        }

        try {
            unsigned long long number = stoull(input);

            if (number == 0 || number > INT_MAX) {
                cout << "Please enter a positive frame count "
                     << "within the supported integer range.\n";
                continue;
            }

            capacity = static_cast<int>(number);
            break;
        }
        catch (...) {
            cout << "Invalid frame count. Please try again.\n";
        }
    }

    vector<int> references;

    // Input and validate the reference string
    while (true) {
        cout << "\nEnter the reference string "
             << "(example: 1,2,3,1,4,5): ";

        getline(cin, input);

        if (validateReferenceString(input, references)) {
            break;
        }

        cout << "Please enter the reference string again.\n";
    }

    // Initialize the LRU cache
    LRUCache lru(capacity);

    cout << "\n====================================\n";
    cout << "          LRU SIMULATION\n";
    cout << "====================================\n";

    cout << "\nPress Enter after each step to continue.\n";
    cout << "H = Hit (page already exists)\n";
    cout << "F = Page Fault (page is not in cache)\n";
    cout << "Removed = page removed during replacement\n";
    cout << "------------------------------------\n";

    // Display table headings
    cout << setw(8) << "Page";

    for (int i = 0; i < capacity; i++) {
        cout << setw(12) << ("Frame " + to_string(i + 1));
    }

    cout << setw(12) << "Result";
    cout << setw(14) << "Removed" << endl;

    cout << string(8 + capacity * 12 + 26, '-') << endl;

    // Process references one at a time
    for (size_t i = 0; i < references.size(); i++) {
        lru.accessPage(references[i]);

        cout << "\nStep " << i + 1 << " of "
             << references.size() << endl;

        if (i + 1 < references.size()) {
            cout << "Press Enter for the next reference...";
            getline(cin, input);
        }
    }

    // Calculate and display the hit ratio
    int totalReferences = static_cast<int>(references.size());
    int totalHits = lru.hits;

    int divisor = gcd(totalHits, totalReferences);

    int simplifiedHits = totalHits / divisor;
    int simplifiedTotal = totalReferences / divisor;

    cout << "\n====================================\n";
    cout << "           FINAL RESULTS\n";
    cout << "====================================\n";

    cout << "Total Page References: " << totalReferences << endl;
    cout << "Total Hits: " << totalHits << endl;
    cout << "Total Page Faults: " << lru.pageFaults << endl;

    cout << fixed << setprecision(2);

    cout << "Hit Ratio (Original): "
         << totalHits << "/" << totalReferences << endl;

    cout << "Hit Ratio (Simplified): "
         << simplifiedHits << "/" << simplifiedTotal << endl;

    cout << "Hit Percentage: "
         << (static_cast<double>(totalHits) / totalReferences)
            * 100.0
         << "%" << endl;

    cout << "\nProgram finished.\n";

    return 0;
}

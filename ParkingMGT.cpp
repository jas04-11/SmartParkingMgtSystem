#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <limits>

using namespace std;

struct User {
    string username;
    string password;
};

struct Vehicle {
    string number;
    string name;
    string type;
    string owner;
    int row, col;
    long long entryTime; 
};

class SmartParking {
private:
    static const int rows = 3, cols = 4;
    char parking[rows][cols];

    vector<Vehicle> vehicles;
    vector<User> users;

    string currentUser;
    int totalEarnings;

public:
    SmartParking() {
        currentUser = "";
        totalEarnings = 0;
        initParking();
        loadData();
    }

    void initParking() {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                parking[i][j] = 'E';
    }

    void saveData() {
        ofstream file("parking.txt");

        // Save Users
        file << users.size() << endl;
        for (int i = 0; i < users.size(); i++)
            file << users[i].username << " " << users[i].password << endl;

        // Save Vehicles
        file << vehicles.size() << endl;
        for (int i = 0; i < vehicles.size(); i++)
            file << vehicles[i].number << " " << vehicles[i].name << " " << vehicles[i].type
                 << " " << vehicles[i].owner << " " << vehicles[i].row << " " << vehicles[i].col
                 << " " << vehicles[i].entryTime << endl;

        // Save Earnings
        file << totalEarnings << endl;
        file.close();
    }

    void loadData() {
        ifstream file("parking.txt");
        if (!file) return;

        int n;

        // Load Users
        file >> n;
        for (int i = 0; i < n; i++) {
            User u;
            file >> u.username >> u.password;
            users.push_back(u);
        }

        // Load Vehicles
        file >> n;
        for (int i = 0; i < n; i++) {
            Vehicle v;
            file >> v.number >> v.name >> v.type >> v.owner >> v.row >> v.col >> v.entryTime;
            vehicles.push_back(v);
            parking[v.row][v.col] = 'O';
        }

        // Load Earnings
        file >> totalEarnings;

        file.close();
    }

    // ================= UI =================
    void pauseScreen() {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void printTop() {
        cout << "\n======= SMART PARKING =======\n";
    }
void printTop(string role) {
    cout << "\n======= SMART PARKING (" << role << ") =======\n";
}

    void showGrid() {
        cout << "\nParking Layout:\n";
        for (int i = 0; i < rows; i++) {
            cout << "   ";
            for (int j = 0; j < cols; j++) {
                cout << (parking[i][j] == 'O' ? "[X] " : "[ ] ");
            }
            cout << "\n";
        }
    }

    bool allocate(int &r, int &c) {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (parking[i][j] == 'E') {
                    parking[i][j] = 'O';
                    r = i; c = j;
                    return true;
                }
        return false;
    }

    // ================= AUTH =================
    void registerUser() {
        User u;
        cout << "\nEnter Username: ";
        cin >> u.username;
        cout << "Enter Password: ";
        cin >> u.password;

        users.push_back(u);
        saveData();
        cout << "[ REGISTERED SUCCESSFULLY ]\n";

        pauseScreen();
    }

    bool loginUser() {
        string uname, pass;
        cout << "\nEnter Username: ";
        cin >> uname;
        cout << "Enter Password: ";
        cin >> pass;

        for (int i = 0; i < users.size(); i++) {
            if (users[i].username == uname && users[i].password == pass) {
                currentUser = uname;
                cout << "[ LOGIN SUCCESS ]\n";
                pauseScreen();
                return true;
            }
        }

        cout << "[ FAILED ] Invalid Credentials\n";
        pauseScreen();
        return false;
    }

    bool adminLogin() {
        string user, pass;
        cout << "\nAdmin Username: "; cin >> user;
        cout << "Password: "; cin >> pass;

        if (user == "admin" && pass == "1234") {
            cout << "[ ADMIN LOGIN SUCCESS ]\n";
            pauseScreen();
            return true;
        }
        cout << "[ ADMIN LOGIN FAILED ]\n";
        pauseScreen();
        return false;
    }

    // ================= VEHICLES =================
    void parkVehicle() {
        Vehicle v;
        cout << "\nEnter Vehicle Number: "; cin >> v.number;
        cout << "Enter Vehicle Name: "; cin >> v.name;
        cout << "Enter Type (Car/Bike): "; cin >> v.type;

        if (!allocate(v.row, v.col)) {
            cout << "[ PARKING FULL ]\n";
            pauseScreen();
            return;
        }

        v.owner = currentUser;
        v.entryTime = (long long)time(0);
        vehicles.push_back(v);
        saveData();

        cout << "[ PARKED at R" << v.row + 1 << "C" << v.col + 1 << " ]\n";
        pauseScreen();
    }

    void removeVehicle(bool isAdmin = false) {
        string num;
        cout << "\nEnter Vehicle Number: "; cin >> num;

        for (int i = 0; i < vehicles.size(); i++) {
            if (vehicles[i].number == num &&
                (isAdmin || vehicles[i].owner == currentUser)) {

                parking[vehicles[i].row][vehicles[i].col] = 'E';

                double hrs = (time(0) - vehicles[i].entryTime) / 3600.0;
                int rate = (vehicles[i].type == "Bike") ? 10 : 20;
                int fee = max(20, (int)(hrs * rate));
                totalEarnings += fee;

                cout << "\nFee: Rs " << fee << endl;

                vehicles.erase(vehicles.begin() + i);
                saveData();
                cout << "[ REMOVED SUCCESSFULLY ]\n";
                pauseScreen();
                return;
            }
        }
        cout << "[ NOT FOUND / ACCESS DENIED ]\n";
        pauseScreen();
    }

    void searchVehicle() {
        string num;
        cout << "\nEnter Vehicle Number: "; cin >> num;

        for (int i = 0; i < vehicles.size(); i++) {
            if (vehicles[i].number == num) {
                cout << "Found at R" << vehicles[i].row + 1 << "C" << vehicles[i].col + 1 << endl;
                pauseScreen();
                return;
            }
        }
        cout << "[ NOT FOUND ]\n";
        pauseScreen();
    }

    void showAllVehicles() {
        if (vehicles.empty()) {
            cout << "\nNo vehicles parked\n";
            pauseScreen();
            return;
        }

        for (int i = 0; i < vehicles.size(); i++) {
            cout << vehicles[i].number << " | " << vehicles[i].name << " | " << vehicles[i].type
                 << " | Owner: " << vehicles[i].owner << " | R" << vehicles[i].row + 1 << "C" << vehicles[i].col + 1 << endl;
        }
        pauseScreen();
    }

    void showEarnings() {
        cout << "\nTotal Earnings: Rs " << totalEarnings << endl;
        pauseScreen();
    }

    // ================= PANELS =================
    void adminPanel() {
        int ch;
        while (true) {
            system("cls"); // clear screen for Dev C++
            printTop();
            showGrid();
            cout << "\n1.View Vehicles  2.Remove Vehicle  3.Earnings  4.Back\n"; 
            cin >> ch;

            if (ch == 1) showAllVehicles();
            else if (ch == 2) removeVehicle(true);
            else if (ch == 3) showEarnings();
            else return;
        }
    }

    void userDashboard() {
        int ch;
        while (true) {
            system("cls");
            printTop();
            showGrid();
            cout << "\n1.Park  2.Search  3.Remove  4.Logout\n"; 
            cin >> ch;

            if (ch == 1) parkVehicle();
            else if (ch == 2) searchVehicle();
            else if (ch == 3) removeVehicle(false);
            else { currentUser = ""; return; }
        }
    }

    void userPanel() {
        int ch;
        while (true) {
            cout << "\n1.Register  2.Login  3.Back\n"; 
            cin >> ch;

            if (ch == 1) registerUser();
            else if (ch == 2 && loginUser()) userDashboard();
            else if (ch == 3) return;
        }
    }

    void run() {
        int role;
        while (true) {
        	system("cls");          // Clear screen
        printTop(); 
            cout << "\n1.Admin  2.User  3.Exit\n"; 
            cin >> role;

            if (role == 1) {
                if (adminLogin()) adminPanel();
            }
            else if (role == 2) userPanel();
            else return;
        }
    }
};

int main() {
    SmartParking sp;

    sp.run();
    return 0;
}

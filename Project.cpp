#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

using namespace std;

// Function to clear the input buffer (without limits library)
void clearInput()
{
    cin.ignore(10000, '\n'); // Clears input buffer
}

// Check if a file is empty
bool isFileEmpty(const string &filename)
{
    ifstream file(filename);
    return file.peek() == ifstream::traits_type::eof();
}

// --- Blood Group Data Structure and Class Implementation ---
struct BloodGroupNode
{
    string bloodGroup;
    int units;
    BloodGroupNode *left;
    BloodGroupNode *right;

    BloodGroupNode(string group, int units) 
    : bloodGroup(group), units(units), left(nullptr), right(nullptr) {}
};

class BloodBankBST
{
    BloodGroupNode *root;

    BloodGroupNode *insert(BloodGroupNode *node, string bloodGroup, int units)
    {
        if (!node)
            return new BloodGroupNode(bloodGroup, units);
        if (bloodGroup < node->bloodGroup)
            node->left = insert(node->left, bloodGroup, units);
        else if (bloodGroup > node->bloodGroup)
            node->right = insert(node->right, bloodGroup, units);
        return node;
    }

    BloodGroupNode *search(BloodGroupNode *node, string bloodGroup)
    {
        if (!node || node->bloodGroup == bloodGroup)
            return node;
        if (bloodGroup < node->bloodGroup)
            return search(node->left, bloodGroup);
        return search(node->right, bloodGroup);
    }

    void inorder(BloodGroupNode *node, ofstream *file = nullptr)
    {
        if (node)
        {
            inorder(node->left, file);
            if (file)
                *file << node->bloodGroup << " " << node->units << "\n";
            else
                cout << node->bloodGroup << " | Units: " << node->units << endl;
            inorder(node->right, file);
        }
    }

public:
    BloodBankBST() : root(nullptr) {}

    void addBloodGroup(string bloodGroup, int units)
    {
        BloodGroupNode *found = search(root, bloodGroup);
        if (found)
            found->units += units;
        else
            root = insert(root, bloodGroup, units);
    }

    bool isAvailable(string bloodGroup, int requiredUnits)
    {
        BloodGroupNode *found = search(root, bloodGroup);
        return found && found->units >= requiredUnits;
    }

    void updateUnits(string bloodGroup, int units)
    {
        BloodGroupNode *found = search(root, bloodGroup);
        if (found)
            found->units += units;
    }

    void displayBloodGroups()
    {
        if (!root)
        {
            cout << "No blood groups available.\n";
            return;
        }
        cout << "\nBlood Group Inventory:\n";
        inorder(root);
    }

    void saveToFile(const string &filename)
    {
        ofstream file(filename);
        if (file)
        {
            inorder(root, &file);
            file.close();
        }
    }

    void loadFromFile(const string &filename)
    {
        ifstream file(filename);
        if (file)
        {
            string bloodGroup;
            int units;
            while (file >> bloodGroup >> units)
            {
                addBloodGroup(bloodGroup, units);
            }
            file.close();
        }
    }
};


BloodBankBST bloodBank;


// Function to get current date and time
string getCurrentDateTime()
{
    time_t now = time(0);      // Current time in seconds since epoch
    tm *ltm = localtime(&now); // Convert to local time

    // Format: YYYY-MM-DD HH:MM:SS
    string dateTime = to_string(1900 + ltm->tm_year) + "-" +
                      (ltm->tm_mon + 1 < 10 ? "0" : "") + to_string(1 + ltm->tm_mon) + "-" +
                      (ltm->tm_mday < 10 ? "0" : "") + to_string(ltm->tm_mday) + " " +
                      (ltm->tm_hour < 10 ? "0" : "") + to_string(ltm->tm_hour) + ":" +
                      (ltm->tm_min < 10 ? "0" : "") + to_string(ltm->tm_min) + ":" +
                      (ltm->tm_sec < 10 ? "0" : "") + to_string(ltm->tm_sec);

    return dateTime;
}


// --- Blood Request Handling ---
void handleBloodRequest()
{
    string name, bloodGroup, gender;
    int age, units;

    clearInput();
    cout << "Enter Patient Name: ";
    getline(cin, name);
    cout << "Enter Patient Age: ";
    cin >> age;
    clearInput();
    cout << "Enter Patient Gender: ";
    getline(cin, gender);
    cout << "Enter Blood Group Required: ";
    cin >> bloodGroup;
    cout << "Enter Units Required: ";
    cin >> units;

    if (bloodBank.isAvailable(bloodGroup, units))
    {
        bloodBank.updateUnits(bloodGroup, -units); // Deduct units from available stock
        bloodBank.saveToFile("bloodInfo.txt");     // Save updated blood info to file

        ofstream requestFile("requests.txt", ios::app);
        if (requestFile)
        {
            string dateTime = getCurrentDateTime(); // Get current date and time
            requestFile << "Request: " << name << ", Age: " << age
                        << ", Gender: " << gender << ", Blood Group: " << bloodGroup
                        << ", Units: " << units << ", Date & Time: " << dateTime << "\n";
            requestFile.close();
            cout << "Request processed successfully.\n";
        }
        else
        {
            cout << "Error saving request to file.\n";
        }
    }
    else
    {
        cout << "Blood group " << bloodGroup << " is not available.\n";
    }
}

// --- Blood Donation Handling ---
void handleBloodDonation()
{
    string name, bloodGroup, phone;
    int age, units;

    clearInput();
    cout << "Enter Donor Name: ";
    getline(cin, name);
    cout << "Enter Donor Age: ";
    cin >> age;
    clearInput();
    cout << "Enter Donor Phone: ";
    getline(cin, phone);
    cout << "Enter Blood Group: ";
    cin >> bloodGroup;
    cout << "Enter Units Donated: ";
    cin >> units;

    bloodBank.addBloodGroup(bloodGroup, units);
    bloodBank.saveToFile("bloodInfo.txt");

    ofstream donationFile("donations.txt", ios::app);
    if (donationFile)
    {
        string dateTime = getCurrentDateTime();
        donationFile << "Donation: " << name << ", Age: " << age
                     << ", Phone: " << phone << ", Blood Group: " << bloodGroup
                     << ", Units: " << units << ", Date & Time: " << dateTime << "\n";
        donationFile.close();
        cout << "Donation recorded successfully.\n";
    }
    else
    {
        cout << "Error saving donation to file.\n";
    }
}

// function to search and see specific blood groups
void searchBloodGroup()
{
    string bloodGroup;
    cout << "Enter the blood group to search: ";
    cin >> bloodGroup;

    ifstream file("bloodInfo.txt");
    if (!file)
    {
        cout << "Error opening bloodInfo.txt file.\n";
        return;
    }

    string group;
    int units;
    bool found = false;
    while (file >> group >> units)
    {
        if (group == bloodGroup)
        {
            cout << "Blood Group: " << group << " | Units Available: " << units << "\n";
            found = true;
            break;
        }
    }
    file.close();

    if (!found)
    {
        cout << "Blood group " << bloodGroup << " is not available.\n";
    }
}

// function to search and see specific blood donors
void searchDonorsByBloodGroup()
{
    string bloodGroup;
    cout << "Enter the blood group to search donors: ";
    cin >> bloodGroup;

    ifstream file("donations.txt");
    if (!file)
    {
        cout << "Error opening donations.txt file.\n";
        return;
    }

    string line, name, group, phone, temp;
    int age, units;
    bool found = false;

    cout << "\nDonors with blood group " << bloodGroup << ":\n";
    while (getline(file, line))
    {
        // Split the line into fields using labels
        size_t pos;

        // Extract name
        pos = line.find("Donation: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 10); // Skip "Donation: "
            pos = line.find(",");
            name = line.substr(0, pos);
            line = line.substr(pos + 1);
        }

        // Extract age
        pos = line.find("Age: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 5); // Skip "Age: "
            pos = line.find(",");
            age = stoi(line.substr(0, pos));
            line = line.substr(pos + 1);
        }

        // Extract phone
        pos = line.find("Phone: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 7); // Skip "Phone: "
            pos = line.find(",");
            phone = line.substr(0, pos);
            line = line.substr(pos + 1);
        }

        // Extract blood group
        pos = line.find("Blood Group: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 13); // Skip "Blood Group: "
            pos = line.find(",");
            group = line.substr(0, pos);
            line = line.substr(pos + 1);
        }

        // Extract units
        pos = line.find("Units: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 7); // Skip "Units: "
            units = stoi(line);
        }

        // Check if blood group matches
        if (group == bloodGroup)
        {
            cout << "Name: " << name << ", Age: " << age
                 << ", Phone: " << phone << ", Units Donated: " << units << "\n";
            found = true;
        }
    }
    file.close();

    if (!found)
    {
        cout << "No donors found with blood group " << bloodGroup << ".\n";
    }
}

// function to search and see specific blood request patients
void searchRequestedPatientsByBloodGroup()
{
    string bloodGroup;
    cout << "Enter the blood group to search patients: ";
    cin >> bloodGroup;

    ifstream file("requests.txt");
    if (!file)
    {
        cout << "Error opening requests.txt file.\n";
        return;
    }

    string line, name, group, gender, temp;
    int age, units;
    bool found = false;

    cout << "\nPatients requesting blood group " << bloodGroup << ":\n";

    while (getline(file, line))
    {
        // Split the line into fields using labels
        size_t pos;

        // Extract name
        pos = line.find("Request: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 9); // Skip "Request: "
            pos = line.find(",");
            name = line.substr(0, pos);
            line = line.substr(pos + 1);
        }

        // Extract age
        pos = line.find("Age: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 5); // Skip "Age: "
            pos = line.find(",");
            age = stoi(line.substr(0, pos));
            line = line.substr(pos + 1);
        }

        // Extract gender
        pos = line.find("Gender: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 8); // Skip "Gender: "
            pos = line.find(",");
            gender = line.substr(0, pos);
            line = line.substr(pos + 1);
        }

        // Extract blood group
        pos = line.find("Blood Group: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 13); // Skip "Blood Group: "
            pos = line.find(",");
            group = line.substr(0, pos);
            line = line.substr(pos + 1);
        }

        // Extract units
        pos = line.find("Units: ");
        if (pos != string::npos)
        {
            line = line.substr(pos + 7); // Skip "Units: "
            units = stoi(line);
        }

        // Check if blood group matches
        if (group == bloodGroup)
        {
            cout << "Name: " << name << ", Age: " << age
                 << ", Gender: " << gender << ", Units Requested: " << units << "\n";
            found = true;
        }
    }
    file.close();

    if (!found)
    {
        cout << "No patients found requesting blood group " << bloodGroup << ".\n";
    }
}


// --- Admin Menu ---
void adminMenu()
{
    int choice;
    do
    {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Blood Group\n";
        cout << "2. View Blood Groups\n";
        cout << "3. View Request History\n";
        cout << "4. View Donation History\n";
        cout << "5. Search Specific Blood Group\n";
        cout << "6. Search Specific Donors\n";
        cout << "7. Search Specific Patients\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string bloodGroup;
            int units;
            cout << "Enter Blood Group: ";
            cin >> bloodGroup;
            cout << "Enter Units: ";
            cin >> units;
            bloodBank.addBloodGroup(bloodGroup, units);
            bloodBank.saveToFile("bloodInfo.txt");
            break;
        }
        case 2:
            bloodBank.displayBloodGroups();
            break;
        case 3:
        {
            if (isFileEmpty("requests.txt"))
                cout << "No requests found.\n";
            else
            {
                ifstream file("requests.txt");
                cout << "Request History:\n";
                string line;
                while (getline(file, line))
                {
                    cout << line << endl;
                }
                file.close();
            }
            break;
        }
        case 4:
        {
            if (isFileEmpty("donations.txt"))
                cout << "No donations found.\n";
            else
            {
                ifstream file("donations.txt");
                cout << "Donation History:\n";
                string line;
                while (getline(file, line))
                {
                    cout << line << endl;
                }
                file.close();
            }
            break;
        }
        case 5:
            searchBloodGroup();
            break;
        case 6:
            searchDonorsByBloodGroup();
            break;
        case 7:
            searchRequestedPatientsByBloodGroup();
            break;
        case 8:
            cout << "Exiting Admin Menu...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 8);
}

// --- Main Function ---
int main()
{
    bloodBank.loadFromFile("bloodInfo.txt");

    int choice;
    do
    {
        cout << "\t\t\t\t\t===============================================\n";
        cout << "\t\t\t\t\t||                                           || \n";
        cout << "\t\t\t\t\t||   Welcome to the Blood Bank Management!   ||\n";
        cout << "\t\t\t\t\t||                                           ||\n";
        cout << "\t\t\t\t\t===============================================\n";

        cout << "\nBlood Bank Management System\n";
        cout << "1. Admin Login\n";
        cout << "2. Employee Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string username, password;
            cout << "Enter Admin Username: ";
            cin >> username;
            cout << "Enter Admin Password: ";
            cin >> password;
            if (username == "admin" && password == "password")
            {
                adminMenu();
            }
            else
            {
                cout << "Invalid credentials.\n";
            }
            break;
        }
        case 2:
        { // Employee Login
            string employeeUsername, employeePassword;
            cout << "Enter Employee Username: ";
            cin >> employeeUsername;
            cout << "Enter Employee Password: ";
            cin >> employeePassword;

            // Validate employee credentials (you can customize this as needed)
            if (employeeUsername == "employee" && employeePassword == "employee123")
            {
                int employeeChoice;
                do
                {
                    cout << "\nEmployee Menu:\n";
                    cout << "1. Request Blood\n";
                    cout << "2. Donate Blood\n";
                    cout << "3. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> employeeChoice;

                    switch (employeeChoice)
                    {
                    case 1:
                        handleBloodRequest();
                        break;
                    case 2:
                        handleBloodDonation();
                        break;
                    case 3:
                        cout << "Exiting Employee Menu...\n";
                        break;
                    default:
                        cout << "Invalid choice.\n";
                    }
                } while (employeeChoice != 3);
            }
            else
            {
                cout << "Invalid credentials.\n";
            }
            break;
        }
        case 3:
            cout << "Exiting Blood Bank Management System...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}

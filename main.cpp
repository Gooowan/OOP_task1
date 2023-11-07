#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

class Ticket {
private:
    static int id;
    string passengerName;
    string seatNumber;
    string flightNumber;
    string date;
    int price;
    bool bookingStatus;
    int bookingID;

public:
    Ticket(const string &passengerName, const string &seatNumber, const string &flightNumber, const string &date, int price)
            : passengerName(passengerName), seatNumber(seatNumber), flightNumber(flightNumber), date(date), price(price), bookingStatus(true), bookingID(++id) {}

    string getTicketInfo() const {
        if (bookingStatus) {
            return "Flight Number: " + flightNumber + "\n" +
                   "Date: " + date + "\n" +
                   "Seat: " + seatNumber + "\n" +
                   "Price: $" + to_string(price);
        } else {
            return "Ticket ID: " + to_string(bookingID) + " is cancelled.";
        }
    }

    void cancelBooking() {
        bookingStatus = false;
    }

    int getBookingID() const {
        return bookingID;
    }

    string getPassengerName() const {
        return passengerName;
    }

    string getSeatNumber() const {
        return seatNumber;
    }
};

int Ticket::id = 0;


class Airplane {
private:
    string flightNumber;
    string date;
    unordered_map<string, pair<int, bool>> seats;
    map<int, Ticket> bookedTickets;

public:
    Airplane(const string &flightInfo) {
        stringstream ss(flightInfo);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ' ')) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }

        if (tokens.size() < 4) {
            cerr << "Error: Not enough information in the configuration string." << endl;
            return;
        }

        date = tokens[0];
        flightNumber = tokens[1];

        int seatsPerRow = stoi(tokens[2]);

        for (size_t i = 3; i + 1 < tokens.size(); i += 2) {
            size_t dashPos = tokens[i].find('-');
            size_t dollarPos = tokens[i + 1].find('$');

            if (dashPos != string::npos && dollarPos != string::npos) {
                int startRow = stoi(tokens[i].substr(0, dashPos));
                int endRow = stoi(tokens[i].substr(dashPos + 1));
                int price = stoi(tokens[i + 1].substr(0, dollarPos));

                for (int row = startRow; row <= endRow; ++row) {
                    for (char seat = 'A'; seat < 'A' + seatsPerRow; ++seat) {
                        string seatID = to_string(row) + seat;
                        seats[seatID] = make_pair(price, true);
                    }
                }
            } else {
                cerr << "Error: Incorrect seat range or price format." << endl;
                return;
            }
        }
    }

    string viewTicketInfo(int ticketID) const {
        auto it = bookedTickets.find(ticketID);
        if (it != bookedTickets.end()) {
            return it->second.getTicketInfo();
        }
        return "Ticket not found.";
    }

    string viewTicketsInfoByUser(const string& username) const {
        stringstream ticketsInfo;
        int ticketsFound = 0;

        for (const auto& ticketPair : bookedTickets) {
            if (ticketPair.second.getPassengerName() == username) {
                ticketsInfo << ticketPair.second.getTicketInfo() << "\n\n";
                ++ticketsFound;
            }
        }

        if (ticketsFound == 0) {
            return "No tickets found for user: " + username;
        }

        return ticketsInfo.str();
    }


    int bookSeat(const string &seatNumber, const string &passengerName) {
        if (seats.find(seatNumber) != seats.end() && seats[seatNumber].second) {
            int price = seats[seatNumber].first;
            seats[seatNumber].second = false;
            Ticket ticket(passengerName, seatNumber, flightNumber, date, price);
            bookedTickets.insert(make_pair(ticket.getBookingID(), ticket));
            return ticket.getBookingID();
        }
        return -1;
    }

    bool returnSeat(const string &seatNumber) {
        auto it = seats.find(seatNumber);
        if (it != seats.end() && !it->second.second) {
            it->second.second = true;

            for (auto &ticket : bookedTickets) {
                if (ticket.second.getSeatNumber() == seatNumber) {
                    ticket.second.cancelBooking();
                    break;
                }
            }
            return true;
        }
        return false;
    }
    bool returnSeatByTicketID(int ticketID) {
        auto ticketIt = bookedTickets.find(ticketID);
        if (ticketIt != bookedTickets.end()) {
            string seatNumber = ticketIt->second.getSeatNumber();
            ticketIt->second.cancelBooking();
            return this->returnSeat(seatNumber);
        }
        return false;
    }

    void displayAvailableSeats() const {
        cout << "Available seats for flight " << flightNumber << " on " << date << ":" << endl;
        for (const auto& seat : seats) {
            if (seat.second.second) {
                cout << "Seat " << seat.first << " - Price: $" << seat.second.first << endl;
            }
        }
    }
    string getFlightNumber() const {
        return flightNumber;
    }
};

class Console {
private:
    vector<Airplane> airplanes;
    static void DisplayAirplaneInterface() {
        cout << "Type command:\n" << endl;
        cout << "1. Book a seat" << endl;
        cout << "2. Cancel a ticket" << endl;
        cout << "3. Display available seats" << endl;
        cout << "4. View booking info" << endl;
        cout << "5. View user info" << endl;
        cout << "6. Exit\n" << endl;
    }

    void DisplayFlightList() {
        cout << "Select a flight to manage:" << endl;
        for (size_t i = 0; i < airplanes.size(); ++i) {
            cout << i + 1 << ". Flight " << airplanes[i].getFlightNumber() << endl;
        }
    }

public:
    Console(const vector<string>& configurations) {
        for (const auto& config : configurations) {
            airplanes.emplace_back(config);
        }
    }

    void run() {
        bool running = true;
        while (running) {
            system("clear");
            DisplayFlightList();
            int choice;
            cout << "Enter the number of the flight to manage or 0 to exit: ";
            cin >> choice;

            if (cin.fail() || choice < 0 || choice > airplanes.size()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a valid number." << endl;
                continue;
            }

            if (choice == 0) {
                running = false;
                break;
            }

            Airplane& selectedAirplane = airplanes[choice - 1];
            bool managingFlight = true;
            while (managingFlight) {
                system("clear");
                DisplayAirplaneInterface();
                int action;
                cout << "Enter your choice: ";
                cin >> action;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number." << endl;
                    continue;
                }

                switch (action) {
                    case 1: {
                        string seatNumber;
                        string passengerName;
                        string waitinput;
                        cout << "Enter seat number: ";
                        cin >> seatNumber;
                        cout << "Enter Passenger Name: ";
                        cin >> passengerName;
                        int ticketID = selectedAirplane.bookSeat(seatNumber, passengerName);
                        if (ticketID != -1) {
                            cout << "Seat booked successfully. Ticket ID: " << ticketID << endl;
                        } else {
                            cout << "Failed to book the seat. It may be taken or does not exist." << endl;
                        }
                        cout << "\nWrite anything to continue..." << endl;
                        cin >> waitinput;
                        break;
                    }
                    case 2: {
                        int ticketID;
                        string waitinput;
                        cout << "Enter ticket ID: ";
                        cin >> ticketID;
                        if (selectedAirplane.returnSeatByTicketID(ticketID)) {
                            cout << "Ticket cancelled successfully." << endl;
                        } else {
                            cout << "Could not cancel ticket. Please check the ticket ID." << endl;
                        }
                        cout << "\nWrite anything to continue..." << endl;
                        cin >> waitinput;
                        break;
                    }
                    case 3: {
                        string waitinput;
                        selectedAirplane.displayAvailableSeats();

                        cout << "\nWrite anything to continue..." << endl;
                        cin >> waitinput;
                        break;
                    }
                    case 4: {
                        string waitinput;
                        int ticketID;
                        cout << "Enter ticket ID: ";
                        cin >> ticketID;
                        cout << selectedAirplane.viewTicketInfo(ticketID) << endl;

                        cout << "\nWrite anything to continue..." << endl;
                        cin >> waitinput;
                        break;
                    }
                    case 5: {
                        string waitinput;
                        string username;
                        cout << "Enter username: ";
                        cin >> username;
                        cout << selectedAirplane.viewTicketsInfoByUser(username) << endl;

                        cout << "\nWrite anything to continue..." << endl;
                        cin >> waitinput;
                        break;
                    }
                    case 6: {
                        managingFlight = false;
                        break;
                    }
                    default: {
                        cout << "Invalid option. Please try again." << endl;
                        break;
                    }
                }
            }
        }
    }

};

int main() {
    ifstream file("1.txt");
    vector<string> configs;
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (!line.empty()) {
                configs.push_back(line);
            }
        }
        file.close();
    } else {
        cerr << "Unable to open file";
        return 1;
    }

    Console console(configs);
    console.run();
    return 0;
}
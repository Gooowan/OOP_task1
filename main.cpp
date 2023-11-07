#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <sstream>
#include "iostream"

using namespace std;

class Ticket {
private:
    static int _idCounter;
    string _passengerName;
    string _seatNumber;
    string _flightNumber;
    string _date;
    int _price;
    bool _bookingStatus;
    int _bookingID;

public:
    Ticket(const string &passengerName, const string &seatNumber, const string &flightNumber, const string &date, int price)
            : _passengerName(passengerName), _seatNumber(seatNumber), _flightNumber(flightNumber), _date(date), _price(price), _bookingStatus(true), _bookingID(++_idCounter) {}

    string getTicketInfo() const {
        if (_bookingStatus) {
            return "Flight Number: " + _flightNumber + "\n" +
                   "Date: " + _date + "\n" +
                   "Seat: " + _seatNumber + "\n" +
                   "Price: $" + to_string(_price);
        } else {
            return "Ticket ID: " + to_string(_bookingID) + " is cancelled.";
        }
    }

    void cancelBooking() {
        _bookingStatus = false;
    }

    int getBookingID() const {
        return _bookingID;
    }

    string getSeatNumber() const {
        return _seatNumber;
    }
};

int Ticket::_idCounter = 0;


class Airplane {
private:
    string _flightNumber;
    string _date;
    unordered_map<string, pair<int, bool>> _seats;
    map<int, Ticket> _bookedTickets;

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

        _date = tokens[0];
        _flightNumber = tokens[1];

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
                        _seats[seatID] = make_pair(price, true);
                    }
                }
            } else {
                cerr << "Error: Incorrect seat range or price format." << endl;
                return;
            }
        }
    }

    string viewTicketInfo(int ticketID) const {
        auto it = _bookedTickets.find(ticketID);
        if (it != _bookedTickets.end()) {
            return it->second.getTicketInfo();
        }
        return "Ticket not found.";
    }

    int bookSeat(const string &seatNumber, const string &passengerName) {
        if (_seats.find(seatNumber) != _seats.end() && _seats[seatNumber].second) {
            int price = _seats[seatNumber].first;
            _seats[seatNumber].second = false;
            Ticket ticket(passengerName, seatNumber, _flightNumber, _date, price);
            _bookedTickets.insert(make_pair(ticket.getBookingID(), ticket));
            return ticket.getBookingID();
        }
        return -1;
    }

    bool returnSeat(const string &seatNumber) {
        auto it = _seats.find(seatNumber);
        if (it != _seats.end() && !it->second.second) {
            it->second.second = true;

            for (auto &ticket : _bookedTickets) {
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
        auto ticketIt = _bookedTickets.find(ticketID);
        if (ticketIt != _bookedTickets.end()) {
            string seatNumber = ticketIt->second.getSeatNumber();
            ticketIt->second.cancelBooking();
            return this->returnSeat(seatNumber);
        }
        return false;
    }

    void displayAvailableSeats() const {
        cout << "Available seats for flight " << _flightNumber << " on " << _date << ":" << endl;
        for (const auto& seat : _seats) {
            if (seat.second.second) {
                cout << "Seat " << seat.first << " - Price: $" << seat.second.first << endl;
            }
        }
    }
    string getFlightNumber() const {
        return _flightNumber;
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
                cin.clear(); // Clear error flags
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
                        break;
                    }
                    case 2: {
                        int ticketID;
                        cout << "Enter ticket ID: ";
                        cin >> ticketID;
                        if (selectedAirplane.returnSeatByTicketID(ticketID)) {
                            cout << "Ticket cancelled successfully." << endl;
                        } else {
                            cout << "Could not cancel ticket. Please check the ticket ID." << endl;
                        }
                        break;
                    }
                    case 3: {
                        selectedAirplane.displayAvailableSeats();
                        break;
                    }
                    case 4: {
                        int ticketID;
                        cout << "Enter ticket ID: ";
                        cin >> ticketID;
                        cout << selectedAirplane.viewTicketInfo(ticketID) << endl;
                        break;
                    }
                    case 5: {
                        // This part of the code needs to be implemented based on how you want to track user info
                        cout << "This feature is not implemented yet." << endl;
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
    vector<string> configs = {
            "11.12.2022 HJ114 2 1-10 10$ 11-20 20$",
            "12.12.2022 HJ115 3 1-5 30$ 6-10 40$"
    };
    Console console(configs);
    console.run();
    return 0;
}
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
    Ticket(string passengerName, string seatNumber, string flightNumber, string date, int price)
            : passengerName(std::move(passengerName)), seatNumber(std::move(seatNumber)),
              flightNumber(std::move(flightNumber)), date(std::move(date)), price(price),
              bookingStatus(true), bookingID(++id) {}

    Ticket(const Ticket& other)
            : passengerName(other.passengerName), seatNumber(other.seatNumber),
              flightNumber(other.flightNumber), date(other.date), price(other.price),
              bookingStatus(other.bookingStatus), bookingID(other.bookingID) {}

    Ticket(Ticket&& other)
            : passengerName(std::move(other.passengerName)), seatNumber(std::move(other.seatNumber)),
              flightNumber(std::move(other.flightNumber)), date(std::move(other.date)), price(other.price),
              bookingStatus(other.bookingStatus), bookingID(other.bookingID) {
        other.bookingStatus = false;
        other.bookingID = 0;
    }

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
    explicit Airplane(const string &flightInfo) {
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
        else{
            return "Ticket not found";
        }
    }

    string viewTicketsInfoByUser(const string& username) const {
        stringstream ticketsInfo;
        int ticketsFound = 0;

        for (const auto& ticketPair : bookedTickets) {
            if (ticketPair.second.getPassengerName() == username) {
                ticketsInfo << ticketPair.second.getTicketInfo() << "\n";
                ++ticketsFound;
            }
        }

        if (ticketsFound == 0) {
            return "No tickets found for user";
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

    static void DisplayMainMenu() {
        cout << "Available commands:\n" << endl;
        cout << "1. Book a seat" << endl;
        cout << "2. Return a ticket" << endl;
        cout << "3. View all available seats" << endl;
        cout << "4. View ticket by ID" << endl;
        cout << "5. View all tickets by username" << endl;
        cout << "6. Exit\n" << endl;
    }

    void DisplayFlightList() {
        cout << "Available flights:" << endl;
        for (size_t i = 0; i < airplanes.size(); ++i) {
            cout << i + 1 << ". Flight " << airplanes[i].getFlightNumber() << endl;
        }
        cout << endl;
    }

    int GetFlightIndexFromUser() {
        int flightIndex;
        cout << "Enter the flight number: ";
        cin >> flightIndex;
        flightIndex--;

        if (cin.fail() || flightIndex < 0 || flightIndex >= airplanes.size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid flight number. Please try again." << endl;
            return -1;
        }

        return flightIndex;
    }

public:
    Console(const vector<string>& configurations) {
        for (const auto& config : configurations) {
            airplanes.emplace_back(config);
        }
    }

    void run() {
        while (true) {
            system("clear");
            DisplayMainMenu();

            int command;
            cout << "Enter your command: ";
            cin >> command;

            if (cin.fail() || command < 1 || command > 6) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid command. Please enter a number between 1 and 6." << endl;
                continue;
            }

            if (command == 6) {
                break;
            }

            string seatNumber, passengerName, username;
            int ticketID;

            switch (command) {
                case 1:{
                    DisplayFlightList();
                    int flightIndex = GetFlightIndexFromUser();
                    if (flightIndex == -1) continue;
                    Airplane& selectedAirplane = airplanes[flightIndex];

                    cout << "Enter seat number: ";
                    cin >> seatNumber;
                    cout << "Enter passenger name: ";
                    cin >> passengerName;
                    ticketID = selectedAirplane.bookSeat(seatNumber, passengerName);
                    if (ticketID != -1) {
                        cout << "Seat booked successfully. Ticket ID: " << ticketID << endl;
                    } else {
                        cout << "Failed to book the seat. It may be taken or does not exist." << endl;
                    }
                    break;
                }

                case 2:{
                    DisplayFlightList();
                    cout << "Enter ticket ID: ";
                    cin >> ticketID;
                    for (size_t i = 0; i < airplanes.size(); ++i) {
                        Airplane& selectedAirplane = airplanes[i];
                        if (selectedAirplane.returnSeatByTicketID(ticketID)) {
                            cout << "Ticket returned successfully." << endl;
                        } else {
                            cout << "Failed to return the ticket. It may not exist or has already been returned." << endl;
                        }
                    }
                    break;
                }

                case 3: {
                    DisplayFlightList();
                    int flightIndex = GetFlightIndexFromUser();
                    if (flightIndex == -1) continue;
                    Airplane& selectedAirplane = airplanes[flightIndex];
                    selectedAirplane.displayAvailableSeats();
                    break;
                }

                case 4:
                    cout << "Enter ticket ID: ";
                    cin >> ticketID;

                    for (size_t i = 0; i < airplanes.size(); ++i) {
                        Airplane& selectedAirplane = airplanes[i];
                        string ticketInfo = selectedAirplane.viewTicketInfo(ticketID);
                        if (ticketInfo != "Ticket not found") {
                            cout << ticketInfo << endl;
                        }
                    }
                    break;

                case 5:
                    cout << "Enter username: ";
                    cin >> username;

                    for (size_t i = 0; i < airplanes.size(); ++i) {
                        Airplane& selectedAirplane = airplanes[i];
                        string ticketInfo = selectedAirplane.viewTicketsInfoByUser(username);
                        if (ticketInfo != "No tickets found for user") {
                            cout << ticketInfo << endl;
                        }
                    }
                    break;

                default:
                    cout << "Invalid command. Please try again." << endl;
                    break;
            }
            cout << "\nPress any key to continue..." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
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
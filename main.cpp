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

    static int getIdCounter() {
        return _idCounter;
    }

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

    string viewTicketInfo(int ticketID) const {
        auto it = _bookedTickets.find(ticketID);
        if (it != _bookedTickets.end()) {
            return it->second.getTicketInfo();
        }
        return "Ticket not found.";
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

    void displayAvailableSeats() const {
        cout << "Available seats for flight " << _flightNumber << " on " << _date << ":" << endl;
        for (const auto& seat : _seats) {
            if (seat.second.second) {
                cout << "Seat " << seat.first << " - Price: $" << seat.second.first << endl;
            }
        }
    }
};



int main(){
    string config = "11.12.2022 HJ114 2 1-10 10$ 11-20 20$";
    Airplane airplane(config);
    int bookingID = airplane.bookSeat("10A", "John Doe");
    if (bookingID != -1) {
        cout << "Booked with ID " << bookingID << endl;
        cout << airplane.viewTicketInfo(bookingID) << endl;
    }
    airplane.displayAvailableSeats();

    return 0;
}





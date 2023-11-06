#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <sstream>
#include "iostream"

using namespace std;

class Airplane {
private:
    string _flightNumber;
    string _date;
    unordered_map<string, pair<int, bool>> _seats;

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

    bool bookSeat(const string &seatNumber) {
        if (_seats.find(seatNumber) != _seats.end() && _seats[seatNumber].second) {
            _seats[seatNumber].second = false;
            return true;
        }
        return false;
    }

    bool returnSeat(const string &seatNumber) {
        auto it = _seats.find(seatNumber);
        if (it != _seats.end() && !it->second.second) {
            it->second.second = true;
            return true;
        }
        return false;
    }
};

// TODO: Add check for free spaces. Implement Ticket and Booking.

int main(){
    string config;
//    cout << "Type plane configuration" << endl;
//    cin >> config;
    config = "11.12.2022 HJ114 2 1-10 10$ 11-20 20$";
    Airplane airplane(config);
    airplane.bookSeat("10A");

    return 0;
}





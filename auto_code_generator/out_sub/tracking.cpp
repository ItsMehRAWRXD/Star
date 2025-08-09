#include <iostream>
#include <string>

void track(const std::string& event, const std::string& user) {
    std::cout << "TRACK: event=" << event << " user=" << user << std::endl;
}

int main() {
    track("app-event", "user-41b5e697");
    return 0;
}
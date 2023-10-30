#include <iostream>
#include <system_error>

std::string AskTimeServer() {
//    throw std::system_error(std::error_code());
    throw std::invalid_argument("invalid_argument");
}

class TimeServer {
public:
    std::string GetCurrentTime() {
        try {
            std::string current_time = AskTimeServer();
            last_fetched_time = current_time;
            return current_time;
        }
        catch(std::system_error& ex) {
            return last_fetched_time;
        }
    }

private:
    std::string last_fetched_time = "00:00:00";
};

int main() {
    TimeServer ts;
    try {
        std::cout << ts.GetCurrentTime() << std::endl;
    } catch (std::exception& e) {
        std::cout << "Exception got: " << e.what() << std::endl;
    }
    return 0;
}

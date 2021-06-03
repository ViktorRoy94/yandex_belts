#pragma once

#include <sstream>
#include <optional>
#include <string>

std::pair<std::string_view, std::optional<std::string_view>>
    SplitTwoStrict(std::string_view s, std::string_view delimiter = " ");

std::pair<std::string_view, std::string_view>
    SplitTwo(std::string_view s, std::string_view delimiter = " ");

std::string_view ReadToken(std::string_view& s, std::string_view delimiter = " ");

int ConvertToInt(std::string_view str);
double ConvertToDouble(std::string_view str);

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value);

std::string_view Lstrip(std::string_view line);
std::string_view Rstrip(std::string_view line);

template <typename Number>
Number ReadNumberOnLine(std::istream& stream) {
    Number number;
    stream >> number;
    std::string dummy;
    std::getline(stream, dummy);
    return number;
}

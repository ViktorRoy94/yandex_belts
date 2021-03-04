#pragma once

#include "date.h"

#include <memory>

using namespace std;

enum Comparison {
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual,
    Equal,
    NotEqual
};

enum LogicalOperation {
    And,
    Or
};

class Node {
public:
    virtual bool Evaluate(const Date& d, const string& e) const = 0;
};

class EmptyNode : public Node{
public:
    bool Evaluate(const Date& d, const string& e) const override;

};

class DateComparisonNode : public Node{
public:
    DateComparisonNode(Comparison cmp, const Date& date);
    bool Evaluate(const Date& d, const string& e) const override;

private:
    Date date_;
    Comparison cmp_;
};

class EventComparisonNode : public Node{
public:
    EventComparisonNode(Comparison cmp, const string& value);
    bool Evaluate(const Date& d, const string& e) const override;

private:
    const string event_;
    Comparison cmp_;
};

class LogicalOperationNode : public Node{
public:
    LogicalOperationNode(LogicalOperation op, shared_ptr<Node> left, shared_ptr<Node> right);
    bool Evaluate(const Date& d, const string& e) const override;

private:
    LogicalOperation op_;
    shared_ptr<Node> left_;
    shared_ptr<Node> right_;
};

void TestNode();

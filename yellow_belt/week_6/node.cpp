#include "node.h"

bool EmptyNode::Evaluate(const Date& d, const string& e) const{
    return true;
}

template <typename T>
bool CompareTo(const T& lhs, const T& rhs, Comparison cmp) {
    switch (cmp) {
    case Comparison::Less:
        return lhs < rhs;
    case Comparison::LessOrEqual:
        return lhs <= rhs;
    case Comparison::Equal:
        return lhs == rhs;
    case Comparison::NotEqual:
        return lhs != rhs;
    case Comparison::Greater:
        return lhs > rhs;
    case Comparison::GreaterOrEqual:
        return lhs >= rhs;
    }
    return false; // make compiler happy
}

DateComparisonNode::DateComparisonNode(Comparison cmp, const Date& date)
    : date_(date)
    , cmp_(cmp)
{
}
bool DateComparisonNode::Evaluate(const Date& d, const string& e) const {
    return CompareTo(d, date_, cmp_);
}

EventComparisonNode::EventComparisonNode(Comparison cmp, const string& value)
    : event_(value)
    , cmp_(cmp)
{
}
bool EventComparisonNode::Evaluate(const Date& d, const string& e) const {
    return CompareTo(e, event_, cmp_);
}

LogicalOperationNode::LogicalOperationNode(
    LogicalOperation op, shared_ptr<Node> left, shared_ptr<Node> right
)
    : op_(op)
    , left_(left)
    , right_(right)
{
}
bool LogicalOperationNode::Evaluate(const Date& d, const string& e) const {
    switch (op_) {
    case LogicalOperation::And:
        return left_->Evaluate(d, e) && right_->Evaluate(d, e);
        break;
    case LogicalOperation::Or:
        return left_->Evaluate(d, e) || right_->Evaluate(d, e);
        break;
    default:
        break;
    }
    return 0;
}

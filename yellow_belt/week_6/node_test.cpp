#include "node.h"
#include "test_runner.h"

void TestNode() {
    {
        shared_ptr<Node> left = make_shared<DateComparisonNode>(Comparison::Equal, Date(2017, 1, 1));
        shared_ptr<Node> right = make_shared<DateComparisonNode>(Comparison::Equal, Date(2017, 1, 2));
        LogicalOperationNode node(LogicalOperation::Or, left, right);
        Assert(node.Evaluate(Date(2017, 1, 1), ""), "LogicalOperationNode test 1");
        Assert(node.Evaluate(Date(2017, 1, 2), ""), "LogicalOperationNode test 2");
        Assert(!node.Evaluate(Date(2017, 2, 2), ""), "LogicalOperationNode test 3");
        Assert(!node.Evaluate(Date(2016, 2, 2), ""), "LogicalOperationNode test 4");
    }
    {
        shared_ptr<Node> left = make_shared<EventComparisonNode>(Comparison::Equal, "holiday");
        shared_ptr<Node> right = make_shared<EventComparisonNode>(Comparison::Equal, "event");
        LogicalOperationNode node(LogicalOperation::Or, left, right);
        Assert(node.Evaluate(Date(2016, 1, 1), "holiday"), "LogicalOperationNode test 5");
        Assert(node.Evaluate(Date(2017, 1, 2), "event"), "LogicalOperationNode test 6");
        Assert(!node.Evaluate(Date(2016, 2, 2), "eventholiday"), "LogicalOperationNode test 7");
        Assert(!node.Evaluate(Date(2018, 2, 2), ""), "LogicalOperationNode test 8");
    }
    {
        shared_ptr<Node> left = make_shared<DateComparisonNode>(Comparison::Less, Date(2017, 1, 1));
        shared_ptr<Node> right = make_shared<EventComparisonNode>(Comparison::Equal, "event");
        LogicalOperationNode node(LogicalOperation::Or, left, right);
        Assert(node.Evaluate(Date(2016, 1, 1), ""), "LogicalOperationNode test 9");
        Assert(node.Evaluate(Date(2017, 1, 2), "event"), "LogicalOperationNode test 10");
        Assert(node.Evaluate(Date(2016, 2, 2), "event"), "LogicalOperationNode test 11");
        Assert(!node.Evaluate(Date(2018, 2, 2), ""), "LogicalOperationNode test 12");
    }

}
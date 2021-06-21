#include <variant>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>


using std::string;
using std::unique_ptr;
using std::cout;
using std::endl;


struct Node_Integer;
struct Node_Variable;
struct Node_BinaryOp;

using Node_Ref = std::variant<
	unique_ptr<Node_Integer>,
	unique_ptr<Node_Variable>,
	unique_ptr<Node_BinaryOp>
>;


struct Node_Integer {
	int number;
};

struct Node_Variable {
	string identifier;
};

struct Node_BinaryOp {
	char op;
	Node_Ref left;
	Node_Ref right;
};


class Analyzer {
private:
	void ReadNode(const Node_Integer& node_integer) {
		cout << node_integer.number << endl;
	}
	void ReadNode(const Node_Variable& node_variable) {
		cout << node_variable.identifier << endl;
	}
	void ReadNode(const Node_BinaryOp& node_binary_op) {
		cout << node_binary_op.op << endl;
		ReadNodeRef(node_binary_op.left);
		ReadNodeRef(node_binary_op.right);
	}

public:
	void ReadNodeRef(const Node_Ref& node_ref) {
		std::visit([&](auto&& node) { assert(node != nullptr); ReadNode(*node); }, node_ref);
	}
};


Node_Ref CreateNode() {
	while (true) {
		int i; std::cin >> i;
		switch (i) {
		case 0:
			return std::make_unique<Node_Integer>(1);
		case 1:
			return std::make_unique<Node_Variable>("x");
		case 2:
			return std::make_unique<Node_BinaryOp>('+', std::make_unique<Node_Variable>("y"), std::make_unique<Node_Integer>(2));
		}
	}
}

int main() {
	Analyzer analyzer;
	Node_Ref node_ref = CreateNode();
	analyzer.ReadNodeRef(node_ref);
}
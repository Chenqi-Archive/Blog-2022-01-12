#include <variant>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>


using std::string;
using std::unique_ptr;
using std::cout;
using std::endl;


struct Node_Base;


struct Node_Integer {
	int number;
};

struct Node_Variable {
	string identifier;
};

struct Node_BinaryOp {
	char op;
	unique_ptr<Node_Base> left;
	unique_ptr<Node_Base> right;
};


using _Node_Base = std::variant<
	Node_Integer,
	Node_Variable,
	Node_BinaryOp
>;

struct Node_Base : public _Node_Base {
	using _Node_Base::_Node_Base;
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
		ReadNodeBase(node_binary_op.left);
		ReadNodeBase(node_binary_op.right);
	}

public:
	void ReadNodeBase(const unique_ptr<Node_Base>& node_base) {
		assert(node_base != nullptr);
		std::visit([&](auto&& node) { ReadNode(node); }, *node_base);
	}
};


unique_ptr<Node_Base> CreateNode() {
	while (true) {
		int i; std::cin >> i;
		switch (i) {
		case 0: 
			return std::make_unique<Node_Base>(Node_Integer(1));
		case 1: 
			return std::make_unique<Node_Base>(Node_Variable("x"));
		case 2: 
			return std::make_unique<Node_Base>(Node_BinaryOp('+', std::make_unique<Node_Base>(Node_Variable("y")), std::make_unique<Node_Base>(Node_Integer(2))));
		}
	}
}

int main() {
	Analyzer analyzer;
	unique_ptr<Node_Base> node_base = CreateNode();
	analyzer.ReadNodeBase(node_base);
}
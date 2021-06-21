#include <string>
#include <memory>
#include <iostream>
#include <cassert>


using std::string;
using std::unique_ptr;
using std::cout;
using std::endl;


enum class NodeType {
	Integer,
	Variable,
	BinaryOp
};

struct Node_Base {
	const NodeType type;
	Node_Base(NodeType type) : type(type) {}
	virtual ~Node_Base() = 0 {}
};

struct Node_Integer : Node_Base {
	int number;
	Node_Integer(int number) : Node_Base(NodeType::Integer), number(number) {}
};

struct Node_Variable : Node_Base {
	string identifier;
	Node_Variable(string identifier) : Node_Base(NodeType::Variable), identifier(identifier) {}
};

struct Node_BinaryOp : Node_Base {
	char op;
	unique_ptr<Node_Base> left;
	unique_ptr<Node_Base> right;
	Node_BinaryOp(char op, unique_ptr<Node_Base> left, unique_ptr<Node_Base> right) : 
		Node_Base(NodeType::BinaryOp), op(op), left(std::move(left)), right(std::move(right)) {}
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
		switch (node_base->type) {
		case NodeType::Integer:
			return ReadNode(static_cast<const Node_Integer&>(*node_base));
		case NodeType::Variable:
			return ReadNode(static_cast<const Node_Variable&>(*node_base));
		case NodeType::BinaryOp:
			return ReadNode(static_cast<const Node_BinaryOp&>(*node_base));
		default:
			assert(false);
		}
	}
};


unique_ptr<Node_Base> CreateNode() {
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
	unique_ptr<Node_Base> node_base = CreateNode();
	analyzer.ReadNodeBase(node_base);
}
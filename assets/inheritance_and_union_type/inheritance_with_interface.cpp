#include <string>
#include <memory>
#include <iostream>
#include <cassert>


using std::string;
using std::unique_ptr;
using std::cout;
using std::endl;


class Analyzer;


struct Node_Base {
	virtual void VisitBy(Analyzer& analyzer) = 0;
	virtual ~Node_Base() = 0 {}
};

struct Node_Integer : Node_Base {
	int number;
	Node_Integer(int number) : number(number) {}
	virtual void VisitBy(Analyzer& analyzer) override;
};

struct Node_Variable : Node_Base {
	string identifier;
	Node_Variable(string identifier) : identifier(identifier) {}
	virtual void VisitBy(Analyzer& analyzer) override;
};

struct Node_BinaryOp : Node_Base {
	char op;
	unique_ptr<Node_Base> left;
	unique_ptr<Node_Base> right;
	Node_BinaryOp(char op, unique_ptr<Node_Base> left, unique_ptr<Node_Base> right) :
		op(op), left(std::move(left)), right(std::move(right)) {
	}
	virtual void VisitBy(Analyzer& analyzer) override;
};


class Analyzer {
public:
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

	void ReadNodeBase(const unique_ptr<Node_Base>& node_base) {
		assert(node_base != nullptr);
		node_base->VisitBy(*this);
	}
};


void Node_Integer::VisitBy(Analyzer& analyzer) { analyzer.ReadNode(*this); }
void Node_Variable::VisitBy(Analyzer& analyzer) { analyzer.ReadNode(*this); }
void Node_BinaryOp::VisitBy(Analyzer& analyzer) { analyzer.ReadNode(*this); }


unique_ptr<Node_Base> CreateNode() {
	while (true) {
		int i; std::cin >> i;
		switch (i) {
		case 0:
			return std::make_unique<Node_Integer>(Node_Integer{ 1 });
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
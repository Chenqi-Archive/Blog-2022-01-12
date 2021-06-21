这学期编译课要手写一个编译器。其中的语法分析阶段会构造AST（抽象语法树）用来等价地表达源代码的结构。AST节点有各种不同的类型，比如变量定义、if语句、表达式、return语句等等，每种类型的节点会维护各自的属性。这通常可以用一个继承关系来表达。

为了方便，这里仅举数值运算表达式的例子，节点类型只包括整数字面量、整数变量、二元运算符。AST节点的定义如下：

```c++
enum class NodeType {
	Integer,
	Variable,
	BinaryOp
};

struct Node_Base {
	NodeType type;
	Node_Base(NodeType type) : type(type) {}
	virtual ~Node_Base() = 0 {}
};

struct Node_Integer : Node_Base {
	int number;
	Node_Integer() : Node_Base(NodeType::Integer) {}
};

struct Node_Variable : Node_Base {
	string identifier;
	Node_Variable() : Node_Base(NodeType::Variable) {}
};

struct Node_BinaryOp : Node_Base {
	char op;
	unique_ptr<Node_Base> left;
	unique_ptr<Node_Base> right;
	Node_BinaryOp() : Node_Base(NodeType::BinaryOp) {}
};
```

注意到，`Node_Base`基类中维护了一个`NodeType`成员变量，用来在运行时判断对象的类型。在后续对AST进行遍历的过程中，可以根据这个类型信息用switch语句进行选择，将`Node_Base`引用downcast成对应的派生类引用。

以输出前缀表达式为例，遍历过程如下：

```c++
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
```

<br>

另一种同样使用继承的写法是，不定义`NodeType`枚举类型，而是借助抽象接口来实现逻辑的动态选择，运行时会通过每个对象的虚函数表直接找到相应的处理函数：

```c++
class Analyzer;

struct Node_Base {
	virtual void VisitBy(Analyzer& analyzer) = 0;
	virtual ~Node_Base() = 0 {}
};

struct Node_Integer : Node_Base {
	int number;
	virtual void VisitBy(Analyzer& analyzer) override;
};

struct Node_Variable : Node_Base {
	string identifier;
	virtual void VisitBy(Analyzer& analyzer) override;
};

struct Node_BinaryOp : Node_Base {
	char op;
	unique_ptr<Node_Base> left;
	unique_ptr<Node_Base> right;
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
```

借助虚函数可以省去繁琐的switch条件分发和显式的downcast。但这样写也并不是特别直观，好像绕了一个圈子。并且为了定义虚函数，需要引入Analyzer这个本来与AstNode无关的外部信息，AstNode本身的结构会被破坏。

***

C/C++中的union可以作为选择性地维护不同类型对象的容器，实际实现中通常会也附加一个标签用来帮助动态确定对象的具体类型，这与上面第一种使用继承的方法类似。因此，可以用union来改写上面的代码，如下：

```c++
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

enum class NodeType {
	Integer,
	Variable,
	BinaryOp
};

struct Node_Base {
    Node_Type type;
    union{
        Node_Integer as_integer;
        Node_Variable as_variable;
        Node_BinaryOp as_binary_op;
    };
};
```

当然，上面的代码不是类型安全的，可能不会通过编译。可以用C++17提供的`std::variant`代替union：

```c++
using Node_Base = std::variant<
	Node_Integer,
	Node_Variable,
	Node_BinaryOp
>;
```

`std::variant`内部自动维护了一个类似于enum的标签，从而可以省去对enum的显式定义和维护。并且，在遍历时，可以利用`std::visit`方便地进行分发：

```c++
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
```

这样，代码就简化了不少。编译器会自动根据类型进行重载决议，按照标签表明的类型分发到对应的处理函数，运行时开销与手写的switch相近。不过因为union的大小等于成员变量的最大大小，如果实际中union需要保存大量较小的对象，剩余的空间就会被浪费掉。

<br>

使用继承时，各个派生类对象的大小可能不同，但都可以统一地用基类的引用维护，从而没有空间浪费。类似地，如果将union中保存的对象换成引用，也可以没有空间浪费（当然会带来引用的额外开销），改写如下：

```c++
using Node_Ref = std::variant<
	unique_ptr<Node_Integer>,
	unique_ptr<Node_Variable>,
	unique_ptr<Node_BinaryOp>
>;
```

这时，还可以把原先树结构中的引用替换成实体对象，因为union本身就已经是引用了：

```c++
...

struct Node_BinaryOp {
	char op;
	Node_Ref left;
	Node_Ref right;
};


class Analyzer {
private:
    ...
    
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
```

***

不管使用哪种方法，都是为了定义一种抽象的容器来容纳不同类型的对象，并且需要能够在运行时辨别这个对象到底是哪种类型，从而作后续的处理。

如果使用继承，可以在基类中显式维护一个标签用来区分派生类，手动用switch进行分发，也可以利用虚函数提供的多态性，直接进入派生类各自的处理逻辑。事实上，switch语句所表示的逻辑跳转和虚函数实现的跳转并没有太大区别，编译器通常就会将switch语句优化成跳转表。

如果使用union，具体实现时也会通常会维护一个标签用来区分对象的类型，只是每个union的盒子需要做得足够大以容纳最大的对象，从而可能会有空间浪费。当然，如果union维护的是对象的引用，就可以没有空间浪费，但会带来额外的引用开销。

<br>

进一步总结一下，为了在运行时区分不同的类型，需要对类型进行编码：

- 使用enum的编号唯一性是在编译时由编译器保证的，编译器会为每个enum表项分配一个特殊的编号。每个派生类都需要知道自己的明确编号，从而在构造时向基类注册自己的编号（因此每个派生类一定会知道一共有多少派生类）。如果要添加一个派生类，需要在enum列表中添加一个表项，并且手动在构造时注册这个类别。从而所有的派生类的编号都可能受到影响，因此需要全部重新编译。（当然，如果编号的具体数值是由程序员手动协调的，而且之前定义的类别的编号没有被调整，那就不需要全部重新编译，只需要重新链接）

- variant内部也是用类似于enum的机制实现的，编译时，编译器也需要知道variant包含了多少种具体的类别，而自动为每一种类别分配一个特殊的编号。如果要添加一个类别，需要让variant知道这个类别的全部信息，但只需要对variant重新编译，而每种具体类别不会受到影响。

- 虚函数的编号实际上就是一个函数地址，它的唯一性除了会在编译时保证，还需要靠链接期函数地址重定位保证。如果添加一个派生类，可以不用重新编译，但至少需要重新链接。（包括动态链接，动态链接也需要重定位）

<br>

一些其他的场景，比如网络协议中，也会使用类似的方法用来区分报文的类型。每一层协议都会有一个enum变量用来编码它所包含的下一级协议的类型。这些具体的编码数值都是在各级协议制定时手动安排的，并且预留了一些编码空间。这有点类似于union，只是报文的长度可以不固定。

网络并不像单台计算机那样可以对地址直接进行重定位，只能借助预先达成的共识来保证编号的唯一性。

<br>

###### 本文相关的完整代码可以通过下面的链接获取：

+ <a href="assets/inheritance_and_union_type/inheritance_with_enum.cpp" target="_blank">包含enum标识的继承</a>
+ <a href="assets/inheritance_and_union_type/inheritance_with_interface.cpp" target="_blank">包含虚函数接口的继承</a>
+ <a href="assets/inheritance_and_union_type/union_of_object.cpp" target="_blank">union作为值的容器</a>
+ <a href="assets/inheritance_and_union_type/union_of_reference.cpp" target="_blank">union作为引用的容器</a>

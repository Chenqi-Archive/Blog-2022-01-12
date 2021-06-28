大多数编译器会在语法分析阶段构造AST（抽象语法树）来表达源代码的结构。AST节点有各种不同的类型，比如变量定义、表达式、if语句、return语句等等，每种类型的节点会维护各自的属性，比如变量名、运算符、子节点等等。这通常可以用一个继承关系来定义。

这里仅举一个简单的数值运算表达式的例子，节点类型只包括整数、变量、二元运算符三种。定义如下：

```c++
enum NodeType {
	Integer,
	Variable,
	BinaryOp
};

struct Node_Base {
	NodeType type;
	Node_Base(NodeType type) : type(type) {}
};

struct Node_Integer : Node_Base {
	int number;
	Node_Integer() : Node_Base(Integer) {}
};

struct Node_Variable : Node_Base {
	string identifier;
	Node_Variable() : Node_Base(Variable) {}
};

struct Node_BinaryOp : Node_Base {
	char op;
	Node_Base* left;
	Node_Base* right;
	Node_BinaryOp() : Node_Base(BinaryOp) {}
};
```

继承关系中，每个派生类对象同时也是基类对象。可以用基类引用（指针）统一地维护各种不同的派生类对象。比如`Node_BinaryOp`的左右子节点虽然看似是对`Node_Base`的引用，但在运行期间可以指向任意一种类型的节点。

为了在运行时判断对象属于那个派生类，`Node_Base`基类中维护了一个`NodeType`枚举类型的变量，在构造时由派生类初始化。在后续遍历节点的过程中，可以根据这个类型信息将`Node_Base`引用转换成对应的派生类引用，送入专门为这个派生类制定的处理函数。

以输出前缀表达式为例，遍历过程如下：

```c++
class Printer {
	void ReadNodeBase(Node_Base* node_base) {
		switch (node_base->type) {
		case Integer: ReadNodeInteger((Node_Integer*)node_base); break;
		case Variable: ReadNodeVariable((Node_Variable*)node_base); break;
		case BinaryOp: ReadNodeBinaryOp((Node_BinaryOp*)node_base); break;
		}
	}
	void ReadNodeInteger(Node_Integer* node_integer) {
		cout << node_integer->number << endl;
	}
	void ReadNodeVariable(Node_Variable* node_variable) {
		cout << node_variable->identifier << endl;
	}
	void ReadNodeBinaryOp(Node_BinaryOp* node_binary_op) {
		cout << node_binary_op->op << endl;
		ReadNodeBase(node_binary_op->left);
		ReadNodeBase(node_binary_op->right);
	}
};
```

<br>

另一种同样使用继承的写法是，不维护`NodeType`枚举变量，而是借助虚函数接口来实现函数的动态选择。当然每个对象同样需要保存虚函数的地址作为自己属于哪个类的标识，但运行时可以不再借助switch语句分发，而只需要直接跳转到这个函数地址：

```c++
struct Node_Base {
	virtual void ReadBy(Printer* printer) = 0;
};

struct Node_Integer : Node_Base {
	int number;
	virtual void ReadBy(Printer* printer) override;
};

struct Node_Variable : Node_Base {
	string identifier;
	virtual void ReadBy(Printer* printer) override;
};

struct Node_BinaryOp : Node_Base {
	char op;
	Node_Base* left;
	Node_Base* right;
	virtual void ReadBy(Printer* printer) override;
};

class Printer {
    void ReadNodeBase(const Node_Base* node_base) {
		node_base->ReadBy(this);   // dispatch using function address
	}
    
    ...  // the same definitions
    
};

void Node_Integer::ReadBy(Printer* printer) { printer->ReadNodeInteger(this); }
void Node_Variable::ReadBy(Printer* printer) { printer->ReadNodeVariable(this); }
void Node_BinaryOp::ReadBy(Printer* printer) { printer->ReadNodeBinaryOp(this); }
```

借助虚函数可以不用再定义枚举类型，并且省去显式的switch分发和类型转换，但这样也不是特别直接，因为每个派生类中都需要定义一个虚函数，虚函数也只起到转发的作用，好像绕了一个圈子。

***

C/C++中的union可以作为选择性地维护不同类型对象的容器，实际实现中通常会也附加一个标签用来帮助动态确定对象的具体类型。因此，可以用union来改写上面的代码，如下：

```c++
struct Node_Integer {
	int number;
};

struct Node_Variable {
	string identifier;
};

struct Node_BinaryOp {
	char op;
	Node_Base* left;
	Node_Base* right;
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

class Printer {
	void ReadNodeBase(Node_Base* node_base) {
		switch (node_base->type) {
		case Integer: ReadNodeInteger(&node_base->as_integer); break;
		case Variable: ReadNodeVariable(&node_base->as_variable); break;
		case BinaryOp: ReadNodeBinaryOp(&node_base->as_binary_op); break;
		}
	}

    ...

};
```

<br>

C++17中可以使用`std::variant`代替union。`std::variant`可以直接用子类对象初始化或者赋值，内部自动维护了一个标签作为类型标记，在遍历节点时，还可以利用`std::visit`方便地分发。从而代码可以简化不少：

```c++
using Node_Base = std::variant<
	Node_Integer,
	Node_Variable,
	Node_BinaryOp
>;

class Printer {
	void ReadNodeBase(const Node_Base* node_base) {
		std::visit([&](auto&& node) { ReadNode(&node); }, *node_base);
	}
    
    // Note that ReadNode is now overloaded.
	void ReadNode(Node_Integer* node_integer) {
		cout << node_integer->number << endl;
	}
	void ReadNode(Node_Variable* node_variable) {
		cout << node_variable->identifier << endl;
	}
	void ReadNode(Node_BinaryOp* node_binary_op) {
		cout << node_binary_op->op << endl;
		ReadNodeBase(node_binary_op->left);
		ReadNodeBase(node_binary_op->right);
	}
};
```

编译器会自动把`std::visit`转化为类似于switch语句的形式，运行时也会根据标签类型分发到对应的重载函数。

<br>

值得注意的是，如果union中直接保存各种类的对象，union的大小会等于各个类对象的最大大小，从而保存较小类的对象时，剩余的空间会被浪费。在使用继承的情形中，各种大小不同的派生类对象统一地用基类的引用维护而没有空间浪费。类似地，如果将union中保存的对象换成引用，也可以没有空间浪费（当然使用引用会带来额外开销）。改写如下：

```c++
using Node_Base = std::variant<
	Node_Integer*,
	Node_Variable*,
	Node_BinaryOp*
>;

class Printer {
	void ReadNodeBase(Node_Base* node_base) {
		std::visit([&](auto&& node) { ReadNode(node); }, *node_base);
	}
    
    ...
};
```

这时，还可以把原先树结构中对子节点的引用换成对象，以减少多余引用的开销：

```c++
...

struct Node_BinaryOp {
	char op;
	Node_Base left;
	Node_Base right;
};

class Printer {
	void ReadNodeBase(Node_Base node_base) {
		std::visit([&](auto&& node) { ReadNode(node); }, node_base);
	}
    
    ...
    
    // The function is unchanged, just shown below.
	void ReadNode(const Node_BinaryOp* node_binary_op) {
		cout << node_binary_op.op << endl;
		ReadNodeBase(node_binary_op->left);
		ReadNodeBase(node_binary_op->right);
	}
};
```

#### 总结

如果使用继承，可以在基类中显式维护一个标签用来区分派生类，手动用switch语句分发，也可以利用虚函数提供的多态性，直接跳转到派生类各自的处理函数。事实上，switch语句所表示的逻辑跳转和虚函数实现的跳转并没有太大区别，编译器通常就会将比较大的switch语句优化成跳转表。

如果使用union，具体的实现中也通常会维护一个标签用来区分对象的类型，只是每个union的盒子需要做得足够大以容纳最大的对象，从而可能会有空间浪费。当然，如果union维护的是对象的引用，就可以没有空间浪费，但会带来额外的引用开销。

<br>

不管使用哪种方法，都是为了定义一种抽象的容器来容纳不同类型的对象，并且需要能够在运行时辨别这个对象到底是哪种具体的类型，从而作后续的处理。为了在运行时区分类型，需要对类型进行唯一编码。

使用enum枚举类型的编号唯一性是在编译时保证的，编译器会为每个enum表项分配一个特殊的编号。如果要添加一个类别，就需要在enum列表中添加一个表项，所有的其它表项的编号都可能受到影响。当然，程序员也可以手动为每种类别指定一个编号值，但需要保证编号不会冲突。

使用虚函数时，类型的编号实际上就是虚函数的地址，它的唯一性会在编译和链接时保证，因为不同函数不会占用同一个地址。虚函数提供了统一的接口，如果要添加一种类别只需重新链接，因此可扩展性较强。

一些其他的场景，比如网络协议中，也会使用类似的方法用来区分数据的类型。每一层协议都会有一个字段用来编码它所包含的下一级协议的类型。这种组织方式有点类似于union，只是网络报文的长度可以不固定。具体的编码数值都是在各级协议制定时手动安排的，并且预留了一些编码空间。网络并不像单台计算机那样可以直接对地址进行重定位，从而只能借助预先达成的共识来保证编号的唯一性。

<br>

###### 本文的完整可运行的代码可以通过下面的链接获取：

+ <a href="assets/inheritance_and_union_type/inheritance_with_enum.cpp" target="_blank">使用枚举类标识的继承</a>
+ <a href="assets/inheritance_and_union_type/inheritance_with_interface.cpp" target="_blank">使用虚函数接口的继承</a>
+ <a href="assets/inheritance_and_union_type/union_of_object.cpp" target="_blank">union作为值的容器</a>
+ <a href="assets/inheritance_and_union_type/union_of_reference.cpp" target="_blank">union作为引用的容器</a>

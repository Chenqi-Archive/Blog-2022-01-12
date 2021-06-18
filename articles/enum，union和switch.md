enum用来定义枚举类型。比如下面这个例子定义了Lang类型，以及一个变量：（用C语言描述）

```c
enum Lang {
    C,
    CPP,
    Java,
    Undefined,
};

Lang lang = determine_lang(text);
```

在运行期间，

枚举类型的对象的取值只能是





如果不考虑数学运算，整数就是一种枚举类型。

（数组下标访问时的数学运算）





C++17提供了`std::variant`模板库用于构造union。



C++中，union的另一种实现方式是继承。（Java似乎只能用这种方式）

如果用基类保存继承

编译器在构建AST（抽象语法树）时，通常会使用一套继承体系来描述各个元素，比如：

```c++
struct AST_Node_Base { ... };

struct AST_Node_FuncDef : public AST_Node_Base { ... };
struct AST_Node_Exp_BinaryOp : public AST_Node_Base { ... };
struct AST_Node_If : public AST_Node_Base { ... };
struct AST_Node_While : public AST_Node_Base { ... };
```





```c
enum bool_enum { 
    false_enum, 
    true_enum 
};
```



```c++
using false_type = struct {};
using true_type = struct {};
using bool_type = std::variant<false, true>;
```



```c
struct bool_struct {
    bool_enum _bool_enum;
};

struct false_struct : public bool_struct {
    false_struct() { _bool_enum = false_enum; }
};

struct true_struct : public bool_struct {
    false_struct() { _bool_enum = true_enum; }
};
```



```c
void f(bool_enum b) {
    switch (b) {
        case false_enum: ...; break;
        case true_enum: ...; break;
    }
}
```





类型和对象



以C++为例，程序可以在运行时获得类型信息，也可以在编译时就算好对象的值。所以类型和对象其实是可以被完全统一的。

（受到C++模版元编程的启发）

每个对象就可以看作一种类型，比如true和false分别可以看作两种不同的类型，bool则可以定义为true和false的union类型。



`optional<T>`可以看作`union<void, T>`。





union用于在运行时同时维护



接口抽象



但归根到底，在处理时，需要用switch区分开。



字符串哈希表！



switch就是一个从enum到函数的哈希表！（静态维护）



所有数据结构都涉及解引用或者遍历操作。拿到某个对象后，需要对对象进行一些操作。数据结构就相当于维护了从对象到函数的映射。数据结构都是哈希表！

（不过不同情形下会执行不同操作）（就相当于不同场景下各自定义了一套被映射到的函数）



程序就是编码、解码和映射。（不考虑数学计算）

（map就是表格形式的函数）

（那么数学计算也是map（基本逻辑运算的真值表））


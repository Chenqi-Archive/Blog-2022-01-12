以下C++代码会引发内存泄漏：

```c++
struct A {
	std::unique_ptr<A> ptr;
};

void f() {
	std::unique_ptr<A> a = std::make_unique<A>();
	a->ptr = std::move(a);
}
```


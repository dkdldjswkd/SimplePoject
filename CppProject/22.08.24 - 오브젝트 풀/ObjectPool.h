#pragma once

#include <stdarg.h>
#include <memory>
#include <BaseTsd.h>

#ifdef _WIN64
#define MEM_GUARD 0xFDFDFDFDFDFDFDFD

#else
#define MEM_GUARD 0xFDFDFDFD

#endif


//#define DEBUG

#ifdef DEBUG
struct T {
	T() : x(1), y(2) {}
	~T() {}
	int x, y;
};

#else
template <typename T>

#endif

// 가변인자 생성자 고려
class ObjectPool {
public:
	// node_num 만큼 node 동적할당, flag set (true : Alloc 시 마다 생성자 call, false : 노드 생성 시 최초 1회 생성자 call)
	ObjectPool(int node_num = 0, bool flag_placementNew = false) :
		integrity((ULONG_PTR)this),
		flag_placementNew(flag_placementNew),

		top(nullptr),
		capacity(node_num),
		use_count(0)
	{
		for (int i = 0; i < node_num; i++) {
			Node* new_node = new Node(this);
			new_node->next_node = top;
			top = new_node;
		}
	}

	// 스택 보유 node delete (스택이 보유하지 않는 node는 delete 불가)
	~ObjectPool() {
		for (; top != nullptr;) {
			auto delete_node = top;
			top = top->next_node;
			delete delete_node;
		}
	}

private:
	// [코드][언더] [ OBJECT ] [오버] [ NEXT* ]
	struct Node {
	public:
		Node(void* integrity) : integrity((ULONG_PTR)integrity), under(MEM_GUARD), obejct(), over(MEM_GUARD), next_node(nullptr) {}

	public:
		const ULONG_PTR integrity;
		const size_t under = MEM_GUARD;
		T obejct;
		const size_t over = MEM_GUARD;
		Node* next_node = nullptr;
	};

private:
	const ULONG_PTR integrity = (ULONG_PTR)this;
	bool flag_placementNew = false;

	Node* top = nullptr;
	int capacity = 0;
	int use_count = 0;

public:

public:
	T* Alloc() {
		if (top) {
			if (flag_placementNew) {
				new (&top->obejct) T;
			}

			auto old_top = top;
			top = top->next_node;
			++use_count;

			return &old_top->obejct;
		}
		// 오브젝트 풀 비었을때
		else {
			++capacity;
			++use_count;

			// Node 중 Object 포인터 ret
			return &(new Node(this))->obejct;
		}
	}

	void Free(T* p_obejct)	{
		Node* node = (Node*)((char*)p_obejct - sizeof(size_t) - sizeof(ULONG_PTR));

		if (integrity != node->integrity)
			throw;
		if (MEM_GUARD != node->over)
			throw;
		if (MEM_GUARD != node->under)
			throw;

		if (flag_placementNew) {
			p_obejct->~T();
		}

		--use_count;
		node->next_node = top;
		top = node;
	}

	inline int GetCapacityCount() const { return capacity; }
	inline int GetUseCount() const { return use_count; }
};

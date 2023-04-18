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

// �������� ������ ���
class ObjectPool {
public:
	// node_num ��ŭ node �����Ҵ�, flag set (true : Alloc �� ���� ������ call, false : ��� ���� �� ���� 1ȸ ������ call)
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

	// ���� ���� node delete (������ �������� �ʴ� node�� delete �Ұ�)
	~ObjectPool() {
		for (; top != nullptr;) {
			auto delete_node = top;
			top = top->next_node;
			delete delete_node;
		}
	}

private:
	// [�ڵ�][���] [ OBJECT ] [����] [ NEXT* ]
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
		// ������Ʈ Ǯ �������
		else {
			++capacity;
			++use_count;

			// Node �� Object ������ ret
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

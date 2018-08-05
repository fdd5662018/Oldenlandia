#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <queue>
#include <iostream>
#include <array>
#include <list>

#include "CollisionDetection.h"
using AlignedBox3r = Eigen::AlignedBox<Real, 3>;
using TraversalPredicate = std::function<bool(uint node_index, uint depth)>;
using TraversalCallback = std::function<void(uint node_index, uint depth)>;
using TraversalPriorityLess = std::function<bool(std::array<int, 2> const &nodes)>;

template <typename HullType>
class KDTree
{
public:

	struct Node
	{
		// b_ begin index according entries in entity list.
		// n_ number of owned entries
		// -1 if child does not exist.
		Node(uint b_, uint n_)
			: children({ { -1, -1 } })
			, begin(b_), n(n_) {}

		Node() = default;

		bool is_leaf() const { return children[0] < 0 && children[1] < 0; }

		// Index of child nodes in nodes array.
		// -1 if child does not exist.
		std::array<int, 2> children;

		uint begin;

		uint n;
	};

	struct QueueItem { uint n, d; };
	using TraversalQueue = std::queue<QueueItem>;

	KDTree(std::size_t n, uint maxPrimitivePerleaf = 1)
		: m_lst(n), m_maxPrimitivesPerLeaf(maxPrimitivePerleaf) {}

	virtual ~KDTree() {}

	// get node and hull
	Node const &node(uint i) const { return m_nodes[i]; }
	HullType const &hull(uint i) const { return m_hulls[i]; }
	uint entity(uint i) const { return m_lst[i]; }

	void construct();
	void update();
	void traverse_depth_first(TraversalPredicate pred, TraversalPredicate cb, TraversalPriorityLess const &pless = nullptr) const;
	void traverse_breadth_first(TraversalPredicate const &pred, TraversalCallback const &cb, uint start_node = 0, TraversalPriorityLess const &pless = nullptr, TraversalQueue &pending = TraversalQueue()) const;

protected:
	void construct(uint node, AlignedBox3r const &box, uint b, uint n);
	void traverse_depth_first(uint node_index, uint depth, TraversalPredicate pred, TraversalCallback cb, TraversalPriorityLess const &pless) const;
	void traverse_breadth_first(TraversalQueue &pending, TraversalPredicate const &pred, TraversalCallback const &cb, TraversalPriorityLess const &pless = nullptr) const;

	uint add_node(uint b, uint n);

	virtual Vector3r const & entity_position(uint i) const = 0;
	virtual void compute_hull(uint b, uint n, HullType &hull) const = 0;
	virtual void compute_hull_approx(uint b, uint n, HullType &hull) const
	{
		compute_hull(b, n, hull);
	}

protected:

	std::vector<uint> m_lst;

	std::vector<Node> m_nodes;
	std::vector<HullType> m_hulls;

	uint m_maxPrimitivesPerLeaf;
};

#include "kdTree.inl"

#endif
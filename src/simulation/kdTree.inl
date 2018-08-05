#include "BoundingSphere.h"
#include <cstring>

template<typename HullType> void
KDTree<HullType>::construct()
{
	m_nodes.clear();
	m_hulls.clear();
	if (m_lst.empty()) return;

	// Assigns to every element in the range [first, last] successive values of val, as if incremented with ++val after each element is written.
	std::iota(m_lst.begin(), m_lst.end(), 0);

	// Determine bounding box of considered domain.
	auto box = AlignedBox3r{};
	for (auto i = 0u; i < m_lst.size(); i++){
		box.extend(entity_position(i));
	}

	auto ni = add_node(0, static_cast<uint>(m_list.size()));
	construct(ni, box, 0, static_cast<uint>(m_lst.size()));

}

// b begin, n number
template<typename HullType> void
KDTree<HullType>::construct(uint node, AlignedBox3r const &box, uint b, uint n)
{
	// if only one element is left end recursion.
	// if ( n == 1 ) return;
	if (n <= m_maxPrimitivePerLeaf) return;

	// Determine longest side of bounding box.
	// --------------------------------------

	// logest side index;
	auto max_dir = 0;

	/** \returns an expression for the bounding box diagonal vector
	* if the length of the diagonal is needed: diagonal().norm()
	* will provide it.
	*/
	auto d = box.diagonal().eval();
	if (d(1) >= d(0) && d(1) >= d(2))
		max_dir = 1;
	else if (d(2) >= d(0) && d(2) >= d(1))
		max_dir = 2;

#ifdef _DEBUG
	for (auto i = 0u; i < n; ++i) {
		if (!box.contains(entity_position(m_lst[b + i])))
			std::cerr << "ERROR: Bounding box wrong!" << std::endl;
	}
#endif

	// Sort range according to center of the longest side.
	std::sort(m_lst.begin() + b, m_lst.begin() + b + n,
		[&](uint a, uint b)
		{
			return entity_position(a)(max_dir) < entity_position(b)(max_dir);
		}
	);

	auto hal = n / 2;
	auto n0 = add_node(b, hal);
	auto n1 = add_node(b + half, n - half);
	m_nodes[node].children[0] = n0;
	m_nodes[node].children[1] = n1;

	auto c = static_cast<Real>(0.5) * (
		entity_position(m_lst[b + hal - 1])(max_dir) +
		entity_position(m_lst[b + hal])(max_dir)	
		);
	// lower corner point.
	auto l_box = box; l_box.max()(max_dir) = c;
	// upper corner point.
	auto r_box = box; r_box.min()(max_dir) = c;
	// recursion
	construct(m_nodes[node].children[0], l_box, b, hal);
	construct(m_nodes[node].children[1], r_box, b + hal, n - hal);
}

template<typename HullType> void
KDTree<HullType>::traverse_depth_first(TraversalPredicate pred, TraversalPredicate cb, TraversalPriorityLess const &pless) const
{
	if (m_nodes.empty())
		return;

	if (pred(0, 0))
		traverse_depth_first(0, 0, pred, cb, pless);
}

template<typename HullType> void
KDTree<HullType>::traverse_depth_first(uint node_index, uint depth, TraversalPredicate pred, TraversalCallback cb, TraversalPriorityLess const &pless) const
{
	Node const &node = m_node[node_index];

	cb(node_index, depth);
	auto is_pred = pred(node_index, depth);
	if (!node.is_leaf() && is_pred) {
		if (pless && !pless(node.children)) {
			traverse_depth_first(m_nodes[node_index].children[1], depth + 1, pred, cb, pless);
			traverse_depth_first(m_nodes[node_index].children[0], depth + 1, pred, cb, pless);
		}
		else
		{
			traverse_depth_first(m_nodes[node_index].children[0], depth + 1, pred, cb, pless);
			traverse_depth_first(m_nodes[node_index].children[1], depth + 1, pred, cb, pless);
		}
	}
}

template<typename HullType> void
KDTree<HullType>::traverse_breadth_first(TraversalPredicate const &pred, TraversalCallback const &cb, uint start_node, TraversalPriorityLess const &pless, TraversalQueue &pending) const
{
	cb(start_node, 0);
	if (pred(start_node, 0)) pending.push({ start_node, 0 });
	traverse_breadth_first(pending, pred, cb, pless);
}

template<typename HullType> void
KDTree<HullType>::traverse_breadth_first(TraversalQueue &pending, TraversalPredicate const &pred, TraversalCallback const &cb, TraversalPriorityLess const &pless) const
{
	while (!pending.empty())
	{
		auto n = pending.front().n;
		auto d = pending.front().d;
		auto const &node = m_nodes[n];
		pending.pop();

		cb(n, d);
		auto is_pred = pred(n, d);
		if (!node.is_leaf() && is_pred)
		{
			if (pless && !pless(node.children)) {
				pending.push({ static_cast<uint>(node.children[1]), d + 1 });
				pending.push({ static_cast<uint>(node.children[0]), d + 1 });
			}
			else
			{
				pending.push({ static_cast<uint>(node.children[0]), d + 1 });
				pending.push({ static_cast<uint>(node.children[1]), d + 1 });
			}
		}
	}
}

template<typename HullType> uint
KDTree<HullType>::add_node(uint b, uint n)
{
	HullType hull;
	compute_hull(b, n, hull);
	m_hulls.push_back(hull);
	m_nodes.push_back({ b, n });
	return static_cast<uint>(m_nodes.size() - 1);
}

template<typename HullType> void
KDTree<HullType>::update()
{
	traverse_depth_first(
		[&](uint, uint) { return true; },
		[&](uint node_index, uint)
		{
			auto const &nd = node(node_index);
			conpute_hull_approx(nd.begin, nd.n, m_hulls[node_index]);
		}
	);
}

//
//  DLCLConstraintLayoutSolver.h
//  DLCLConstraintLayout
//
//  Created by Vincent Esche on 3/22/13.
//  Copyright (c) 2013 Vincent Esche. All rights reserved.
//

#ifndef DLCLConstraintLayout_DLCLConstraintLayoutSolver_hh
#define DLCLConstraintLayout_DLCLConstraintLayoutSolver_hh

#import "DLCLConstraint.h"

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include <stdexcept>

namespace dlcl {
		
	template <typename Cell>
	struct trait { };
	
	typedef enum : bool {
		axis_x = 0,
		axis_y = 1
	} axis_type;
	
	typedef enum : unsigned char {
		axis_attribute_min = 0,
		axis_attribute_mid = 1,
		axis_attribute_max = 2,
		axis_attribute_size = 3
	} axis_attribute_type;
	
	struct attribute_type {
		axis_type axis;
		axis_attribute_type axis_attribute;
		attribute_type(axis_type axis, axis_attribute_type axis_attribute) : axis(axis), axis_attribute(axis_attribute) {
			// intentionally left blank
		}
	};
	
	template <typename Cell>
	struct constraint {
		typedef constraint<Cell> constraint_type;
		typedef dlcl::trait<Cell> trait;
		typedef typename trait::value_type value_type;
		typedef typename trait::cell_type cell_type;
		
		const dlcl::attribute_type attribute;
		const dlcl::attribute_type source_attribute;
		const cell_type *source_cell_ptr;
		const value_type scale;
		const value_type offset;
		
		constraint(const attribute_type &attribute, const attribute_type &source_attribute, const cell_type *source_cell_ptr, const value_type &scale, const value_type &offset) : attribute(attribute), source_attribute(source_attribute), source_cell_ptr(source_cell_ptr), scale(scale), offset(offset) {
			// intentionally left blank
		}
		
		static std::string axis_as_string(const axis_type &axis) {
			std::string axes[] = {"x", "y"};
			return axes[axis];
		}
		
		static std::string axis_attribute_as_string(const axis_attribute_type &attribute) {
			switch ((int)attribute) {
				case axis_attribute_min:  { return "min";  }
				case axis_attribute_mid:  { return "mid";  }
				case axis_attribute_max:  { return "max";  }
				case axis_attribute_size: { return "size"; }
				default: { break; }
			}
			return "unknown";
		}
		
		friend std::ostream &operator<<(std::ostream &stream, const constraint_type &constraint) {
			stream << "<constraint 0x" << (long)&constraint << " ";
			stream << "attribute: {";
			stream << "axis: " << constraint_type::axis_as_string(constraint.attribute.axis) << ", ";
			stream << "attr: " << constraint_type::axis_attribute_as_string(constraint.attribute.axis_attribute);
			stream << "}, ";
			stream << "source_attribute: {";
			stream << "axis: " << constraint_type::axis_as_string(constraint.source_attribute.axis) << ", ";
			stream << "attr: " << constraint_type::axis_attribute_as_string(constraint.source_attribute.axis_attribute);
			stream << "}, ";
			stream << "source_cell_ptr: " << (__bridge void *)constraint.source_cell_ptr << " (" << ([[constraint.source_cell_ptr name] UTF8String]) << ")" << ", ";
			stream << "scale: " << constraint.scale << ", ";
			stream << "offset: " << constraint.offset;
			stream << ">";
			return stream;
		}
	};
	
	template <typename Cell>
	class node {
		typedef node<Cell> node_type;
		typedef dlcl::trait<Cell> trait;
		typedef typename trait::value_type value_type;
		typedef typename trait::cell_type cell_type;
		typedef constraint<Cell> constraint_type;
		typedef std::vector<constraint_type> constraint_vector_type;
		typedef std::set<node_type *> node_ptr_set_type;
		
		const cell_type *cell_ptr;
		const axis_type axis;
		constraint_vector_type constraints;
		node_ptr_set_type incoming;
		node_ptr_set_type outgoing;
		
	public:
		
		node(const cell_type *cell_ptr, axis_type axis) : cell_ptr(cell_ptr), axis(axis) {
			// intentionally left blank
		}
		
		static std::pair<node, node> nodes_by_axis(const cell_type *cell_ptr) {
			std::pair<node, node> nodes({node(cell_ptr, axis_x), node(cell_ptr, axis_y)});
			trait::enumerate_constraints(cell_ptr, [&](const attribute_type &attribute, const attribute_type &source_attribute, const cell_type *source, const value_type &scale, const value_type &offset, bool *stop) {
				node &node = (attribute.axis == axis_x) ? nodes.first : nodes.second;
				node.add_constraint(constraint_type(attribute, source_attribute, source, scale, offset));
			});
			return nodes;
		}
		
		const cell_type *get_cell_ptr() const {
			return this->cell_ptr;
		}
		
		const axis_type get_axis() const {
			return this->axis;
		}
		
		const constraint_vector_type &get_constraints() const {
			return this->constraints;
		}

		void enumerate_constraints(const std::function<void(const constraint_type &constraint, bool *stop)> &enumerator) const {
			bool stop = false;
			for (const constraint_type &constraint : this->constraints) {
				enumerator(constraint, &stop);
				if (stop) {
					break;
				}
			}
		}
		
		void add_constraint(const constraint_type &constraint) {
			if (constraint.attribute.axis != this->axis) {
				return;
			}
			this->constraints.push_back(constraint);
		}
		
		const node_ptr_set_type &get_incoming() const {
			return this->incoming;
		}
		
		const node_ptr_set_type &get_outgoing() const {
			return this->outgoing;
		}
		
		bool depends_on(const node_type &node) const {
			bool depends = false;
			this->enumerate_constraints([&](const constraint_type &constraint, bool *stop) {
				if (constraint.source_cell_ptr != node.cell_ptr) {
					return;
				}
				node.enumerate_constraints([&](const constraint_type &other_constraint, bool *stop) {
					if (constraint.source_attribute.axis == other_constraint.attribute.axis) {
						depends = true;
						return;
					}
				});
			});
			return depends;
		}
		
		static void add_dependency(node_type &node, node_type &dependency_node) {
			node.incoming.insert(&dependency_node);
			dependency_node.outgoing.insert(&node);
		}
		
		static void remove_dependency(node_type &node, node_type &dependency_node) {
			node.incoming.erase(&dependency_node);
			dependency_node.outgoing.erase(&node);
		}
		
		friend std::ostream &operator<<(std::ostream &stream, const node_type &node) {
			stream << "<node 0x" << (long)&node << " ";
			stream << "cell_ptr: 0x" << (long)node.cell_ptr << " (" << ([[node.cell_ptr name] UTF8String]) << ")" << ", ";
			stream << "axis: " << constraint_type::axis_as_string(node.axis) << ", ";
			stream << "constraints: {\n";
			for (const constraint_type &constraint : node.constraints) {
				stream << "\t" << constraint << std::endl;
			}
			stream << "}, ";
			stream << "incoming: {";
			int i = 1;
			for (const node_type *node_ptr : node.incoming) {
				stream << "0x" << (long)node_ptr;
				if (i++ != node.incoming.size()) {
					stream << ", ";
				}
			}
			stream << "}, ";
			stream << "outgoing: {";
			i = 1;
			for (const node_type *node_ptr : node.outgoing) {
				stream << "0x" << (long)node_ptr;
				if (i++ != node.outgoing.size()) {
					stream << ", ";
				}
			}
			stream << "}";
			stream << ">";
			return stream;
		}
	};
	
	template <typename Cell>
	class solver {
		typedef dlcl::trait<Cell> trait;
		typedef typename trait::cell_type cell_type;
		typedef dlcl::node<Cell> node_type;
		typedef constraint<Cell> constraint_type;
		typedef typename trait::frame_type frame_type;
		typedef typename trait::value_type value_type;
		typedef std::map<dlcl::axis_attribute_type, value_type> value_by_axis_attribute_type;
		
		typedef std::vector<node_type> node_vector_type;
		typedef std::vector<node_type *> node_ptr_vector_type;
		typedef std::set<node_type *> node_ptr_set_type;
		
		const cell_type *root_cell;
		node_vector_type nodes;
		node_ptr_vector_type sorted_nodes;
		
	public:
		
		solver(const cell_type *root_cell = nullptr) throw(std::runtime_error) : root_cell(root_cell) {
			this->generate_nodes(root_cell);
			this->add_node_dependencies();
			this->sort_nodes_topologically();
			this->validate_sorted_nodes(); // throws exception on dependency circles
		}
		
		void solve() {
			this->enumerate_sorted_nodes([&](const node_type &node, bool *stop) {
				solver::solve_node(node);
			});
		}
		
	private:
		
		void enumerate_nodes(const std::function<void(node_type &node, bool *stop)> &enumerator) {
			bool stop = false;
			for (auto &node : this->nodes) {
				enumerator(node, &stop);
				if (stop) {
					break;
				}
			}
		}
		
		void enumerate_sorted_nodes(const std::function<void(const node_type &node, bool *stop)> &enumerator) {
			bool stop = false;
			for (auto &node : this->sorted_nodes) {
				enumerator(*node, &stop);
				if (stop) {
					break;
				}
			}
		}
		
		void generate_nodes(const cell_type *root_cell) {
			this->nodes.clear();
			trait::enumerate_subcells(root_cell, [&](const cell_type *sub_cell, bool *stop) {
				std::pair<node_type, node_type> node_pair = node_type::nodes_by_axis(sub_cell);
				if (!node_pair.first.get_constraints().empty()) {
					this->nodes.push_back(std::move(node_pair.first));
					// do not touch node_pair.first from now on!
				}
				if (!node_pair.second.get_constraints().empty()) {
					this->nodes.push_back(std::move(node_pair.second));
					// do not touch node_pair.second from now on!
				}
			});
		}
		
		void add_node_dependencies() {
			typedef std::map<const cell_type *, node_ptr_set_type> nodes_by_cell;
			nodes_by_cell node_maps_by_axis[] {
				nodes_by_cell(),
				nodes_by_cell()
			};
			this->enumerate_nodes([&](node_type &node, bool *stop) {
				nodes_by_cell &nodes_on_axis = node_maps_by_axis[node.get_axis()];
				nodes_on_axis[node.get_cell_ptr()].insert(&node);
			});
			this->enumerate_nodes([&](node_type &node, bool *stop) {
				node.enumerate_constraints([&](const constraint_type &constraint, bool *stop) {
					const cell_type *source_cell = constraint.source_cell_ptr;
					if (!source_cell) {
						return;
					}
					nodes_by_cell &nodes_on_axis = node_maps_by_axis[constraint.source_attribute.axis];
					if (!nodes_on_axis.count(source_cell)) {
						return;
					}
					for (auto &source_node_ptr : nodes_on_axis[source_cell]) {
						if (node.depends_on(*source_node_ptr)) {
							node_type::add_dependency(node, *source_node_ptr);
						}
					}
				});
			});
		}
		
		void sort_nodes_topologically() {
			this->sorted_nodes.clear();
			std::deque<node_type *> queue;
			this->enumerate_nodes([&](node_type &node, bool *stop) {
				if (!node.get_incoming().size()) {
					queue.push_back((node_type *)&node);
				}
			});
			while (!queue.empty()) {
				node_type &node = *queue.front();
				queue.pop_front();
				this->sorted_nodes.push_back(&node);
				node_ptr_set_type outgoing = node.get_outgoing();
				for (auto &outgoing_node_ptr : outgoing) {
					node_type::remove_dependency(*outgoing_node_ptr, node);
					if (outgoing_node_ptr->get_incoming().empty()) {
						queue.push_back(outgoing_node_ptr);
					}
				}
			}
		}
		
		void validate_sorted_nodes() throw(std::runtime_error) {
			for (node_type *node_ptr : this->sorted_nodes) {
				if (!node_ptr->get_outgoing().empty() || !node_ptr->get_incoming().empty()) {
					throw std::runtime_error("Circle detected. Constraint dependencies must not form cycles.");
				}
			}
		}
		
		static void solve_node(const node_type &node) {
			const cell_type *cell_ptr = node.get_cell_ptr();
			if (!cell_ptr) {
				return;
			}
			frame_type cell_frame = trait::get_frame(cell_ptr);
			value_by_axis_attribute_type source_values;
			int axis_attributes_mask = 0x0;
			node.enumerate_constraints([&](const constraint_type &constraint, bool *stop) {
				attribute_type attribute = constraint.attribute;
				axis_attribute_type axis_attribute = attribute.axis_attribute;
				axis_attributes_mask |= (0x1 << (int)axis_attribute);
				frame_type source_cell_frame = trait::get_frame(constraint.source_cell_ptr);
				attribute_type source_attribute = constraint.source_attribute;
				value_type source_attribute_value = trait::get_attribute(source_cell_frame, source_attribute);
				source_values[constraint.attribute.axis_attribute] = (source_attribute_value * constraint.scale) + constraint.offset;
			});			
			trait::set_frame(cell_ptr, frame_after_setting_attributes(cell_frame, axis_attributes_mask, node.get_axis(), source_values));
		}
		
		static frame_type frame_after_setting_attributes(frame_type frame, const int &axis_attributes_mask, const axis_type &axis, const value_by_axis_attribute_type &source) {
			value_type cell_min_value = trait::get_frame_origin(frame, axis);
			value_type cell_size_value = trait::get_frame_size(frame, axis);
			if (axis_attributes_mask & (0x1 << axis_attribute_min)) {
				cell_min_value = source.at(axis_attribute_min); // min
				if (axis_attributes_mask & (0x1 << axis_attribute_mid)) { // min & mid
					cell_size_value = (source.at(axis_attribute_mid) - source.at(axis_attribute_min)) * 2;
				} else if (axis_attributes_mask & (0x1 << axis_attribute_max)) { // min & max
					cell_size_value = (source.at(axis_attribute_max) - source.at(axis_attribute_min));
				} else if (axis_attributes_mask & (0x1 << axis_attribute_size)) { // min & size
					cell_size_value = source.at(axis_attribute_size);
				}
			} else if (axis_attributes_mask & (0x1 << axis_attribute_size)) {
				cell_size_value = source.at(axis_attribute_size); // size
				if (axis_attributes_mask & (0x1 << axis_attribute_mid)) { // size & mid
					cell_min_value = source.at(axis_attribute_mid) - (source.at(axis_attribute_size) / 2);
				} else if (axis_attributes_mask & (0x1 << axis_attribute_max)) { // size & max
					cell_min_value = source.at(axis_attribute_max) - source.at(axis_attribute_size);
				}
			} else if (axis_attributes_mask & (0x1 << axis_attribute_mid)) {
				cell_min_value = source.at(axis_attribute_mid) - (cell_size_value / 2); // mid
				if (axis_attributes_mask & (0x1 << axis_attribute_max)) { // mid & max
					cell_size_value = (source.at(axis_attribute_max) - source.at(axis_attribute_mid)) * 2;
					cell_min_value = source.at(axis_attribute_max) - cell_size_value;
				}
			} else if (axis_attributes_mask & (0x1 << axis_attribute_max)) {
				cell_min_value = source.at(axis_attribute_max) - cell_size_value; // max
			}
			trait::set_frame_origin(frame, axis, cell_min_value);
			trait::set_frame_size(frame, axis, cell_size_value);
			return frame;
		}
		
	};
	
}

#endif

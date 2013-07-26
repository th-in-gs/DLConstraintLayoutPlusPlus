//
//  DLCLConstraintLayoutSolverCALayerTrait.hh
//  DLCLConstraintLayout
//
//  Created by Definite Loop on 3/23/13.
//  Copyright (c) 2013 Vincent Esche. All rights reserved.
//

#ifndef DLCLConstraintLayout_DLCLConstraintLayoutSolverCALayerTrait_hh
#define DLCLConstraintLayout_DLCLConstraintLayoutSolverCALayerTrait_hh

#include "DLCLConstraintLayoutSolver.hh"

namespace dlcl {
	
	template <typename Cell>
	class constraint;
	
	template<>
	struct trait<CALayer>{
		typedef CALayer cell_type;
		typedef DLCLConstraint cell_constraint_type;
		typedef CGFloat value_type;
		typedef CGRect frame_type;
		typedef int constraint_attribute_type;
		typedef NSString * __autoreleasing identifier_type;
		typedef dlcl::constraint<cell_type> constraint_type;
		
		static frame_type get_frame(const cell_type *cell) {
			return cell.frame;
		}
		
		static void set_frame(const cell_type *cell, const frame_type &frame) {
			cell.frame = frame;
		}
		
		static value_type get_attribute(const frame_type &frame, const attribute_type &attribute) {
			return get_attribute(frame, attribute.axis, attribute.axis_attribute);
		}
		
		static value_type get_attribute(const frame_type &frame, const axis_type &axis, const axis_attribute_type &axis_attribute) throw(std::runtime_error) {
			value_type value;
			switch (axis_attribute) {
				case axis_attribute_min:  { value = (axis == axis_x) ? CGRectGetMinX(frame) : CGRectGetMinY(frame);   break; }
				case axis_attribute_mid:  { value = (axis == axis_x) ? CGRectGetMidX(frame) : CGRectGetMidY(frame);   break; }
				case axis_attribute_max:  { value = (axis == axis_x) ? CGRectGetMaxX(frame) : CGRectGetMaxY(frame);   break; }
				case axis_attribute_size: { value = (axis == axis_x) ? CGRectGetWidth(frame) : CGRectGetHeight(frame);  break; }
				default: {
					throw std::runtime_error("Unknown attribute.");
					break;
				}
			}
			return value;
		}
		
		static value_type get_frame_origin(frame_type &frame, axis_type axis) {
			return (axis == axis_x) ? frame.origin.x : frame.origin.y;
		}
		
		static value_type get_frame_size(frame_type &frame, axis_type axis) {
			return (axis == axis_x) ? frame.size.width : frame.size.height;
		}
		
		static void set_frame_origin(frame_type &frame, axis_type axis, value_type value) {
			if (axis == axis_x) {
				frame.origin.x = value;
			} else {
				frame.origin.y = value;
			}
		}
		
		static void set_frame_size(frame_type &frame, axis_type axis, value_type value) {
			if (axis == axis_x) {
				frame.size.width = value;
			} else {
				frame.size.height = value;
			}
		}
		
		static void enumerate_subcells(const cell_type *sub_cell, const std::function<void(const cell_type *cell, bool *stop)> &enumerator) {
			bool stop = false;
			for (cell_type *sub_layer in sub_cell.sublayers) {
				if (stop) {
					break;
				}
				enumerator(sub_layer, &stop);
			}
		}
		
		static void enumerate_constraints(const cell_type *cell, const std::function<void(const attribute_type &attribute, const attribute_type &source_attribute, const cell_type *source, const value_type &scale, const value_type &offset, bool *stop)> &enumerator) {
			bool stop = false;
			for (cell_constraint_type *constraint in cell.constraints) {
				if (stop) {
					break;
				}
				const cell_type *super_cell = trait::get_super_cell(cell);
				if (!super_cell) {
					continue;
				}
				const cell_type *identified_cell = trait::cell_with_identifier(super_cell, constraint.sourceName);
				if (!identified_cell) {
					// no cell with given name. Ignore constraint.
					continue;
				}
				enumerator(bridged_attribute(constraint.attribute),
						   bridged_attribute(constraint.sourceAttribute),
						   identified_cell,
						   constraint.scale,
						   constraint.offset,
						   &stop);
			}
		}
		
	private: // utility functions:
		
		static const cell_type *cell_with_identifier(const cell_type *super_cell, const identifier_type &identifier) {
			const cell_type *cell_ptr = nullptr;
			if (!identifier || [identifier isEqualToString:@"superlayer"]) {
				cell_ptr = super_cell;
			} else {
				for (cell_type *sublayer in [super_cell sublayers]) {
					if ([sublayer.name isEqualToString:identifier]) {
						cell_ptr = sublayer;
						break;
					}
				}
			}
			return cell_ptr;
		}
		
		static const cell_type *get_super_cell(const cell_type *cell) {
			return [cell superlayer];
		}
		
		static attribute_type bridged_attribute(constraint_attribute_type constraint_attribute) {
			axis_type axis = (axis_type)(constraint_attribute <= kDLCLConstraintWidth) ? axis_x : axis_y;
			axis_attribute_type attribute = (axis_attribute_type)constraint_attribute;
			if (axis == axis_y) {
				// 4 is the value of the largest x axis attribute:
				// subtracting it turns y axis attributes into x axis attributes
				attribute = (axis_attribute_type)(attribute - 4);
			}
			return attribute_type(axis, attribute);
		}
	};
	
}

#endif

/*
 * PiecewiseLinearInterpolation.cpp
 *
 *  Created on: Sep 7, 2010
 *      Author: TF
 * \copyright
 * Copyright (c) 2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 */

#include "PiecewiseLinearInterpolation.h"
#include "binarySearch.h"
#include "swap.h"

#include <iostream>

namespace MathLib
{
PiecewiseLinearInterpolation::PiecewiseLinearInterpolation(
    const std::vector<double>& supporting_points,
    const std::vector<double>& values_at_supp_pnts)
    : _supporting_points(supporting_points),
      _values_at_supp_pnts(values_at_supp_pnts)
{
}

PiecewiseLinearInterpolation::PiecewiseLinearInterpolation(
    const std::vector<double>& supporting_points,
    const std::vector<double>& values_at_supp_pnts,
    const std::vector<double>& points_to_interpolate,
    std::vector<double>& values_at_interpol_pnts)
    : _supporting_points(supporting_points),
      _values_at_supp_pnts(values_at_supp_pnts)
{
    //	std::cout << "PiecewiseLinearInterpolation::PiecewiseLinearInterpolation
    // support_points, values_at_supp_pnts: "
    //<< "\n";
    //	for (size_t k(0); k<supporting_points.size(); k++) {
    //		std::cout << supporting_points[k] << " " << values_at_supp_pnts[k]
    //<<
    //"\n";
    //	}
    //	std::cout << "\n";
    values_at_interpol_pnts.clear();
    for (size_t k(0); k < points_to_interpolate.size(); k++)
        values_at_interpol_pnts.push_back(
            this->getValue(points_to_interpolate[k]));
}

PiecewiseLinearInterpolation::~PiecewiseLinearInterpolation() {}

double PiecewiseLinearInterpolation::getValue(double pnt_to_interpolate)
{
    // search interval that has the point inside
    size_t interval_idx(std::numeric_limits<size_t>::max());
    for (size_t k(1); k < _supporting_points.size() &&
                      interval_idx == std::numeric_limits<size_t>::max();
         k++)
        if (_supporting_points[k - 1] <= pnt_to_interpolate &&
            pnt_to_interpolate <= _supporting_points[k])
            interval_idx = k - 1;

    // compute linear interpolation polynom: y = m * x + n
    long double m((_values_at_supp_pnts[interval_idx + 1] -
                   _values_at_supp_pnts[interval_idx]) /
                  (_supporting_points[interval_idx + 1] -
                   _supporting_points[interval_idx]));
    //	double m ((_values_at_supp_pnts[interval_idx] -
    //_values_at_supp_pnts[interval_idx+1]) /
    //(_supporting_points[interval_idx] - _supporting_points[interval_idx+1]));
    //	double n (_values_at_supp_pnts[interval_idx+1] - m *
    //_supporting_points[interval_idx+1]);
    long double n(_values_at_supp_pnts[interval_idx] -
                  m * _supporting_points[interval_idx]);

    return m * pnt_to_interpolate + n;
}
}  // namespace MathLib

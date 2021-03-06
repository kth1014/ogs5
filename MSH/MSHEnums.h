/**
 * \file MSHEnums.h
 * 15/11/2010 KR initial implementation
 *
 * \copyright
 * Copyright (c) 2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 */

#ifndef MSHENUMS_H
#define MSHENUMS_H

#include <string>

/**
 * \brief Types of mesh elements supported by OpenGeoSys.
 * Classification and associated int are identical with
 * identifier in MeshLib::CElem::geo_type
 */
struct MshElemType
{
    enum type
    {
        LINE = 1,
        QUAD = 2,
        HEXAHEDRON = 3,
        TRIANGLE = 4,
        TETRAHEDRON = 5,
        PRISM = 6,
        PYRAMID = 7,
        QUAD8 = 8,
        INVALID = -1,
        NUM_ELEM_TYPES = 8  /// Number of element types
    };
};

/// Given a MshElemType this returns the appropriate string.
const std::string MshElemType2String(const MshElemType::type t);

/// Given a string describing an element type this returns the corresponding
/// MshElemType.
MshElemType::type String2MshElemType(const std::string& s);

#endif  // MSHENUMS_H

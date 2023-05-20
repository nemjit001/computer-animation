#pragma once

/// @brief The face union allows for setting and retrieving the indices
/// of a mesh face in the vertex array. The memory layout is contiguous so
/// that a Face array of size n is the same as a uint array of size n * 3, allowing
/// for easier use with the same data access style
union Face
{
    unsigned int m_indices[3];
    struct _FaceData {
        unsigned int m_first;
        unsigned int m_second;
        unsigned int m_third;
    };
};

#ifndef CONST_H
#define CONST_H

enum ACTION_TYPE {
    NOTHING,
    POINT_LINEARTRANSFORMATION,
    POINT_CONVERTTOGRAYSCALE,
    POINT_ANTICOLOR,
    POINT_THRESHOLD,
    POINT_HISTOGRAMNORMALIZATION,
    ALGEBRAIC_ADDITION,
    ALGEBRAIC_SUBTRACTION,
    ALGEBRAIC_TRANSLATION,
    GEOMETRICAL_HORIZONTALMIRROR,
    GEOMETRICAL_VERTICALMIRROR,
    GEOMETRICAL_SCALING,
    GEOMETRICAL_ROTATION,
    GEOMETRICAL_PERSPECTIVE,
    MORPHOLOGICAL_EROSION,
    MORPHOLOGICAL_DILATION,
    MORPHOLOGICAL_OPEN,
    MORPHOLOGICAL_CLOSE,
    MORPHOLOGICAL_THINNING,
    NEIGHBOR_ROBERT,
    NEIGHBOR_SOBEL,
    NEIGHBOR_CANNY,
    NEIGHBOR_AVERAGING,
    NEIGHBOR_GAUSSIAN,
    NEIGHBOR_MEDIAN,
    OTHER_OPEN
};

#endif // CONST_H

#ifndef slic3r_ExPolygon_hpp_
#define slic3r_ExPolygon_hpp_

#include "libslic3r.h"
#include "BoundingBox.hpp"
#include "Polygon.hpp"
#include "Polyline.hpp"
#include <ostream>
#include <vector>

namespace Slic3r {

class ExPolygon;
typedef std::vector<ExPolygon> ExPolygons;

class ExPolygon
{
    public:
    Polygon contour;
    Polygons holes;
    ExPolygon() {};
    explicit ExPolygon(const Polygon &_contour) : contour(_contour) {};
    explicit ExPolygon(const Points &_contour) : contour(Polygon(_contour)) {};
    /// Constructor to build a single holed 
    explicit ExPolygon(const Points &_contour, const Points &_hole) : contour(Polygon(_contour)), holes(Polygons(Polygon(_hole))) { };
    operator Points() const;
    operator Polygons() const;
    void scale(double factor);
    void translate(double x, double y);
    void rotate(double angle);
    void rotate(double angle, const Point &center);
    double area() const;
    bool is_valid() const;
    bool contains(const Line &line) const;
    bool contains(const Polyline &polyline) const;
    bool contains(const Point &point) const;
    bool contains_b(const Point &point) const;
    bool has_boundary_point(const Point &point) const;
    BoundingBox bounding_box() const { return this->contour.bounding_box(); };
    /// removes collinear points within SCALED_EPSILON tolerance
    void remove_colinear_points();
    void remove_vertical_collinear_points(coord_t tolerance);
    void simplify_p(double tolerance, Polygons* polygons) const;
    Polygons simplify_p(double tolerance) const;
    ExPolygons simplify(double tolerance) const;
    void simplify(double tolerance, ExPolygons* expolygons) const;
    void medial_axis(const ExPolygon &bounds, double max_width, double min_width, ThickPolylines* polylines) const;
    void medial_axis(double max_width, double min_width, Polylines* polylines) const;
    void get_trapezoids(Polygons* polygons) const;
    void get_trapezoids(Polygons* polygons, double angle) const;
    void get_trapezoids2(Polygons* polygons) const;
    void get_trapezoids2(Polygons* polygons, double angle) const;
    void triangulate(Polygons* polygons) const;
    void triangulate_pp(Polygons* polygons) const;
    void triangulate_p2t(Polygons* polygons) const;
    Lines lines() const;
    std::string dump_perl() const;
};

// Count a nuber of polygons stored inside the vector of expolygons.
// Useful for allocating space for polygons when converting expolygons to polygons.
inline size_t number_polygons(const ExPolygons &expolys)
{
    size_t n_polygons = 0;
    for (ExPolygons::const_iterator it = expolys.begin(); it != expolys.end(); ++ it)
        n_polygons += it->holes.size() + 1;
    return n_polygons;
}

inline ExPolygons
operator+(ExPolygons src1, const ExPolygons &src2) {
    append_to(src1, src2);
    return src1;
};

std::ostream& operator <<(std::ostream &s, const ExPolygons &expolygons);


inline void 
polygons_append(Polygons &dst, const ExPolygon &src) 
{ 
    dst.reserve(dst.size() + src.holes.size() + 1);
    dst.push_back(src.contour);
    dst.insert(dst.end(), src.holes.begin(), src.holes.end());
}

inline void 
polygons_append(Polygons &dst, const ExPolygons &src) 
{ 
    dst.reserve(dst.size() + number_polygons(src));
    for (ExPolygons::const_iterator it = src.begin(); it != src.end(); ++ it) {
        dst.push_back(it->contour);
        dst.insert(dst.end(), it->holes.begin(), it->holes.end());
    }
}

inline void 
polygons_append(Polygons &dst, ExPolygon &&src)
{ 
    dst.reserve(dst.size() + src.holes.size() + 1);
    dst.push_back(std::move(src.contour));
    std::move(std::begin(src.holes), std::end(src.holes), std::back_inserter(dst));
    src.holes.clear();
}

inline void 
polygons_append(Polygons &dst, ExPolygons &&src)
{ 
    dst.reserve(dst.size() + number_polygons(src));
    for (ExPolygons::iterator it = src.begin(); it != src.end(); ++ it) {
        dst.push_back(std::move(it->contour));
        std::move(std::begin(it->holes), std::end(it->holes), std::back_inserter(dst));
        it->holes.clear();
    }
}

inline void 
expolygons_append(ExPolygons &dst, const ExPolygons &src) 
{ 
    dst.insert(dst.end(), src.begin(), src.end());
}

inline void 
expolygons_append(ExPolygons &dst, ExPolygons &&src)
{ 
    if (dst.empty()) {
        dst = std::move(src);
    } else {
        std::move(std::begin(src), std::end(src), std::back_inserter(dst));
        src.clear();
    }
}

inline Polygons 
to_polygons(const ExPolygon &src)
{
    Polygons polygons;
    polygons.reserve(src.holes.size() + 1);
    polygons.push_back(src.contour);
    polygons.insert(polygons.end(), src.holes.begin(), src.holes.end());
    return polygons;
}

inline Polygons 
to_polygons(const ExPolygons &src)
{
    Polygons polygons;
    polygons.reserve(number_polygons(src));
    for (ExPolygons::const_iterator it = src.begin(); it != src.end(); ++it) {
        polygons.push_back(it->contour);
        polygons.insert(polygons.end(), it->holes.begin(), it->holes.end());
    }
    return polygons;
}

inline Polygons 
to_polygons(ExPolygon &&src)
{
    Polygons polygons;
    polygons.reserve(src.holes.size() + 1);
    polygons.push_back(std::move(src.contour));
    std::move(std::begin(src.holes), std::end(src.holes), std::back_inserter(polygons));
    src.holes.clear();
    return polygons;
}

inline Polygons 
to_polygons(ExPolygons &&src)
{
    Polygons polygons;
    polygons.reserve(number_polygons(src));
    for (ExPolygons::iterator it = src.begin(); it != src.end(); ++it) {
        polygons.push_back(std::move(it->contour));
        std::move(std::begin(it->holes), std::end(it->holes), std::back_inserter(polygons));
        it->holes.clear();
    }
    return polygons;
}

} // namespace Slic3r


#endif

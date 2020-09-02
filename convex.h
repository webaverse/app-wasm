//
// Created by Mayank on 16-03-2019.
//

#ifndef DAA_CONVEXHULL_CHALGORITHMS_H
#define DAA_CONVEXHULL_CHALGORITHMS_H

// using namespace std;

#include <vector>
#include <algorithm>
// using namespace std;

template <class T>
class Point{
public:
    T x, y;
    /* Point(T x, T y){
        this->x = x;
        this->y = y;
    } */
};

template <class T>
class ConvexHull{
public:
    std::vector<Point<T>> points;
    std::vector<Point<T>> &convex_hull_points;
    ConvexHull(std::vector<Point<T>> points, std::vector<Point<T>> &convex_hull_points) : points(points), convex_hull_points(convex_hull_points) {
      GrahamScan();
    }

private:
    T value(Point<T> p){
        return p.x*p.x + p.y*p.y;
    }
    T crossProduct(Point<T> p1, Point<T> p2){
        return (p1.x*p2.y - p2.x*p1.y);
    }
//    void merge(vector<Point<T>> &p, int l, int m, int r){
//        //TODO: check this function of sorting [output points]
//        vector<Point<T>> p1, p2;
//        cout << l << ':' << m << ':' << r << ':';
//        int n1 = m - l + 1;
//        int n2 = r - m;
//
//        cout << "p1 ";
//        for(auto i = l; i < n1; i++){
//            p1.push_back(p[i]);
//            p[i].print();
//        }
//        cout << endl;
//        cout << "p2 ";
//        for(auto i = m; i < n2; i++){
//            p2.push_back(p[i]);
//            p[i].print();
//        }
//        int i=0, j=0, k=l;
//        while(i < n1 && j < n2){
//            if(crossProduct(p1[i], p2[j]) > 0){
//                p1[i].print();
//                p2[j].print();
//                p[k] = p1[i];
//                i++;
//            }
//            else{
//                p[k] = p2[j];
//                j++;
//            }
//            k++;
//        }
//        while(i < n1){
//            p[k] = p1[i];
//            i++;
//            k++;
//        }
//        while(j < n2){
//            p[k] = p2[j];
//            j++;
//            k++;
//        }
//    }
//    void mergeSort(vector<Point<T>> &p, int l, int r){
//        if (l < r){
//            cout << l << ' ' << r <<endl;
//            int m = (l + r)/2;
//            mergeSort(p, l, m);
//            mergeSort(p, m + 1, r);
//            merge(p, l, m, r);
//            cout << "printing" << endl;
//            for(auto i = l; i <= r; i++){
//                cout << p[i].x << ',' << p[i].y << ' ';
//            }
//        }
//    }
    static bool compare(const Point<T> &p1, const Point<T> &p2) {
        return ((p1.x*p2.y - p2.x*p1.y) > 0);
    }
    void GrahamScan(){
        /**
         * Implementation of Graham Scan to find the convex hull
         * Steps:
         *  1. Find the bottom most point
         *  2. Use it to sort all other points (w.r.t. angle)
         *  3. Remove points with same angle.
         *  4. Push first 3 points.
         *  5. For the remaining ones, check for cross product before pushing.
         */

        //Finding the bottommost point (comparing y coordinates)
        int bottom_most = 0;
        for(auto i = 1; i < points.size(); i++){
            if(points[bottom_most].y > points[i].y){
                bottom_most = i;
            }
            else if(points[bottom_most].y == points[i].y){
                // In case two points lie at same bottom most level,
                // both will be on the convex hull, therefore we can
                // choose any one point. (leftmost here)
                if(points[bottom_most].x > points[i].x){
                    bottom_most = i;
                }
            }
        }
        //Swapping the bottom most point with the first point
        // before continuing further iteration
        Point<T> temp_Point = points[bottom_most];
        points[bottom_most] = points[0];
        points[0] = temp_Point;

        // cout << points[0].x << ',' << points[0].y << endl;


        //Finding angles with respect to point[0]
        std::vector<Point<T>>angles = points;
        for(auto i = 1; i < points.size(); i++){
            Point<T> temp{points[i].x - points[0].x, points[i].y - points[0].y};
            angles[i] = temp;
        }

        //Sorting wrt angles
        // TODO: replace sort with merge sort
        sort(angles.begin()+1, angles.end(), compare);
//
//        mergeSort(angles, 1, angles.size()-1);
//        for(auto i = 0; i< angles.size(); i++) {
//            cout << angles[i].x << ' ' << angles[i].y << endl;
//        }
        //Removing points with same slope (keep the one with higher value)
        for(auto i = 1;i < angles.size() - 1; i++){
            if(crossProduct(angles[i], angles[i+1]) == 0){
                if(value(angles[i]) < value(angles[i+1])){
                    angles.erase(angles.begin() + i);
                }
                else{
                    angles.erase(angles.begin() + i + 1);
                }
                i--;
            }
        }
        //Get points back from angle values
        for(auto i = 1; i < angles.size(); i++){
            angles[i].x = angles[0].x + angles[i].x;
            angles[i].y = angles[0].y + angles[i].y;
        }

        //Push first three points
        //convex_hull_points is maintained as stack
        convex_hull_points.push_back(angles[0]);
        convex_hull_points.push_back(angles[1]);
        convex_hull_points.push_back(angles[2]);

        //Remaining points
        for(auto i = 3; i < angles.size(); i++){
            // Get top 2 points from the convex hull stack
            Point<T> first = convex_hull_points.back();
            convex_hull_points.pop_back();
            Point<T> second = convex_hull_points.back();
            Point<T> new_point = angles[i];
            Point<T> val1 {first.x - second.x, first.y - second.y};
            Point<T> val2 {new_point.x - first.x, new_point.y - first.y};
            if(crossProduct(val1, val2) < 0){
                i--;
            }
            else{
                convex_hull_points.push_back(first);
                convex_hull_points.push_back(new_point);
            }
        }
    }
};

#endif //DAA_CONVEXHULL_CHALGORITHMS_H
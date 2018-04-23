#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H

#include "sceneobject.h"
#include <vector>
#include <QVector3D>

class BSplineCurve : public SceneObject
{
public:
    BSplineCurve(int degree = 3);
    ~BSplineCurve();

    void addPoint(const QVector3D &p);
    void clearPoints();
    void addKnot(float knot);
    void addKnots(std::vector<float> knots);
    void clearKnots();
    void makeBSpline();

private:
    void init() override;
    int findKnotInterval(float t);
    QVector3D deBoor(float t, int interval);

    std::vector<QVector3D> mControlPoints;
    std::vector<float> mKnotVector;
    float mStep;
    int mDegree;
};

#endif // BSPLINECURVE_H

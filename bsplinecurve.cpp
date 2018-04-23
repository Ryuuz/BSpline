#include "bsplinecurve.h"
#include "vertex.h"
#include <QDebug>


BSplineCurve::BSplineCurve(int degree) : mDegree(degree)
{
    mStep = 0.02f;
}


BSplineCurve::~BSplineCurve()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}


void BSplineCurve::addPoint(const QVector3D &p)
{
    mControlPoints.push_back(p);
}


void BSplineCurve::clearPoints()
{
    mControlPoints.clear();
}


void BSplineCurve::addKnot(float knot)
{
    mKnotVector.push_back(knot);
}


void BSplineCurve::addKnots(std::vector<float> knots)
{
    mKnotVector = knots;
}


void BSplineCurve::clearKnots()
{
    mKnotVector.clear();
}


void BSplineCurve::makeBSpline()
{
    mNumberOfVertices = (mKnotVector.back()/mStep)+1;
    mVertices = new Vertex[mNumberOfVertices];
    int index = 0;

    if(mControlPoints.size() > 1)
    {
        int knotSpan;

        for(float t = mKnotVector[0]; t <= mKnotVector.back(); t += mStep)
        {
            knotSpan = findKnotInterval(t);

            mVertices[index].setPosition(deBoor(t, knotSpan));
            mVertices[index].setNormal(1.f, 1.f, 1.f);
            mVertices[index].setUV(0.f, 0.f);

            index++;
        }

        init();
    }
}


void BSplineCurve::init()
{
    initializeOpenGLFunctions();

    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    //Sets up and fills the VBO
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mNumberOfVertices*sizeof(Vertex), mVertices, GL_STATIC_DRAW);

    //Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6*sizeof(GLfloat)));

    mDrawType = GL_POINTS;

    delete[] mVertices;
}


int BSplineCurve::findKnotInterval(float t)
{
    bool spanFound = false;
    int knotSpan = -1;

    for(unsigned int i = 0; i < mKnotVector.size(); i++)
    {
        if(!spanFound && t < mKnotVector[i])
        {
            knotSpan = i-1;
            spanFound = true;
        }
    }

    return knotSpan;
}


QVector3D BSplineCurve::deBoor(float t, int interval)
{
    QVector3D *a = new QVector3D[mDegree+1];

    for(int j = 0; j <= mDegree; j++)
    {
        a[mDegree-j] = mControlPoints[interval-j];
    }
    for(int d = mDegree; d >= 1; d--)
    {
        int j = interval - d;
        for(int i = 0; i < d; i++)
        {
            j = j+1;
            float w = (t - mKnotVector[j]) / (mKnotVector[j+d] - mKnotVector[j]);
            a[i] = ((1-w) * a[i]) + (w * a[i+1]);
        }
    }

    QVector3D temp = a[0];
    delete[] a;

    return temp;
}

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <QGLWidget>
#include <QKeyEvent>
#include <QTimer>
#include "Ground.h"
#include "Cube.h"

///////////////////////////////////////////////////////
// Some data types
typedef GLfloat GLTVector2[2];      // Two component floating point vector
typedef GLfloat GLTVector3[3];      // Three component floating point vector
typedef GLfloat GLTVector4[4];      // Four component floating point vector
typedef GLfloat GLTMatrix[16];      // A column major 4x4 matrix of type GLfloat

typedef struct{                     // The Frame of reference container
    GLTVector3 vLocation;
    GLTVector3 vUp;
    GLTVector3 vForward;
} GLTFrame;

class Scene : public QGLWidget
{
    Q_OBJECT
public:
    Scene( QWidget *parent = 0 );

private slots:
    void slotUpdate();

private:
    void initializeGL();
    void paintGL();
    void resizeGL( int w, int h );

    void keyPressEvent( QKeyEvent *event );

    void drawGround();
    void drawCube();
    void initField();
    void initCube();
    void genTexture();

    void gltApplyCameraTransform( GLTFrame *pCamera );
    void gltVectorCrossProduct( const GLTVector3 vU,
                                const GLTVector3 vV,
                                GLTVector3 vResult);
    void gltInitFrame(GLTFrame *pFrame);
    void gltMoveFrameForward(GLTFrame *pFrame, GLfloat fStep);
    void gltRotateFrameLocalY(GLTFrame *pFrame, GLfloat fAngle);
    void gltRotationMatrix(float angle, float x, float y, float z,
                           GLTMatrix mMatrix);
    void gltLoadIdentityMatrix(GLTMatrix m);
    void gltRotateVector( const GLTVector3 vSrcVector,
                          const GLTMatrix mMatrix,
                          GLTVector3 vPointOut);
private:
    GLTFrame frameCamera;
    std::vector<GLfloat> m_vertices;
    std::vector<GLfloat> m_textures;
    std::vector<GLuint> m_indices;
    GLuint m_groundTextureID;
    GLuint m_cubeTextureID;
    Ground m_ground;
    Cube m_cube;
    QTimer m_timer;
    GLfloat m_yRot;
};

#endif // SCENE_H

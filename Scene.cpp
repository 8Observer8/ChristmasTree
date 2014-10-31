#include "Scene.h"
#include <GL/glu.h>
#include <math.h>
#include <QDebug>

///////////////////////////////////////////////////////
// Useful constants
#define GLT_PI_DIV_180 0.017453292519943296

///////////////////////////////////////////////////////////////////////////////
// Useful shortcuts and macros
// Radians are king... but we need a way to swap back and forth
#define gltDegToRad(x)	((x)*GLT_PI_DIV_180)

Scene::Scene( QWidget *parent ) :
    QGLWidget( parent ),
    m_yRot( 0.0f )
{
    this->setFocusPolicy( Qt::StrongFocus );

    connect( &m_timer, SIGNAL( timeout() ),
             this, SLOT( slotUpdate() ) );

    m_timer.start( 10 );
}

void Scene::slotUpdate()
{
    m_yRot += 0.1;
    updateGL();
}

void Scene::initializeGL()
{
    // Bluish background
    glClearColor(0.0f, 0.0f, .50f, 1.0f );

    // Draw everything as wire frame
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    gltInitFrame( &frameCamera );  // Initialize the camera

    // Set drawing color to green
    //glColor3f( 0.0f, 1.0f, 0.0f );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glEnable( GL_TEXTURE_2D);

    initField();
    initCube();

    genTexture();

    // Enable the vertex array
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

void Scene::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    {
        gltApplyCameraTransform( &frameCamera );
        glPushMatrix();
        {
            drawGround();
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef( 0.0f, 0.8f, -7.0f );
            glRotatef( m_yRot, 0.0f, 1.0f, 0.0f );
            drawCube();
        }
        glPopMatrix();
    }
    glPopMatrix();
}

void Scene::resizeGL(int w, int h)
{
    GLfloat fAspect;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Scene::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() ) {
        case Qt::Key_Up:
            gltMoveFrameForward(&frameCamera, 0.1f);
            break;
        case Qt::Key_Down:
            gltMoveFrameForward(&frameCamera, -0.1f);
            break;
        case Qt::Key_Left:
            gltRotateFrameLocalY(&frameCamera, 0.1);
            break;
        case Qt::Key_Right:
            gltRotateFrameLocalY(&frameCamera, -0.1);
            break;
    }

    updateGL();
}

///////////////////////////////////////////////////////////
// Draw a gridded ground
void Scene::drawGround()
{
    glBindTexture( GL_TEXTURE_2D, m_groundTextureID );
    glVertexPointer( 3, GL_FLOAT, 0, m_ground.vertices.data() );
    glTexCoordPointer( 2, GL_FLOAT, 0, m_ground.textures.data() );
    glDrawElements( GL_TRIANGLES, m_ground.indices.size(), GL_UNSIGNED_INT,
                    m_ground.indices.data() );
}

void Scene::drawCube()
{
    glBindTexture( GL_TEXTURE_2D, m_cubeTextureID );
    glVertexPointer( 3, GL_FLOAT, 0, m_cube.vertices.data() );
    glTexCoordPointer( 2, GL_FLOAT, 0, m_cube.textures.data() );
    glDrawElements( GL_TRIANGLES, m_cube.indices.size(), GL_UNSIGNED_INT,
                    m_cube.indices.data() );
}

void Scene::initField()
{
    size_t counterOfIndeces = 0;
    size_t counterOfTextures = 0;

    for ( int z = 20; z >= -19; --z ) {
        for ( int x = -20; x <= 19; ++x ) {
            counterOfIndeces += 6;
            ++counterOfTextures;
            // 0
            m_ground.vertices.push_back( ( GLfloat ) x );
            m_ground.vertices.push_back( -0.4f );
            m_ground.vertices.push_back( ( GLfloat ) z );

            // 1
            m_ground.vertices.push_back( ( GLfloat ) ( x + 1 ) );
            m_ground.vertices.push_back( -0.4f );
            m_ground.vertices.push_back( ( GLfloat ) z );

            // 2
            m_ground.vertices.push_back( ( GLfloat ) ( x + 1 ) );
            m_ground.vertices.push_back( -0.4f );
            m_ground.vertices.push_back( ( GLfloat ) ( z - 1 ) );

            // 3
            m_ground.vertices.push_back( ( GLfloat ) x );
            m_ground.vertices.push_back( -0.4f );
            m_ground.vertices.push_back( ( GLfloat ) z );

            // 4
            m_ground.vertices.push_back( ( GLfloat ) ( x + 1 ) );
            m_ground.vertices.push_back( -0.4f );
            m_ground.vertices.push_back( ( GLfloat ) ( z - 1 ) );

            // 5
            m_ground.vertices.push_back( ( GLfloat ) x );
            m_ground.vertices.push_back( -0.4f );
            m_ground.vertices.push_back( ( GLfloat ) ( z - 1 ) );
        }
    }

    for ( size_t i = 0; i < counterOfIndeces; ++i ) {
        m_ground.indices.push_back( i );
    }

    // Texture
    for ( size_t i = 0; i < counterOfTextures; ++i ) {
        m_ground.textures.push_back( 0.0f );
        m_ground.textures.push_back( 0.0f );

        m_ground.textures.push_back( 1.0f );
        m_ground.textures.push_back( 0.0f );

        m_ground.textures.push_back( 1.0f );
        m_ground.textures.push_back( 1.0f );

        m_ground.textures.push_back( 0.0f );
        m_ground.textures.push_back( 0.0f );

        m_ground.textures.push_back( 1.0f );
        m_ground.textures.push_back( 1.0f );

        m_ground.textures.push_back( 0.0f );
        m_ground.textures.push_back( 1.0f );
    }
}

void Scene::initCube()
{
    // 0 1 2
    m_cube.vertices.push_back( -1.0f );    // X
    m_cube.vertices.push_back( -1.0f );    // Y
    m_cube.vertices.push_back( 1.0f );      // Z

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    // 3 4 5
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    // 6 7 8
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    // 9 10 11
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    // 12 13 14
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    // 15 16 17
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    // 18 19 20
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    // 21 22 23
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    // 24 25 26
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    // 27 28 29
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );

    // 30 31 32
    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    // 33 34 35
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );

    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    m_cube.vertices.push_back( -1.0f );
    m_cube.vertices.push_back( 1.0f );
    m_cube.vertices.push_back( -1.0f );

    // Indices
    m_cube.indices.push_back( 0 );
    m_cube.indices.push_back( 1 );
    m_cube.indices.push_back( 2 );

    m_cube.indices.push_back( 3 );
    m_cube.indices.push_back( 4 );
    m_cube.indices.push_back( 5 );

    m_cube.indices.push_back( 6 );
    m_cube.indices.push_back( 7 );
    m_cube.indices.push_back( 8 );

    m_cube.indices.push_back( 9 );
    m_cube.indices.push_back( 10 );
    m_cube.indices.push_back( 11 );

    m_cube.indices.push_back( 12 );
    m_cube.indices.push_back( 13 );
    m_cube.indices.push_back( 14 );

    m_cube.indices.push_back( 15 );
    m_cube.indices.push_back( 16 );
    m_cube.indices.push_back( 17 );

    m_cube.indices.push_back( 18 );
    m_cube.indices.push_back( 19 );
    m_cube.indices.push_back( 20 );

    m_cube.indices.push_back( 21 );
    m_cube.indices.push_back( 22 );
    m_cube.indices.push_back( 23 );

    m_cube.indices.push_back( 24 );
    m_cube.indices.push_back( 25 );
    m_cube.indices.push_back( 26 );

    m_cube.indices.push_back( 27 );
    m_cube.indices.push_back( 28 );
    m_cube.indices.push_back( 29 );

    m_cube.indices.push_back( 30 );
    m_cube.indices.push_back( 31 );
    m_cube.indices.push_back( 32 );

    m_cube.indices.push_back( 33 );
    m_cube.indices.push_back( 34 );
    m_cube.indices.push_back( 35 );

    // Texture
    for ( size_t i = 0; i < 6; ++i ) {
        m_cube.textures.push_back( 0.0f );
        m_cube.textures.push_back( 0.0f );

        m_cube.textures.push_back( 1.0f );
        m_cube.textures.push_back( 0.0f );

        m_cube.textures.push_back( 0.0f );
        m_cube.textures.push_back( 1.0f );

        m_cube.textures.push_back( 1.0f );
        m_cube.textures.push_back( 0.0f );

        m_cube.textures.push_back( 1.0f );
        m_cube.textures.push_back( 1.0f );

        m_cube.textures.push_back( 0.0f );
        m_cube.textures.push_back( 1.0f );
    }
}

//////////////////////////////////////////////////////////////////
// Apply a camera transform given a frame of reference. This is
// pretty much just an alternate implementation of gluLookAt using
// floats instead of doubles and having the forward vector specified
// instead of a point out in front of me.
void Scene::gltApplyCameraTransform(GLTFrame *pCamera)
{
    GLTMatrix mMatrix;
    GLTVector3 vAxisX;
    GLTVector3 zFlipped;

    zFlipped[0] = -pCamera->vForward[0];
    zFlipped[1] = -pCamera->vForward[1];
    zFlipped[2] = -pCamera->vForward[2];

    // Derive X vector
    gltVectorCrossProduct(pCamera->vUp, zFlipped, vAxisX);

    // Populate matrix, note this is just the rotation and is transposed
    mMatrix[0] = vAxisX[0];
    mMatrix[4] = vAxisX[1];
    mMatrix[8] = vAxisX[2];
    mMatrix[12] = 0.0f;

    mMatrix[1] = pCamera->vUp[0];
    mMatrix[5] = pCamera->vUp[1];
    mMatrix[9] = pCamera->vUp[2];
    mMatrix[13] = 0.0f;

    mMatrix[2] = zFlipped[0];
    mMatrix[6] = zFlipped[1];
    mMatrix[10] = zFlipped[2];
    mMatrix[14] = 0.0f;

    mMatrix[3] = 0.0f;
    mMatrix[7] = 0.0f;
    mMatrix[11] = 0.0f;
    mMatrix[15] = 1.0f;

    // Do the rotation first
    glMultMatrixf(mMatrix);

    // Now, translate backwards
    glTranslatef( -pCamera->vLocation[0],
            -pCamera->vLocation[1],
            -pCamera->vLocation[2]);
}

// Calculate the cross product of two vectors
void Scene::gltVectorCrossProduct(const GLTVector3 vU,
                                  const GLTVector3 vV,
                                  GLTVector3 vResult)
{
    vResult[0] = vU[1]*vV[2] - vV[1]*vU[2];
    vResult[1] = -vU[0]*vV[2] + vV[0]*vU[2];
    vResult[2] = vU[0]*vV[1] - vV[0]*vU[1];
}

// Initialize a frame of reference.
// Uses default OpenGL viewing position and orientation
void Scene::gltInitFrame(GLTFrame *pFrame)
{
    pFrame->vLocation[0] = 0.0f;
    pFrame->vLocation[1] = 0.0f;
    pFrame->vLocation[2] = 0.0f;

    pFrame->vUp[0] = 0.0f;
    pFrame->vUp[1] = 1.0f;
    pFrame->vUp[2] = 0.0f;

    pFrame->vForward[0] = 0.0f;
    pFrame->vForward[1] = 0.0f;
    pFrame->vForward[2] = -1.0f;
}

/////////////////////////////////////////////////////////
// March a frame of reference forward. This simply moves
// the location forward along the forward vector.
void Scene::gltMoveFrameForward(GLTFrame *pFrame, GLfloat fStep)
{
    pFrame->vLocation[0] += pFrame->vForward[0] * fStep;
    pFrame->vLocation[1] += pFrame->vForward[1] * fStep;
    pFrame->vLocation[2] += pFrame->vForward[2] * fStep;
}

/////////////////////////////////////////////////////////
// Rotate a frame around it's local Y axis
void Scene::gltRotateFrameLocalY(GLTFrame *pFrame, GLfloat fAngle)
{
    GLTMatrix mRotation;
    GLTVector3 vNewForward;

    gltRotationMatrix((float)gltDegToRad(fAngle), 0.0f, 1.0f, 0.0f, mRotation);
    gltRotationMatrix(fAngle, pFrame->vUp[0], pFrame->vUp[1], pFrame->vUp[2], mRotation);

    gltRotateVector(pFrame->vForward, mRotation, vNewForward);
    memcpy(pFrame->vForward, vNewForward, sizeof(GLTVector3));
}

///////////////////////////////////////////////////////////////////////////////
// Creates a 4x4 rotation matrix, takes radians NOT degrees
void Scene::gltRotationMatrix(float angle, float x, float y, float z,
                              GLTMatrix mMatrix)
{
    float vecLength, sinSave, cosSave, oneMinusCos;
    float xx, yy, zz, xy, yz, zx, xs, ys, zs;

    // If NULL vector passed in, this will blow up...
    if(x == 0.0f && y == 0.0f && z == 0.0f)
    {
        gltLoadIdentityMatrix(mMatrix);
        return;
    }

    // Scale vector
    vecLength = (float)sqrt( x*x + y*y + z*z );

    // Rotation matrix is normalized
    x /= vecLength;
    y /= vecLength;
    z /= vecLength;

    sinSave = (float)sin(angle);
    cosSave = (float)cos(angle);
    oneMinusCos = 1.0f - cosSave;

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * sinSave;
    ys = y * sinSave;
    zs = z * sinSave;

    mMatrix[0] = (oneMinusCos * xx) + cosSave;
    mMatrix[4] = (oneMinusCos * xy) - zs;
    mMatrix[8] = (oneMinusCos * zx) + ys;
    mMatrix[12] = 0.0f;

    mMatrix[1] = (oneMinusCos * xy) + zs;
    mMatrix[5] = (oneMinusCos * yy) + cosSave;
    mMatrix[9] = (oneMinusCos * yz) - xs;
    mMatrix[13] = 0.0f;

    mMatrix[2] = (oneMinusCos * zx) - ys;
    mMatrix[6] = (oneMinusCos * yz) + xs;
    mMatrix[10] = (oneMinusCos * zz) + cosSave;
    mMatrix[14] = 0.0f;

    mMatrix[3] = 0.0f;
    mMatrix[7] = 0.0f;
    mMatrix[11] = 0.0f;
    mMatrix[15] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Load a matrix with the Idenity matrix
void Scene::gltLoadIdentityMatrix(GLTMatrix m)
{
    static GLTMatrix identity = { 1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f };

    memcpy(m, identity, sizeof(GLTMatrix));
}

// Rotates a vector using a 4x4 matrix. Translation column is ignored
void Scene::gltRotateVector( const GLTVector3 vSrcVector,
                             const GLTMatrix mMatrix,
                             GLTVector3 vOut)
{
    vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] *  vSrcVector[2];
    vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] *  vSrcVector[2];
    vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2];
}

void Scene::genTexture()
{
    m_groundTextureID=bindTexture(QPixmap(QString(":textures/Snow.jpg")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_cubeTextureID=bindTexture(QPixmap(QString(":textures/ChristmasTree.jpg")), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //    // Ground
    //    QImage groundImage;
    //    groundImage.load( QString(":/textures/Snow.jpg") );
    //    groundImage = QGLWidget::convertToGLFormat( groundImage );

    //    GLuint textureIDs[2];

    //    glGenTextures( 2, textureIDs );

    //    m_groundTextureID = textureIDs[0];
    //    m_cubeTextureID = textureIDs[1];

    //    glBindTexture( GL_TEXTURE_2D, m_groundTextureID );

    //    glTexImage2D( GL_TEXTURE_2D, 0, 3,
    //                  ( GLsizei ) groundImage.width(),
    //                  ( GLsizei ) groundImage.height(), 0,
    //                  GL_RGBA, GL_UNSIGNED_BYTE, groundImage.bits() );

    //    QImage cubeImage;
    //    cubeImage.load( QString(":/textures/picture1.jpg") );
    //    cubeImage = QGLWidget::convertToGLFormat( cubeImage );

    //    glBindTexture( GL_TEXTURE_2D, m_cubeTextureID );

    //    glTexImage2D( GL_TEXTURE_2D, 0, 3,
    //                  ( GLsizei ) cubeImage.width(),
    //                  ( GLsizei ) cubeImage.height(), 0,
    //                  GL_RGBA, GL_UNSIGNED_BYTE, cubeImage.bits() );

    //    // дополнительные параметры текстурного объекта
    //    // задаём линейную фильтрацию вблизи:
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    // задаём линейную фильтрацию вдали:
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для s координаты
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    // задаём: цвет текселя полностью замещает цвет фрагмента фигуры
    //    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

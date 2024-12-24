#include "GLTool.h"
#include "MathTool.h"
#include "Mesh.h"
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <math.h>

void calColorMap(double _value, Vec3d* _color)
{
    int Hi;
    double H, f;
    if (_value > 1) {
        _value = 1;
    }
    H = 6 * 0.7 * (1 - _value);
    Hi = (int)floor(H) % 6;
    f = H - (double)Hi;
    switch (Hi) {
    case 0:
        _color->x = 1;
        _color->y = f;
        _color->z = 0;
        break;
    case 1:
        _color->x = 1 - f;
        _color->y = 1;
        _color->z = 0;
        break;
    case 2:
        _color->x = 0;
        _color->y = 1;
        _color->z = f;
        break;
    case 3:
        _color->x = 0;
        _color->y = 1 - f;
        _color->z = 1;
        break;
    case 4:
        _color->x = f;
        _color->y = 0;
        _color->z = 1;
        break;
    case 5:
        _color->x = 1;
        _color->y = 0;
        _color->z = 1 - f;
        break;
    }
}

void renderFEMMesh(Mesh* _mesh, double _max_mises_stress)
{
    Vec3d color;
    unsigned int i, j, k;
    //[TODO3]描画処理の書き写し
    // ノードの描画
    glPointSize(10);
    glBegin(GL_POINTS);
    for (i = 0; i < _mesh->num_node; i++) {
        // 頂点の状態に応じて色を変える
        switch (_mesh->node[i].state) {
        case NODE_FREE:
            glColor3d(0, 1, 1);
            break;
        case NODE_FIXED:
            glColor3d(1, 0, 1);
            break;
        case NODE_DEFORM:
            glColor3d(1, 1, 0);
            break;
        }
        // 頂点の座標を指定
        glVertex3dv(_mesh->node[i].new_position.X);
    }
    glEnd();
    // ノード間ラインの描画(変形前・変形後)
    for (i = 0; i < _mesh->num_tetrahedra; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < j; k++) {
                glLineWidth(0.5);
                glColor3d(0.3, 0.3, 0.3);
                glBegin(GL_LINE_STRIP);
                glVertex3dv(_mesh->tetrahedra[i].position[j].X);
                glVertex3dv(_mesh->tetrahedra[i].position[k].X);
                glEnd();
                glLineWidth(1);
                glColor3d(0, 0, 0);
                glBegin(GL_LINE_STRIP);
                glVertex3dv(_mesh->tetrahedra[i].new_position[j].X);
                glVertex3dv(_mesh->tetrahedra[i].new_position[k].X);
                glEnd();
            }
        }
    }
    // 要素ポリゴンの描画
    glColor3d(0, 0, 1);
    for (i = 0; i < _mesh->num_tetrahedra; i++) {
        calColorMap(_mesh->tetrahedra[i].mises_stress / _max_mises_stress, &color);
        for (j = 0; j < 4; j++) {
            glColor3dv(color.X);
            glBegin(GL_TRIANGLES);
            glVertex3dv(_mesh->tetrahedra[i].new_position[(j + 0) % 4].X);
            glVertex3dv(_mesh->tetrahedra[i].new_position[(j + 1) % 4].X);
            glVertex3dv(_mesh->tetrahedra[i].new_position[(j + 2) % 4].X);
            glEnd();
        }
    }
}


float getDepth(int _pos_window_x, int _pos_window_y)
{
    float depth;
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    // デプスバッファの取得
    glReadPixels(_pos_window_x, viewport[3] - _pos_window_y, 1, 1,
        GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
}

void convertWorld2Window(Vec3d* _position_world, Vec3d* _position_window)
{
    GLdouble matrix_modelview[16];
    GLdouble matrix_projection[16];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, matrix_modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, matrix_projection);
    // ワールド座標系からウィンドウ座標系へ変換
    gluProject(_position_world->x, _position_world->y, _position_world->z,
        matrix_modelview, matrix_projection, viewport,
        &_position_window->x, &_position_window->y, &_position_window->z);
}

void convertWindow2World(Vec3d* _position_window, Vec3d* _position_world)
{
    GLdouble matrix_modelview[16];
    GLdouble matrix_projection[16];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, matrix_modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, matrix_projection);
    // ウィンドウ座標系からワールド座標系へ変換
    gluUnProject(_position_window->x, (double)viewport[3] - _position_window->y, _position_window->z,
        matrix_modelview, matrix_projection, viewport,
        &_position_world->x, &_position_world->y, &_position_world->z);
}

void glInit(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void setCamera(int _width, int _height)
{
    glViewport(0, 0, _width, _height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.02, 0.02, -0.02 * (double)_height / _width, 0.02 * (double)_height / _width, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 500.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void renderGrid(double _scale)
{
    int i;
    glColor3d(0, 0, 0);
    for (i = -10; i <= 10; i++) {
        glBegin(GL_LINE_STRIP);
        glVertex3d(-10 * _scale, 0, i * _scale);
        glVertex3d(10 * _scale, 0, i * _scale);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3d(i * _scale, 0, -10 * _scale);
        glVertex3d(i * _scale, 0, 10 * _scale);
        glEnd();
    }
}

void setMouseRotation(double _x, double _y, Matd* _dst)
{
    Matd matrix_rot_x;
    Matd matrix_rot_y;
    Matd matrix_temp;
    initMat(&matrix_rot_x);
    initMat(&matrix_rot_y);
    initMat(&matrix_temp);
    setRotationalMatrix(_x, ROT_AXIS_Y, &matrix_rot_y);
    setRotationalMatrix(_y, ROT_AXIS_X, &matrix_rot_x);
    multiMatandMat(&matrix_rot_y, _dst, &matrix_temp);
    multiMatandMat(&matrix_rot_x, &matrix_temp, _dst);
    releaseMat(&matrix_rot_x);
    releaseMat(&matrix_rot_y);
    releaseMat(&matrix_temp);
}

void setMouseScroll(double _s, Matd* _dst)
{
    _dst->X[0] = _dst->X[5] = _dst->X[10] = _s;
}

void drawdeformationArrow(Mesh* _mesh)
{

    glLineWidth(1.0);

    glColor3f(1.0, 0.0, 0.0);

    glBegin(GL_LINES);
    for (int i = 0; i < _mesh->num_node; i++) {
        // 元の節点位置
        double x0 = _mesh->node[i].position.x;
        double y0 = _mesh->node[i].position.y;
        double z0 = _mesh->node[i].position.z;

        // 変位後の節点位置
        double x1 = x0 + _mesh->deformation.X[3 * i];
        double y1 = y0 + _mesh->deformation.X[3 * i + 1];
        double z1 = z0 + _mesh->deformation.X[3 * i + 2];

        glVertex3f(x0, y0, z0);  // 始点
        glVertex3f(x1, y1, z1);  // 終点
    }
    glEnd();

    // 矢印の三角形を描く
    for (int i = 0; i < _mesh->num_node; i++) {
        // 元の節点位置
        double x0 = _mesh->node[i].position.x;
        double y0 = _mesh->node[i].position.y;
        double z0 = _mesh->node[i].position.z;

        // 変位後の節点位置
        double x1 = x0 + _mesh->deformation.X[3 * i];
        double y1 = y0 + _mesh->deformation.X[3 * i + 1];
        double z1 = z0 + _mesh->deformation.X[3 * i + 2];
        glPushMatrix();
        glTranslated(x1, y1, z1);
        if (x1 != x0 || y1 != y0 || z1 != z0) {
            glRotated(atan2(z1 - z0, y1 - y0) * M_PI, 1, 0, 0);
            glRotated(atan2(x1 - x0, z1 - z0) * M_PI, 0, 1, 0);
            glRotated(atan2(y1 - y0, x1 - x0) * M_PI, 0, 0, 1);
        }
        glutSolidCone(0.5, 3, 5, 5);
        glPopMatrix();
    }
}

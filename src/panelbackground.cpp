#include "panelbackground.h"

PanelBackground *pbg_Black = new PBG_Solid( zero3f );
PanelBackground *pbg_White = new PBG_Solid( one3f );
PanelBackground *pbg_Gray = new PBG_Solid( makevec3f( 0.50, 0.50, 0.50 ) );
PanelBackground *pbg_DarkGray = new PBG_Solid( makevec3f( 0.20, 0.20, 0.20 ) );
PanelBackground *pbg_Maya = new PBG_VGrad( makevec3f( 0.30, 0.30, 0.35 ), makevec3f( 0.12, 0.12, 0.15 ) );
PanelBackground *pbg_GrayGradient = new PBG_VGrad( makevec3f( 0.35, 0.35, 0.35 ), makevec3f( 0.15, 0.15, 0.15 ) );




void PanelBackground::draw() {
    glsCheckError();
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glPushAttrib( GL_ENABLE_BIT );
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    _draw();
    
    glPopAttrib();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glsCheckError();
}

void PBG_Solid::_draw() {
    glBegin(GL_QUADS);
    glsColor(color);
    glVertex3f(1,1,1);
    glVertex3f(-1,1,1);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,1);
    glEnd();
}

void PBG_VGrad::_draw() {
    glBegin(GL_QUADS);
    glsColor(color_top);
    glVertex3f(1,1,1);
    glVertex3f(-1,1,1);
    glsColor(color_bottom);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,1);
    glEnd();
}


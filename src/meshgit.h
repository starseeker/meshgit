#ifndef _MESHGIT_H_
#define _MESHGIT_H_

#include "viewer.h"
#include "panel.h"
#include "meshgit_lib/mesh.h"
#include "meshgit_lib/binheap.h"

#include "stdio.h"


namespace MeshGitViews {
    enum Enum {
        None,
        View,
        Diff,
        Diff_Load,
        Diff_Save,
        Diff_Explore,
        Diff_Sequence,
        Diff_Compare,
        Diff_Timed,
        Merge,
        Info,
        Execute,
    };
};
namespace MeshGitCommands {
    enum Enum {
        None,
        MeshGit,
        NoAdjacency,
        Exact,
        Greedy1000, Greedy2000, Greedy3000, Greedy,
        GreedyBacktrack,
    };
};
namespace BackgroundType {
    enum Enum { Gradient, Black, White };
    const vector<string> Names = { "Gradient", "Black", "White" };
}


struct material_set {
    material face, face_;
    material face_del, face_del_;
    material face_add, face_add_;
    vec3f vert;
    
    material_set(material face, vec3f vert) :
    material_set(face, face, face, face, face, face, vert) { }
    
    material_set(material face, material face_del, material face_add, vec3f vert) :
    material_set(face, face, face_del, face_del, face_add, face_add, vert) { }
    
    material_set(material face, material face_, material face_del, material face_del_, material face_add, material face_add_, vec3f vert) :
    face(face), face_(face_), face_del(face_del), face_del_(face_del_), face_add(face_add), face_add_(face_add_), vert(vert) { }
};



/////////////////////////////////
// meshgit_cameras.cpp

struct VPCameraSettings {
    string name;
    frame3f f;
    float d;
    float h;
    float l;
    bool ortho;
    Property *prop = nullptr;
    
    VPCameraSettings( FILE *fp ) { from_file(fp); }
    VPCameraSettings( VPCamera *camera, string name ) : name(name) { from_camera(camera); }
    
    void from_file( FILE *fp ) {
        char buf[80]; int c,i;
        for(i=0;;i++) if((buf[i]=fgetc(fp))=='\n') { buf[i]='\0'; break; } name=buf;
        //fscanf(fp,"%d\n",&c); for(i=0;i<c;i++) buf[i]=fgetc(fp); buf[c]='\0'; name=buf;
        fscanf(fp,"%f %f %f\n", &f.o.x, &f.o.y, &f.o.z);
        fscanf(fp,"%f %f %f\n", &f.x.x, &f.x.y, &f.x.z);
        fscanf(fp,"%f %f %f\n", &f.y.x, &f.y.y, &f.y.z);
        fscanf(fp,"%f %f %f\n", &f.z.x, &f.z.y, &f.z.z);
        fscanf(fp,"%f %f %f %d\n", &d, &h, &l, &c); ortho = (c != 0);
    }
    
    void to_file( FILE *fp ) {
        fprintf(fp,"%s\n",name.c_str() );
        //fprintf(fp,"%d\n%s\n",(int)name.length(), name.c_str() );
        fprintf(fp," %f %f %f\n", f.o.x, f.o.y, f.o.z);
        fprintf(fp," %f %f %f\n", f.x.x, f.x.y, f.x.z);
        fprintf(fp," %f %f %f\n", f.y.x, f.y.y, f.y.z);
        fprintf(fp," %f %f %f\n", f.z.x, f.z.y, f.z.z);
        fprintf(fp," %f %f %f %d\n", d, h, l, (ortho?1:0) );
    }
    
    void from_camera( VPCamera *camera ) {
        f = camera->f;
        d = camera->d;
        h = camera->h;
        l = camera->l;
        ortho = camera->orthographic;
    }
    
    void to_camera( VPCamera *camera ) {
        camera->f = f;
        camera->d = d;
        camera->h = h;
        camera->l = l;
        camera->orthographic = ortho;
    }
};

extern vector<VPCameraSettings*> settings;

void add_camera_properties( VPCamera *camera );



/////////////////////////////////
// meshgit_globals.cpp

extern bool  glob_mesh_edges;
extern int   glob_mesh_edges_darkness;
extern bool  glob_mesh_faces;
extern bool  glob_mesh_faces_chgsonly;
extern bool  glob_mesh_faces_sameonly;
extern bool  glob_mesh_faces_twotoned;
extern bool  glob_mesh_components;
extern bool  glob_overlay_edges;
extern bool  glob_morph_verts;
extern bool  glob_morph_faces;
extern bool  glob_connect_matched_verts;
extern bool  glob_connect_matched_faces;

extern float glob_part_offset;

extern bool  glob_knn_vert;
extern bool  glob_knn_face;

extern int   glob_patch_size;
extern float glob_patch_per;
extern int   glob_exact_knn;
extern float glob_exact_dist;

extern vec3f glob_offset;
extern float glob_scale;

extern material mat_neutral;
extern material mat_match, mat_match_;
extern material mat_unmatch0, mat_unmatch1, mat_unmatch01, mat_unmatch0_, mat_unmatch1_, mat_unmatch01_;
extern material mat_unmatch0a, mat_unmatch0b, mat_unmatch0ab, mat_unmatcha0, mat_unmatchb0;
extern material mat_unmatch0a_, mat_unmatch0b_, mat_unmatch0ab_, mat_unmatcha0_, mat_unmatchb0_;

extern VP_PropertyView *pnl_properties;
extern VP_Container *pnl_main;




/////////////////////////////////
// meshgit_drawutils.cpp

void material_setdarker( material &mat, material &mat_);
vec3f get_move_color( vec3f &v0, vec3f &v1, vec3f &color0, vec3f &color1 );
vec3f get_move_color( float dist, vec3f &color0, vec3f &color1 );
void set_material( material &mat, material &mat_ );
void set_material( GLenum pname, vec3f c, vec3f c_ );
void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos );
void draw_face( vector<ivert> &vinds, vector<vec3f> &vpos );
void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vector<int> &vm, vector<vec3f> &vpos_, vec3f &c0, vec3f &c0_, vec3f &c1, vec3f &c1_ );
void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vector<vec3f> &vpos_, vec3f &c0, vec3f &c0_, vec3f &c1, vec3f &c1_ );
void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vec3f &offset );


SimpleMesh * subd_mesh(Mesh *m, int subd_level=0);
SimpleMesh * subd_m1_2way(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level=0);
SimpleMesh * subd_m0_2way(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level=0);
SimpleMesh * subd_m1_2way_changes(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level=0);
SimpleMesh * subd_m0_2way_changes(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level=0);
SimpleMesh * subd_m0_3way(Mesh *m0, MeshMatch *mm0a, MeshMatch *mm0b, int subd_level=0);
SimpleMesh * subd_m0_3way_changes(Mesh *m0, MeshMatch *mm0a, MeshMatch *mm0b, int subd_level=0);
SimpleMesh * subd_seq(Mesh *mprev, Mesh *m, Mesh *mnext, MeshMatch *mmprev, MeshMatch *mmnext, int subd_level=0);
SimpleMesh * subd_mesh(Mesh *m0, const vector<vec3f> &vpos, const vector<vector<int>> &finds, const vector<bool> &fshow, const vector<material> &fmats, const vector<bool> &fcolor, int subd_level=0);


/////////////////////////////////
// meshgit_drawmesh.cpp

void draw_mesh( Mesh *m );
void draw_mesh( Mesh *m, vector<ivert> &vm, vector<iface> &fm, material &mat_unmatch, material &mat_unmatch_ );
void draw_mesh( Mesh *m, Mesh *m_prev, vector<ivert> &vm, vector<iface> &fm, material &mat_unmatch, material &mat_unmatch_ );
void draw_mesh( Mesh *m, vector<ivert> &vm0, vector<iface> &fm0, vector<ivert> &vm1, vector<iface> &fm1, bool ab );
void draw_mesh( Mesh *m, Mesh *m_prev, vector<ivert> &vm0, vector<iface> &fm0, vector<ivert> &vm1, vector<iface> &fm1, bool ab );

void draw_mesh( Mesh *m, vector<ivert> &vm, vector<iface> &fm, vec3f c_e_unm, Mesh *m_, vector<vector<ivert>> &vknn, vector<vector<ivert>> &fknn );



SimpleMesh *subdivide_mesh(Mesh *m0, Mesh *m1, int subd_level, MeshMatch *mm, const material_set &matset);


/////////////////////////////////
// meshgit_viewer.cpp

void simple_viewer( Viewer *viewer, vector<Mesh*> &meshes );




/////////////////////////////////
// meshgit_diff.cpp

void diff_2way( Viewer *viewer, vector<Mesh*> &meshes );
void diff_2way( Viewer *viewer, vector<Mesh*> &meshes, string fn_load );
void diff_2way_explore( Viewer *viewer, vector<Mesh*> &meshes );
void diff_3way( Viewer *viewer, vector<Mesh*> &meshes );
void diff_seq( Viewer *viewer, vector<Mesh*> &meshes );
void diff_compare( Viewer *viewer, vector<Mesh*> &meshes, vector<string> &fn_maps );




/////////////////////////////////
// meshgit_merge.cpp

void merge( Viewer *viewer, vector<Mesh*> &meshes );


#endif


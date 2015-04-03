#include "meshgit.h"

VP_PropertyView *pnl_properties = nullptr;
VP_Container *pnl_main = nullptr;



bool  glob_mesh_edges = true;
int   glob_mesh_edges_darkness = 2;
bool  glob_mesh_faces = true;
bool  glob_mesh_faces_chgsonly = false;
bool  glob_mesh_faces_sameonly = false;
bool  glob_mesh_faces_twotoned = false;
bool  glob_mesh_components = false;
bool  glob_overlay_edges = false;
bool  glob_morph_verts = false;
bool  glob_morph_faces = false;
bool  glob_connect_matched_verts = false;
bool  glob_connect_matched_faces = false;

float glob_part_offset = 0.0f;

bool  glob_knn_vert = false;
bool  glob_knn_face = false;

int   glob_patch_size = 10;
float glob_patch_per = 0.1f;
int   glob_exact_knn = 1;
float glob_exact_dist = 0.1f;

vec3f glob_offset;
float glob_scale;


material mat_neutral;
material mat_match, mat_match_;
material mat_unmatch0, mat_unmatch1, mat_unmatch01, mat_unmatch0_, mat_unmatch1_, mat_unmatch01_;
material mat_unmatch0a, mat_unmatch0b, mat_unmatch0ab, mat_unmatcha0, mat_unmatchb0;
material mat_unmatch0a_, mat_unmatch0b_, mat_unmatch0ab_, mat_unmatcha0_, mat_unmatchb0_;


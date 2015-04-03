#include "meshgit.h"



// shows a 2-way diff with a bunch of extra controls
// note: some may be very old (may not be updated)... careful!

void diff_2way_explore( Viewer *viewer, vector<Mesh*> &meshes ) {
    Mesh *m0 = meshes[0];
    Mesh *m1 = meshes[1];
    
    dlog.start("Creating initial mesh matching");
    auto mm = new MeshMatch(m0,m1);
    mm->assign_exact( 1, 0.01f );
    mm->cleanup();
    dlog.end();
    
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    auto pnl_3dview0 = new VP_CallBacks();
    pnl_3dview0->name = "mesh0";
    pnl_3dview0->background = pbg_White;// Maya;
    pnl_3dview0->data = mm;
    pnl_3dview0->camera = camera;
    pnl_3dview0->s_display = {
        to_string("File: %s",m0->filename.c_str()),
        to_string("Counts: %d %d", (int)m0->n_v, (int)m0->n_f )
    };
    pnl_3dview0->fndraw = [](VP_CallBacks *vp) {
        MeshMatch *mm = (MeshMatch*)vp->data;
        draw_mesh( mm->m0, mm->vm01, mm->fm01, mat_unmatch0, mat_unmatch0_ );
        //draw_mesh( mm->m0, mm->vm01, mm->fm01, { 0.5f, 0.05f, 0.05f }, mm->m1, mm->v0_knn1, mm->f0_knn1 );
    };
    
    auto pnl_3dview1 = new VP_CallBacks();
    pnl_3dview1->name = "mesh1";
    pnl_3dview1->background = pbg_White;
    pnl_3dview1->data = mm;
    pnl_3dview1->camera = camera;
    pnl_3dview1->s_display = {
        to_string("File: %s",m1->filename.c_str()),
        to_string("Counts: %d %d", (int)m1->n_v, (int)m1->n_f )
    };
    pnl_3dview1->fndraw = [](VP_CallBacks *vp) {
        MeshMatch *mm = (MeshMatch*)vp->data;
        //draw_mesh( mm->m1, mm->vm10, mm->fm10, { 0.05f, 0.5f, 0.05f }, mm->m0, mm->v1_knn0, mm->f1_knn0 );
        draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatch1, mat_unmatch1_ );
    };
    
    
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    pnl_properties->add( new Prop_Bind_Float("Lens", &camera->d, 0.001f, 1.0f ));
    bool b_drawtext = false;
    auto prop_drawtext = new Prop_Bind_Bool("Text", &b_drawtext);
    prop_drawtext->lfn_change = [&](void*) {
        pnl_3dview0->b_display = pnl_3dview1->b_display = b_drawtext;
    };
    pnl_properties->add( prop_drawtext );
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Same Only", &glob_mesh_faces_sameonly));
    pnl_properties->add( new Prop_Bind_Bool("  Changes Only", &glob_mesh_faces_chgsonly));
    pnl_properties->add( new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned));
    pnl_properties->add( new Prop_Bind_Bool(" Components", &glob_mesh_components));
    pnl_properties->add( new Prop_Bind_Float(" Part Offset", &glob_part_offset, 0.0f, 10.0f));
    pnl_properties->add( new Prop_Text("Overlay",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_overlay_edges));
    pnl_properties->add( new Prop_Bind_Bool(" VKNN", &glob_knn_vert));
    pnl_properties->add( new Prop_Bind_Bool(" FKNN", &glob_knn_face));
    pnl_properties->add( new Prop_Text("Matches",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" V0-V1", &glob_connect_matched_verts));
    pnl_properties->add( new Prop_Bind_Bool(" F0-F1", &glob_connect_matched_faces));
    pnl_properties->add( new Prop_Bind_Bool(" Morph Verts", &glob_morph_verts));
    pnl_properties->add( new Prop_Bind_Bool(" Morph Faces", &glob_morph_faces));
    pnl_properties->add( new Prop_Divider(' ') );
    
    add_camera_properties( camera );
    
    pnl_properties->add( new Prop_Divider('_') );
    pnl_properties->add( new Prop_Text("Matches",TEXT_CENTER) );
    pnl_properties->add( new Prop_Divider('-') );
    pnl_properties->add( new Prop_Divider(' ') );
    
    pnl_properties->add( new Prop_Text("Logic Algorithms",TEXT_CENTER) );
    pnl_properties->add( new Prop_Button("Reset", mm, [](void*data) { ((MeshMatch*)data)->reset(); } ));
    pnl_properties->add( new Prop_Button("- Face:Patches", mm, [](void*data) { ((MeshMatch*)data)->unassign_small_patches(glob_patch_size); }));
    pnl_properties->add( new Prop_Bind_Int(" size", &glob_patch_size, 1, 1000 ));
    pnl_properties->add( new Prop_Button("- Face:Patches", mm, [](void*data) { ((MeshMatch*)data)->unassign_small_patches(glob_patch_per); }));
    pnl_properties->add( new Prop_Bind_Float(" per", &glob_patch_per, 0.0f, 1.0f ));
    pnl_properties->add( new Prop_Button("- Face:Twisted", mm, [](void*data) { ((MeshMatch*)data)->unassign_twisted(); }));
    pnl_properties->add( new Prop_Button("- Face:Any Mis", mm, [](void*data) { ((MeshMatch*)data)->unassign_mismatched(); }));
    pnl_properties->add( new Prop_Button("- Face:Any Unm", mm, [](void*data) { ((MeshMatch*)data)->unassign_faces_with_unmatched_verts(); }));
    pnl_properties->add( new Prop_Button("- Vert:All Unm", mm, [](void*data) { ((MeshMatch*)data)->unassign_verts_with_all_unmatched_faces(); }));
    pnl_properties->add( new Prop_Button("- Vert:Any Unm", mm, [](void*data) { ((MeshMatch*)data)->unassign_verts_with_any_unmatched_faces(); }));
    pnl_properties->add( new Prop_Button("+ All:Exact", mm, [](void*data) { ((MeshMatch*)data)->assign_no_geo_cost(); })); //(glob_exact_knn,glob_exact_dist); }));
    pnl_properties->add( new Prop_Bind_Int(" knn", &glob_exact_knn, 1, 100 ));
    pnl_properties->add( new Prop_Bind_Float(" dist", &glob_exact_dist, 0.01f, 1.0f ));
    pnl_properties->add( new Prop_Button("Clean Up", mm, [](void*data) { ((MeshMatch*)data)->cleanup(); }));
    pnl_properties->add( new Prop_Divider(' ') );
    
    pnl_properties->add( new Prop_Text("Greedy Algorithm",TEXT_CENTER) );
    pnl_properties->add( new Prop_Button("Set0",mm,[](void*data){
        auto mm = (MeshMatch*)data;
        mm->costs.alpha = 2.5f;
        mm->costs.dist = 1.0f;
        mm->costs.norm = 1.0f;
        mm->costs.unm = 0.8f;
        mm->costs.mis = 3.5f;
        mm->costs.valence = 0.0f;
        mm->costs.stretch = 0.0f;
        mm->costs.knn = 20;
        mm->costs.srball = 1;
        mm->costs.expand_knn = false;
    }));
    float a = 1.0, b = 0.08;
    for( int iter = 0; iter < 5; iter++ ) {
        pnl_properties->add( new Prop_Button(to_string("iter%d",iter),nullptr,[iter,a,b,&mm](void*){
            mm->costs.alpha = 1.0f;
            mm->costs.dist = mm->costs.norm = a;
            mm->costs.unm = 1.0f;
            mm->costs.mis = 1.0f;
            mm->costs.valence = 0.0f;
            mm->costs.stretch = 1.0f;
            mm->costs.knn = 10;
            mm->costs.srball = 2;
            mm->costs.expand_knn = true;
            glob_patch_per = b;
        }));
        a *= 0.75f;
        b *= 0.50f;
    }
    pnl_properties->add( new Prop_Text("Parameters",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Float(" add/del", &mm->costs.alpha, 0.0, 10.0 ));
    pnl_properties->add( new Prop_Bind_Float(" dist", &mm->costs.dist, 0.0, 10.0 ));
    pnl_properties->add( new Prop_Bind_Float("  alpha", &mm->costs.dist_mult, 0.0, 1.0 ));
    pnl_properties->add( new Prop_Bind_Float(" norm", &mm->costs.norm, 0.0, 10.0 ));
    pnl_properties->add( new Prop_Bind_Float("  alpha", &mm->costs.norm_mult, 0.0, 1.0 ));
    pnl_properties->add( new Prop_Bind_Float(" unmatch", &mm->costs.unm, 0.0, 10.0 ));
    pnl_properties->add( new Prop_Bind_Float(" unmatch", &mm->costs.mis, 0.0, 10.0 ));
    pnl_properties->add( new Prop_Bind_Float(" valence", &mm->costs.valence, 0.0, 2.0));
    pnl_properties->add( new Prop_Bind_Float(" stretch", &mm->costs.stretch, 0.0, 10.0));
    pnl_properties->add( new Prop_Bind_Int(" knn", &mm->costs.knn, 0, 100));
    pnl_properties->add( new Prop_Bind_Int(" srball", &mm->costs.srball, 0, 10));
    pnl_properties->add( new Prop_Bind_Bool(" expand knn", &mm->costs.expand_knn ) );
    pnl_properties->add( new Prop_Button("Run", mm, [](void*data) { ((MeshMatch*)data)->assign_greedy(); } ));
    pnl_properties->add( new Prop_Button("Step", mm, [](void*data) { ((MeshMatch*)data)->assign_greedy_step(); } ));
    pnl_properties->add( new Prop_Button("Cleanup", nullptr, [&](void*) { mm->cleanup(); }));
    pnl_properties->add( new Prop_Button("Step+Cleanup", nullptr, [&](void*) {
        mm->assign_greedy_step();
        mm->cleanup();
    } ));
    pnl_properties->add( new Prop_Button("Cleanup", nullptr, [&](void*) { mm->cleanup(); }));
    pnl_properties->add( new Prop_Button("Backtrack", mm, [](void*data) { ((MeshMatch*)data)->greedy_backtrack(glob_patch_per); } ));
    pnl_properties->add( new Prop_Button("Reset+Run", mm, [](void*data) {
        MeshMatch *mm = (MeshMatch*)data;
        mm->reset();
        mm->assign_greedy();
    }));
    pnl_properties->add( new Prop_Button("Reset+Step", mm, [](void*data) {
        MeshMatch *mm = (MeshMatch*)data;
        mm->reset();
        mm->assign_greedy_step();
    }));
    pnl_properties->add( new Prop_Divider(' ') );
    
    
    
    pnl_main->panel = new VP_Divide("3dviews", pnl_3dview0, pnl_3dview1, 0.5f, DivideType::LEFTRIGHT, true);
    
    viewer->run();
}



// shows a 2-way diff:   orig -> der

void diff_2way(Viewer *viewer, vector<Mesh*> &meshes) {
    diff_2way(viewer, meshes, "");
}
void diff_2way( Viewer *viewer, vector<Mesh*> &meshes, string fn_load ) {
    Mesh *m0 = meshes[0];
    Mesh *m1 = meshes[1];
    
    
    dlog.start("Mesh matching");
    auto mm = new MeshMatch(m0,m1);
    if(fn_load.empty()) {
        //mm->costs.mis = 0.0f;
        //mm->costs.unm = 0.0f;
        //mm->costs.stretch = 0.0f;
        mm->algorithm();
    } else {
        mm->name = fn_load;
        mm->load(fn_load.c_str());
        mm->cleanup();
    }

    dlog.end();
    
    int num_del_faces = 0, num_add_faces = 0;
    dlog.start("Partitioning");
    vector<MeshMatchPartition*> l_mmps = MeshMatchPartition::partition(mm);
    vector<vec3f> l_mmp_norms0, l_mmp_norms1;
    for( auto mmp : l_mmps ) {
        vec3f n0 = zero3f, n1 = zero3f;
        for( int i_f0 : mmp->sif0 ) n0 += m0->fnorms[i_f0];
        n0 = normalize(n0);
        if( n0%n0<0.1f ) n0 = { 1,0,0 };
        l_mmp_norms0.push_back(n0);
        num_del_faces += mmp->sif0.size();
        
        for( int i_f1 : mmp->sif1 ) n1 += m1->fnorms[i_f1];
        n1 = normalize(n1);
        if( n1%n1<0.1f ) n1 = { 1,0,0 };
        l_mmp_norms1.push_back(n1);
        num_add_faces += mmp->sif1.size();
    }
    dlog.end();
    
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    vector<SimpleMesh*> meshes_subd;
    int subd_level = 0;
    auto update_subdivide = [&]{
        for(auto m : meshes_subd) delete m;
        meshes_subd.clear();
        
        auto matset = material_set(mat_match,mat_unmatch0,mat_unmatch1,zero3f);
        if(glob_mesh_faces_twotoned) {
            matset.face_ = mat_match_;
            matset.face_del_ = mat_unmatch0_;
            matset.face_add_ = mat_unmatch1_;
        }
        
        meshes_subd.push_back(subdivide_mesh(m0, m1, subd_level, mm, matset));
        meshes_subd.push_back(subdivide_mesh(m1, m0, subd_level, mm, matset));
        
        meshes_subd[0]->save_light("diff0.lmesh");
        meshes_subd[1]->save_light("diff1.lmesh");
    };
    update_subdivide();
    
    
    auto pnl_3dview0 = new VP_CallBacks();
    pnl_3dview0->name = "mesh0";
    pnl_3dview0->background = pbg_Black;// Maya;
    pnl_3dview0->data = mm;
    pnl_3dview0->camera = camera;
    pnl_3dview0->s_display = {
        to_string("File: %s",m0->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)m0->n_v, (int)m0->n_f ),
        to_string("Changes: %d parts, -%d faces", (int)l_mmps.size(), num_del_faces),
    };
    pnl_3dview0->lfn_draw = [&]{
        meshes_subd[0]->draw( glob_mesh_faces, glob_mesh_edges, mat_match, mat_match_ );
        //draw_mesh( mm->m0, mm->vm01, mm->fm01, mat_unmatch0, mat_unmatch0_ );
        
        if( glob_part_offset > 0.0f ) {
            for( int i = 0; i < l_mmps.size(); i++ ) {
                auto mmp = l_mmps[i];
                auto n0 = l_mmp_norms0[i] * glob_part_offset;
                
                glBegin(GL_TRIANGLES);
                set_material(mat_unmatch0,mat_unmatch0_);
                for( int i_f0 : mmp->sif0 ) draw_face( m0->finds[i_f0], m0->fnorms[i_f0], m0->vpos, n0 );
                glEnd();
                
                glDisable(GL_LIGHTING);
                glBegin(GL_LINES);
                glColor3f( 0.7f, 0.7f, 0.7f );
                for( int i_v0 : mmp->siv0_border ) {
                    glsVertex(m0->vpos[i_v0]);
                    glsVertex(m0->vpos[i_v0]+n0);
                }
                glEnd();
                glEnable(GL_LIGHTING);
            }
        }
    };
    
    auto pnl_3dview1 = new VP_CallBacks();
    pnl_3dview1->name = "mesh1";
    pnl_3dview1->background = pbg_Black;
    pnl_3dview1->data = mm;
    pnl_3dview1->camera = camera;
    pnl_3dview1->s_display = {
        to_string("File: %s",m1->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)m1->n_v, (int)m1->n_f ),
        to_string("Changes: %d parts, +%d faces", (int)l_mmps.size(), num_add_faces),
    };
    pnl_3dview1->lfn_draw = [&]{
        meshes_subd[1]->draw( glob_mesh_faces, glob_mesh_edges, mat_match, mat_match_ );
        //draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatch1, mat_unmatch1_ );
        
        if( glob_part_offset > 0.0f ) {
            for( int i = 0; i < l_mmps.size(); i++ ) {
                auto mmp = l_mmps[i];
                auto n1 = l_mmp_norms1[i] * glob_part_offset;
                
                glBegin(GL_TRIANGLES);
                set_material(mat_unmatch1,mat_unmatch1_);
                for( int i_f1 : mmp->sif1 ) draw_face( m1->finds[i_f1], m1->fnorms[i_f1], m1->vpos, n1 );
                glEnd();
                
                glDisable(GL_LIGHTING);
                glBegin(GL_LINES);
                glColor3f( 0.7f, 0.7f, 0.7f );
                for( int i_v1 : mmp->siv1_border ) {
                    glsVertex(m1->vpos[i_v1]);
                    glsVertex(m1->vpos[i_v1]+n1);
                }
                glEnd();
                glEnable(GL_LIGHTING);
            }
        }
    };
    
    pnl_main->panel = new VP_Divide("3dviews", pnl_3dview0, pnl_3dview1, 0.5f, DivideType::LEFTRIGHT, true);
    
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    int bgchoice = BackgroundType::Black;
    auto prop_background = new Prop_Bind_Enum("Background", BackgroundType::Names, &bgchoice );
    prop_background->fn_change = [&](void*) {
        switch( bgchoice ) {
            case BackgroundType::Gradient: {
                pnl_3dview0->background = pbg_Maya;
                pnl_3dview1->background = pbg_Maya;
            } break;
            case BackgroundType::Black: {
                pnl_3dview0->background = pbg_Black;
                pnl_3dview1->background = pbg_Black;
            } break;
            case BackgroundType::White: {
                pnl_3dview0->background = pbg_White;
                pnl_3dview1->background = pbg_White;
            } break;
        }
    };
    pnl_properties->add(prop_background);
    pnl_properties->add( new Prop_Bind_Float("Lens", &camera->d, 0.001f, 1.0f ));
    bool b_drawtext = false;
    auto prop_drawtext = new Prop_Bind_Bool("Text", &b_drawtext);
    prop_drawtext->lfn_change = [&](void*) {
        pnl_3dview0->b_display = pnl_3dview1->b_display = b_drawtext;
    };
    pnl_properties->add( prop_drawtext );
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Same Only", &glob_mesh_faces_sameonly));
    pnl_properties->add( new Prop_Bind_Bool("  Changes Only", &glob_mesh_faces_chgsonly));
    auto prop_twotone = new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned);
    prop_twotone->fn_change_done = [&](void*) { update_subdivide(); };
    pnl_properties->add(prop_twotone);
    pnl_properties->add( new Prop_Bind_Float(" Part Offset", &glob_part_offset, 0.0f, 10.0f));
    auto prop_subdlvl = new Prop_Bind_Int(" Subd", &subd_level, 0, 5);
    prop_subdlvl->fn_change_done = [&](void*) { update_subdivide(); };
    pnl_properties->add( prop_subdlvl );
    
    pnl_properties->add( new Prop_Divider() );
    add_camera_properties( camera );
    
    
    viewer->run();
}


// show a 3-way diff:   dera <- orig -> derb

void diff_3way( Viewer *viewer, vector<Mesh*> &meshes ) {
    Mesh *m0 = meshes[0];
    Mesh *ma = meshes[1];
    Mesh *mb = meshes[2];
    
    dlog.start("Mesh matching");
    auto mm0a = new MeshMatch(m0,ma);
    auto mm0b = new MeshMatch(m0,mb);
    mm0a->algorithm();
    mm0b->algorithm();
    dlog.end();
    
    vector<MeshMatch*> l_mm = { mm0a, mm0b };
    
    dlog.start("Partitioning");
    int num_del_faces0a = 0, num_add_faces0a = 0;
    vector<MeshMatchPartition*> l_mmps0a = MeshMatchPartition::partition(mm0a);
    vector<vec3f> l_mmp0a_norms0, l_mmp0a_norms1;
    for( auto mmp : l_mmps0a ) {
        vec3f n0 = zero3f, n1 = zero3f;
        for( int i_f0 : mmp->sif0 ) n0 += m0->fnorms[i_f0];
        n0 = normalize(n0);
        if( n0%n0<0.1f ) n0 = { 1,0,0 };
        l_mmp0a_norms0.push_back(n0);
        num_del_faces0a += mmp->sif0.size();
        
        for( int i_f1 : mmp->sif1 ) n1 += ma->fnorms[i_f1];
        n1 = normalize(n1);
        if( n1%n1<0.1f ) n1 = { 1,0,0 };
        l_mmp0a_norms1.push_back(n1);
        num_add_faces0a += mmp->sif1.size();
    }
    
    int num_del_faces0b = 0, num_add_faces0b = 0;
    vector<MeshMatchPartition*> l_mmps0b = MeshMatchPartition::partition(mm0b);
    vector<vec3f> l_mmp0b_norms0, l_mmp0b_norms1;
    for( auto mmp : l_mmps0b ) {
        vec3f n0 = zero3f, n1 = zero3f;
        for( int i_f0 : mmp->sif0 ) n0 += m0->fnorms[i_f0];
        n0 = normalize(n0);
        if( n0%n0<0.1f ) n0 = { 1,0,0 };
        l_mmp0b_norms0.push_back(n0);
        num_del_faces0b += mmp->sif0.size();
        
        for( int i_f1 : mmp->sif1 ) n1 += mb->fnorms[i_f1];
        n1 = normalize(n1);
        if( n1%n1<0.1f ) n1 = { 1,0,0 };
        l_mmp0b_norms1.push_back(n1);
        num_add_faces0b += mmp->sif1.size();
    }
    dlog.end();
    
    
    // export
    auto matset0a = material_set(mat_match,mat_unmatch0a,mat_unmatcha0,zero3f);
    auto matset0b = material_set(mat_match,mat_unmatch0a,mat_unmatchb0,zero3f);
    auto m0_s = subd_m0_3way(m0, mm0a, mm0b);
    auto ma_s = subd_m1_2way(m0, ma, mm0a, matset0a);
    auto mb_s = subd_m1_2way(m0, mb, mm0b, matset0b);
    m0_s->save_light("diff0.lmesh");
    ma_s->save_light("diffa.lmesh");
    mb_s->save_light("diffb.lmesh");
    m0_s->save_light_changes("diff0_changes.lmesh");
    ma_s->save_light_changes("diffa_changes.lmesh");
    mb_s->save_light_changes("diffb_changes.lmesh");
    delete m0_s;
    delete ma_s;
    delete mb_s;
    /*auto m0_sc = subd_m0_3way_changes(m0, mm0a, mm0b);
    auto ma_sc = subd_m1_2way_changes(m0, ma, mm0a, matset0a);
    auto mb_sc = subd_m1_2way_changes(m0, mb, mm0b, matset0b);
    m0_sc->save_light("diff0_changes.lmesh");
    ma_sc->save_light("diffa_changes.lmesh");
    mb_sc->save_light("diffb_changes.lmesh");
    delete m0_sc;
    delete ma_sc;
    delete mb_sc;*/
    
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    
    auto pnl_3dview0 = new VP_CallBacks();
    pnl_3dview0->name = "mesh0";
    pnl_3dview0->background = pbg_Black;// Maya;
    pnl_3dview0->data = &l_mm;
    pnl_3dview0->camera = camera;
    pnl_3dview0->s_display = {
        to_string("File: %s",m0->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)m0->n_v, (int)m0->n_f ),
        to_string("Changes: %d,%d parts, -%d,-%d faces", (int)l_mmps0a.size(), (int)l_mmps0b.size(), num_del_faces0a, num_del_faces0b),
    };
    pnl_3dview0->lfn_draw = [&] {
        draw_mesh( mm0a->m0, mm0a->vm01, mm0a->fm01, mm0b->vm01, mm0b->fm01, true );
        
        if( glob_part_offset > 0.0f ) {
            glBegin(GL_TRIANGLES);
            for( int i = 0; i < l_mmps0a.size(); i++ ) {
                auto mmp = l_mmps0a[i];
                auto n0 = l_mmp0a_norms0[i] * glob_part_offset;
                set_material(mat_unmatch0a,mat_unmatch0a_);
                for( int i_f0 : mmp->sif0 ) draw_face( m0->finds[i_f0], m0->fnorms[i_f0], m0->vpos, n0 );
            }
            for( int i = 0; i < l_mmps0b.size(); i++ ) {
                auto mmp = l_mmps0b[i];
                auto n0 = l_mmp0b_norms0[i] * glob_part_offset * 2.0f;
                set_material(mat_unmatch0b,mat_unmatch0b_);
                for( int i_f0 : mmp->sif0 ) draw_face( m0->finds[i_f0], m0->fnorms[i_f0], m0->vpos, n0 );
            }
            glEnd();
            
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
            glColor4f( 0.7f, 0.7f, 0.7f, 0.2f );
            for( int i = 0; i < l_mmps0a.size(); i++ ) {
                auto mmp = l_mmps0a[i];
                auto n0 = l_mmp0a_norms0[i] * glob_part_offset;
                for( int i_v0 : mmp->siv0_border ) {
                    glsVertex(m0->vpos[i_v0]);
                    glsVertex(m0->vpos[i_v0]+n0);
                }
            }
            
            for( int i = 0; i < l_mmps0b.size(); i++ ) {
                auto mmp = l_mmps0b[i];
                auto n0 = l_mmp0b_norms0[i] * glob_part_offset * 2.0f;
                for( int i_v0 : mmp->siv0_border ) {
                    glsVertex(m0->vpos[i_v0]);
                    glsVertex(m0->vpos[i_v0]+n0);
                }
            }
            glEnd();
            glEnable(GL_LIGHTING);
        }
    };
    
    auto pnl_3dviewa = new VP_CallBacks();
    pnl_3dviewa->name = "mesha";
    pnl_3dviewa->background = pbg_Black;
    pnl_3dviewa->data = &l_mm;
    pnl_3dviewa->camera = camera;
    pnl_3dviewa->s_display = {
        to_string("File: %s",ma->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)ma->n_v, (int)ma->n_f ),
        to_string("Changes: %d parts, +%d faces", (int)l_mmps0a.size(), num_add_faces0a),
    };
    pnl_3dviewa->lfn_draw = [&] {
        MeshMatch *mm = mm0a;
        draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatcha0, mat_unmatcha0_ );
        
        if( glob_part_offset > 0.0f ) {
            glBegin(GL_TRIANGLES);
            for( int i = 0; i < l_mmps0a.size(); i++ ) {
                auto mmp = l_mmps0a[i];
                auto n1 = l_mmp0a_norms1[i] * glob_part_offset;
                set_material(mat_unmatcha0,mat_unmatcha0_);
                for( int i_f1 : mmp->sif1 ) draw_face( ma->finds[i_f1], ma->fnorms[i_f1], ma->vpos, n1 );
            }
            glEnd();
            
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
            glColor4f( 0.7f, 0.7f, 0.7f, 0.2f );
            for( int i = 0; i < l_mmps0a.size(); i++ ) {
                auto mmp = l_mmps0a[i];
                auto n1 = l_mmp0a_norms1[i] * glob_part_offset;
                for( int i_v1 : mmp->siv1_border ) {
                    glsVertex(ma->vpos[i_v1]);
                    glsVertex(ma->vpos[i_v1]+n1);
                }
            }
            glEnd();
            glEnable(GL_LIGHTING);
        }
    };
    
    auto pnl_3dviewb = new VP_CallBacks();
    pnl_3dviewb->name = "meshb";
    pnl_3dviewb->background = pbg_Black;
    pnl_3dviewb->data = &l_mm;
    pnl_3dviewb->camera = camera;
    pnl_3dviewb->s_display = {
        to_string("File: %s",mb->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)mb->n_v, (int)mb->n_f ),
        to_string("Changes: %d parts, +%d faces", (int)l_mmps0b.size(), num_add_faces0b),
    };
    pnl_3dviewb->lfn_draw = [&] {
        MeshMatch *mm = mm0b;
        draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatchb0, mat_unmatchb0_ );
        
        if( glob_part_offset > 0.0f ) {
            glBegin(GL_TRIANGLES);
            for( int i = 0; i < l_mmps0b.size(); i++ ) {
                auto mmp = l_mmps0b[i];
                auto n1 = l_mmp0b_norms1[i] * glob_part_offset;
                set_material(mat_unmatchb0,mat_unmatchb0_);
                for( int i_f1 : mmp->sif1 ) draw_face( mb->finds[i_f1], mb->fnorms[i_f1], mb->vpos, n1 );
            }
            glEnd();
            
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
            glColor4f( 0.7f, 0.7f, 0.7f, 0.2f );
            for( int i = 0; i < l_mmps0b.size(); i++ ) {
                auto mmp = l_mmps0b[i];
                auto n1 = l_mmp0b_norms1[i] * glob_part_offset;
                for( int i_v1 : mmp->siv1_border ) {
                    glsVertex(mb->vpos[i_v1]);
                    glsVertex(mb->vpos[i_v1]+n1);
                }
            }
            glEnd();
            glEnable(GL_LIGHTING);
        }
    };
    
    vector<ViewPanel*> panels = { pnl_3dviewa, pnl_3dview0, pnl_3dviewb };
    pnl_main->panel = new VP_Divide_N("3dviews", panels, DivideType::LEFTRIGHT);
    
    
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    int bgchoice = BackgroundType::Black;
    auto prop_background = new Prop_Bind_Enum("Background", BackgroundType::Names, &bgchoice );
    prop_background->fn_change = [&](void*) {
        switch( bgchoice ) {
            case BackgroundType::Gradient: {
                pnl_3dview0->background = pbg_Maya;
                pnl_3dviewa->background = pbg_Maya;
                pnl_3dviewb->background = pbg_Maya;
            } break;
            case BackgroundType::Black: {
                pnl_3dview0->background = pbg_Black;
                pnl_3dviewa->background = pbg_Black;
                pnl_3dviewb->background = pbg_Black;
            } break;
            case BackgroundType::White: {
                pnl_3dview0->background = pbg_White;
                pnl_3dviewa->background = pbg_White;
                pnl_3dviewb->background = pbg_White;
            } break;
        }
    };
    pnl_properties->add(prop_background);
    pnl_properties->add( new Prop_Bind_Float("Lens", &camera->d, 0.001f, 1.0f ));
    bool b_drawtext = false;
    auto prop_drawtext = new Prop_Bind_Bool("Text", &b_drawtext);
    prop_drawtext->lfn_change = [&](void*) {
        for( auto pnl : panels ) pnl->b_display = b_drawtext;
    };
    pnl_properties->add( prop_drawtext );
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Same Only", &glob_mesh_faces_sameonly));
    pnl_properties->add( new Prop_Bind_Bool("  Changes Only", &glob_mesh_faces_chgsonly));
    pnl_properties->add( new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned));
    pnl_properties->add( new Prop_Bind_Float(" Part Offset", &glob_part_offset, 0.0f, 10.0f));
    pnl_properties->add( new Prop_Divider(' ') );
    
    add_camera_properties( camera );
    
    
    
    
    viewer->run();
}



// runs over a sequence:   ver0 -> ver1 -> ver2 -> ... -> vern

void diff_seq( Viewer *viewer, vector<Mesh*> &meshes ) {
    
    int n_meshes = meshes.size();
    int h = (int)sqrt(n_meshes);
    int w = ceilf( (float)n_meshes / (float)h );
    
    dlog.start("Mesh matchings");
    vector<MeshMatch*> l_mm;
    for( int i = 0; i < n_meshes-1; i++ ) {
        auto mm = new MeshMatch(meshes[i],meshes[i+1]);
        mm->algorithm();
        
        l_mm.push_back(mm);
    }
    dlog.end();
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    for(int i=0; i<n_meshes;i++) {
        SimpleMesh *mm = nullptr;
        if(i==0) {
            auto matset = material_set(mat_match,mat_unmatch0,mat_unmatch1,zero3f);
            mm = subd_m0_2way(meshes[i], meshes[i+1], l_mm[i], matset);
        } else if(i==n_meshes-1) {
            auto matset = material_set(mat_match,mat_unmatch0,mat_unmatch1,zero3f);
            mm = subd_m1_2way(meshes[i-1], meshes[i], l_mm[i-1], matset);
        } else {
            mm = subd_seq(i>0?meshes[i-1]:nullptr, meshes[i], i<n_meshes-1?meshes[i+1]:nullptr, i>0?l_mm[i-1]:nullptr, i<n_meshes-1?l_mm[i]:nullptr);
        }
        char fn[80]; sprintf(fn, "diff%d.lmesh", i);
        mm->save_light(fn);
        delete mm;
    }
    
    
    vector<ViewPanel*> l_pnls;
    
    for( int i = 0; i < n_meshes; i++ ) {
        auto p = new pair<MeshMatch*,MeshMatch*>( (i>0?l_mm[i-1]:nullptr), (i<n_meshes-1?l_mm[i]:nullptr) );
        
        auto pnl = new VP_CallBacks();
        pnl->name = to_string("mesh%d",i);
        pnl->background = pbg_Black;
        pnl->data = p;
        pnl->camera = camera;
        pnl->s_display = {
            to_string("File: %s", meshes[i]->filename.c_str()),
            to_string("Counts: %d %d", (int)meshes[i]->n_v, (int)meshes[i]->n_f )
        };
        pnl->fndraw = [](VP_CallBacks*vp) {
            auto p = (pair<MeshMatch*,MeshMatch*>*)vp->data;
            MeshMatch *mm0 = p->first;
            MeshMatch *mm1 = p->second;
            
            if( !mm0 && !mm1 ) {                                                                    // only
            } else if( !mm0 ) {                                                                     // first
                draw_mesh( mm1->m0, mm1->vm01, mm1->fm01, mat_unmatch0, mat_unmatch0_ );
            } else if( !mm1 ) {                                                                     // last
                draw_mesh( mm0->m1, mm0->m0, mm0->vm10, mm0->fm10, mat_unmatch1, mat_unmatch1_ );
            } else {                                                                                // middle
                draw_mesh( mm1->m0, mm0->m0, mm0->vm10, mm0->fm10, mm1->vm01, mm1->fm01, false );
            }
        };
        
        l_pnls.push_back(pnl);
    }
    
    vector<ViewPanel*> grid;
    for( int y = 0, i = 0; y < h; y++ ) {
        vector<ViewPanel*> row;
        for( int x = 0; x < w && i < n_meshes; x++, i++ ) {
            if( i < n_meshes ) row.push_back(l_pnls[i]);
            else row.push_back(nullptr);
        }
        grid.push_back(new VP_Divide_N("row", row, DivideType::LEFTRIGHT));
    }
    reverse(grid.begin(),grid.end());
    
    //pnl_main->panel = new VP_Divide_N("3dviews", l_pnls, DivideType::LEFTRIGHT);
    pnl_main->panel = new VP_Divide_N("grid", grid, DivideType::TOPBOTTOM );
    
    
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    int bgchoice = BackgroundType::Black;
    auto prop_background = new Prop_Bind_Enum("Background", BackgroundType::Names, &bgchoice );
    prop_background->fn_change = [&](void*) {
        switch( bgchoice ) {
            case BackgroundType::Gradient: {
                for( auto p : l_pnls ) p->background = pbg_Maya;
            } break;
            case BackgroundType::Black: {
                for( auto p : l_pnls ) p->background = pbg_Black;
            } break;
            case BackgroundType::White: {
                for( auto p : l_pnls ) p->background = pbg_White;
            } break;
        }
    };
    pnl_properties->add(prop_background);
    pnl_properties->add( new Prop_Bind_Float("Lens", &camera->d, 0.001f, 1.0f ));
    bool b_drawtext = false;
    auto prop_drawtext = new Prop_Bind_Bool("Text", &b_drawtext);
    prop_drawtext->lfn_change = [&](void*) {
        for( auto pnl : l_pnls ) pnl->b_display = b_drawtext;
    };
    pnl_properties->add( prop_drawtext );
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Same Only", &glob_mesh_faces_sameonly));
    pnl_properties->add( new Prop_Bind_Bool("  Changes Only", &glob_mesh_faces_chgsonly));
    pnl_properties->add( new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned));
    pnl_properties->add( new Prop_Bind_Float(" Part Offset", &glob_part_offset, 0.0f, 10.0f));
    pnl_properties->add( new Prop_Divider() );
    
    add_camera_properties( camera );
    
    
    
    viewer->run();
}



// compares different matching algorithms

void diff_compare( Viewer *viewer, vector<Mesh*> &meshes, vector<string> &fn_maps ) {
    int n_meshes = 2+fn_maps.size();
    int h = (int)sqrt(n_meshes*2);
    int w = ceilf( (float)n_meshes / (float)h );
    bool b_drawtext = false;
    
    dlog.start("Creating/loading mesh matchings");
    vector<MeshMatch*> l_mms;
    MeshMatch *mm;
    
    mm = new MeshMatch(meshes[0],meshes[1]);
    mm->name = "MeshGit";
    mm->algorithm();
    l_mms.push_back(mm);
    
    mm = new MeshMatch(meshes[0],meshes[1]);
    mm->name = "Exact";
    mm->assign_exact( 1, 0.1f );
    mm->cleanup();
    l_mms.push_back(mm);
    
    for( string &fn_map : fn_maps ) {
        mm = new MeshMatch(meshes[0],meshes[1]);
        mm->name = fn_map;
        mm->load(fn_map.c_str());
        
        if(true) {
            mm->cleanup();
        }
        
        l_mms.push_back(mm);
    }
    
    dlog.end();
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    auto f_create_panel = [camera](MeshMatch*mm)->ViewPanel* {
        VP_CallBacks *pnl0, *pnl1;
        
        pnl0 = new VP_CallBacks();
        pnl0->name = "mesh0";
        //pnl0->s_display = { mm->name };
        pnl0->background = pbg_White;
        pnl0->camera = camera;
        pnl0->lfn_draw = [mm] {
            draw_mesh( mm->m0, mm->vm01, mm->fm01, mat_unmatch0, mat_unmatch0_ );
        };
        
        pnl1 = new VP_CallBacks();
        pnl1->name = "mesh1";
        //pnl1->s_display = { mm->name };
        pnl1->background = pbg_White;
        pnl1->camera = camera;
        pnl1->lfn_draw = [mm] {
            draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatch1, mat_unmatch1_ );
        };
        
        auto pnl_text = new ViewPanel();
        pnl_text->s_display = { mm->name };
        pnl_text->background = pbg_Gray;
        pnl_text->b_display = true;
        
        vector<ViewPanel*> l_pnls = {pnl0,pnl1};
        auto pnl_pair = new VP_Divide_N("pair", l_pnls, DivideType::LEFTRIGHT);
        
        auto pnl_pairset = new VP_Split("pairset", pnl_text, pnl_pair, 15, SplitTypes::SPLIT_TOP );
        pnl_pairset->resizable = false;
        pnl_pairset->border = 1;
        return pnl_pairset;
    };
    
    vector<ViewPanel*> l_pnls;
    for( MeshMatch *mm : l_mms ) {
        l_pnls.push_back( f_create_panel(mm) );
    }
    vector<ViewPanel*> grid;
    for( int y = 0, i = 0; y < h; y++ ) {
        vector<ViewPanel*> row;
        for( int x = 0; x < w; x++, i++ ) {
            if( i < n_meshes ) row.push_back(l_pnls[i]);
            else row.push_back(nullptr);
        }
        grid.push_back(new VP_Divide_N("row", row, DivideType::LEFTRIGHT));
    }
    reverse(grid.begin(),grid.end());
    pnl_main->panel = new VP_Divide_N("grid", grid, DivideType::TOPBOTTOM );
    
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    int bgchoice = BackgroundType::White;
    auto prop_background = new Prop_Bind_Enum("Background", BackgroundType::Names, &bgchoice );
    prop_background->fn_change = [&](void*) {
        switch( bgchoice ) {
            case BackgroundType::Gradient: {
                for( auto p : l_pnls ) {
                    auto p0 = (VP_Split*)p;
                    auto p1 = (VP_Divide_N*)p0->panel1;
                    p1->panels[0]->background = pbg_Maya;
                    p1->panels[1]->background = pbg_Maya;
                }
            } break;
            case BackgroundType::Black: {
                for( auto p : l_pnls ) {
                    auto p0 = (VP_Split*)p;
                    auto p1 = (VP_Divide_N*)p0->panel1;
                    p1->panels[0]->background = pbg_Black;
                    p1->panels[1]->background = pbg_Black;
                }
            } break;
            case BackgroundType::White: {
                for( auto p : l_pnls ) {
                    auto p0 = (VP_Split*)p;
                    auto p1 = (VP_Divide_N*)p0->panel1;
                    p1->panels[0]->background = pbg_White;
                    p1->panels[1]->background = pbg_White;
                }
            } break;
        }
    };
    pnl_properties->add(prop_background);
    pnl_properties->add( new Prop_Bind_Float("Lens", &camera->d, 0.001f, 1.0f ));
    auto prop_drawtext = new Prop_Bind_Bool("Text", &b_drawtext);
    prop_drawtext->lfn_change = [&](void*) {
        for( auto pnl : l_pnls ) {
            auto pnl_ = (VP_Divide_N*)pnl;
            for( auto pnl__ : pnl_->panels ) pnl__->b_display = b_drawtext;
        }
    };
    pnl_properties->add( prop_drawtext );
    
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Same Only", &glob_mesh_faces_sameonly));
    pnl_properties->add( new Prop_Bind_Bool("  Changes Only", &glob_mesh_faces_chgsonly));
    pnl_properties->add( new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned));
    pnl_properties->add( new Prop_Divider(' ') );
    
    add_camera_properties( camera );
    
    
    
    
    viewer->run();
}

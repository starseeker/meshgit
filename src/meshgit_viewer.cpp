#include "meshgit.h"


// shows a simple side-by-side / overlap view of meshes 

void simple_viewer( Viewer *viewer, vector<Mesh*> &meshes ) {
    int n_meshes = meshes.size();
    int h = (int)sqrt(n_meshes);
    int w = ceilf( (float)n_meshes / (float)h );
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    vector<SimpleMesh*> meshes_subd;
    int subd_level = 0;
    auto update_subdivide = [&]{
        for( auto m : meshes_subd ) delete m;
        meshes_subd.clear();
        
        for( auto m : meshes ) {
            /*auto mm = new SimpleMesh();
            mm->vpos = m->vpos;
            for( auto v : m->vpos ) mm->vinfo.push_back( { 0.0f, true } );
            auto &finds = m->finds;
            for( int i_f = 0; i_f < finds.size(); i_f++ ) {
                //if( !fshow[i_f] ) continue;
                mm->finds.push_back(finds[i_f]);
                mm->fmats.push_back(mat_neutral);
                mm->fmats_.push_back(mat_neutral);
                mm->fcolor.push_back(true);
            }
            mm->clean();
            mm->recompute_normals();
            for( int l = 0; l < subd_level; l++ ) {
                auto mm_ = mm->subd_catmullclark();
                delete mm;
                mm = mm_;
            }
            meshes_subd.push_back(mm);*/
            
            meshes_subd.push_back(subd_mesh(m, subd_level));
        }
        
        int sz = meshes.size();
        for(int i=0; i<sz; i++) {
            char buf[80]; sprintf(buf, "view%d.lmesh", i);
            meshes_subd[i]->save_light(buf);
        }
    };
    update_subdivide();
    
    
    
    vector<ViewPanel*> l_pnls;
    
    for( int i = 0; i < n_meshes; i++ ) {
        
        auto pnl = new VP_CallBacks();
        pnl->name = to_string("mesh%d",i);
        pnl->background = pbg_Black;
        pnl->camera = camera;
        pnl->s_display = {
            to_string("File: %s", meshes[i]->filename.c_str()),
            to_string("Counts: %d %d", (int)meshes[i]->n_v, (int)meshes[i]->n_f )
        };
        pnl->fndraw = [i,&subd_level,&meshes,&meshes_subd](VP_CallBacks*vp) {
            meshes_subd[i]->draw_override(glob_mesh_faces, glob_mesh_edges, mat_neutral, mat_neutral, glob_mesh_edges_darkness );
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
    
    auto pnl_overlap = new VP_CallBacks();
    pnl_overlap->name = "overlap";
    pnl_overlap->background = pbg_Black;
    pnl_overlap->camera = camera;
    pnl_overlap->s_display = { "Overlap" };
    bool overlap_colors = true;
    auto loverlap_colors = vector<material> { mat_unmatch0, mat_unmatch1 };
    pnl_overlap->fndraw = [&](VP_CallBacks*) {
        for( int i = 0; i < meshes_subd.size(); i++ ) {
            auto m = meshes_subd[i];
            auto &mat = overlap_colors ? loverlap_colors[i % (int)loverlap_colors.size()] : mat_neutral;
            m->draw_override(glob_mesh_faces,glob_mesh_edges,mat,mat, glob_mesh_edges_darkness );
        }
    };
    
    
    vector<string> UIs = { "Side-by-Side", "Overlap" };
    auto pnl_switch = new VP_SwitchableContainer();
    pnl_switch->name = "switcher";
    pnl_switch->panels = { new VP_Divide_N("grid", grid, DivideType::TOPBOTTOM ), pnl_overlap };
    
    pnl_main->panel = pnl_switch;
    
    
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    int bgchoice = BackgroundType::Black;
    auto prop_background = new Prop_Bind_Enum("Background", BackgroundType::Names, &bgchoice );
    prop_background->fn_change = [&](void*) {
        switch( bgchoice ) {
            case BackgroundType::Gradient: {
                for( auto p : l_pnls ) p->background = pbg_Maya;
                pnl_overlap->background = pbg_Maya;
            } break;
            case BackgroundType::Black: {
                for( auto p : l_pnls ) p->background = pbg_Black;
                pnl_overlap->background = pbg_Black;
            } break;
            case BackgroundType::White: {
                for( auto p : l_pnls ) p->background = pbg_White;
                pnl_overlap->background = pbg_White;
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
    pnl_properties->add(new Prop_Bind_Enum("UI", UIs, &pnl_switch->i_panel) );
    pnl_properties->add( new Prop_Bind_Bool(" Colors", &overlap_colors));
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned));
    auto prop_subdlvl = new Prop_Bind_Int("  Subd", &subd_level, 0, 5);
    prop_subdlvl->fn_change_done = [&](void*) { update_subdivide(); };
    pnl_properties->add( prop_subdlvl );
    pnl_properties->add( new Prop_Divider() );
    
    add_camera_properties( camera );
    
    
    
    viewer->run();
}

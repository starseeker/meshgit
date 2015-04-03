#include "meshgit.h"

vector<VPCameraSettings*> settings;

void add_camera_properties( VPCamera *camera ) {
    FILE *fp = fopen("cameras.txt","rt");
    if( fp ) {
        int n; fscanf(fp,"%d\n¡",&n);
        dlog.print("Loading %d camera settings", n);
        for( int i = 0; i < n; i++ )
            settings.push_back( new VPCameraSettings(fp) );
        fclose(fp);
    }
    
    auto f_save = []{
        FILE *fp = fopen("cameras.txt","wt");
        fprintf(fp,"%d\n",(int)settings.size());
        for( auto setting : settings ) setting->to_file(fp);
        fclose(fp);
    };
    
    Property *prop_div = new Prop_Text("Saved Settings", TEXT_LEFT);
    
    pnl_properties->add( new Prop_Text("Camera Settings",TEXT_CENTER) );
    string *s_name = new string();
    auto prop_name = new Prop_Bind_String("Name: ", s_name);
    pnl_properties->add( prop_name );
    prop_name->valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_., <>{}[]!@#$%^&*()";
    pnl_properties->add( new Prop_Button(" Save",nullptr,[s_name,camera,prop_div,&f_save](void*){
        for( auto camset : settings ) {
            if( camset->name == *s_name ) {
                camset->from_camera(camera);
                f_save();
                return;
            }
        }
        
        auto prop_prev = (!settings.empty() ? settings[settings.size()-1]->prop : nullptr );
        auto camset = new VPCameraSettings(camera,*s_name);
        settings.push_back( camset );
        camset->prop = new Prop_Button( to_string(" %s",camset->name.c_str()),nullptr,[camera,camset,s_name,prop_div](void*) {
            camset->to_camera(camera);
            s_name->assign( camset->name );
        });
        
        int insert_at = -1;
        for( int i_prop = 0; i_prop < pnl_properties->properties.size(); i_prop++ ) {
            auto prop_search = pnl_properties->properties[i_prop];
            if( prop_search == prop_div ) insert_at = i_prop+1;
            if( prop_search == prop_prev ) { insert_at = i_prop+1; break; }
        }
        if( insert_at != -1 ) {
            pnl_properties->properties.insert(pnl_properties->properties.begin()+insert_at, camset->prop);
        }
        f_save();
    }));
    pnl_properties->add( new Prop_Button(" Delete",nullptr,[s_name,&f_save](void*){
        Property *prop = nullptr;
        for( int i_camset = 0; i_camset < settings.size(); i_camset++ ) {
            auto camset = settings[i_camset];
            if( camset->name == *s_name ) {
                settings.erase(settings.begin()+i_camset);
                prop = camset->prop;
                delete camset;
                break;
            }
        }
        if( !prop ) return;
        
        int remove_at = -1;
        for( int i_prop = 0; i_prop < pnl_properties->properties.size(); i_prop++ ) {
            auto prop_search = pnl_properties->properties[i_prop];
            if( prop_search == prop ) { remove_at = i_prop; break; }
        }
        if( remove_at != -1 ) {
            pnl_properties->properties.erase(pnl_properties->properties.begin()+remove_at);
        }
        f_save();
        
    }));
    
    pnl_properties->add( prop_div );
    for( VPCameraSettings *camset : settings ) {
        camset->prop = new Prop_Button( to_string(" %s",camset->name.c_str()),nullptr,[camera,camset,s_name](void*) {
            camset->to_camera(camera);
            s_name->assign( camset->name );
        });
        pnl_properties->add( camset->prop );
    }
}

